/*******************************************************************************
 * Copyright (c) 2025 Eclipse 4diac contributors
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *    Mohammed Adan - initial implementation (IO layer port)
 *******************************************************************************/

#include "i2c_config_fb.h"
#include "i2c_controller.h"

using namespace forte::literals;

namespace forte::eclipse4diac::io::i2c {

  namespace {
    const auto cEventInputNames = std::array{"INIT"_STRID};
    const auto cEventOutputNames = std::array{"INITO"_STRID, "IND"_STRID};
    const auto cDataInputNames =
        std::array{"QI"_STRID, "VALUE"_STRID, "BusNumber"_STRID,
                   "DeviceAddress"_STRID, "BitAddress"_STRID, "ReadWriteMode"_STRID};
    const auto cDataOutputNames = std::array{"QO"_STRID, "STATUS"_STRID};
    const SFBInterfaceSpec cFBInterfaceSpec = {
        .mEINames = cEventInputNames,
        .mEITypeNames = {},
        .mEONames = cEventOutputNames,
        .mEOTypeNames = {},
        .mDINames = cDataInputNames,
        .mDONames = cDataOutputNames,
        .mDIONames = {},
        .mSocketNames = {},
        .mPlugNames = {},
    };
  } // namespace

  DEFINE_FIRMWARE_FB(FORTE_I2CDev, "eclipse4diac::io::i2c::I2CDev"_STRID)

  FORTE_I2CDev::FORTE_I2CDev(const forte::StringId paInstanceNameId, CFBContainer &paContainer) :
      IOConfigFBController(paContainer, cFBInterfaceSpec, paInstanceNameId),
      var_QI(0_BOOL),
      var_VALUE(u""_WSTRING),
      var_BusNumber(0_UINT),
      var_DeviceAddress(0_UINT),
      var_BitAddress(0_UINT),
      var_ReadWriteMode(0_UINT),
      var_QO(0_BOOL),
      var_STATUS(u""_WSTRING),
      conn_INITO(*this, 0),
      conn_IND(*this, 1),
      conn_QI(nullptr),
      conn_VALUE(nullptr),
      conn_BusNumber(nullptr),
      conn_DeviceAddress(nullptr),
      conn_BitAddress(nullptr),
      conn_ReadWriteMode(nullptr),
      conn_QO(*this, 0, var_QO),
      conn_STATUS(*this, 1, var_STATUS) {
  }

  void FORTE_I2CDev::setInitialValues() {
    CFunctionBlock::setInitialValues();
    var_QI = 0_BOOL;
    var_VALUE = u""_WSTRING;
    var_BusNumber = 0_UINT;
    var_DeviceAddress = 0_UINT;
    var_BitAddress = 0_UINT;
    var_ReadWriteMode = 0_UINT;
    var_QO = 0_BOOL;
    var_STATUS = u""_WSTRING;
  }

  void FORTE_I2CDev::readInputData(const TEventID paEIID) {
    switch (paEIID) {
      case scmEventINITID: {
        readData(0, var_QI, conn_QI);
        readData(1, var_VALUE, conn_VALUE);
        readData(2, var_BusNumber, conn_BusNumber);
        readData(3, var_DeviceAddress, conn_DeviceAddress);
        readData(4, var_BitAddress, conn_BitAddress);
        readData(5, var_ReadWriteMode, conn_ReadWriteMode);
        break;
      }
      default: break;
    }
  }

  void FORTE_I2CDev::writeOutputData(const TEventID paEIID) {
    switch (paEIID) {
      case scmEventINITOID: {
        writeData(6, var_QO, conn_QO);
        writeData(7, var_STATUS, conn_STATUS);
        break;
      }
      case scmEventINDID: {
        writeData(6, var_QO, conn_QO);
        writeData(7, var_STATUS, conn_STATUS);
        break;
      }
      default: break;
    }
  }

  CIEC_ANY *FORTE_I2CDev::getDI(const size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_QI;
      case 1: return &var_VALUE;
      case 2: return &var_BusNumber;
      case 3: return &var_DeviceAddress;
      case 4: return &var_BitAddress;
      case 5: return &var_ReadWriteMode;
    }
    return nullptr;
  }

  CIEC_ANY *FORTE_I2CDev::getDO(const size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_QO;
      case 1: return &var_STATUS;
    }
    return nullptr;
  }

  CEventConnection *FORTE_I2CDev::getEOConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_INITO;
      case 1: return &conn_IND;
    }
    return nullptr;
  }

  CDataConnection **FORTE_I2CDev::getDIConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_QI;
      case 1: return &conn_VALUE;
      case 2: return &conn_BusNumber;
      case 3: return &conn_DeviceAddress;
      case 4: return &conn_BitAddress;
      case 5: return &conn_ReadWriteMode;
    }
    return nullptr;
  }

  CDataConnection *FORTE_I2CDev::getDOConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_QO;
      case 1: return &conn_STATUS;
    }
    return nullptr;
  }

  forte::io::IODeviceController *FORTE_I2CDev::createDeviceController(CDeviceExecution &paDeviceExecution) {
    return new I2CController(paDeviceExecution);
  }

  void FORTE_I2CDev::setConfig() {
    I2CController::Config config;
    config.mBusNumber = TForteUInt16(var_BusNumber);
    config.mDeviceAddress = TForteUInt16(var_DeviceAddress);
    getDeviceController()->setConfig(&config);
  }

  void FORTE_I2CDev::onStartup(CEventChainExecutionThread *const paECET) {
    I2CController::HandleDescriptor desc(
        var_VALUE.getValue(),
        TForteUInt16(var_ReadWriteMode) ? forte::io::IOMapper::Out : forte::io::IOMapper::In,
        TForteUInt16(var_BitAddress));
    initHandle(desc);
    started(paECET);
  }

} // namespace forte::eclipse4diac::io::i2c
