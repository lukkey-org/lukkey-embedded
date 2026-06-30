#ifndef __HAL_H__
#define __HAL_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#include "hal_gpio.h"


/*=============================================================================
 * HAL — single include for hardware modules
 *============================================================================*/

 /*-----------------------------------------------------------------------------
 * UART
 *----------------------------------------------------------------------------*/
#include "hal_uart.h"

/*-----------------------------------------------------------------------------
 * HAL core
 *----------------------------------------------------------------------------*/
void hal_init(void);
void read_nrf_status(void);
void power_off(void);

/*-----------------------------------------------------------------------------
 * RNG
 *----------------------------------------------------------------------------*/
void rng_set_seed(void);
uint8_t rng_get_random_byte(void);
uint16_t rng_get_random_halfword(void);
uint32_t rng_get_random_word(void);
void rng_get_random_bytes(uint8_t *buffer, size_t size);

/*-----------------------------------------------------------------------------
 * Battery
 *----------------------------------------------------------------------------*/
#include "battery.h"

/*-----------------------------------------------------------------------------
 * Motor
 *----------------------------------------------------------------------------*/
bool motor_init(void);
void motor_on(void);
void motor_off(void);

/*-----------------------------------------------------------------------------
 * Backlight
 *----------------------------------------------------------------------------*/
bool backlight_init(void);
void backlight_deinit(void);
void backlight_on(void);
void backlight_off(void);
void backlight_set_brightness(uint8_t value);  /* 0–255 */
uint8_t backlight_get_brightness(void);
uint8_t backlight_percent_to_level(uint8_t percent);
void backlight_set_brightness_percent(uint8_t percent);



#endif // __HAL_H__
