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

#include "ModbusSlaveBase.h"

using namespace forte::literals;
using namespace forte::io;

namespace forte::eclipse4diac::io::modbus_io {

  const TForteUInt8 ModbusSlaveBase::scmSlaveConfigurationIO[] = {};
  const TForteUInt8 ModbusSlaveBase::scmSlaveConfigurationIONum = 0;

  ModbusSlaveBase::ModbusSlaveBase(int paType,
                                   CFBContainer &paContainer,
                                   const SFBInterfaceSpec &paInterfaceSpec,
                                   const forte::StringId paInstanceNameId) :
      IOConfigFBMultiSlave(
          scmSlaveConfigurationIO, scmSlaveConfigurationIONum, paType, paContainer, paInterfaceSpec, paInstanceNameId),
      var_BusAdapterOut("BusAdapterOut"_STRID, *this, 0),
      var_BusAdapterIn("BusAdapterIn"_STRID, *this, 0) {
  }

  void ModbusSlaveBase::initModbusHandle(int paDIIndex,
                                         ModbusIOController::EModbusFunction paFunction,
                                         unsigned int paAddress,
                                         CIEC_ANY::EDataTypeID paType,
                                         IOMapper::Direction paDirection) {
    ModbusIOController::ModbusHandleDescriptor desc(
        static_cast<CIEC_STRING *>(getDI(paDIIndex))->getStorage(),
        paDirection, mIndex, paType, paFunction, paAddress);
    initHandle(desc);
  }

  forte::IPlugPin *ModbusSlaveBase::getPlugPinUnchecked(size_t paIndex) {
    return (paIndex == 0) ? &var_BusAdapterOut : nullptr;
  }

  forte::ISocketPin *ModbusSlaveBase::getSocketPinUnchecked(size_t paIndex) {
    return (paIndex == 0) ? &var_BusAdapterIn : nullptr;
  }

} // namespace forte::eclipse4diac::io::modbus_io
