/**********************************************************************************************************************
 * File Name    : nx_rz_driver.c
 * Version      : x.xx
 * Description  : Ethernet driver for NetX/NetXDuo
 *********************************************************************************************************************/
#include "nx_api.h"
#include "nx_rz_driver.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define ACTIVE_ETHER_CHANNEL ETHER_CHANNEL_1
#define ETHER_BUFSIZE_MIN (60)

#define NX_PACKET_SHIFT 2

/* Define the Link MTU. Note this is not the same as the IP MTU.  The Link MTU
   includes the addition of the Physical Network header (usually Ethernet). This
   should be larger than the IP instance MTU by the size of the physical header. */
#define NX_LINK_MTU      1514


/* Define Ethernet address format.  This is prepended to the incoming IP
   and ARP/RARP messages.  The frame beginning is 14 bytes, but for speed
   purposes, we are going to assume there are 16 bytes free in front of the
   prepend pointer and that the prepend pointer is 32-bit aligned.

    Byte Offset     Size            Meaning

        0           6           Destination Ethernet Address
        6           6           Source Ethernet Address
        12          2           Ethernet Frame Type, where:

                                        0x0800 -> IP Datagram
                                        0x0806 -> ARP Request/Reply
                                        0x0835 -> RARP request reply

        42          18          Padding on ARP and RARP messages only.  */

#define NX_ETHERNET_IP   0x0800
#define NX_ETHERNET_ARP  0x0806
#define NX_ETHERNET_RARP 0x8035
#define NX_ETHERNET_IPV6 0x86DD
#define NX_ETHERNET_SIZE 14

#define NX_MAX_INTERFACES                 4
#define NX_MAX_MCAST_ADDRESSES            3

/* muticast address support in this driver */
/*
#define NX_MCAST_ENABLE
*/

/**********************************************************************************************************************
Private global variables and functions
**********************************************************************************************************************/

/*
 * Private global function prototypes
 */
static void _deferred_receive_process(void);
static int _convert_packet_data(uint8_t *p_buffer_pointer, int32_t xbytes_received, NX_PACKET**packet_top);
static void  _packet_receive(NX_IP *ip_ptr, NX_PACKET *packet_ptr, UINT interface_instance_id);
static int _packet_send(NX_PACKET *packet_ptr);

static void check_ether_link(ULONG thread_input);
static void clear_all_ether_rx_discriptors(UINT interface_instance_id);

static int32_t callback_ether_regist(void);
static void callback_ether(void * pparam);
static void callback_wakeon_lan(uint32_t channel);
static void callback_link_on(uint32_t channel);
static void callback_link_off(uint32_t channel);

/*
 * Private global variables
 */
static volatile uint8_t  magic_packet_detect[ETHER_CHANNEL_MAX];
static volatile uint8_t  link_detect[ETHER_CHANNEL_MAX];

static ULONG   first_address_msw =  0x7490;
static ULONG   first_address_lsw =  0x50007903;

static TX_THREAD ether_link_check_thread;
static CHAR ether_link_check_thread_stack[1024];


/* Define of MAC address structure. */
typedef struct MAC_ADDRESS_STRUCT
{
    ULONG nx_mac_address_msw;
    ULONG nx_mac_address_lsw;
} MAC_ADDRESS;


/* Define an application-specific data structure that holds internal
   data (such as the state information) of a device driver. */
typedef struct _nx_network_driver_instance_type
{
    UINT          nx_network_driver_in_use;
    UINT          nx_network_driver_id;
    NX_INTERFACE *nx_driver_interface_ptr;

    NX_IP        *nx_driver_ip_ptr;

    MAC_ADDRESS   nx_driver_mac_address;
    MAC_ADDRESS   nx_driver_mcast_address[NX_MAX_MCAST_ADDRESSES];

    UINT          nx_driver_open;

    ULONG         nx_driver_error_count;
    ULONG         nx_driver_rx_count;
    ULONG         nx_driver_tx_count;
    ULONG         nx_driver_alloc_error_count;
} _nx_network_driver_instance_type;

/* Instance of network driver */
static _nx_network_driver_instance_type _nx_driver_instance[NX_MAX_INTERFACES];
static int _nx_driver_initialize = 0;


/**********************************************************************************************************************
 * Function Name: _nx_rz_network_driver ()
 * Description  : Network driver for NetX/NetXDuo
 * Arguments    : NX_IP_DRIVER *driver_req_ptr
 * Return Value : none
 *********************************************************************************************************************/
