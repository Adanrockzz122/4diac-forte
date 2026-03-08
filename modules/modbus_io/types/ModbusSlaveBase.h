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

#include "forte/io/configFB/io_slave_multi.h"
#include "ModbusBusAdapter.h"
#include "../ModbusIOController.h"

namespace forte::eclipse4diac::io::modbus_io {

  /*! @brief Base class for Modbus slave function blocks
   *
   * Provides common functionality for all Modbus slave type FBs.
   * Each slave FB represents a Modbus slave device on the bus,
   * addressed by its unit ID (slave index in the adapter chain).
   */
  class ModbusSlaveBase : public forte::io::IOConfigFBMultiSlave {
    public:
      ModbusSlaveBase(int paType,
                      CFBContainer &paContainer,
                      const SFBInterfaceSpec &paInterfaceSpec,
                      const forte::StringId paInstanceNameId);
      ~ModbusSlaveBase() override = default;

      forte::CPlugPin<FORTE_ModbusBusAdapter_Plug> var_BusAdapterOut;
      forte::CSocketPin<FORTE_ModbusBusAdapter_Socket> var_BusAdapterIn;

    protected:
      void initModbusHandle(int paDIIndex,
                            ModbusIOController::EModbusFunction paFunction,
                            unsigned int paAddress,
                            CIEC_ANY::EDataTypeID paType,
                            forte::io::IOMapper::Direction paDirection);

      void initHandles() override = 0;

      forte::IPlugPin *getPlugPinUnchecked(size_t) override;
      forte::ISocketPin *getSocketPinUnchecked(size_t) override;

    private:
      static const TForteUInt8 scmSlaveConfigurationIO[];
      static const TForteUInt8 scmSlaveConfigurationIONum;
  };

} // namespace forte::eclipse4diac::io::modbus_io
