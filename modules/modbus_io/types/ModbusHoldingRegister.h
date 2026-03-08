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

#include "ModbusSlaveBase.h"
#include "forte/datatypes/forte_bool.h"
#include "forte/datatypes/forte_string.h"
#include "forte/datatypes/forte_wstring.h"

namespace forte::eclipse4diac::io::modbus_io {

  /*! @brief Modbus Holding Register Slave Function Block
   *
   * Represents a Modbus slave device that exposes 4 input registers
   * (read-only, 16-bit) and 4 holding registers (read-write, 16-bit).
   *
   * Each register is identified by a STRING data input that maps
   * to an IO handle identifier used by the IO mapper.
   *
   * Data Inputs:
   *   QI              - Qualifier Input (BOOL)
   *   InputRegister_1..4  - IO handle IDs for input registers (STRING)
   *   HoldingRegister_1..4 - IO handle IDs for holding registers (STRING)
   *
   * Data Outputs:
   *   QO     - Qualifier Output (BOOL)
   *   STATUS - Status message (WSTRING)
   */
  class FORTE_ModbusHoldingRegister : public ModbusSlaveBase {
      DECLARE_FIRMWARE_FB(FORTE_ModbusHoldingRegister)

    public:
      FORTE_ModbusHoldingRegister(const forte::StringId paInstanceNameId, CFBContainer &paContainer);
      ~FORTE_ModbusHoldingRegister() override = default;

      CIEC_BOOL var_QI;
      CIEC_STRING var_InputRegister_1;
      CIEC_STRING var_InputRegister_2;
      CIEC_STRING var_InputRegister_3;
      CIEC_STRING var_InputRegister_4;
      CIEC_STRING var_HoldingRegister_1;
      CIEC_STRING var_HoldingRegister_2;
      CIEC_STRING var_HoldingRegister_3;
      CIEC_STRING var_HoldingRegister_4;

      CIEC_BOOL var_QO;
      CIEC_WSTRING var_STATUS;

      CEventConnection conn_MAPO;
      CEventConnection conn_IND;

      CDataConnection *conn_QI;
      CDataConnection *conn_InputRegister_1;
      CDataConnection *conn_InputRegister_2;
      CDataConnection *conn_InputRegister_3;
      CDataConnection *conn_InputRegister_4;
      CDataConnection *conn_HoldingRegister_1;
      CDataConnection *conn_HoldingRegister_2;
      CDataConnection *conn_HoldingRegister_3;
      CDataConnection *conn_HoldingRegister_4;

      COutDataConnection<CIEC_BOOL> conn_QO;
      COutDataConnection<CIEC_WSTRING> conn_STATUS;

      CIEC_ANY *getDI(size_t) override;
      CIEC_ANY *getDO(size_t) override;

      CEventConnection *getEOConUnchecked(TPortId) override;
      CDataConnection **getDIConUnchecked(TPortId) override;
      CDataConnection *getDOConUnchecked(TPortId) override;

    protected:
      void initHandles() override;

    private:
      static const TEventID scmEventMAPID = 0;
      static const TEventID scmEventMAPOID = 0;
      static const TEventID scmEventINDID = 1;
      static const int scmBusAdapterInAdpNum = 0;
      static const int scmBusAdapterOutAdpNum = 1;

      static const int scmNumInputRegisters = 4;
      static const int scmNumHoldingRegisters = 4;

      void readInputData(TEventID paEIID) override;
      void writeOutputData(TEventID paEIID) override;
      void setInitialValues() override;
  };

} // namespace forte::eclipse4diac::io::modbus_io