VOID  _nx_rz_network_driver(NX_IP_DRIVER *driver_req_ptr)
{
UINT          i;
NX_IP        *ip_ptr;
NX_PACKET    *packet_ptr;
ULONG        *ethernet_frame_ptr;
NX_INTERFACE *interface_ptr;
UINT          interface_index;

    ether_param_t   param;
    ether_return_t eth_ret;
    uint8_t myethaddr[6];

    /* Setup the IP pointer from the driver request.  */
    ip_ptr =  driver_req_ptr -> nx_ip_driver_ptr;

    /* Default to successful return.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;

    /* Setup interface pointer.  */
    interface_ptr = driver_req_ptr -> nx_ip_driver_interface;

    /* Obtain the index number of the network interface. */
    interface_index = interface_ptr -> nx_interface_index;

    /* Find out the driver interface if the driver command is not ATTACH. */
    i = 0;
    if (driver_req_ptr -> nx_ip_driver_command != NX_LINK_INTERFACE_ATTACH)
    {
        for (i = 0; i < NX_MAX_INTERFACES; i++)
        {
            if (_nx_driver_instance[i].nx_network_driver_in_use == 0)
            {
                continue;
            }

            if (_nx_driver_instance[i].nx_driver_ip_ptr != ip_ptr)
            {
                continue;
            }

            if (_nx_driver_instance[i].nx_driver_interface_ptr == driver_req_ptr -> nx_ip_driver_interface)
            {
                break;
            }
        }

        if (i == NX_MAX_INTERFACES)
        {
            driver_req_ptr -> nx_ip_driver_status =  NX_INVALID_INTERFACE;
            return;
        }
    }


    /* Process according to the driver request type in the IP control
       block.  */
    switch (driver_req_ptr -> nx_ip_driver_command)
    {

    case NX_LINK_INTERFACE_ATTACH:
    {
        if(_nx_driver_initialize==0)
        {
            R_ETHER_PinSet_ETHERC_RMII();       /* RZ/A2M RTK */
            R_ETHER_Initial();

            callback_ether_regist();

            param.channel = ACTIVE_ETHER_CHANNEL;

            eth_ret = R_ETHER_Control(CONTROL_POWER_ON, param);        /* PHY mode settings, module stop cancellation */
            if (ETHER_SUCCESS != eth_ret)
            {
                driver_req_ptr -> nx_ip_driver_status =  NX_INVALID_INTERFACE;
            }
            _nx_driver_initialize = 1;
        }

        /* Find an available driver instance to attach the interface. */
        for (i = 0; i < NX_MAX_INTERFACES; i++)
        {
            if (_nx_driver_instance[i].nx_network_driver_in_use == 0)
            {
                break;
            }
        }
        /* An available entry is found. */
        if (i < NX_MAX_INTERFACES)
        {
            /* Set the IN USE flag.*/
            _nx_driver_instance[i].nx_network_driver_in_use  = 1;

            _nx_driver_instance[i].nx_network_driver_id = i;

            /* Record the interface attached to the IP instance. */
            _nx_driver_instance[i].nx_driver_interface_ptr = driver_req_ptr -> nx_ip_driver_interface;

            /* Record the IP instance. */
            _nx_driver_instance[i].nx_driver_ip_ptr = ip_ptr;

            _nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_msw = first_address_msw;
            _nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_lsw = first_address_lsw;

            _nx_driver_instance[i].nx_driver_open = 0;

            _nx_driver_instance[i].nx_driver_error_count = 0;
            _nx_driver_instance[i].nx_driver_rx_count = 0;
            _nx_driver_instance[i].nx_driver_tx_count = 0;
            _nx_driver_instance[i].nx_driver_alloc_error_count = 0;
        }
        else
        {
            driver_req_ptr -> nx_ip_driver_status =  NX_INVALID_INTERFACE;
        }

        break;
    }

    case NX_LINK_INTERFACE_DETACH:
    {

        /* Zero out the driver instance. */
        memset(&(_nx_driver_instance[i]), 0, sizeof(_nx_network_driver_instance_type));

        break;
    }

    case NX_LINK_INITIALIZE:
    {

        /* Device driver shall initialize the Ethernet Controller here. */

        /* Once the Ethernet controller is initialized, the driver needs to
           configure the NetX Interface Control block, as outlined below. */

        /* The nx_interface_ip_mtu_size should be the MTU for the IP payload.
           For regular Ethernet, the IP MTU is 1500. */
        nx_ip_interface_mtu_set(ip_ptr, interface_index, (NX_LINK_MTU - NX_ETHERNET_SIZE));

        /* Set the physical address (MAC address) of this IP instance.  */
        nx_ip_interface_physical_address_set(ip_ptr, interface_index,
                                             _nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_msw,
                                             _nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_lsw, NX_FALSE);

        /* Indicate to the IP software that IP to physical mapping is required.  */
        nx_ip_interface_address_mapping_configure(ip_ptr, interface_index, NX_TRUE);

        break;
    }

    case NX_LINK_UNINITIALIZE:
    {

        /* Zero out the driver instance. */
        memset(&(_nx_driver_instance[i]), 0, sizeof(_nx_network_driver_instance_type));

        break;
    }

    case NX_LINK_ENABLE:
    {

        /* Process driver link enable.  An Ethernet driver shall enable the
           transmit and reception logic.  Once the IP stack issues the
           LINK_ENABLE command, the stack may start transmitting IP packets. */

        if(_nx_driver_instance[i].nx_driver_open==0)
        {
            myethaddr[0] = (uint8_t)((_nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_msw>>8)&0x00ff);
            myethaddr[1] = (uint8_t)(_nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_msw&0x00ff);
            myethaddr[2] = (uint8_t)((_nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_lsw>>24)&0x00ff);
            myethaddr[3] = (uint8_t)((_nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_lsw>>16)&0x00ff);
            myethaddr[4] = (uint8_t)((_nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_lsw>>8)&0x00ff);
            myethaddr[5] = (uint8_t)(_nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_lsw&0x00ff);

            eth_ret = R_ETHER_Open_ZC2(ACTIVE_ETHER_CHANNEL, myethaddr, ETHER_FLAG_OFF);
            if (eth_ret!=ETHER_SUCCESS)
            {
                driver_req_ptr -> nx_ip_driver_status = NX_FALSE;
               _nx_driver_instance[i].nx_driver_error_count++;
            }
            else
            {
                /* Create the link check thread.  */
                tx_thread_create(&ether_link_check_thread, "ethernet link check thread", check_ether_link, (ULONG)interface_ptr,  
                        ether_link_check_thread_stack, sizeof(ether_link_check_thread_stack), 
                        1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

                interface_ptr -> nx_interface_link_up = R_ETHER_CheckLink_ZC(ACTIVE_ETHER_CHANNEL)==ETHER_SUCCESS?NX_TRUE:NX_FALSE;

                _nx_driver_instance[i].nx_driver_open++;
            }
        }
        else
        {
           _nx_driver_instance[i].nx_driver_error_count++;
        }
        break;
    }

    case NX_LINK_DISABLE:
    {

        /* Process driver link disable.  This command indicates the IP layer
           is not going to transmit any IP datagrams, nor does it expect any
           IP datagrams from the interface.  Therefore after processing this command,
           the device driver shall not send any incoming packets to the IP
           layer.  Optionally the device driver may turn off the interface. */

        if(0<_nx_driver_instance[i].nx_driver_open)
        {
            eth_ret = R_ETHER_Close_ZC2(ACTIVE_ETHER_CHANNEL);
            if (eth_ret!=ETHER_SUCCESS)
            {
                driver_req_ptr -> nx_ip_driver_status = NX_FALSE;
                _nx_driver_instance[i].nx_driver_error_count++;
            }
            else
            {
                _nx_driver_instance[i].nx_driver_open--;
            }
        }

        interface_ptr -> nx_interface_link_up =  NX_FALSE;
        break;
    }

    case NX_LINK_PACKET_SEND:
    case NX_LINK_PACKET_BROADCAST:
    case NX_LINK_ARP_SEND:
    case NX_LINK_ARP_RESPONSE_SEND:
    case NX_LINK_RARP_SEND:
    {

        /*
           The IP stack sends down a data packet for transmission.
           The device driver needs to prepend a MAC header, and fill in the
           Ethernet frame type (assuming Ethernet protocol for network transmission)
           based on the type of packet being transmitted.

           The following sequence illustrates this process.
         */


        /* Place the ethernet frame at the front of the packet.  */
        packet_ptr =  driver_req_ptr -> nx_ip_driver_packet;

        /* Adjust the prepend pointer.  */
        packet_ptr -> nx_packet_prepend_ptr =  packet_ptr -> nx_packet_prepend_ptr - NX_ETHERNET_SIZE;

        /* Adjust the packet length.  */
        packet_ptr -> nx_packet_length =  packet_ptr -> nx_packet_length + NX_ETHERNET_SIZE;

        /* Setup the ethernet frame pointer to build the ethernet frame.  Backup another 2
           bytes to get 32-bit word alignment.  */
        /*lint -e{927} -e{826} suppress cast of pointer to pointer, since it is necessary  */
        ethernet_frame_ptr =  (ULONG *)(packet_ptr -> nx_packet_prepend_ptr - 2);

        /* Build the ethernet frame.  */
        *ethernet_frame_ptr     =  driver_req_ptr -> nx_ip_driver_physical_address_msw;
        *(ethernet_frame_ptr + 1) =  driver_req_ptr -> nx_ip_driver_physical_address_lsw;
        *(ethernet_frame_ptr + 2) =  (interface_ptr -> nx_interface_physical_address_msw << 16) |
            (interface_ptr -> nx_interface_physical_address_lsw >> 16);
        *(ethernet_frame_ptr + 3) =  (interface_ptr -> nx_interface_physical_address_lsw << 16);

        if (driver_req_ptr -> nx_ip_driver_command == NX_LINK_ARP_SEND)
        {
            *(ethernet_frame_ptr + 3) |= NX_ETHERNET_ARP;
        }
        else if (driver_req_ptr -> nx_ip_driver_command == NX_LINK_ARP_RESPONSE_SEND)
        {
            *(ethernet_frame_ptr + 3) |= NX_ETHERNET_ARP;
        }
        else if (driver_req_ptr -> nx_ip_driver_command == NX_LINK_RARP_SEND)
        {
            *(ethernet_frame_ptr + 3) |= NX_ETHERNET_RARP;
        }
        else if (packet_ptr -> nx_packet_ip_version == 4)
        {
            *(ethernet_frame_ptr + 3) |= NX_ETHERNET_IP;
        }
        else
        {
            *(ethernet_frame_ptr + 3) |= NX_ETHERNET_IPV6;
        }


        /* Endian swapping if NX_LITTLE_ENDIAN is defined.  */
        NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr));
        NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 1));
        NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 2));
        NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 3));

        /* At this point, the packet is a complete Ethernet frame, ready to be transmitted.
           The driver shall call the actual Ethernet transmit routine and put the packet
           on the wire. */

        if(0<_packet_send(packet_ptr))
        {
            _nx_driver_instance[i].nx_driver_tx_count++;

            /* Remove the Ethernet header.  In real hardware environments, this is typically
               done after a transmit complete interrupt.  */
            packet_ptr -> nx_packet_prepend_ptr =  packet_ptr -> nx_packet_prepend_ptr + NX_ETHERNET_SIZE;

            /* Adjust the packet length.  */
            packet_ptr -> nx_packet_length =  packet_ptr -> nx_packet_length - NX_ETHERNET_SIZE;

            /* Now that the Ethernet frame has been removed, release the packet.  */
            nx_packet_transmit_release(packet_ptr);

        }
        else
        {
           _nx_driver_instance[i].nx_driver_error_count++;
        }
        break;
    }


    case NX_LINK_MULTICAST_JOIN:
    {
    UINT mcast_index;

        /* The IP layer issues this command to join a multicast group.  Note that
           multicast operation is required for IPv6.

           On a typically Ethernet controller, the driver computes a hash value based
           on MAC address, and programs the hash table.

           It is likely the driver also needs to maintain an internal MAC address table.
           Later if a multicast address is removed, the driver needs
           to reprogram the hash table based on the remaining multicast MAC addresses. */

        for (mcast_index = 0; mcast_index < NX_MAX_MCAST_ADDRESSES; mcast_index++)
        {
            if (_nx_driver_instance[i].nx_driver_mcast_address[mcast_index].nx_mac_address_msw == 0 &&
                _nx_driver_instance[i].nx_driver_mcast_address[mcast_index].nx_mac_address_lsw == 0)
            {
                _nx_driver_instance[i].nx_driver_mcast_address[mcast_index].nx_mac_address_msw = driver_req_ptr -> nx_ip_driver_physical_address_msw;
                _nx_driver_instance[i].nx_driver_mcast_address[mcast_index].nx_mac_address_lsw = driver_req_ptr -> nx_ip_driver_physical_address_lsw;
                break;
            }
        }
        if (mcast_index == NX_MAX_MCAST_ADDRESSES)
        {
            driver_req_ptr -> nx_ip_driver_status =  NX_NO_MORE_ENTRIES;
           _nx_driver_instance[i].nx_driver_error_count++;
        }

        break;
    }


    case NX_LINK_MULTICAST_LEAVE:
    {

    UINT mcast_index;

        /* The IP layer issues this command to remove a multicast MAC address from the
           receiving list.  A device driver shall properly remove the multicast address
           from the hash table, so the hardware does not receive such traffic.  Note that
           in order to reprogram the hash table, the device driver may have to keep track of
           current active multicast MAC addresses. */

        for (mcast_index = 0; mcast_index < NX_MAX_MCAST_ADDRESSES; mcast_index++)
        {
            if (_nx_driver_instance[i].nx_driver_mcast_address[mcast_index].nx_mac_address_msw == driver_req_ptr -> nx_ip_driver_physical_address_msw &&
                _nx_driver_instance[i].nx_driver_mcast_address[mcast_index].nx_mac_address_lsw == driver_req_ptr -> nx_ip_driver_physical_address_lsw)
            {
                _nx_driver_instance[i].nx_driver_mcast_address[mcast_index].nx_mac_address_msw = 0;
                _nx_driver_instance[i].nx_driver_mcast_address[mcast_index].nx_mac_address_lsw = 0;
                break;
            }
        }
        if (mcast_index == NX_MAX_MCAST_ADDRESSES)
        {
            driver_req_ptr -> nx_ip_driver_status =  NX_ENTRY_NOT_FOUND;
           _nx_driver_instance[i].nx_driver_error_count++;
        }

        break;
    }

    case NX_LINK_GET_STATUS:
    {

        /* Return the link status in the supplied return pointer.  */
        *(driver_req_ptr -> nx_ip_driver_return_ptr) =  ip_ptr -> nx_ip_interface[0].nx_interface_link_up;
        break;
    }

    case NX_LINK_GET_SPEED:
    {

        /* Return the link's line speed in the supplied return pointer. Unsupported feature.  */
        *(driver_req_ptr -> nx_ip_driver_return_ptr) = 0;
        break;
    }

    case NX_LINK_GET_DUPLEX_TYPE:
    {

        /* Return the link's line speed in the supplied return pointer. Unsupported feature.  */
        *(driver_req_ptr -> nx_ip_driver_return_ptr) = 0;
        break;
    }

    case NX_LINK_GET_ERROR_COUNT:
    {

        /* Return the link's line speed in the supplied return pointer. Unsupported feature.  */
        *(driver_req_ptr -> nx_ip_driver_return_ptr) = _nx_driver_instance[i].nx_driver_error_count;
        break;
    }

    case NX_LINK_GET_RX_COUNT:
    {

        /* Return the link's line speed in the supplied return pointer. Unsupported feature.  */
        *(driver_req_ptr -> nx_ip_driver_return_ptr) = _nx_driver_instance[i].nx_driver_rx_count;
        break;
    }

    case NX_LINK_GET_TX_COUNT:
    {

        /* Return the link's line speed in the supplied return pointer. Unsupported feature.  */
        *(driver_req_ptr -> nx_ip_driver_return_ptr) = _nx_driver_instance[i].nx_driver_tx_count;
        break;
    }

    case NX_LINK_GET_ALLOC_ERRORS:
    {

        /* Return the link's line speed in the supplied return pointer. Unsupported feature.  */
        *(driver_req_ptr -> nx_ip_driver_return_ptr) = _nx_driver_instance[i].nx_driver_alloc_error_count;
        break;
    }

    case NX_LINK_DEFERRED_PROCESSING:
    {
        _deferred_receive_process();
        break;
    }

    case NX_LINK_SET_PHYSICAL_ADDRESS:
    {
        /* Find an driver instance to attach the interface. */
        for (i = 0; i < NX_MAX_INTERFACES; i++)
        {
            if (_nx_driver_instance[i].nx_driver_interface_ptr == driver_req_ptr -> nx_ip_driver_interface)
            {
                break;
            }
        }

        /* An available entry is found. */
        if (i < NX_MAX_INTERFACES)
        {

            /* Set the physical address.  */
            _nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_msw = driver_req_ptr -> nx_ip_driver_physical_address_msw;
            _nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_lsw = driver_req_ptr -> nx_ip_driver_physical_address_lsw;

            if(0<_nx_driver_instance[i].nx_driver_open)
            {
                myethaddr[0] = (uint8_t)((_nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_msw>>8)&0x00ff);
                myethaddr[1] = (uint8_t)(_nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_msw&0x00ff);
                myethaddr[2] = (uint8_t)((_nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_lsw>>24)&0x00ff);
                myethaddr[3] = (uint8_t)((_nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_lsw>>16)&0x00ff);
                myethaddr[4] = (uint8_t)((_nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_lsw>>8)&0x00ff);
                myethaddr[5] = (uint8_t)(_nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_lsw&0x00ff);

                R_ETHER_Close_ZC2(ACTIVE_ETHER_CHANNEL);
                R_ETHER_Open_ZC2(ACTIVE_ETHER_CHANNEL, myethaddr, ETHER_FLAG_OFF);
            }
        }
        else
        {
            driver_req_ptr -> nx_ip_driver_status =  NX_INVALID_INTERFACE;
        }

        break;
    }

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
    case NX_INTERFACE_CAPABILITY_GET:
    {

        /* Return the capability of the Ethernet controller speed in the supplied return pointer. Unsupported feature.  */
        *(driver_req_ptr -> nx_ip_driver_return_ptr) = 0;
        break;
    }

    case NX_INTERFACE_CAPABILITY_SET:
    {

        /* Set the capability of the Ethernet controller. Unsupported feature.  */
        break;
    }
