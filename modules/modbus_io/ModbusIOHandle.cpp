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

#include "ModbusIOHandle.h"

namespace forte::eclipse4diac::io::modbus_io {

  ModbusIOHandle::ModbusIOHandle(ModbusIOController *paController,
                                 forte::io::IOMapper::Direction paDirection,
                                 CIEC_ANY::EDataTypeID paType,
                                 unsigned int paAddress) :
      IOHandle(paController, paDirection, paType),
      mAddress(paAddress),
      mLastValue(0),
      mCurrentValue(0) {
  }

  ModbusIOHandle::~ModbusIOHandle() = default;

  void ModbusIOHandle::set(const CIEC_ANY &paValue) {
    switch (mType) {
      case CIEC_ANY::e_BOOL:
        mCurrentValue = static_cast<const CIEC_BOOL &>(paValue) ? 1 : 0;
        break;
      case CIEC_ANY::e_WORD:
        mCurrentValue = static_cast<const CIEC_WORD &>(paValue);
        break;
      default: break;
    }
  }

  void ModbusIOHandle::get(CIEC_ANY &paValue) {
    switch (mType) {
      case CIEC_ANY::e_BOOL:
        static_cast<CIEC_BOOL &>(paValue) = CIEC_BOOL(mCurrentValue != 0);
        break;
      case CIEC_ANY::e_WORD:
        static_cast<CIEC_WORD &>(paValue) = CIEC_WORD(mCurrentValue);
        break;
      default: break;
    }
  }

  bool ModbusIOHandle::check() {
    if (mCurrentValue != mLastValue) {
      mLastValue = mCurrentValue;
      return true;
    }
    return false;
  }

  void ModbusIOHandle::dropObserver() {
    IOHandle::dropObserver();
    mCurrentValue = 0;
    mLastValue = 0;
  }

} // namespace forte::eclipse4diac::io::modbus_io
