#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__

#include <stdint.h>

//gpio macro
// #define PA15_NUM 15
// #define PA15_NAME "PA15"

#define PB7_NUM 23
#define PB7_NAME "PB7"

#define PD5_NUM 53
#define PD5_NAME "PD5"

#define PH4_NUM 116
#define PH4_NAME "PH4"

#define PH5_NUM 117
#define PH5_NAME "PH5"

#define PG4_NUM 100
#define PG4_NAME "PG4"

//gpio function
int is_gpio_exported(const char *gpio_name);
int export_gpio(int gpio_num);
int set_gpio_direction(const char *gpio_name, const char *direction);
int set_gpio_value(const char *gpio_name, int value);
int read_gpio_value(const char *gpio_name);
void gpio_init(void);
#endif