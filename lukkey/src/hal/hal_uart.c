#include "hal_uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/select.h>
#include <stdarg.h>
#include <pthread.h>

/* Fallback baud constants when libc headers lack them */
#ifndef B460800
#define B460800 0010004
#endif

#ifndef B921600
#define B921600 0010007
#endif

#ifndef B1000000
#define B1000000 0010010
#endif

/*=============================================================================
 * Internal types
 *============================================================================*/

/** Ring buffer for RX stream */
typedef struct {
    uint8_t buffer[HAL_UART_BUFFER_SIZE * 2];
    size_t write_index;
    size_t read_index;
    size_t size;
    pthread_mutex_t mutex;
} ring_buffer_t;

typedef struct {
    int fd;
    bool initialized;

    pthread_t rx_thread;
    volatile bool rx_thread_running;
    hal_uart_rx_callback_t rx_callback;

    uint8_t tx_buffer[HAL_UART_BUFFER_SIZE];
    ring_buffer_t rx_ring_buffer;

    pthread_mutex_t tx_mutex;
} hal_uart_context_t;

static hal_uart_context_t g_uart_ctx = {
    .fd = -1,
    .initialized = false,
    .rx_thread_running = false,
    .rx_callback = NULL
};

static struct timeval g_uart_start_time = {0};

/*=============================================================================
 * Forward declarations
 *============================================================================*/
static hal_uart_error_t setup_serial_port(int fd);
static void* uart_rx_thread_func(void *arg);
static void trim_trailing_crlf(char *str);

static void ring_buffer_init(ring_buffer_t *rb);
static void ring_buffer_deinit(ring_buffer_t *rb);
static size_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, size_t len);
static size_t ring_buffer_read_packet(ring_buffer_t *rb, uint8_t *packet, size_t max_len);
static void ring_buffer_process_packets(void);

/*=============================================================================
 * Timestamps
 *============================================================================*/

/** Monotonic ms since first call (UART debug timestamps). */
long uart_get_timestamp_ms(void)
{
    struct timeval current_tv;
    gettimeofday(&current_tv, NULL);

    if (g_uart_start_time.tv_sec == 0) {
        g_uart_start_time = current_tv;
    }

    long ms = (current_tv.tv_sec - g_uart_start_time.tv_sec) * 1000 +
              (current_tv.tv_usec - g_uart_start_time.tv_usec) / 1000;
    
    return ms;
}

/*=============================================================================
 * Public API
 *============================================================================*/

