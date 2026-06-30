#include "hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define URANDOM_DEVICE "/dev/urandom"

/** Seed libc rand() from /dev/urandom (stir loop count from one byte). */
void rng_set_seed(void)
{
    uint8_t random_byte;
    uint8_t count;
    uint32_t seed;

    random_byte = rng_get_random_byte();
    count = random_byte % 10;
    for (int i = 0; i < count; i++) {
        seed = rng_get_random_word();
    }
    srand(seed);
}

/** One byte from /dev/urandom; 0 if open/read fails. */
uint8_t rng_get_random_byte(void)
{
    uint8_t random_byte = 0;
    int fd;

    fd = open(URANDOM_DEVICE, O_RDONLY);
    if (fd < 0) {
        return 0;
    }

    if (read(fd, &random_byte, 1) != 1) {
        random_byte = 0;
    }

    close(fd);

    return random_byte;
}

/** 16-bit value from /dev/urandom; 0 if I/O fails. */
uint16_t rng_get_random_halfword(void)
{
    uint16_t random_halfword = 0;
    int fd;

    fd = open(URANDOM_DEVICE, O_RDONLY);
    if (fd < 0) {
        return 0;
    }

    if (read(fd, &random_halfword, sizeof(uint16_t)) != sizeof(uint16_t)) {
        random_halfword = 0;
    }

    close(fd);

    return random_halfword;
}

/** 32-bit value from /dev/urandom; 0 if I/O fails. */
uint32_t rng_get_random_word(void)
{
    uint32_t random_word = 0;
    int fd;

    fd = open(URANDOM_DEVICE, O_RDONLY);
    if (fd < 0) {
        return 0;
    }

    if (read(fd, &random_word, sizeof(uint32_t)) != sizeof(uint32_t)) {
        random_word = 0;
    }

    close(fd);

    return random_word;
}

/** Fill @p buffer with @p size bytes from /dev/urandom; zero on failure. */
void rng_get_random_bytes(uint8_t *buffer, size_t size)
{
    if (buffer == NULL || size == 0) {
        return;
    }

    int fd;
    ssize_t bytes_read;
    size_t total_read = 0;

    fd = open(URANDOM_DEVICE, O_RDONLY);
    if (fd < 0) {
        memset(buffer, 0, size);
        return;
    }

    while (total_read < size) {
        bytes_read = read(fd, buffer + total_read, size - total_read);
        if (bytes_read <= 0) {
            memset(buffer + total_read, 0, size - total_read);
            break;
        }
        total_read += bytes_read;
    }

    close(fd);
}