#endif /* NX_ENABLE_INTERFACE_CAPABILITY  */

    default:

        /* Invalid driver request.  */

        /* Return the unhandled command status.  */
        driver_req_ptr -> nx_ip_driver_status =  NX_UNHANDLED_COMMAND;
        break;
    }
}

/**********************************************************************************************************************
 * Function Name: _deferred_receive_process ()
 * Description  : Deferred receive processing
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
static void _deferred_receive_process(void)
{
    int32_t xbytes_received;
    uint8_t *p_buffer_pointer;

    NX_PACKET *packet_ptr;
    int interface_instance_id;

    UINT old_threshold = 0;

    /* Disable preemption.  */
    tx_thread_preemption_change(tx_thread_identify(), 0, &old_threshold);

    while(1)
    {
        /* See how much data was received.  */
        xbytes_received = R_ETHER_Read_ZC2(ACTIVE_ETHER_CHANNEL, (void **)&p_buffer_pointer);

        if( xbytes_received < 0 )
        {
            /* This is an error. Ignored. */
            break;
        }
        else if( xbytes_received > 0 )
        {
            /* Allocate a network buffer descriptor that points to a buffer
            large enough to hold the received frame.  As this is the simple
            rather than efficient example the received data will just be copied
            into this buffer. */
            interface_instance_id = _convert_packet_data(p_buffer_pointer, xbytes_received, &packet_ptr);
            R_ETHER_Read_ZC2_BufRelease(ACTIVE_ETHER_CHANNEL);

            if(0<=interface_instance_id)
            {
                _nx_driver_instance[interface_instance_id].nx_driver_rx_count++;

                _packet_receive(_nx_driver_instance[interface_instance_id].nx_driver_ip_ptr, packet_ptr, (UINT)interface_instance_id);
            }
            else
            {
                /* The event was lost because a network buffer was not available.
                Call the standard trace macro to log the occurrence. */
                clear_all_ether_rx_discriptors((UINT)interface_instance_id);
            }
        }
        else
        {
            /* nothing is received */
            break;
        }
    }

    /* Restore preemption.  */
    tx_thread_preemption_change(tx_thread_identify(), old_threshold, &old_threshold);
} /* End of function _deferred_receive_process() */

