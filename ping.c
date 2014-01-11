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
                    printf("Received \"%s\" (len = %d) !\n", data, len);
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


void send_task(void *arg) {
    ip_addr_t destination, self_ip;
    printf("%s()\n", __FUNCTION__);
    struct netconn *conn;
    err_t err;

    const char *test_str = "data";
    /* Create a new connection identifier. */
    conn = netconn_new(NETCONN_TCP);


    /* Sets the device we want to connect to. */
#if 0
    IP4_ADDR(&destination, 192,168,1,3);
#else
    ip_addr_set_loopback(&destination);
#endif

    /* Gets our own IP adress. */
    ip_addr_set_loopback(&self_ip);

    /* Bind connection to well known port number 7. */
    netconn_bind(conn, &self_ip, 7);

    printf("Connecting...\n");
    err = netconn_connect(conn, &destination, 7);
    if(err != ERR_OK) {
        printf("tcpsend: netconn_connect: error %d \"%s\"\n", err, lwip_strerr(err));
        for(;;);
    }

    /* Don't send final \0 */
    err = netconn_write(conn, test_str, strlen(test_str), NETCONN_NOCOPY);
    if(err != ERR_OK) {
        printf("tcpsend: netconn_write: error %d \"%s\"\n",err, lwip_strerr(err));
        for(;;);
    }
    for(;;);
}

void ping_init(void) {
    printf("%s()\n", __FUNCTION__);
    sys_thread_new("echo",tcpecho_thread , NULL, DEFAULT_THREAD_STACKSIZE, 32);
    sys_thread_new("echo",send_task, NULL, DEFAULT_THREAD_STACKSIZE, 33);
    //send_task(NULL);

}
