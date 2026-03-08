# *******************************************************************************
# Copyright (c) 2026 Contributors to the Eclipse Foundation
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
#
# Contributors:
#   Mohammed Adan - initial API and implementation
# *******************************************************************************/

if (NOT LINUX)
    return()
endif ()

option(FORTE_MODULE_MODBUS_IO "Support for Modbus as an IO fieldbus module" OFF)

if (NOT FORTE_MODULE_MODBUS_IO)
    return()
endif ()

SET(FORTE_MODBUS_LIB_DIR "" CACHE STRING "Path to libmodbus library directory")

add_library(forte-modbus_io
            ModbusIOHandle.h
            ModbusIOHandle.cpp
            ModbusIOController.h
            ModbusIOController.cpp
            types/ModbusBusAdapter.h
            types/ModbusBusAdapter.cpp
            types/ModbusMaster.h
            types/ModbusMaster.cpp
            types/ModbusSlaveBase.h
            types/ModbusSlaveBase.cpp
            types/ModbusHoldingRegister.h
            types/ModbusHoldingRegister.cpp
)

target_link_libraries(forte-modbus_io PUBLIC forte-core)

find_library(MODBUS_LIBRARY modbus HINTS ${FORTE_MODBUS_LIB_DIR})
if (MODBUS_LIBRARY)
    target_link_libraries(forte-modbus_io PUBLIC ${MODBUS_LIBRARY})
else ()
    target_link_libraries(forte-modbus_io PUBLIC modbus)
endif ()

find_path(MODBUS_INCLUDE_DIR modbus/modbus.h)
if (MODBUS_INCLUDE_DIR)
    target_include_directories(forte-modbus_io PUBLIC ${MODBUS_INCLUDE_DIR})
endif ()

target_link_libraries(forte PUBLIC $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,forte-modbus_io,$<LINK_LIBRARY:WHOLE_ARCHIVE,forte-modbus_io>>)
install(TARGETS forte-modbus_io EXPORT forte-export FILE_SET HEADERS)