/**********************************************************************************************************************
 * Function Name: _convert_packet_data ()
 * Description  : Convert received packet data to NX_PACKET data
 * Arguments    : uint8_t *p_buffer_pointer
 *                int32_t xbytes_received
 *                NX_PACKET**packet_top
 * Return Value : interface instance id
 *********************************************************************************************************************/
static int _convert_packet_data(uint8_t *p_buffer_pointer, int32_t xbytes_received, NX_PACKET**packet_top)
{
int rtn = -1;
NX_IP     *ip_ptr;
NX_PACKET* packet_ptr;
UINT       i;

#ifdef NX_MCAST_ENABLE
ULONG      destination_address_msw;
ULONG      destination_address_lsw;
UINT       mcast_index;

    /* Pickup the destination IP address from the p_buffer_pointer.  */
    destination_address_msw =  (ULONG)*(p_buffer_pointer);
    destination_address_msw =  (destination_address_msw << 8) | (ULONG)*(p_buffer_pointer + 1);
    destination_address_lsw =  (ULONG)*(p_buffer_pointer + 2);
    destination_address_lsw =  (destination_address_lsw << 8) | (ULONG)*(p_buffer_pointer + 3);
    destination_address_lsw =  (destination_address_lsw << 8) | (ULONG)*(p_buffer_pointer + 4);
    destination_address_lsw =  (destination_address_lsw << 8) | (ULONG)*(p_buffer_pointer + 5);

    for (i = 0; (i < NX_MAX_INTERFACES) && (rtn==-1); i++)
    {
        /* Skip the instance that has not been initialized. */
        if (_nx_driver_instance[i].nx_network_driver_in_use == 0)
        {
            continue;
        }

        /* Set the IP instance.  */
        ip_ptr = _nx_driver_instance[i].nx_driver_ip_ptr;

        /* If the destination MAC address is broadcast or the destination matches the interface MAC,
           accept the packet. */
        if (((destination_address_msw == ((ULONG)0x0000FFFF)) && (destination_address_lsw == ((ULONG)0xFFFFFFFF))) ||   /* Broadcast match */
            ((destination_address_msw == _nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_msw) &&
             (destination_address_lsw == _nx_driver_instance[i].nx_driver_mac_address.nx_mac_address_lsw)) ||
            (destination_address_msw == ((ULONG)0x00003333)) ||
            ((destination_address_msw == 0) && (destination_address_lsw == 0)))
        {
            if(nx_packet_allocate(ip_ptr->nx_ip_default_packet_pool, &packet_ptr, NX_RECEIVE_PACKET, NX_NO_WAIT)==NX_SUCCESS)
            {
                /* Adjust the address to the 32-bit boundary. */
                packet_ptr -> nx_packet_prepend_ptr = packet_ptr -> nx_packet_prepend_ptr + NX_PACKET_SHIFT;
                packet_ptr -> nx_packet_append_ptr = packet_ptr -> nx_packet_prepend_ptr;

                if(nx_packet_data_append(packet_ptr, p_buffer_pointer, xbytes_received, ip_ptr->nx_ip_default_packet_pool, NX_NO_WAIT)==NX_SUCCESS)
                {
                    *packet_top = packet_ptr;
                    rtn = i;
                }
                else
                {
                    nx_packet_release(packet_ptr);
                    packet_ptr = NULL;

                    _nx_driver_instance[i].nx_driver_alloc_error_count++;
                }
            }
            else
            {
                _nx_driver_instance[i].nx_driver_alloc_error_count++;
            }

            break;
        }
        else
        {
            for (mcast_index = 0; mcast_index < NX_MAX_MCAST_ADDRESSES; mcast_index++)
            {
                if (destination_address_msw == _nx_driver_instance[i].nx_driver_mcast_address[mcast_index].nx_mac_address_msw &&
                    destination_address_lsw == _nx_driver_instance[i].nx_driver_mcast_address[mcast_index].nx_mac_address_lsw)
                {
                    if(nx_packet_allocate(ip_ptr->nx_ip_default_packet_pool, &packet_ptr, NX_RECEIVE_PACKET, NX_NO_WAIT)==NX_SUCCESS)
                    {
                        /* Adjust the address to the 32-bit boundary. */
                        packet_ptr -> nx_packet_prepend_ptr = packet_ptr -> nx_packet_prepend_ptr + NX_PACKET_SHIFT;
                        packet_ptr -> nx_packet_append_ptr = packet_ptr -> nx_packet_prepend_ptr;

                        if(nx_packet_data_append(packet_ptr, p_buffer_pointer, xbytes_received, ip_ptr->nx_ip_default_packet_pool, NX_NO_WAIT)==NX_SUCCESS)
                        {
                            *packet_top = packet_ptr;
                            rtn = i;
                        }
                        else
                        {
                            nx_packet_release(packet_ptr);
                            packet_ptr = NULL;

                            _nx_driver_instance[i].nx_driver_alloc_error_count++;
                        }
                    }
                    else
                    {
                        _nx_driver_instance[i].nx_driver_alloc_error_count++;
                    }

                    break;
                }
            }
        }
    }
#else
    for (i = 0; (i < NX_MAX_INTERFACES) && (rtn==-1); i++)
    {
        /* Skip the instance that has not been initialized. */
        if (_nx_driver_instance[i].nx_network_driver_in_use == 0)
        {
            continue;
        }

        /* Set the IP instance.  */
        ip_ptr = _nx_driver_instance[i].nx_driver_ip_ptr;

        if(nx_packet_allocate(ip_ptr->nx_ip_default_packet_pool, &packet_ptr, NX_RECEIVE_PACKET, NX_NO_WAIT)==NX_SUCCESS)
        {
            /* Adjust the address to the 32-bit boundary. */
            packet_ptr -> nx_packet_prepend_ptr = packet_ptr -> nx_packet_prepend_ptr + NX_PACKET_SHIFT;
            packet_ptr -> nx_packet_append_ptr = packet_ptr -> nx_packet_prepend_ptr;

            if(nx_packet_data_append(packet_ptr, p_buffer_pointer, (ULONG)xbytes_received, ip_ptr->nx_ip_default_packet_pool, NX_NO_WAIT)==NX_SUCCESS)
            {
                *packet_top = packet_ptr;
                rtn = (int)i;
                break;
            }
            else
            {
                nx_packet_release(packet_ptr);
                packet_ptr = NULL;

                _nx_driver_instance[i].nx_driver_alloc_error_count++;
                break;
            }
        }
        else
        {
            _nx_driver_instance[i].nx_driver_alloc_error_count++;
            break;
        }
    }
#endif

    return rtn;
} /* End of function _convert_packet_data() */

