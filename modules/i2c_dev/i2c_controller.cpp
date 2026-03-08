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

#include <string>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "forte/util/devlog.h"
#include "forte/io/mapper/io_handle_bit.h"
#include "i2c_controller.h"

namespace forte::eclipse4diac::io::i2c {

  const char *const I2CController::scmFailedToOpenBus = "Failed to open I2C bus device";
  const char *const I2CController::scmFailedToSetSlave = "Failed to set I2C slave address";
  const char *const I2CController::scmFailedToRead = "Failed to read from I2C device";
  const char *const I2CController::scmFailedToWrite = "Failed to write to I2C device";
  const char *const I2CController::scmOK = "OK";

  const char *I2CController::init() {
    auto devPath = "/dev/i2c-" + std::to_string(mConfig.mBusNumber);

    mFd = open(devPath.c_str(), O_RDWR);
    if (mFd < 0) {
      DEVLOG_ERROR("[I2CController::init] Could not open I2C bus %u: %s\n", mConfig.mBusNumber, strerror(errno));
      return scmFailedToOpenBus;
    }

    if (ioctl(mFd, scmSetSlaveId, mConfig.mDeviceAddress) < 0) {
      DEVLOG_ERROR("[I2CController::init] Could not set slave address 0x%02X on bus %u: %s\n",
                   mConfig.mDeviceAddress, mConfig.mBusNumber, strerror(errno));
      close(mFd);
      mFd = -1;
      return scmFailedToSetSlave;
    }

    DEVLOG_INFO("[I2CController::init] I2C bus %u, slave 0x%02X ready\n",
                mConfig.mBusNumber, mConfig.mDeviceAddress);
    return nullptr;
  }

  void I2CController::deInit() {
    if (mFd >= 0) {
      close(mFd);
      mFd = -1;
    }
    DEVLOG_INFO("[I2CController::deInit] I2C bus %u shutdown\n", mConfig.mBusNumber);
  }

  void I2CController::runLoop() {
    while (isAlive() && !hasError()) {
      uint8_t readBuf;
      if (1 == read(mFd, &readBuf, 1)) {
        mDeviceValue = readBuf;
        checkForInputChanges();
      } else {
        DEVLOG_WARNING("[I2CController::runLoop] Read failed on bus %u slave 0x%02X\n",
                       mConfig.mBusNumber, mConfig.mDeviceAddress);
      }
      sleepThread(mConfig.mUpdateInterval);
    }
  }

  void I2CController::handleChangeEvent(forte::io::IOHandle *) {
    if (1 != write(mFd, &mDeviceValue, 1)) {
      DEVLOG_ERROR("[I2CController::handleChangeEvent] Write failed on bus %u slave 0x%02X\n",
                   mConfig.mBusNumber, mConfig.mDeviceAddress);
      mError = scmFailedToWrite;
    }
  }

  ::forte::io::IOHandle *I2CController::createIOHandle(IODeviceController::HandleDescriptor &paHandleDescriptor) {
    auto &desc = static_cast<HandleDescriptor &>(paHandleDescriptor);
    return new forte::io::IOHandleBit(this, desc.mDirection, 0, desc.mBitAddress, &mDeviceValue);
  }

} // namespace forte::eclipse4diac::io::i2c
