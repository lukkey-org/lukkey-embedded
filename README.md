# Lukkey Embedded

Embedded software components for the Lukkey hardware wallet.

This repository is the public home for Lukkey's embedded code release. It is intended to support transparent technical review, integration work, and long-term community collaboration around the device firmware stack.

## What Is Included

- `lukkey/src/hal`: hardware abstraction layer modules for board-level services such as GPIO, UART, battery, backlight, motor, and RNG access.
- `lukkey/src/gui`: GUI support code, including the view stack implementation used by the device interface.
- `lukkey/src/utils`: embedded utility modules, including base64, byte stuffing, event queue, MD5, and JSON support.
- `cmake`: cross-compilation and third-party dependency build configuration.
- `third_party`: vendored upstream components used by the embedded graphics and media stack.

## Third-Party Components

The repository includes third-party projects required by the embedded build environment, including LVGL, FreeType, libjpeg-turbo, libdrm, and rlottie. These projects retain their original licenses and notices in their respective directories.

## Build Environment

The included Docker setup provides a reproducible Linux build environment with CMake, compiler tooling, and dependency build tools installed.

```sh
docker compose build
docker compose run --rm build bash
```

For ARM cross-compilation, mount or install the target toolchain and expose it through `TOOLCHAIN_PATH` for Docker Compose or `TOOLCHAIN_ROOT` for the CMake toolchain file.

```sh
export TOOLCHAIN_PATH=/path/to/arm-toolchain
docker compose run --rm build bash
```

The ARM CMake toolchain is configured for `arm-none-linux-gnueabihf` and Cortex-A7/NEON optimization.

## Repository Status

This is the public embedded source repository for Lukkey. The current release focuses on the embedded support modules and third-party build foundation that can be reviewed independently. Additional firmware modules, build instructions, and integration documentation may be added as the public codebase evolves.

## Official Links

- Website: <https://lukkey.com>
- Product page: <https://lukkey.com/lukkey>
- GitHub organization: <https://github.com/lukkey-org>

## License

See the license notices in this repository and in each third-party component directory. Third-party code remains governed by its upstream license terms.
