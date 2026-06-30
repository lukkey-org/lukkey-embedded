#include "hal.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define BACKLIGHT_PATH "/sys/class/backlight/backlight/brightness"
#define BACKLIGHT_DEFAULT 128

static int fd = -1;
static uint8_t brightness = BACKLIGHT_DEFAULT;
static uint8_t last_written = 0;
static bool initialized = false;

static int write_brightness(uint8_t value)
{
    if (fd < 0) return -1;
    if (initialized && value == last_written) return 0;

    char buf[8];
    int len = snprintf(buf, sizeof(buf), "%d\n", value);
    if (pwrite(fd, buf, len, 0) != len) {
        close(fd);
        fd = -1;
        initialized = false;
        return -1;
    }
    last_written = value;
    initialized = true;
    return 0;
}

bool backlight_init(void)
{
    if (fd >= 0) return true;

    fd = open(BACKLIGHT_PATH, O_WRONLY);
    if (fd < 0) {
        perror("backlight open failed");
        return false;
    }
    initialized = false;
    write_brightness(brightness);
    return true;
}

void backlight_deinit(void)
{
    if (fd < 0) return;
    close(fd);
    fd = -1;
    initialized = false;
}

void backlight_on(void)
{
    write_brightness(brightness);
}

void backlight_off(void)
{
    write_brightness(0);
}

void backlight_set_brightness(uint8_t value)
{
    brightness = value;
    write_brightness(value);
}

uint8_t backlight_get_brightness(void)
{
    return brightness;
}

uint8_t backlight_percent_to_level(uint8_t percent)
{
    if (percent >= 100)
        return 255;
    uint32_t scaled = 255U * percent + 50U;
    uint8_t level = (uint8_t)(scaled / 100U);
    if (level == 0 && percent > 0)
        level = 1;
    return level;
}

void backlight_set_brightness_percent(uint8_t percent)
{
    backlight_set_brightness(backlight_percent_to_level(percent));
}
