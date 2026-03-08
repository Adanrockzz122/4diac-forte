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

#include "ModbusHoldingRegister.h"
#include "../ModbusIOController.h"

using namespace forte::literals;
using namespace forte::io;

namespace forte::eclipse4diac::io::modbus_io {

  DEFINE_FIRMWARE_FB(FORTE_ModbusHoldingRegister, "ModbusHoldingRegister"_STRID)

  namespace {
    const auto cDataInputNames = std::array{
        "QI"_STRID,
        "InputRegister_1"_STRID,  "InputRegister_2"_STRID,  "InputRegister_3"_STRID,  "InputRegister_4"_STRID,
        "HoldingRegister_1"_STRID, "HoldingRegister_2"_STRID, "HoldingRegister_3"_STRID, "HoldingRegister_4"_STRID};
    const auto cDataOutputNames = std::array{"QO"_STRID, "STATUS"_STRID};
    const auto cEventInputNames = std::array{"MAP"_STRID};
    const auto cEventInputTypeIds = std::array{"Event"_STRID};
    const auto cEventOutputNames = std::array{"MAPO"_STRID, "IND"_STRID};
    const auto cEventOutputTypeIds = std::array{"Event"_STRID, "Event"_STRID};

    const auto cSocketNameIds = std::array{"BusAdapterIn"_STRID};
    const auto cPlugNameIds = std::array{"BusAdapterOut"_STRID};
    const SFBInterfaceSpec cFBInterfaceSpec = {
        .mEINames = cEventInputNames,
        .mEITypeNames = cEventInputTypeIds,
        .mEONames = cEventOutputNames,
        .mEOTypeNames = cEventOutputTypeIds,
        .mDINames = cDataInputNames,
        .mDONames = cDataOutputNames,
        .mDIONames = {},
        .mSocketNames = cSocketNameIds,
        .mPlugNames = cPlugNameIds,
    };
  } // namespace

  // Slave type identifier — using 0 since Modbus doesn't have hardware type IDs
  static const int scmSlaveType = 0;

  FORTE_ModbusHoldingRegister::FORTE_ModbusHoldingRegister(const forte::StringId paInstanceNameId,
                                                           CFBContainer &paContainer) :
      ModbusSlaveBase(scmSlaveType, paContainer, cFBInterfaceSpec, paInstanceNameId),
      var_QI(0_BOOL),
      var_InputRegister_1(""_STRING),
      var_InputRegister_2(""_STRING),
      var_InputRegister_3(""_STRING),
      var_InputRegister_4(""_STRING),
      var_HoldingRegister_1(""_STRING),
      var_HoldingRegister_2(""_STRING),
      var_HoldingRegister_3(""_STRING),
      var_HoldingRegister_4(""_STRING),
      var_QO(0_BOOL),
      var_STATUS(u""_WSTRING),
      conn_MAPO(*this, 0),
      conn_IND(*this, 1),
      conn_QI(nullptr),
      conn_InputRegister_1(nullptr),
      conn_InputRegister_2(nullptr),
      conn_InputRegister_3(nullptr),
      conn_InputRegister_4(nullptr),
      conn_HoldingRegister_1(nullptr),
      conn_HoldingRegister_2(nullptr),
      conn_HoldingRegister_3(nullptr),
      conn_HoldingRegister_4(nullptr),
      conn_QO(*this, 0, var_QO),
      conn_STATUS(*this, 1, var_STATUS) {
  }

  void FORTE_ModbusHoldingRegister::setInitialValues() {
    var_QI = 0_BOOL;
    var_InputRegister_1 = ""_STRING;
    var_InputRegister_2 = ""_STRING;
    var_InputRegister_3 = ""_STRING;
    var_InputRegister_4 = ""_STRING;
    var_HoldingRegister_1 = ""_STRING;
    var_HoldingRegister_2 = ""_STRING;
    var_HoldingRegister_3 = ""_STRING;
    var_HoldingRegister_4 = ""_STRING;
    var_QO = 0_BOOL;
    var_STATUS = u""_WSTRING;
  }

  void FORTE_ModbusHoldingRegister::initHandles() {
    // Initialize input register handles (read-only, 16-bit)
    for (int i = 0; i < scmNumInputRegisters; i++) {
      initModbusHandle(1 + i, // DI index (skip QI at 0)
                       ModbusIOController::eInputRegister,
                       static_cast<unsigned int>(i), // Register address
                       CIEC_ANY::e_WORD,
                       IOMapper::In);
    }

    // Initialize holding register handles (read-write, 16-bit)
    for (int i = 0; i < scmNumHoldingRegisters; i++) {
      initModbusHandle(1 + scmNumInputRegisters + i, // DI index
                       ModbusIOController::eHoldingRegister,
                       static_cast<unsigned int>(i), // Register address
                       CIEC_ANY::e_WORD,
                       IOMapper::Out);
    }
  }

  void FORTE_ModbusHoldingRegister::readInputData(const TEventID paEIID) {
    switch (paEIID) {
      case scmEventMAPID: {
        readData(0, var_QI, conn_QI);
        readData(1, var_InputRegister_1, conn_InputRegister_1);
        readData(2, var_InputRegister_2, conn_InputRegister_2);
        readData(3, var_InputRegister_3, conn_InputRegister_3);
        readData(4, var_InputRegister_4, conn_InputRegister_4);
        readData(5, var_HoldingRegister_1, conn_HoldingRegister_1);
        readData(6, var_HoldingRegister_2, conn_HoldingRegister_2);
        readData(7, var_HoldingRegister_3, conn_HoldingRegister_3);
        readData(8, var_HoldingRegister_4, conn_HoldingRegister_4);
        break;
      }
      default: break;
    }
  }

  void FORTE_ModbusHoldingRegister::writeOutputData(const TEventID paEIID) {
    switch (paEIID) {
      case scmEventMAPOID: {
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

  CIEC_ANY *FORTE_ModbusHoldingRegister::getDI(const size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_QI;
      case 1: return &var_InputRegister_1;
      case 2: return &var_InputRegister_2;
      case 3: return &var_InputRegister_3;
      case 4: return &var_InputRegister_4;
      case 5: return &var_HoldingRegister_1;
      case 6: return &var_HoldingRegister_2;
      case 7: return &var_HoldingRegister_3;
      case 8: return &var_HoldingRegister_4;
    }
    return nullptr;
  }

  CIEC_ANY *FORTE_ModbusHoldingRegister::getDO(const size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_QO;
      case 1: return &var_STATUS;
    }
    return nullptr;
  }

  CEventConnection *FORTE_ModbusHoldingRegister::getEOConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_MAPO;
      case 1: return &conn_IND;
    }
    return nullptr;
  }

  CDataConnection **FORTE_ModbusHoldingRegister::getDIConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_QI;
      case 1: return &conn_InputRegister_1;
      case 2: return &conn_InputRegister_2;
      case 3: return &conn_InputRegister_3;
      case 4: return &conn_InputRegister_4;
      case 5: return &conn_HoldingRegister_1;
      case 6: return &conn_HoldingRegister_2;
      case 7: return &conn_HoldingRegister_3;
      case 8: return &conn_HoldingRegister_4;
    }
    return nullptr;
  }

  CDataConnection *FORTE_ModbusHoldingRegister::getDOConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_QO;
      case 1: return &conn_STATUS;
    }
    return nullptr;
  }

} // namespace forte::eclipse4diac::io::modbus_io
