#include "hal.h"
#include "hal_gpio.h"
#include <stdio.h>
#include <stdbool.h>

static bool motor_init_done = false;

bool motor_init(void)
{
    if (!is_gpio_exported(PH4_NAME))
    {
        printf("PH4 not exported, exporting now...\n");
        if (export_gpio(PH4_NUM) < 0)
        {
            printf("Failed to export %s\n", PH4_NAME);
            return motor_init_done;
        }
        set_gpio_direction(PH4_NAME, "out");
        set_gpio_value(PH4_NAME, 0);
    }

    if (!is_gpio_exported(PH5_NAME))
    {
        printf("PH5 not exported, exporting now...\n");
        if (export_gpio(PH5_NUM) < 0)
        {
            printf("Failed to export %s\n", PH5_NAME);
            return motor_init_done;
        }
        set_gpio_direction(PH5_NAME, "out");
        set_gpio_value(PH5_NAME, 0);
    }

    set_gpio_value(PH4_NAME, 0);
    set_gpio_value(PH5_NAME, 0);
    motor_init_done = true;
    return motor_init_done;
}

void motor_on(void)
{
    if (!motor_init_done)
    {
        printf("Motor not initialized\n");
        return;
    }

    set_gpio_value(PH5_NAME, 1);
}

void motor_off(void)
{
    if (!motor_init_done)
    {
        printf("Motor not initialized\n");
        return;
    }

    set_gpio_value(PH5_NAME, 0);
}

