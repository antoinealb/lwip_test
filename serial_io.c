#include <stdio.h>
#include <lwip/ip.h>
#include <netif/slipif.h>
#include <unistd.h>
#include <fcntl.h>

#include "lwip/sio.h"



/** Enables activity LEDs. */
#ifndef __unix__
#define ACTIVITY_LED_TOGGLE
#endif

#ifdef ACTIVITY_LED_TOGGLE
/* Altera files, to toggle LEDs */
#include <os_cfg.h>
#include <io.h>
#include <os_cpu.h>

#define ACTIVITY_LED_TX 0
#define ACTIVITY_LED_RX 1
#endif

int in, out;

/**
 * Sends a single character to the serial device.
 *
 * @param c character to send
 * @param fd serial device handle
 *
 * @note This function will block until the character can be sent.
 * @bug On UNIX platform, writing to a broken FIFO pipe will cause a SIGPIPE
 * to be raised.
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

    write(out, &c, 1);
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
    return (u32_t)read(in, (void *)data, (size_t)len);
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
    printf("opening devnum = %d\n", devnum);
#ifdef __unix__
    if (devnum) {
        in  = open("server_in.fifo", O_RDONLY);
        out = open("server_out.fifo", O_WRONLY);
    } else {
        out = open("server_in.fifo", O_WRONLY);
        in  = open("server_out.fifo", O_RDONLY);
    }
#else
    out = open("/dev/comBT2", O_WRONLY);
    in  = open("/dev/comBT2", O_RDONLY);
#endif

    if (in == -1 || out == -1)
        return NULL;

    return &in;
}