hal_uart_error_t hal_uart_init(void)
{
    if (g_uart_ctx.initialized) {
        return HAL_UART_OK;
    }

    g_uart_ctx.fd = open(HAL_UART_DEFAULT_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
    if (g_uart_ctx.fd == -1) {
        UART_PRINT("Failed to open device: %s\n", HAL_UART_DEFAULT_DEVICE);
        return HAL_UART_ERROR;
    }

    if (setup_serial_port(g_uart_ctx.fd) != HAL_UART_OK) {
        close(g_uart_ctx.fd);
        g_uart_ctx.fd = -1;
        return HAL_UART_ERROR;
    }

    pthread_mutex_init(&g_uart_ctx.tx_mutex, NULL);

    ring_buffer_init(&g_uart_ctx.rx_ring_buffer);
    
    g_uart_ctx.initialized = true;
    
    UART_PRINT("initialized: %s @ %d baud\n", 
               HAL_UART_DEFAULT_DEVICE, HAL_UART_DEFAULT_BAUDRATE);
    
    return HAL_UART_OK;
}

hal_uart_error_t hal_uart_deinit(void)
{
    if (!g_uart_ctx.initialized) {
        return HAL_UART_ERROR_NOT_INIT;
    }

    hal_uart_stop_rx();

    if (g_uart_ctx.fd != -1) {
        close(g_uart_ctx.fd);
        g_uart_ctx.fd = -1;
    }

    pthread_mutex_destroy(&g_uart_ctx.tx_mutex);

    ring_buffer_deinit(&g_uart_ctx.rx_ring_buffer);

    g_uart_ctx.initialized = false;
    g_uart_ctx.rx_callback = NULL;
    
    UART_PRINT("deinitialized\n");
    
    return HAL_UART_OK;
}

hal_uart_error_t hal_uart_send(const uint8_t *data, size_t len)
{
    if (data == NULL || len == 0) {
        return HAL_UART_ERROR_PARAM;
    }
    
    if (!g_uart_ctx.initialized) {
        return HAL_UART_ERROR_NOT_INIT;
    }
    
    pthread_mutex_lock(&g_uart_ctx.tx_mutex);
    
    ssize_t bytes_written = write(g_uart_ctx.fd, data, len);
    if (bytes_written != (ssize_t)len) {
        pthread_mutex_unlock(&g_uart_ctx.tx_mutex);
        return HAL_UART_ERROR;
    }
    
    pthread_mutex_unlock(&g_uart_ctx.tx_mutex);

    char temp_buffer[HAL_UART_BUFFER_SIZE];
    size_t copy_len = (len < HAL_UART_BUFFER_SIZE - 1) ? len : (HAL_UART_BUFFER_SIZE - 1);
    memcpy(temp_buffer, data, copy_len);
    temp_buffer[copy_len] = '\0';
    
    UART_PRINT("[Send] %s\r\n", temp_buffer);
    
    return HAL_UART_OK;
}

hal_uart_error_t hal_uart_printf(const char *format, ...)
{
    if (format == NULL) {
        return HAL_UART_ERROR_PARAM;
    }
    
    if (!g_uart_ctx.initialized) {
        return HAL_UART_ERROR_NOT_INIT;
    }
    
    va_list args;
    va_start(args, format);
    
    int result = vsnprintf((char*)g_uart_ctx.tx_buffer, HAL_UART_BUFFER_SIZE, format, args);
    va_end(args);
    
    if (result < 0) {
        return HAL_UART_ERROR_PARAM;
    }
    
    if (result >= HAL_UART_BUFFER_SIZE) {
        UART_PRINT("Warning: printf data truncated (needed %d, buffer %d)\n", 
                   result, HAL_UART_BUFFER_SIZE);
    }
    
    size_t len = (result < HAL_UART_BUFFER_SIZE) ? result : (HAL_UART_BUFFER_SIZE - 1);
    
    return hal_uart_send(g_uart_ctx.tx_buffer, len);
}

hal_uart_error_t hal_uart_set_rx_callback(hal_uart_rx_callback_t callback)
{
    g_uart_ctx.rx_callback = callback;
    return HAL_UART_OK;
}

hal_uart_error_t hal_uart_start_rx(void)
{
    if (!g_uart_ctx.initialized) {
        return HAL_UART_ERROR_NOT_INIT;
    }
    
    if (g_uart_ctx.rx_thread_running) {
        return HAL_UART_OK;
    }
    
    g_uart_ctx.rx_thread_running = true;
    
    if (pthread_create(&g_uart_ctx.rx_thread, NULL, uart_rx_thread_func, NULL) != 0) {
        g_uart_ctx.rx_thread_running = false;
        return HAL_UART_ERROR;
    }
    
    UART_PRINT("async RX started\n");
    
    return HAL_UART_OK;
}

hal_uart_error_t hal_uart_stop_rx(void)
{
    if (!g_uart_ctx.rx_thread_running) {
        return HAL_UART_OK;
    }

    g_uart_ctx.rx_thread_running = false;

    pthread_join(g_uart_ctx.rx_thread, NULL);
    
    UART_PRINT("async RX stopped\n");
    
    return HAL_UART_OK;
}

bool hal_uart_is_ready(void)
{
    return g_uart_ctx.initialized;
}

hal_uart_error_t hal_uart_clear_rx_buffer(void)
{
    if (!g_uart_ctx.initialized) {
        return HAL_UART_ERROR_NOT_INIT;
    }
    
    ring_buffer_t *rb = &g_uart_ctx.rx_ring_buffer;
    pthread_mutex_lock(&rb->mutex);
    rb->read_index = rb->write_index;
    pthread_mutex_unlock(&rb->mutex);
    
    UART_PRINT("RX buffer cleared\n");
    
    return HAL_UART_OK;
}

/*=============================================================================
 * Internals
 *============================================================================*/

/** termios from HAL_UART_DEFAULT_BAUDRATE and 8N1 raw */
static hal_uart_error_t setup_serial_port(int fd)
{
    struct termios options;

    if (tcgetattr(fd, &options) < 0) {
        perror("tcgetattr");
        return HAL_UART_ERROR;
    }

    speed_t baud;
    switch (HAL_UART_DEFAULT_BAUDRATE) {
        case 9600:    baud = B9600; break;
        case 19200:   baud = B19200; break;
        case 38400:   baud = B38400; break;
        case 57600:   baud = B57600; break;
        case 115200:  baud = B115200; break;
        case 230400:  baud = B230400; break;
        case 460800:  baud = B460800; break;
        case 921600:  baud = B921600; break;
        case 1000000: baud = B1000000; break;
        default:
            UART_PRINT("Warning: Unsupported baudrate %d, fallback to 115200\n", HAL_UART_DEFAULT_BAUDRATE);
            baud = B115200;
            break;
    }
    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    options.c_cflag |= CLOCAL | CREAD;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;

    /* No CR/LF input translation — keep 0x0D 0x0A on the wire */
    options.c_iflag &= ~(ICRNL | INLCR | IGNCR);

    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 5; /* 0.5 s unit */

    if (tcsetattr(fd, TCSANOW, &options) < 0) {
        perror("tcsetattr");
        return HAL_UART_ERROR;
    }
    
    return HAL_UART_OK;
}

/*=============================================================================
 * Ring buffer
 *============================================================================*/

static void ring_buffer_init(ring_buffer_t *rb)
{
    rb->write_index = 0;
    rb->read_index = 0;
    rb->size = sizeof(rb->buffer);
    pthread_mutex_init(&rb->mutex, NULL);
}

static void ring_buffer_deinit(ring_buffer_t *rb)
{
    pthread_mutex_destroy(&rb->mutex);
}

static size_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, size_t len)
{
    pthread_mutex_lock(&rb->mutex);
    
    size_t written = 0;
    for (size_t i = 0; i < len; i++) {
        size_t next_write = (rb->write_index + 1) % rb->size;

        if (next_write == rb->read_index) {
            UART_PRINT("Ring buffer full, dropping data\n");
            break;
        }
        
        rb->buffer[rb->write_index] = data[i];
        rb->write_index = next_write;
        written++;
    }
    
    pthread_mutex_unlock(&rb->mutex);
    return written;
}

