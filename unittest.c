#include <lwip/api.h>
#include <lwip/inet.h>

#include "unittest.h"
#include <stdio.h>

#define TEST_SUCCESS NULL

#define TEST_ASSERT(message, test) do { if (!(test)) return message; } while (0)
#define TEST_RUN(test) do { char *message = test(); tests_run++; \
                        if (message == TEST_SUCCESS) \
                            tests_success++; \
                        else \
                            printf("%s failed : %s\n", #test, message); \
                        } while (0)


char* simple_test(void) {
    ip_addr_t destination, self_ip;
    printf("%s()\n", __FUNCTION__);
    struct netconn *conn;
    err_t err;
    struct netbuf *buf;
    void *data;
    u16_t len;

    const char *test_str = "data\n";

    /* Create a new connection identifier. */
    conn = netconn_new(NETCONN_TCP);

    /* Sets the device we want to connect to. */
    IP4_ADDR(&destination, 10, 0, 0, 2);
    IP4_ADDR(&self_ip, 10, 0, 0, 3);

    /* Bind connection to well known port number 7. */
    netconn_bind(conn, &self_ip, 1235);

    printf("Connecting...\n");
    err = netconn_connect(conn, &destination, 1235);

    TEST_ASSERT("TCP connection failed.", err == ERR_OK);

    /* Don't send final \0 */
    err = netconn_write(conn, test_str, strlen(test_str), NETCONN_NOCOPY);

    TEST_ASSERT("Netconn write failed.\n", err == ERR_OK);

    err = netconn_recv(conn, &buf);
    TEST_ASSERT("Recv failed.", err == ERR_OK);

    netbuf_data(buf, &data, &len);
    TEST_ASSERT("Data is not echoed correctly", !strcmp(data, test_str));

#ifdef __unix__
    printf("Test ok.");
    exit(EXIT_SUCCESS);
#endif
    for(;;);
}

void unit_test_run_all(void *arg) {
    int tests_run = 0, tests_success = 0;
    TEST_RUN(simple_test);
}
