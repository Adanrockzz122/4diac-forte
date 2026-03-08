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

#pragma once

#include "forte/io/device/io_controller_multi.h"
#include <modbus/modbus.h>
#include <vector>
#include <memory>

namespace forte::eclipse4diac::io::modbus_io {

  class ModbusIOHandle;

  /*! @brief Modbus IO Device Controller
   *
   * Implements the IO device controller for Modbus TCP and RTU fieldbus.
   * Extends IODeviceMultiController to support multiple slave devices on a
   * single Modbus bus. Each slave corresponds to a Modbus unit/slave ID
   * addressable through the master connection.
   *
   * The controller manages a single Modbus master connection (TCP or RTU)
   * and periodically polls all configured slave handles in its runLoop.
   */
  class ModbusIOController : public forte::io::IODeviceMultiController {
    public:
      /*! @brief Modbus function types matching the standard Modbus data model */
      enum EModbusFunction {
        eDiscreteInput = 0, //!< 1-bit read-only inputs
        eCoil,              //!< 1-bit read-write outputs
        eInputRegister,     //!< 16-bit read-only registers
        eHoldingRegister    //!< 16-bit read-write registers
      };

      /*! @brief Modbus connection mode */
      enum EModbusConnectionType {
        eTCP = 0,
        eRTU
      };

      struct ModbusConfig : forte::io::IODeviceController::Config {
          EModbusConnectionType connectionType = eTCP;
          char ipAddress[64] = {0};
          unsigned int port = 502;
          char serialDevice[256] = {0};
          int baud = 19200;
          char parity = 'N';
          int dataBits = 8;
          int stopBits = 1;
          unsigned int updateInterval = 25; //!< Polling frequency in Hz (default 25 Hz)
          unsigned int responseTimeout = 500; //!< Response timeout in milliseconds
      };

      class ModbusHandleDescriptor : public forte::io::IODeviceMultiController::HandleDescriptor {
        public:
          CIEC_ANY::EDataTypeID mType;
          EModbusFunction mFunction;
          unsigned int mAddress;

          ModbusHandleDescriptor(std::string const &paId,
                                 forte::io::IOMapper::Direction paDirection,
                                 size_t paSlaveIndex,
                                 CIEC_ANY::EDataTypeID paType,
                                 EModbusFunction paFunction,
                                 unsigned int paAddress) :
              forte::io::IODeviceMultiController::HandleDescriptor(paId, paDirection, paSlaveIndex),
              mType(paType),
              mFunction(paFunction),
              mAddress(paAddress) {
          }
      };

      explicit ModbusIOController(CDeviceExecution &paDeviceExecution);
      ~ModbusIOController() override;

      void setConfig(struct forte::io::IODeviceController::Config *paConfig) override;

      void addSlaveHandle(size_t paIndex, std::unique_ptr<forte::io::IOHandle> paHandle) override;
      void dropSlaveHandles(size_t paIndex) override;

    protected:
      const char *init() override;
      void deInit() override;
      void runLoop() override;

      forte::io::IOHandle *createIOHandle(forte::io::IODeviceController::HandleDescriptor &paHandleDescriptor) override;
      bool isHandleValueEqual(forte::io::IOHandle &paHandle) override;

    private:
      bool isSlaveAvailable(size_t paIndex) override;
      bool checkSlaveType(size_t paIndex, int paType) override;

      bool connectModbus();
      void disconnectModbus();
      void pollInputHandles();
      void writeOutputHandles();

      modbus_t *mModbusCtx;
      ModbusConfig mConfig;

      struct SlaveHandles {
          size_t slaveId = 0;
          std::vector<ModbusIOHandle *> inputHandles;
          std::vector<ModbusIOHandle *> outputHandles;
      };
      std::vector<SlaveHandles> mSlaveHandles;

      static const char *const scmFailedToCreateContext;
      static const char *const scmFailedToConnect;
      static const char *const scmFailedToSetSlave;
      static const char *const scmOK;
  };

} // namespace forte::eclipse4diac::io::modbus_io
