/*******************************************************************************
 * Copyright (c) 2015, 2016, 2025 fortiss GmbH
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *    Gerd Kainz, Alois Zoitl - initial API and implementation and/or initial documentation
 *    Mohammed Adan - refactored to IO layer architecture
 *******************************************************************************/

#pragma once

#include "forte/io/device/io_controller.h"

namespace forte::eclipse4diac::io::i2c {

  class I2CController : public forte::io::IODeviceController {
    public:
      explicit I2CController(CDeviceExecution &paDeviceExecution) : IODeviceController(paDeviceExecution) {}

      struct Config : IODeviceController::Config {
          unsigned int mBusNumber = 0;
          unsigned int mDeviceAddress = 0;
          unsigned int mUpdateInterval = 25; // ms
      };

      struct HandleDescriptor : IODeviceController::HandleDescriptor {
          unsigned int mBitAddress;

          HandleDescriptor(std::string const &paId, forte::io::IOMapper::Direction paDirection,
                           unsigned int paBitAddress) :
              IODeviceController::HandleDescriptor(paId, paDirection),
              mBitAddress(paBitAddress) {
          }
      };

      void setConfig(struct IODeviceController::Config *paConfig) override {
        mConfig = *static_cast<Config *>(paConfig);
      }

      bool isHandleValueEqual(forte::io::IOHandle &) override { return false; }

      void handleChangeEvent(forte::io::IOHandle *paHandle) override;
      ::forte::io::IOHandle *createIOHandle(IODeviceController::HandleDescriptor &paHandleDescriptor) override;

    protected:
      const char *init() override;
      void deInit() override;
      void runLoop() override;

    private:
      static const int scmSetSlaveId = 0x0703;

      int mFd = -1;
      uint8_t mDeviceValue = 0;
      Config mConfig;

      static const char *const scmFailedToOpenBus;
      static const char *const scmFailedToSetSlave;
      static const char *const scmFailedToRead;
      static const char *const scmFailedToWrite;
      static const char *const scmOK;
  };

} // namespace forte::eclipse4diac::io::i2c
