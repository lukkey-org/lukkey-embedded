# Hardware abstraction layer module.

add_library(lukkey_hal STATIC
    hal.c
    hal_gpio.c
    hal_uart.c
    rng.c
    motor.c
    backlight.c
    battery.c
)

target_include_directories(lukkey_hal
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
