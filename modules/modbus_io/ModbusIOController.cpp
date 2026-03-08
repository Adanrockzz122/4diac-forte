/*******************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Mohammed Adan - initial API and implementation
 *******************************************************************************/

#include "ModbusIOController.h"
#include "ModbusIOHandle.h"
#include "forte/util/criticalregion.h"
#include "forte/util/devlog.h"

namespace forte::eclipse4diac::io::modbus_io {

  const char *const ModbusIOController::scmFailedToCreateContext = "Failed to create Modbus context";
  const char *const ModbusIOController::scmFailedToConnect = "Failed to connect to Modbus device";
  const char *const ModbusIOController::scmFailedToSetSlave = "Failed to set Modbus slave ID";
  const char *const ModbusIOController::scmOK = "OK";

  ModbusIOController::ModbusIOController(CDeviceExecution &paDeviceExecution) :
      IODeviceMultiController(paDeviceExecution),
      mModbusCtx(nullptr) {
  }

  ModbusIOController::~ModbusIOController() {
    disconnectModbus();
  }

  void ModbusIOController::setConfig(struct forte::io::IODeviceController::Config *paConfig) {
    mConfig = *static_cast<ModbusConfig *>(paConfig);
  }

  const char *ModbusIOController::init() {
    if (!connectModbus()) {
      return mError;
    }
    DEVLOG_INFO("[ModbusIOController] Modbus connection established\n");
    return nullptr;
  }

  void ModbusIOController::deInit() {
    disconnectModbus();
    DEVLOG_INFO("[ModbusIOController] Modbus connection closed\n");
  }

  void ModbusIOController::runLoop() {
    while (isAlive() && !hasError()) {
      sleepThread(mConfig.updateInterval);

      {
        CCriticalRegion criticalRegion(mHandleMutex);
        pollInputHandles();
        writeOutputHandles();
      }

      checkForInputChanges();
    }
  }

  bool ModbusIOController::connectModbus() {
    if (mConfig.connectionType == ModbusConfig::eTCP) {
      mModbusCtx = modbus_new_tcp(mConfig.ipAddress, static_cast<int>(mConfig.port));
    } else {
      mModbusCtx = modbus_new_rtu(mConfig.serialDevice, mConfig.baud, mConfig.parity,
                                  mConfig.dataBits, mConfig.stopBits);
    }

    if (mModbusCtx == nullptr) {
      DEVLOG_ERROR("[ModbusIOController] %s\n", scmFailedToCreateContext);
      mError = scmFailedToCreateContext;
      return false;
    }

    // Set response timeout
    modbus_set_response_timeout(mModbusCtx, mConfig.responseTimeout / 1000,
                                (mConfig.responseTimeout % 1000) * 1000);

    if (modbus_connect(mModbusCtx) == -1) {
      DEVLOG_ERROR("[ModbusIOController] %s: %s\n", scmFailedToConnect, modbus_strerror(errno));
      modbus_free(mModbusCtx);
      mModbusCtx = nullptr;
      mError = scmFailedToConnect;
      return false;
    }

    return true;
  }

  void ModbusIOController::disconnectModbus() {
    if (mModbusCtx != nullptr) {
      modbus_close(mModbusCtx);
      modbus_free(mModbusCtx);
      mModbusCtx = nullptr;
    }
  }

  forte::io::IOHandle *ModbusIOController::createIOHandle(
      forte::io::IODeviceController::HandleDescriptor &paHandleDescriptor) {
    auto &desc = static_cast<ModbusHandleDescriptor &>(paHandleDescriptor);

    CIEC_ANY::EDataTypeID type = desc.mType;
    if (type != CIEC_ANY::e_BOOL && type != CIEC_ANY::e_WORD) {
      DEVLOG_ERROR("[ModbusIOController] Unsupported data type for handle '%s'\n", desc.mId.c_str());
      return nullptr;
    }

    return new ModbusIOHandle(this, desc.mDirection, type, desc.mAddress);
  }

  void ModbusIOController::addSlaveHandle(size_t paIndex, std::unique_ptr<forte::io::IOHandle> paHandle) {
    CCriticalRegion criticalRegion(mHandleMutex);

    auto *modbusHandle = static_cast<ModbusIOHandle *>(paHandle.get());

    // Find or create slave entry
    SlaveHandles *slave = nullptr;
    for (auto &sh : mSlaveHandles) {
      if (sh.slaveId == paIndex) {
        slave = &sh;
        break;
      }
    }
    if (slave == nullptr) {
      mSlaveHandles.push_back({paIndex, {}, {}});
      slave = &mSlaveHandles.back();
    }

    if (paHandle->isInput()) {
      slave->inputHandles.push_back(modbusHandle);
      mInputHandles.push_back(std::move(paHandle));
    } else {
      slave->outputHandles.push_back(modbusHandle);
      mOutputHandles.push_back(std::move(paHandle));
    }
  }