/**********************************************************************************************************************
 * Function Name: _packet_receive ()
 * Description  : 
 * Arguments    : NX_IP *ip_ptr
 *                NX_PACKET *packet_ptr
 *                UINT interface_instance_id
 * Return Value : none
 *********************************************************************************************************************/
static void  _packet_receive(NX_IP *ip_ptr, NX_PACKET *packet_ptr, UINT interface_instance_id)
{
UINT packet_type;

    /* Pickup the packet header to determine where the packet needs to be
       sent.  */
    packet_type =  (((UINT)(*(packet_ptr -> nx_packet_prepend_ptr + 12))) << 8) |
        ((UINT)(*(packet_ptr -> nx_packet_prepend_ptr + 13)));

    /* Setup interface pointer.  */
    packet_ptr -> nx_packet_address.nx_packet_interface_ptr = _nx_driver_instance[interface_instance_id].nx_driver_interface_ptr;

    /* Clean off the Ethernet header.  */
    packet_ptr -> nx_packet_prepend_ptr =  packet_ptr -> nx_packet_prepend_ptr + NX_ETHERNET_SIZE;

    /* Adjust the packet length.  */
    packet_ptr -> nx_packet_length =  packet_ptr -> nx_packet_length - NX_ETHERNET_SIZE;

    /* Route the incoming packet according to its ethernet type.  */
    /* The driver accepts both IPv4 and IPv6 frames. */
    if ((packet_type == NX_ETHERNET_IP) || (packet_type == NX_ETHERNET_IPV6))
    {
        /* Route to the ip receive function.  */
#ifdef NX_DIRECT_ISR_CALL
        _nx_ip_packet_receive(ip_ptr, packet_ptr);
#else
        _nx_ip_packet_deferred_receive(ip_ptr, packet_ptr);
#endif
    }
#ifndef NX_DISABLE_IPV4
    else if (packet_type == NX_ETHERNET_ARP)
    {
        /* Route to the ARP receive function.  */
        _nx_arp_packet_deferred_receive(ip_ptr, packet_ptr);
    }
    else if (packet_type == NX_ETHERNET_RARP)
    {
        /* Route to the RARP receive function.  */
        _nx_rarp_packet_deferred_receive(ip_ptr, packet_ptr);
    }
#endif /* !NX_DISABLE_IPV4  */
    else
    {
        /* Invalid ethernet header... release the packet.  */
        nx_packet_release(packet_ptr);

       _nx_driver_instance[interface_instance_id].nx_driver_error_count++;
    }
} /* End of function _packet_receive() */

