#include <lwip/sockets.h>
#include <lwip/api.h>
#include <lwip/inet.h>

#ifdef __unix__
#include <stdlib.h>
#include <string.h>
#endif


static void tcpecho_thread(void *arg) {
    struct netconn *conn, *newconn;
    err_t err;
    LWIP_UNUSED_ARG(arg);

    printf("%s()\n", __FUNCTION__);

    /* Create a new connection identifier. */
    conn = netconn_new(NETCONN_TCP);

    /* Bind connection to well known port number 7. */
    netconn_bind(conn, NULL, 1235);

    /* Tell connection to go into listening mode. */
    netconn_listen(conn);

    while(1) {
        /* Grab new connection. */
        err = netconn_accept(conn, &newconn);
        printf("accepted new connection %p\n", newconn);
        /* Process the new connection. */
        if(err == ERR_OK) {
            struct netbuf *buf;
            void *data;
            u16_t len;

            while((err = netconn_recv(newconn, &buf)) == ERR_OK) {
                do {
                    netbuf_data(buf, &data, &len);
                    err = netconn_write(newconn, data, len, NETCONN_COPY);
#if 1
                    if (err != ERR_OK) {
                        printf("tcpecho: netconn_write: error \"%s\"\n", lwip_strerr(err));
                    }
#endif
                } while (netbuf_next(buf) >= 0);
                netbuf_delete(buf);
            }
            /* Close connection and discard connection identifier. */
            netconn_close(newconn);
            netconn_delete(newconn);
        }
    }
}



void tcpecho_init(void)
{
    sys_thread_new("echo",tcpecho_thread , NULL, DEFAULT_THREAD_STACKSIZE, 32);
}
