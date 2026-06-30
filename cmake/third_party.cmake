# Shared third-party library build configuration.
# Use ExternalProject_Add to cross-compile dependencies into the sysroot.

include(ExternalProject)

set(SYSROOT ${CMAKE_BINARY_DIR}/sysroot)
set(SYSROOT_LIB ${SYSROOT}/lib)
set(SYSROOT_INCLUDE ${SYSROOT}/include)
file(MAKE_DIRECTORY ${SYSROOT_LIB} ${SYSROOT_INCLUDE})

set(TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain-arm.cmake)
set(NPROC 4)

execute_process(
    COMMAND nproc
    OUTPUT_VARIABLE NPROC
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# ============================================================================
# freetype 2.13.3
# ============================================================================
ExternalProject_Add(ext_freetype
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/freetype
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${SYSROOT}
        -DCMAKE_BUILD_TYPE=Release
        -DBUILD_SHARED_LIBS=ON
        -DFT_DISABLE_BZIP2=ON
        -DFT_DISABLE_BROTLI=ON
        -DFT_DISABLE_HARFBUZZ=ON
        -DFT_DISABLE_PNG=ON
        -DFT_DISABLE_ZLIB=OFF
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> -j${NPROC}
    INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR>
    BUILD_BYPRODUCTS ${SYSROOT_LIB}/libfreetype.so
)

# ============================================================================
# libjpeg-turbo 3.1.3
# ============================================================================
ExternalProject_Add(ext_jpeg
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/libjpeg-turbo
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${SYSROOT}
        -DCMAKE_BUILD_TYPE=Release
        -DENABLE_SHARED=ON
        -DENABLE_STATIC=ON
        -DWITH_JAVA=OFF
        -DWITH_TESTS=OFF
        -DWITH_TOOLS=OFF
        -DWITH_TURBOJPEG=ON
        -DWITH_SIMD=ON
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> -j${NPROC}
    INSTALL_COMMAND
        ${CMAKE_COMMAND} --install <BINARY_DIR> &&
        ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/third_party/libjpeg-turbo/src/jpegint.h
            ${SYSROOT_INCLUDE}/jpegint.h
    BUILD_BYPRODUCTS ${SYSROOT_LIB}/libjpeg.so ${SYSROOT_LIB}/libturbojpeg.so
)

# ============================================================================
# libdrm 2.4.89 (autotools)
# ============================================================================
ExternalProject_Add(ext_drm
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/libdrm
    CONFIGURE_COMMAND
        cd <SOURCE_DIR> && autoreconf -fi &&
        <SOURCE_DIR>/configure
            --host=arm-none-linux-gnueabihf
            --prefix=${SYSROOT}
            --enable-shared
            --disable-static
            --disable-intel
            --disable-radeon
            --disable-amdgpu
            --disable-nouveau
            --disable-vmwgfx
            --disable-freedreno
            --disable-vc4
            --disable-cairo-tests
            --disable-manpages
            --disable-valgrind
    BUILD_COMMAND make -j${NPROC}
    INSTALL_COMMAND make install
    BUILD_IN_SOURCE 1
    BUILD_BYPRODUCTS ${SYSROOT_LIB}/libdrm.so ${SYSROOT_LIB}/libkms.so
)

# ============================================================================
# rlottie 0.2
# ============================================================================
ExternalProject_Add(ext_rlottie
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/rlottie
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${SYSROOT}
        -DCMAKE_BUILD_TYPE=Release
        -DBUILD_SHARED_LIBS=ON
        -DLIB_INSTALL_DIR=lib
        -DARCH=arm
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> -j${NPROC}
    INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR>
    BUILD_BYPRODUCTS ${SYSROOT_LIB}/librlottie.so
)

# ============================================================================
# LVGL 9.4.0
# ============================================================================
ExternalProject_Add(ext_lvgl
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/lvgl
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${SYSROOT}
        -DCMAKE_BUILD_TYPE=Release
        -DBUILD_SHARED_LIBS=ON
        -DLV_BUILD_SET_CONFIG_OPTS=ON
        -DLV_BUILD_CONF_DIR=${CMAKE_SOURCE_DIR}
        -DCONFIG_LV_BUILD_DEMOS=OFF
        -DCONFIG_LV_BUILD_EXAMPLES=OFF
        -DCMAKE_C_FLAGS=-mfpu=neon\ -march=armv7-a\ -mtune=cortex-a7\ -ffast-math\ -O3\ -I${SYSROOT_INCLUDE}\ -I${SYSROOT_INCLUDE}/freetype2\ -I${SYSROOT_INCLUDE}/libdrm
        -DCMAKE_CXX_FLAGS=-mfpu=neon\ -march=armv7-a\ -mtune=cortex-a7\ -ffast-math\ -O3\ -I${SYSROOT_INCLUDE}\ -I${SYSROOT_INCLUDE}/freetype2\ -I${SYSROOT_INCLUDE}/libdrm
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> -j${NPROC}
    INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR>
    DEPENDS ext_freetype ext_drm ext_jpeg ext_rlottie
    BUILD_BYPRODUCTS ${SYSROOT_LIB}/liblvgl.so ${SYSROOT_LIB}/liblvgl_thorvg.so
)