/**********************************************************************************************************************
 * Function Name: _packet_send ()
 * Description  : 
 * Arguments    : NX_PACKET *packet_ptr
 * Return Value : length
 *********************************************************************************************************************/
static int _packet_send(NX_PACKET *packet_ptr)
{
    ether_return_t ret;
    uint8_t * p_write_buffer;
    uint16_t write_buf_size;
    size_t length;
    ULONG bytes_copied;

    /* (1) Retrieve the transmit buffer location controlled by the  descriptor. */
    ret = R_ETHER_Write_ZC2_GetBuf(ACTIVE_ETHER_CHANNEL, (void **) &p_write_buffer, &write_buf_size);

    if (ETHER_SUCCESS == ret)
    {
        length = (size_t)packet_ptr -> nx_packet_length;

        if (write_buf_size >= length)
        {
            /* copy buffer */
            if(nx_packet_data_retrieve(packet_ptr, (VOID *)p_write_buffer, &bytes_copied)==NX_SUCCESS)
            {
                if (bytes_copied < ETHER_BUFSIZE_MIN)                                         /*under minimum*/
                {
                    memset((p_write_buffer + bytes_copied), 0, (ETHER_BUFSIZE_MIN - bytes_copied));  /*padding*/
                    length = ETHER_BUFSIZE_MIN;                                         /*resize*/
                }
                else
                {
                    length = bytes_copied;
                }

                /* write data */
                ret = R_ETHER_Write_ZC2_SetBuf(ACTIVE_ETHER_CHANNEL, (uint16_t)length);
                ret = R_ETHER_CheckWrite(ACTIVE_ETHER_CHANNEL);
            }
            else
            {
                length = 0;
            }
        }
        else
        {
            length = 0;
        }
    }
    else
    {
        length = 0;
    }

    if (ETHER_SUCCESS != ret)
    {
        /* Remove the Ethernet header.  */
        packet_ptr -> nx_packet_prepend_ptr =  packet_ptr -> nx_packet_prepend_ptr + NX_ETHERNET_SIZE;

        /* Adjust the packet length.  */
        packet_ptr -> nx_packet_length =  packet_ptr -> nx_packet_length - NX_ETHERNET_SIZE;

        /* Error, no point in continuing, just release the packet.  */
        nx_packet_transmit_release(packet_ptr);

        length = 0;
    }

    return (int)length;
} /* End of function _packet_send() */

