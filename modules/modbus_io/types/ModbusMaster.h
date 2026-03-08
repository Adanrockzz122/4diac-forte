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

#include "ModbusBusAdapter.h"
#include "forte/io/configFB/io_master_multi.h"

namespace forte::eclipse4diac::io::modbus_io {

  /*! @brief Modbus Master Configuration Function Block
   *
   * Configures and manages the Modbus IO controller.
   * Supports both Modbus TCP and RTU connections.
   *
   * Data Inputs:
   *   QI            - Qualifier Input (BOOL): TRUE to initialize, FALSE to de-initialize
   *   IPAddress     - IP address for TCP connection (WSTRING)
   *   Port          - TCP port number (UINT), default 502
   *   UpdateInterval - Polling interval in ms (UINT), default 25
   *   ResponseTimeout - Modbus response timeout in ms (UINT), default 500
   *
   * Data Outputs:
   *   QO     - Qualifier Output (BOOL): TRUE if initialized successfully
   *   STATUS - Status message (WSTRING)
   */
  class FORTE_ModbusMaster : public forte::io::IOConfigFBMultiMaster {
      DECLARE_FIRMWARE_FB(FORTE_ModbusMaster)

    public:
      FORTE_ModbusMaster(forte::StringId paInstanceNameId, CFBContainer &paContainer);
      ~FORTE_ModbusMaster() override = default;

      CIEC_BOOL var_QI;
      CIEC_WSTRING var_IPAddress;
      CIEC_UINT var_Port;
      CIEC_UINT var_UpdateInterval;
      CIEC_UINT var_ResponseTimeout;

      CIEC_BOOL var_QO;
      CIEC_WSTRING var_STATUS;

      CEventConnection conn_INITO;
      CEventConnection conn_IND;

      CDataConnection *conn_QI;
      CDataConnection *conn_IPAddress;
      CDataConnection *conn_Port;
      CDataConnection *conn_UpdateInterval;
      CDataConnection *conn_ResponseTimeout;

      COutDataConnection<CIEC_BOOL> conn_QO;
      COutDataConnection<CIEC_WSTRING> conn_STATUS;

      forte::CPlugPin<FORTE_ModbusBusAdapter_Plug> var_BusAdapterOut;

      CIEC_ANY *getDI(size_t) override;
      CIEC_ANY *getDO(size_t) override;

      forte::IPlugPin *getPlugPinUnchecked(size_t) override;

      CEventConnection *getEOConUnchecked(TPortId) override;
      CDataConnection **getDIConUnchecked(TPortId) override;
      CDataConnection *getDOConUnchecked(TPortId) override;

    private:
      static const TEventID scmEventINITID = 0;
      static const TEventID scmEventINITOID = 0;
      static const TEventID scmEventINDID = 1;
      static const int scmBusAdapterOutAdpNum = 0;

      void readInputData(TEventID paEIID) override;
      void writeOutputData(TEventID paEIID) override;
      void setInitialValues() override;

      forte::io::IODeviceController *createDeviceController(CDeviceExecution &paDeviceExecution) override;
      void setConfig() override;
  };

} // namespace forte::eclipse4diac::io::modbus_io
