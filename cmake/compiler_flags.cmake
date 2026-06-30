# Build optimization and reverse-engineering hardening

# ============================================
# Debug build
# ============================================
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DDEBUG" CACHE STRING "" FORCE)

# ============================================
# Release build: optimization and reverse-engineering hardening
# ============================================
set(CMAKE_C_FLAGS_RELEASE "-O3 -g0 -DNDEBUG" CACHE STRING "" FORCE)

# Common warning options, kept aligned with the original Makefile.
add_compile_options(
    -Wall
    -Wshadow
    -Wundef
    -Wmissing-prototypes
    -Wno-discarded-qualifiers
    -Wextra
    -Wno-unused-function
    -Wno-error=strict-prototypes
    -Wpointer-arith
    -fno-strict-aliasing
    -Wno-error=cpp
    -Wuninitialized
    -Wmaybe-uninitialized
    -Wno-unused-parameter
    -Wno-missing-field-initializers
    -Wtype-limits
    -Wsizeof-pointer-memaccess
    -Wno-format-nonliteral
    -Wno-cast-qual
    -Wunreachable-code
    -Wno-switch-default
    -Wreturn-type
    -Wmultichar
    -Wformat-security
    -Wno-ignored-qualifiers
    -Wno-error=pedantic
    -Wno-sign-compare
    -Wno-error=missing-prototypes
    -Wdouble-promotion
    -Wclobbered
    -Wdeprecated
    -Wempty-body
    -Wshift-negative-value
#   -Wstack-usage=4096
    -Wno-unused-value
)

# Size optimization for all build types.
add_compile_options(
    -ffunction-sections
    -fdata-sections
)

# Reverse-engineering hardening for Release builds only.
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(
        -fvisibility=hidden
        -fno-ident
        -fomit-frame-pointer
        -fno-asynchronous-unwind-tables
    )
    
    add_link_options(
        -Wl,--gc-sections
        -Wl,--strip-all
        -Wl,--build-id=none
        -Wl,--hash-style=sysv
        -Wl,-z,relro
        -Wl,-z,now
    )
else()
    # Debug builds also need gc-sections.
    add_link_options(
        -Wl,--gc-sections
    )
endif()