/**********************************************************************************************************************
 * Function Name: check_ether_link ()
 * Description  : Checks status of Ethernet link. Should be executed on a periodic basis.
 * Arguments    : ULONG thread_input
 * Return Value : none
 *********************************************************************************************************************/
static void check_ether_link(ULONG thread_input)
{
NX_INTERFACE *interface_ptr;

    UINT old_interrupt;

    interface_ptr = (NX_INTERFACE*)thread_input;

    while(1)
    {
        old_interrupt = tx_interrupt_control(TX_INT_DISABLE);

        if((link_detect[ACTIVE_ETHER_CHANNEL]!=ETHER_FLAG_ON_LINK_ON) || (interface_ptr -> nx_interface_link_up==NX_FALSE))
        {
            tx_interrupt_control(old_interrupt);

            while(1)
            {
                R_ETHER_LinkProcess(ACTIVE_ETHER_CHANNEL);

                tx_thread_sleep(100);

                if(R_ETHER_CheckLink_ZC(ACTIVE_ETHER_CHANNEL)==ETHER_SUCCESS)
                {
                    interface_ptr -> nx_interface_link_up = NX_TRUE;
                    break;
                }
            }
        }
        else
        {
            tx_interrupt_control(old_interrupt);

            tx_thread_sleep(1000);

            if(R_ETHER_CheckLink_ZC(ACTIVE_ETHER_CHANNEL)!=ETHER_SUCCESS)
            {
                interface_ptr -> nx_interface_link_up = NX_FALSE;
            }
        }
    }
} /* End of function check_ether_link() */

/**********************************************************************************************************************
 * Function Name: clear_all_ether_rx_discriptors ()
 * Description  : Clear all Ether RX descriptors
 * Arguments    : UINT interface_instance_id
 * Return Value : none
 *********************************************************************************************************************/
