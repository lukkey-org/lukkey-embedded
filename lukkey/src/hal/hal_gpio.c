#include "hal_gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define GPIO_BASE_PATH "/sys/class/gpio"



int is_gpio_exported(const char *gpio_name)
{
    char path[50];
    snprintf(path, sizeof(path), GPIO_BASE_PATH "/%s", gpio_name);
    return access(path, F_OK) == 0;
}

int export_gpio(int gpio_num)
{
    int fd, ret;
    char buf[10];

    fd = open(GPIO_BASE_PATH "/export", O_WRONLY);
    if (fd < 0)
    {
        perror("Error opening export file");
        return -1;
    }

    snprintf(buf, sizeof(buf), "%d", gpio_num);
    ret = write(fd, buf, strlen(buf));
    if (ret < 0)
    {
        perror("Error writing to export file");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int set_gpio_direction(const char *gpio_name, const char *direction)
{
    char path[50];
    int fd, ret;

    snprintf(path, sizeof(path), GPIO_BASE_PATH "/%s/direction", gpio_name);
    fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        perror("Error opening direction file");
        return -1;
    }

    ret = write(fd, direction, strlen(direction));
    if (ret < 0)
    {
        perror("Error writing to direction file");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int set_gpio_value(const char *gpio_name, int value)
{
    char path[50];
    int fd, ret;
    char val = value ? '1' : '0';

    snprintf(path, sizeof(path), GPIO_BASE_PATH "/%s/value", gpio_name);
    fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        perror("Error opening value file");
        return -1;
    }

    ret = write(fd, &val, 1);
    if (ret < 0)
    {
        perror("Error writing to value file");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int read_gpio_value(const char *gpio_name)
{
    char path[50];
    int fd, ret;
    char val;

    snprintf(path, sizeof(path), GPIO_BASE_PATH "/%s/value", gpio_name);
    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        perror("Error opening value file");
        return -1;
    }

    ret = read(fd, &val, 1);
    if (ret < 0)
    {
        perror("Error reading value file");
        close(fd);
        return -1;
    }

    close(fd);
    return (val == '1') ? 1 : 0;
}

void gpio_init(void)
{
    if (!is_gpio_exported(PG4_NAME)) {
        printf("PG4 not exported, exporting now...\n");
        if (export_gpio(PG4_NUM) < 0) {
            printf("Failed to export %s\n", PG4_NAME);
        }
    }
    set_gpio_direction(PG4_NAME, "in");

    // if (!is_gpio_exported(PD5_NAME)) {
    //     printf("PD5 not exported, exporting now...\n");
    //     if (export_gpio(PD5_NUM) < 0) {
    //         printf("Failed to export %s\n", PD5_NAME);
    //     }
    // }
    // set_gpio_direction(PD5_NAME, "in");
    // set_gpio_value(PD5_NAME, 0);
}






