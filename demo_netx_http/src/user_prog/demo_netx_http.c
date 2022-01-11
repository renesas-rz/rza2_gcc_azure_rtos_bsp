/* This is a small demo of the high-performance NetX TCP/IP stack.  */

#include "tx_api.h"
#include "nx_api.h"

#include "nx_rz_driver.h"

#ifdef FEATURE_NX_IPV6
#include "nxd_http_server.h"
#else
#include "nx_http_server.h"
#endif

#define DEMO_STACK_SIZE         2048

/* Define the ThreadX and NetX object control blocks...  */

TX_THREAD               thread_0;

NX_PACKET_POOL          pool_0;
NX_IP                   ip_0;

char demo_pool_area[40*1024] __attribute__ ((section("__sectop__ETHERNET_BUFFERS")));
char demo_ip_stack[4096];

ULONG error_counter = 0;

char test_contents[] = 
"<HTML>\r\n"
"<HEAD><TITLE>NetX HTTP Test</TITLE></HEAD>\r\n"
"<BODY>\r\n"
"<H1>Another NetX Test Page!</H1>\r\n"
"</BODY>\r\n"
"</HTML>\r\n";

/* Set up the HTTP server global variables */
NX_HTTP_SERVER  my_server;
char demo_http_server_stack[4096];


/* Define thread prototypes.  */

void thread_0_entry(ULONG thread_input);
UINT user_request_notify(NX_HTTP_SERVER *server_ptr, UINT request_type, CHAR *resource, NX_PACKET *packet_ptr);


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

CHAR    *pointer;
UINT    status;

    /* Setup the working pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + DEMO_STACK_SIZE;

    /* Initialize the NetX system.  */
    nx_system_initialize();

#ifndef NX_HTTP_NO_FILEX
    /* Initialize FileX.  */
    fx_system_initialize();
#endif

    /* Create a packet pool.  */
    status =  nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", 1520, demo_pool_area, sizeof(demo_pool_area));

    /* Check for pool creation error.  */
    if (status)
        error_counter++;

    /* Create an IP instance.  */
    status = nx_ip_create(&ip_0, "NetX IP Instance 0", IP_ADDRESS(192, 168, 2, 120), 0xFFFFFF00UL, &pool_0, _nx_rz_network_driver,
                    demo_ip_stack, sizeof(demo_ip_stack), 1);

    /* Check for IP create errors.  */
    if (status)
        error_counter++;

    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status =  nx_arp_enable(&ip_0, (void *) pointer, 1024);
    pointer = pointer + 1024;

    /* Check for ARP enable errors.  */
    if (status)
        error_counter++;

    /* Enable ICMP for IP Instance 0.  */
    status =  nx_icmp_enable(&ip_0);

    /* Check for ICMP enable errors.  */
    if (status)
        error_counter++;

    status = nx_ip_fragment_enable(&ip_0);

    /* Check for fragment enable errors.  */
    if (status)
        error_counter++;

    /* Enable TCP traffic.  */
    status = nx_tcp_enable(&ip_0);

    /* Check for TCP enable errors.  */
    if (status)
        error_counter++;

    /* Create the HTTP Server.  */
    status = nx_http_server_create(&my_server, "My HTTP Server", &ip_0, NX_NULL, 
                          demo_http_server_stack, sizeof(demo_http_server_stack), &pool_0, NX_NULL, user_request_notify);
}


/* Define the test threads.  */

void    thread_0_entry(ULONG thread_input)
{
UINT    status;

    /* OK to start the HTTP Server.   */
    status = nx_http_server_start(&my_server);

    while(1)
    {
        tx_thread_sleep(100);
    }
}

UINT user_request_notify(NX_HTTP_SERVER *server_ptr, UINT request_type, CHAR *resource, NX_PACKET *packet_ptr)
{
    char buff[128];
    int len;

    /* Look for the test resource! */
    if(strcmp(resource, "/") == 0)
    {
        if(request_type == NX_HTTP_SERVER_GET_REQUEST)
        {
            len = strlen(test_contents);
            sprintf(buff, "HTTP/1.0 200 \r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n", len);

            nx_http_server_callback_data_send(server_ptr, buff, strlen(buff));
            nx_http_server_callback_data_send(server_ptr, test_contents, len);

            return(NX_HTTP_CALLBACK_COMPLETED);
        }
    }

    /* Search for content by FileX. */
    return(NX_SUCCESS);
}