  void ModbusIOController::dropSlaveHandles(size_t paIndex) {
    CCriticalRegion criticalRegion(mHandleMutex);

    // Remove from slave tracking
    for (auto it = mSlaveHandles.begin(); it != mSlaveHandles.end(); ++it) {
      if (it->slaveId == paIndex) {
        // Remove matching handles from the main input/output lists
        auto removeFromList = [&](std::vector<std::unique_ptr<forte::io::IOHandle>> &paList,
                                  const std::vector<ModbusIOHandle *> &paHandlePtrs) {
          for (auto *handlePtr : paHandlePtrs) {
            for (auto lit = paList.begin(); lit != paList.end(); ++lit) {
              if (lit->get() == handlePtr) {
                paList.erase(lit);
                break;
              }
            }
          }
        };
        removeFromList(mInputHandles, it->inputHandles);
        removeFromList(mOutputHandles, it->outputHandles);
        mSlaveHandles.erase(it);
        break;
      }
    }
  }

  bool ModbusIOController::isSlaveAvailable(size_t) {
    // For Modbus, any slave ID is potentially valid
    return true;
  }

  bool ModbusIOController::checkSlaveType(size_t, int) {
    // Modbus does not have strict slave type checking
    return true;
  }

  bool ModbusIOController::isHandleValueEqual(forte::io::IOHandle &paHandle) {
    return !static_cast<ModbusIOHandle &>(paHandle).check();
  }

  void ModbusIOController::pollInputHandles() {
    if (mModbusCtx == nullptr) {
      return;
    }

    for (auto &slave : mSlaveHandles) {
      if (modbus_set_slave(mModbusCtx, static_cast<int>(slave.slaveId)) == -1) {
        DEVLOG_ERROR("[ModbusIOController] %s %zu: %s\n", scmFailedToSetSlave, slave.slaveId,
                     modbus_strerror(errno));
        continue;
      }

      for (auto *handle : slave.inputHandles) {
        uint16_t regValue = 0;
        int rc = -1;

        switch (handle->getIOHandleDataType()) {
          case CIEC_ANY::e_BOOL: {
            uint8_t bitValue = 0;
            rc = modbus_read_input_bits(mModbusCtx, static_cast<int>(handle->getAddress()), 1, &bitValue);
            if (rc == 1) {
              CIEC_BOOL val(bitValue != 0);
              handle->set(val);
            }
            break;
          }
          case CIEC_ANY::e_WORD:
            rc = modbus_read_input_registers(mModbusCtx, static_cast<int>(handle->getAddress()), 1, &regValue);
            if (rc == 1) {
              CIEC_WORD val(regValue);
              handle->set(val);
            }
            break;
          default: break;
        }

        if (rc == -1) {
          DEVLOG_WARNING("[ModbusIOController] Failed to read from slave %zu addr %u: %s\n",
                         slave.slaveId, handle->getAddress(), modbus_strerror(errno));
        }
      }
    }
  }

  void ModbusIOController::writeOutputHandles() {
    if (mModbusCtx == nullptr) {
      return;
    }

    for (auto &slave : mSlaveHandles) {
      if (modbus_set_slave(mModbusCtx, static_cast<int>(slave.slaveId)) == -1) {
        DEVLOG_ERROR("[ModbusIOController] %s %zu: %s\n", scmFailedToSetSlave, slave.slaveId,
                     modbus_strerror(errno));
        continue;
      }

      for (auto *handle : slave.outputHandles) {
        CIEC_WORD wordVal;
        handle->get(wordVal);
        int rc = -1;

        switch (handle->getIOHandleDataType()) {
          case CIEC_ANY::e_BOOL: {
            CIEC_BOOL boolVal;
            handle->get(boolVal);
            rc = modbus_write_bit(mModbusCtx, static_cast<int>(handle->getAddress()),
                                  static_cast<bool>(boolVal) ? TRUE : FALSE);
            break;
          }
          case CIEC_ANY::e_WORD: {
            uint16_t regVal = static_cast<TForteWord>(wordVal);
            rc = modbus_write_register(mModbusCtx, static_cast<int>(handle->getAddress()), static_cast<int>(regVal));
            break;
          }
          default: break;
        }

        if (rc == -1) {
          DEVLOG_WARNING("[ModbusIOController] Failed to write to slave %zu addr %u: %s\n",
                         slave.slaveId, handle->getAddress(), modbus_strerror(errno));
        }
      }
    }
  }

} // namespace forte::eclipse4diac::io::modbus_io
