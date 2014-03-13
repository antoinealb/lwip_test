#include <stdio.h>
#include <lwip/ip.h>
#include <netif/slipif.h>

#include "lwip/sio.h"



/** Enables activity LEDs. */
#define ACTIVITY_LED_TOGGLE

#ifdef ACTIVITY_LED_TOGGLE
/* Altera files, to toggle LEDs */
#include <os_cfg.h>
#include <io.h>
#include <os_cpu.h>

#define ACTIVITY_LED_TX 0
#define ACTIVITY_LED_RX 1
#endif

/**
 * Sends a single character to the serial device.
 *
 * @param c character to send
 * @param fd serial device handle
 *
 * @note This function will block until the character can be sent.
 */
void sio_send(u8_t c, sio_fd_t fd)
{
    int32_t led_val;

    /* Toggles Tx led. */
#ifdef ACTIVITY_LED_TOGGLE
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    led_val = IORD(LED_BASE, 0);
    led_val ^= (1 << ACTIVITY_LED_TX);
    IOWR(LED_BASE, 0, led_val);
    OS_EXIT_CRITICAL();
#endif

    OS_ENTER_CRITICAL();
    led_val = IORD(LED_BASE, 0);
    led_val |= (1 << 7);
    IOWR(LED_BASE, 0, led_val);
    OS_EXIT_CRITICAL();

    fputc((int)c, (FILE *)fd);

    // FLUSH MAKE DATA SLOW

    fflush(out);

    OS_ENTER_CRITICAL();
    led_val = IORD(LED_BASE, 0);
    led_val &= ~(1 << 7);
    IOWR(LED_BASE, 0, led_val);
    OS_EXIT_CRITICAL();
}

/**
 * Reads from the serial device.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received - may be 0 if aborted by sio_read_abort
 *
 */
u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
    int32_t led_val;

    /* Toggles Rx led */
#ifdef ACTIVITY_LED_TOGGLE
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    led_val = IORD(LED_BASE, 0);
    led_val ^= (1 << ACTIVITY_LED_RX);
    IOWR(LED_BASE, 0, led_val);
    OS_EXIT_CRITICAL();
#endif
    return (u32_t)fread((void *)data, 1, (size_t)len, (FILE *)fd);
}



/**
 * Tries to read from the serial device. Same as sio_read but returns
 * immediately if no data is available and never blocks.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received
 */
u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len)
{
    return sio_read(fd, data, len);
}

/**
 * Opens a serial device for communication.
 *
 * @param devnum device number
 * @return handle to serial device if successful, NULL otherwise
 */
sio_fd_t sio_open(u8_t devnum)
{
    FILE *fp;

    fp = fopen("/dev/comBT1", "rw");

    return (sio_fd_t)fp;
}
