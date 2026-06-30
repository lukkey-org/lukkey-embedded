#ifndef __BATTERY_H__
#define __BATTERY_H__

#include <stdint.h>
#include <stdbool.h>


extern const uint16_t battery_voltage_table[100+1];

uint8_t chrg_level_read(void);
uint16_t battery_adc_read(void);
#endif