static size_t ring_buffer_available_data(ring_buffer_t *rb)
{
    pthread_mutex_lock(&rb->mutex);
    size_t available;
    
    if (rb->write_index >= rb->read_index) {
        available = rb->write_index - rb->read_index;
    } else {
        available = rb->size - rb->read_index + rb->write_index;
    }
    
    pthread_mutex_unlock(&rb->mutex);
    return available;
}

/** Extract one CRLF-terminated line; NUL-terminate in @p packet. */
static size_t ring_buffer_read_packet(ring_buffer_t *rb, uint8_t *packet, size_t max_len)
{
    pthread_mutex_lock(&rb->mutex);

    size_t read_pos = rb->read_index;
    size_t count = 0;
    size_t packet_len = 0;

    while (read_pos != rb->write_index) {
        size_t next_pos = (read_pos + 1) % rb->size;
        
        if (rb->buffer[read_pos] == '\r' && next_pos != rb->write_index && rb->buffer[next_pos] == '\n') {
            packet_len = count;
            break;
        }
        
        read_pos = next_pos;
        count++;
    }
    
    if (packet_len == 0) {
        pthread_mutex_unlock(&rb->mutex);
        return 0;
    }

    if (packet_len > max_len - 1) {
        packet_len = max_len - 1;
    }

    for (size_t i = 0; i < packet_len; i++) {
        packet[i] = rb->buffer[rb->read_index];
        rb->read_index = (rb->read_index + 1) % rb->size;
    }

    rb->read_index = (rb->read_index + 2) % rb->size;

    packet[packet_len] = '\0';

    pthread_mutex_unlock(&rb->mutex);
    return packet_len;
}

static void ring_buffer_process_packets(void)
{
    uint8_t packet[HAL_UART_BUFFER_SIZE];
    size_t packet_len;

    while ((packet_len = ring_buffer_read_packet(&g_uart_ctx.rx_ring_buffer, packet, sizeof(packet))) > 0) {
        if(packet_len < 210)
            UART_PRINT("[Recv] %s\r\n", packet);
        else
            UART_PRINT("[Recv] %d bytes...\r\n", packet_len);

        if (g_uart_ctx.rx_callback != NULL) {
            g_uart_ctx.rx_callback(packet, packet_len);
        }
    }
}

static void* uart_rx_thread_func(void *arg)
{
    (void)arg;

    fd_set readfds;
    uint8_t read_buffer[256];
    
    UART_PRINT("RX thread started\n");
    
    while (g_uart_ctx.rx_thread_running) {
        FD_ZERO(&readfds);
        FD_SET(g_uart_ctx.fd, &readfds);

        struct timeval timeout = {1, 0}; /* 1 s — wake to re-check rx_thread_running */
        int ret = select(g_uart_ctx.fd + 1, &readfds, NULL, NULL, &timeout);
        
        if (ret == -1) {
            if (errno != EINTR) {
                perror("select");
                break;
            }
            continue;
        } else if (ret == 0) {
            continue;
        }

        if (FD_ISSET(g_uart_ctx.fd, &readfds)) {
            ssize_t n = read(g_uart_ctx.fd, read_buffer, sizeof(read_buffer));
            if (n < 0) {
                if (errno != EAGAIN && errno != EINTR) {
                    perror("read");
                    break;
                }
                continue;
            } else if (n == 0) {
                UART_PRINT("read returned 0 bytes\n");
                continue;
            } else { /* n > 0 */
                // UART_PRINT("[Raw] Received %zd bytes: ", n);
                // #if HAL_UART_DEBUG
                // for (ssize_t i = 0; i < n; i++) {
                //     if (read_buffer[i] >= 32 && read_buffer[i] <= 126) {
                //         fprintf(stderr, "%c", read_buffer[i]);
                //     } else {
                //         fprintf(stderr, "<%02X>", read_buffer[i]);
                //     }
                // }
                // fprintf(stderr, "\n");
                // #endif
                ring_buffer_write(&g_uart_ctx.rx_ring_buffer, read_buffer, n);

                ring_buffer_process_packets();
            }
        }
    }
    
    UART_PRINT("RX thread exited\n");
    return NULL;
}