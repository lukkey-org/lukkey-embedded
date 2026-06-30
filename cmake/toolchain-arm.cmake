# ARM cross-compilation toolchain configuration (STM32MP157).
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm.cmake ..

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Toolchain prefix. TOOLCHAIN_ROOT can override the path.
if(DEFINED ENV{TOOLCHAIN_ROOT})
    set(TOOLCHAIN_PREFIX "$ENV{TOOLCHAIN_ROOT}/arm-none-linux-gnueabihf-")
else()
    set(TOOLCHAIN_PREFIX "arm-none-linux-gnueabihf-")
endif()

set(CMAKE_C_COMPILER   "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}g++")
set(CMAKE_AR           "${TOOLCHAIN_PREFIX}ar")
set(CMAKE_RANLIB       "${TOOLCHAIN_PREFIX}ranlib")
set(CMAKE_STRIP        "${TOOLCHAIN_PREFIX}strip")
set(CMAKE_OBJCOPY      "${TOOLCHAIN_PREFIX}objcopy")
set(CMAKE_OBJDUMP      "${TOOLCHAIN_PREFIX}objdump")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# NEON + Cortex-A7 optimization.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon -march=armv7-a -mtune=cortex-a7 -ffast-math")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon -march=armv7-a -mtune=cortex-a7 -ffast-math")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mfpu=neon -march=armv7-a -mtune=cortex-a7")
