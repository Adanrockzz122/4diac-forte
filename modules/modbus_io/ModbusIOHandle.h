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

#include "forte/datatypes/forte_word.h"
#include "forte/datatypes/forte_bool.h"
#include "forte/io/mapper/io_handle.h"
#include "ModbusIOController.h"

namespace forte::eclipse4diac::io::modbus_io {

  class ModbusIOHandle : public forte::io::IOHandle {
    public:
      ModbusIOHandle(ModbusIOController *paController,
                     forte::io::IOMapper::Direction paDirection,
                     CIEC_ANY::EDataTypeID paType,
                     unsigned int paAddress);

      ~ModbusIOHandle() override;

      void set(const CIEC_ANY &paValue) override;
      void get(CIEC_ANY &paValue) override;

      bool check();

      unsigned int getAddress() const {
        return mAddress;
      }

    private:
      unsigned int mAddress;
      TForteWord mLastValue;
      TForteWord mCurrentValue;

      void dropObserver() override;
  };

} // namespace forte::eclipse4diac::io::modbus_io
