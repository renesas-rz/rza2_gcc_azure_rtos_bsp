/**********************************************************************************************************************
 * File Name    : nx_rz_driver.h
 * Version      : x.xx
 * Description  : Ethernet driver for NetX/NetXDuo
 *********************************************************************************************************************/
#ifndef __NX_RZ_DRIVER_H__
#define __NX_RZ_DRIVER_H__

#include "r_pinset.h"
#include "r_ether_rza2_if.h"

VOID _nx_rz_network_driver(NX_IP_DRIVER *driver_req_ptr);
void _nx_rz_network_isr(void *ectrl);

#endif /* __NX_RZ_DRIVER_H__ */
