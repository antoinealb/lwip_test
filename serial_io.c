#include <lwip/ip.h>
#include <netif/slipif.h>

#include "lwip/sio.h"

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
    printf("%s(%d)\n", __FUNCTION__, (int)c);
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
    return 0;
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
    printf("%s(%d)\n", __FUNCTION__, devnum);
    return 42;
}
