#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/time.h>

/*=============================================================================
 * UART HAL (init, send, recv)
 *============================================================================*/

/*-----------------------------------------------------------------------------
 * Debug logging
 *----------------------------------------------------------------------------*/
#define HAL_UART_DEBUG 0  /* 1=on, 0=off */

long uart_get_timestamp_ms(void);

#if HAL_UART_DEBUG
    #define UART_PRINT(fmt, ...) fprintf(stderr, "<UART>[%ldms] " fmt, uart_get_timestamp_ms(), ##__VA_ARGS__)
#else
    #define UART_PRINT(fmt, ...) do {} while(0)
#endif

/*-----------------------------------------------------------------------------
 * Constants
 *----------------------------------------------------------------------------*/
#define HAL_UART_DEFAULT_DEVICE     "/dev/ttySTM2"
#define HAL_UART_DEFAULT_BAUDRATE   1000000
#define HAL_UART_BUFFER_SIZE        8192

/*-----------------------------------------------------------------------------
 * Error codes
 *----------------------------------------------------------------------------*/
typedef enum {
    HAL_UART_OK = 0,
    HAL_UART_ERROR = -1,
    HAL_UART_ERROR_PARAM = -2,
    HAL_UART_ERROR_NOT_INIT = -3
} hal_uart_error_t;

/*-----------------------------------------------------------------------------
 * Callbacks
 *----------------------------------------------------------------------------*/
/** RX: one line, CRLF stripped; @p data NUL-terminated, @p len = payload bytes. */
typedef void (*hal_uart_rx_callback_t)(const uint8_t *data, size_t len);

/*-----------------------------------------------------------------------------
 * API
 *----------------------------------------------------------------------------*/

/** @brief Open and configure default UART. */
hal_uart_error_t hal_uart_init(void);

/** @brief Close UART and release resources. */
hal_uart_error_t hal_uart_deinit(void);

/** @brief Write raw bytes to UART. */
hal_uart_error_t hal_uart_send(const uint8_t *data, size_t len);

/** @brief printf-style send (may truncate to HAL_UART_BUFFER_SIZE). */
hal_uart_error_t hal_uart_printf(const char *format, ...);

/** @brief Set RX callback; NULL disables. */
hal_uart_error_t hal_uart_set_rx_callback(hal_uart_rx_callback_t callback);

/** @brief Start background RX thread. */
hal_uart_error_t hal_uart_start_rx(void);

/** @brief Stop RX thread (join). */
hal_uart_error_t hal_uart_stop_rx(void);

/** @brief True if hal_uart_init() succeeded. */
bool hal_uart_is_ready(void);

#endif // __HAL_UART_H__ 