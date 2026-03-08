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

#pragma once

#include "forte/funcbloc.h"
#include "forte/datatypes/forte_bool.h"
#include "forte/datatypes/forte_uint.h"
#include "forte/datatypes/forte_wstring.h"
#include "forte/io/configFB/io_configFB_controller.h"

namespace forte::eclipse4diac::io::i2c {

  class FORTE_I2CDev : public forte::io::IOConfigFBController {
      DECLARE_FIRMWARE_FB(FORTE_I2CDev)

    private:
      static const TEventID scmEventINITOID = 0;
      static const TEventID scmEventINDID = 1;
      static const TEventID scmEventINITID = 0;

      void readInputData(TEventID paEIID) override;
      void writeOutputData(TEventID paEIID) override;
      void setInitialValues() override;

    public:
      FORTE_I2CDev(const forte::StringId paInstanceNameId, CFBContainer &paContainer);

      CIEC_BOOL var_QI;
      CIEC_WSTRING var_VALUE;
      CIEC_UINT var_BusNumber;
      CIEC_UINT var_DeviceAddress;
      CIEC_UINT var_BitAddress;
      CIEC_UINT var_ReadWriteMode;

      CIEC_BOOL var_QO;
      CIEC_WSTRING var_STATUS;

      CEventConnection conn_INITO;
      CEventConnection conn_IND;

      CDataConnection *conn_QI;
      CDataConnection *conn_VALUE;
      CDataConnection *conn_BusNumber;
      CDataConnection *conn_DeviceAddress;
      CDataConnection *conn_BitAddress;
      CDataConnection *conn_ReadWriteMode;

      COutDataConnection<CIEC_BOOL> conn_QO;
      COutDataConnection<CIEC_WSTRING> conn_STATUS;

      CIEC_ANY *getDI(size_t) override;
      CIEC_ANY *getDO(size_t) override;
      CEventConnection *getEOConUnchecked(TPortId) override;
      CDataConnection **getDIConUnchecked(TPortId) override;
      CDataConnection *getDOConUnchecked(TPortId) override;

    protected:
      forte::io::IODeviceController *createDeviceController(CDeviceExecution &paDeviceExecution) override;
      void setConfig() override;
      void onStartup(CEventChainExecutionThread *const paECET) override;
  };

} // namespace forte::eclipse4diac::io::i2c