static void clear_all_ether_rx_discriptors(UINT interface_instance_id)
{
    int32_t xbytes_received;
    uint8_t *p_buffer_pointer;

    while(1)
    {
        /* See how much data was received.  */
        xbytes_received = R_ETHER_Read_ZC2(ACTIVE_ETHER_CHANNEL, (void **)&p_buffer_pointer);
        if(0 > xbytes_received)
        {
            /* This is an error. Ignored. */
        }
        else if(0 < xbytes_received)
        {
           _nx_driver_instance[interface_instance_id].nx_driver_rx_count++;
           _nx_driver_instance[interface_instance_id].nx_driver_error_count++;
            R_ETHER_Read_ZC2_BufRelease(ACTIVE_ETHER_CHANNEL);
        }
        else
        {
            break;
        }
    }
} /* End of function clear_all_ether_rx_discriptors() */

/**********************************************************************************************************************
 * Function Name: callback_ether_regist
 * Description  : Regist of callback function.
 * Arguments    : None.
 * Return Value : 0: success, -1:failed.
 *********************************************************************************************************************/
static int32_t callback_ether_regist(void)
{
    ether_param_t   param;
    ether_cb_t      cb_func;

    int32_t         ret;

    /* Set the callback function (LAN cable connect/disconnect event) */
    cb_func.pcb_func     = &callback_ether;
    param.ether_callback = cb_func;
    ret = R_ETHER_Control(CONTROL_SET_CALLBACK, param);
    if (ETHER_SUCCESS != ret)
    {
        return -1;
    }

    /* Set the callback function (Ether interrupt event) */
    cb_func.pcb_int_hnd     = &_nx_rz_network_isr;
    param.ether_callback = cb_func;
    ret = R_ETHER_Control(CONTROL_SET_INT_HANDLER, param);
    if (ETHER_SUCCESS != ret)
    {
        return -1;
    }
    return 0;
} /* End of function callback_ether_regist() */

/**********************************************************************************************************************
* Function Name: callback_ether
* Description  : Sample of the callback function
* Arguments    : pparam -
*
* Return Value : none
**********************************************************************************************************************/
static void callback_ether(void * pparam)
{
    ether_cb_arg_t    * p_decode;
    uint32_t            channel;

    /* cast to non void* pointer */
    p_decode = (ether_cb_arg_t *)pparam;
    channel = p_decode->channel;                             /* Get Ethernet channel number */

    switch (p_decode->event_id)
    {
            /* Callback function that notifies user to have detected magic packet. */
        case ETHER_CB_EVENT_ID_WAKEON_LAN:
            callback_wakeon_lan(channel);
            break;

            /* Callback function that notifies user to have become Link up. */
        case ETHER_CB_EVENT_ID_LINK_ON:
            callback_link_on(channel);
            break;

            /* Callback function that notifies user to have become Link down. */
        case ETHER_CB_EVENT_ID_LINK_OFF:
            callback_link_off(channel);
            break;

        default:
            break;
    }
} /* End of function callback_ether() */

/**********************************************************************************************************************
* Function Name: callback_wakeon_lan
* Description  :
* Arguments    : channel -
*                    Ethernet channel number
* Return Value : none
**********************************************************************************************************************/
static void callback_wakeon_lan(uint32_t channel)
{
    if (ETHER_CHANNEL_MAX > channel)
    {
        magic_packet_detect[channel] = 1;

        /* Please add necessary processing when magic packet is detected.  */
    }
} /* End of function callback_wakeon_lan() */

/**********************************************************************************************************************
* Function Name: callback_link_on
* Description  :
* Arguments    : channel -
*                    Ethernet channel number
* Return Value : none
**********************************************************************************************************************/
static void callback_link_on(uint32_t channel)
{
    if (ETHER_CHANNEL_MAX > channel)
    {
        link_detect[channel] = ETHER_FLAG_ON_LINK_ON;

        /* Please add necessary processing when becoming Link up. */
    }
} /* End of function callback_link_on() */

/**********************************************************************************************************************
* Function Name: callback_link_off
* Description  :
* Arguments    : channel -
*                    Ethernet channel number
* Return Value : none
**********************************************************************************************************************/
static void callback_link_off(uint32_t channel)
{
    if (ETHER_CHANNEL_MAX > channel)
    {
        link_detect[channel] = ETHER_FLAG_ON_LINK_OFF;

        /* Please add necessary processing when becoming Link down. */
    }
} /* End of function ether_cb_link_off() */

/**********************************************************************************************************************
* Function Name: _nx_rz_network_isr
* Description  : Standard frame received interrupt handler
* Arguments    : ectrl - EDMAC and ETHERC control structure
* Return Value : None
* Note         : This callback function is executed when EINT0 interrupt occurred.
**********************************************************************************************************************/
void _nx_rz_network_isr(void *ectrl)
{
    /* set the initial values */
    ether_cb_arg_t *p_decode;
    int i;

    /* set temporary pointer */
    p_decode = (ether_cb_arg_t*)ectrl;

    if (p_decode->status_eesr & 0x00040000)// EDMAC FR (Frame Receive Event) interrupt
    {
        for (i = 0; i < NX_MAX_INTERFACES; i++)
        {
            if (_nx_driver_instance[i].nx_network_driver_in_use == 0)
            {
                continue;
            }

            if (_nx_driver_instance[i].nx_driver_ip_ptr != NULL)
            {
                _nx_ip_driver_deferred_processing(_nx_driver_instance[i].nx_driver_ip_ptr );
                break;
            }
        }
    }
} /* End of function _nx_rz_network_isr() */

