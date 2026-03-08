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

#include "forte/io/configFB/io_adapter_multi.h"
#include "forte/datatypes/forte_bool.h"
#include "forte/datatypes/forte_uint.h"

namespace forte::eclipse4diac::io::modbus_io {

  class FORTE_ModbusBusAdapter : public forte::io::IOConfigFBMultiAdapter {
      DECLARE_ADAPTER_TYPE(FORTE_ModbusBusAdapter)

    public:
      static const TEventID scmEventINITOID = 0;
      static const TEventID scmEventINITID = 0;

      void setInitialValues() override;

      CIEC_BOOL var_QO;

      CIEC_BOOL var_QI;
      CIEC_UINT var_MasterId;
      CIEC_UINT var_Index;

      TEventID evt_INITO() {
        return getParentAdapterListEventID() + scmEventINITOID;
      }

      TEventID evt_INIT() {
        return getParentAdapterListEventID() + scmEventINITID;
      }

      CIEC_ANY *getDeviceConfigPin(int) override {
        return nullptr;
      }

      ~FORTE_ModbusBusAdapter() override = default;

    protected:
      FORTE_ModbusBusAdapter(CFBContainer &paContainer,
                             const SFBInterfaceSpec &paInterfaceSpec,
                             const forte::StringId paInstanceNameId,
                             TForteUInt8 paParentAdapterlistID);
  };

  class FORTE_ModbusBusAdapter_Plug final : public FORTE_ModbusBusAdapter {
    public:
      FORTE_ModbusBusAdapter_Plug(forte::StringId paInstanceNameId,
                                  CFBContainer &paContainer,
                                  TForteUInt8 paParentAdapterlistID);
      ~FORTE_ModbusBusAdapter_Plug() override = default;

      CEventConnection conn_INITO;

      CDataConnection *conn_QI;
      CDataConnection *conn_MasterId;
      CDataConnection *conn_Index;

      COutDataConnection<CIEC_BOOL> conn_QO;

    private:
      void readInputData(TEventID paEIID) override;
      void writeOutputData(TEventID paEIID) override;
      CIEC_ANY *getDI(size_t) override;
      CIEC_ANY *getDO(size_t) override;
      CEventConnection *getEOConUnchecked(TPortId) override;
      CDataConnection **getDIConUnchecked(TPortId) override;
      CDataConnection *getDOConUnchecked(TPortId) override;
  };

  class FORTE_ModbusBusAdapter_Socket final : public FORTE_ModbusBusAdapter {
    public:
      FORTE_ModbusBusAdapter_Socket(forte::StringId paInstanceNameId,
                                    CFBContainer &paContainer,
                                    TForteUInt8 paParentAdapterlistID);
      ~FORTE_ModbusBusAdapter_Socket() override = default;

      CEventConnection conn_INIT;

      CDataConnection *conn_QO;

      COutDataConnection<CIEC_BOOL> conn_QI;
      COutDataConnection<CIEC_UINT> conn_MasterId;
      COutDataConnection<CIEC_UINT> conn_Index;

    private:
      void readInputData(TEventID paEIID) override;
      void writeOutputData(TEventID paEIID) override;
      CIEC_ANY *getDI(size_t) override;
      CIEC_ANY *getDO(size_t) override;
      CEventConnection *getEOConUnchecked(TPortId) override;
      CDataConnection **getDIConUnchecked(TPortId) override;
      CDataConnection *getDOConUnchecked(TPortId) override;
  };

} // namespace forte::eclipse4diac::io::modbus_io
