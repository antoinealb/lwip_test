#include <lwip/sockets.h>
#include <lwip/api.h>
#include <lwip/inet.h>


static void tcpecho_thread(void *arg) {
    struct netconn *conn, *newconn;
    err_t err;
    LWIP_UNUSED_ARG(arg);

    /* Create a new connection identifier. */
    conn = netconn_new(NETCONN_TCP);

    /* Bind connection to well known port number 7. */
    netconn_bind(conn, NULL, 7);

    /* Tell connection to go into listening mode. */
    netconn_listen(conn);

    while (1) {

        /* Grab new connection. */
        err = netconn_accept(conn, &newconn);
        printf("accepted new connection %p\n", newconn);
        /* Process the new connection. */
        if (err == ERR_OK) {
            struct netbuf *buf;
            void *data;
            u16_t len;

            while ((err = netconn_recv(newconn, &buf)) == ERR_OK) {
                printf("Recved\n");
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
            /*printf("Got EOF, looping\n");*/ 
            /* Close connection and discard connection identifier. */
            netconn_close(newconn);
            netconn_delete(newconn);
        }
    }
}

const char *test_str = "data";

void send_task(void *arg) {
    ip_addr_t destination;
	printf("%s()\n", __FUNCTION__);
    struct netconn *conn, *newconn;
    err_t err;
    /* Create a new connection identifier. */
    conn = netconn_new(NETCONN_TCP);


    ip_addr_set_loopback(&destination);

    /* Bind connection to well known port number 7. */
    netconn_bind(conn, &destination, 7);

    err = netconn_connect(conn, &destination, 7);
    if (err != ERR_OK) {
        printf("tcpsend: netconn_connect: error %d \"%s\"\n", err, lwip_strerr(err));
        for(;;);
    }

    printf("TCP is : %p actual type is %p\n", NETCONN_TCP, conn->type);
    //err = netconn_write(newconn, test_str, strlen(test_str), NETCONN_NOCOPY);
    if (err != ERR_OK) {
        printf("tcpsend: netconn_write: error %d \"%s\"\n",err, lwip_strerr(err));
        for(;;);
    }
}

void ping_init(void) {
    printf("%s()\n", __FUNCTION__);
  sys_thread_new("echo",tcpecho_thread , NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
  send_task(NULL);
  //sys_thread_new("send_task", send_task, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
