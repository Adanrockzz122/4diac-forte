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

#include "ModbusMaster.h"
#include "../ModbusIOController.h"

using namespace forte::literals;
using namespace forte::io;

namespace forte::eclipse4diac::io::modbus_io {

  DEFINE_FIRMWARE_FB(FORTE_ModbusMaster, "ModbusMaster"_STRID)

  namespace {
    const auto cDataInputNames =
        std::array{"QI"_STRID, "IPAddress"_STRID, "Port"_STRID, "UpdateInterval"_STRID, "ResponseTimeout"_STRID};
    const auto cDataOutputNames = std::array{"QO"_STRID, "STATUS"_STRID};
    const auto cEventInputNames = std::array{"INIT"_STRID};
    const auto cEventInputTypeIds = std::array{"EInit"_STRID};
    const auto cEventOutputNames = std::array{"INITO"_STRID, "IND"_STRID};
    const auto cEventOutputTypeIds = std::array{"EInit"_STRID, "Event"_STRID};

    const auto cPlugNameIds = std::array{"BusAdapterOut"_STRID};
    const SFBInterfaceSpec cFBInterfaceSpec = {
        .mEINames = cEventInputNames,
        .mEITypeNames = cEventInputTypeIds,
        .mEONames = cEventOutputNames,
        .mEOTypeNames = cEventOutputTypeIds,
        .mDINames = cDataInputNames,
        .mDONames = cDataOutputNames,
        .mDIONames = {},
        .mSocketNames = {},
        .mPlugNames = cPlugNameIds,
    };
  } // namespace

  FORTE_ModbusMaster::FORTE_ModbusMaster(const forte::StringId paInstanceNameId, CFBContainer &paContainer) :
      IOConfigFBMultiMaster(paContainer, cFBInterfaceSpec, paInstanceNameId),
      var_QI(0_BOOL),
      var_IPAddress(u""_WSTRING),
      var_Port(502_UINT),
      var_UpdateInterval(25_UINT),
      var_ResponseTimeout(500_UINT),
      var_QO(0_BOOL),
      var_STATUS(u""_WSTRING),
      conn_INITO(*this, 0),
      conn_IND(*this, 1),
      conn_QI(nullptr),
      conn_IPAddress(nullptr),
      conn_Port(nullptr),
      conn_UpdateInterval(nullptr),
      conn_ResponseTimeout(nullptr),
      conn_QO(*this, 0, var_QO),
      conn_STATUS(*this, 1, var_STATUS),
      var_BusAdapterOut("BusAdapterOut"_STRID, *this, 0) {
  }

  void FORTE_ModbusMaster::setInitialValues() {
    var_QI = 0_BOOL;
    var_IPAddress = u""_WSTRING;
    var_Port = 502_UINT;
    var_UpdateInterval = 25_UINT;
    var_ResponseTimeout = 500_UINT;
    var_QO = 0_BOOL;
    var_STATUS = u""_WSTRING;
  }

  void FORTE_ModbusMaster::setConfig() {
    ModbusIOController::ModbusConfig config;
    config.connectionType = ModbusIOController::eTCP;

    // Convert WSTRING IP address to char array
    std::string ipStr;
    var_IPAddress.toString(ipStr);
    // Remove surrounding quotes if present
    if (ipStr.size() >= 2 && ipStr.front() == '\'' && ipStr.back() == '\'') {
      ipStr = ipStr.substr(1, ipStr.size() - 2);
    }
    size_t len = ipStr.size() < sizeof(config.ipAddress) - 1 ? ipStr.size() : sizeof(config.ipAddress) - 1;
    ipStr.copy(config.ipAddress, len);
    config.ipAddress[len] = '\0';

    config.port = static_cast<CIEC_UINT::TValueType>(var_Port);
    config.updateInterval = static_cast<CIEC_UINT::TValueType>(var_UpdateInterval);
    config.responseTimeout = static_cast<CIEC_UINT::TValueType>(var_ResponseTimeout);
    getDeviceController()->setConfig(&config);
  }

  IODeviceController *FORTE_ModbusMaster::createDeviceController(CDeviceExecution &paDeviceExecution) {
    return new ModbusIOController(paDeviceExecution);
  }

  void FORTE_ModbusMaster::readInputData(const TEventID paEIID) {
    switch (paEIID) {
      case scmEventINITID: {
        readData(0, var_QI, conn_QI);
        readData(1, var_IPAddress, conn_IPAddress);
        readData(2, var_Port, conn_Port);
        readData(3, var_UpdateInterval, conn_UpdateInterval);
        readData(4, var_ResponseTimeout, conn_ResponseTimeout);
        break;
      }
      default: break;
    }
  }

  void FORTE_ModbusMaster::writeOutputData(const TEventID paEIID) {
    switch (paEIID) {
      case scmEventINITOID: {
        writeData(cFBInterfaceSpec.getNumDIs() + 0, var_QO, conn_QO);
        writeData(cFBInterfaceSpec.getNumDIs() + 1, var_STATUS, conn_STATUS);
        break;
      }
      case scmEventINDID: {
        writeData(cFBInterfaceSpec.getNumDIs() + 0, var_QO, conn_QO);
        writeData(cFBInterfaceSpec.getNumDIs() + 1, var_STATUS, conn_STATUS);
        break;
      }
      default: break;
    }
  }

  CIEC_ANY *FORTE_ModbusMaster::getDI(const size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_QI;
      case 1: return &var_IPAddress;
      case 2: return &var_Port;
      case 3: return &var_UpdateInterval;
      case 4: return &var_ResponseTimeout;
    }
    return nullptr;
  }

  CIEC_ANY *FORTE_ModbusMaster::getDO(const size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_QO;
      case 1: return &var_STATUS;
    }
    return nullptr;
  }

  forte::IPlugPin *FORTE_ModbusMaster::getPlugPinUnchecked(size_t paIndex) {
    return (paIndex == 0) ? &var_BusAdapterOut : nullptr;
  }

  CEventConnection *FORTE_ModbusMaster::getEOConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_INITO;
      case 1: return &conn_IND;
    }
    return nullptr;
  }

  CDataConnection **FORTE_ModbusMaster::getDIConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_QI;
      case 1: return &conn_IPAddress;
      case 2: return &conn_Port;
      case 3: return &conn_UpdateInterval;
      case 4: return &conn_ResponseTimeout;
    }
    return nullptr;
  }

  CDataConnection *FORTE_ModbusMaster::getDOConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_QO;
      case 1: return &conn_STATUS;
    }
    return nullptr;
  }

} // namespace forte::eclipse4diac::io::modbus_io
