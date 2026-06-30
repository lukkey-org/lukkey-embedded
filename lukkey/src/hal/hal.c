#include "hal.h"

void hal_init(void)
{
    rng_set_seed();
    backlight_init();
    gpio_init();
    motor_init();
    hal_uart_init();
}

void read_nrf_status(void)
{
    hal_uart_printf("%s\r\n", "get_ble_all");
}

void power_off(void)
{
    hal_uart_printf("%s\r\n", "shutdown");    
}
