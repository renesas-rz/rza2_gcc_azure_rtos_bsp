/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   RZ Controller Driver                                                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_dcd_rz.h"

/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_dcd_rz_fifod_write                              PORTABLE C      */ 
/*                                                           5.8          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function writes a buffer to FIFOC                              */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_rz                           Pointer to RZ controller           */ 
/*    ed                                 Register to the ed               */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                                              */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    RZ Controller Driver                                                */
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  06-01-2014     TCRG                     Initial Version 5.7           */ 
/*  06-01-2017     TCRG                     Modified comment(s),          */ 
/*                                            resulting in version 5.8    */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT  _ux_dcd_rz_fifoc_write(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed)
{

ULONG            fifo_access_status;
ULONG            max_packet_size;
ULONG            data_buffer_size;
ULONG            payload_length;
UINT             status;
UCHAR            *payload_buffer;
ULONG            transfer_length_width;
ULONG            rz_register;

    /* Check if we are using endpoint 0.  */
    if (ed -> ux_dcd_rz_ed_index == 0)
    
    
        /* Get the Fifo access status for endpoint 0.  */
        fifo_access_status =  _ux_dcd_rz_fifo_port_change(dcd_rz, ed, UX_RZ_DCD_CFIFOSEL_ISEL);

    else
    
        /* Get the Fifo access status for non endpoint 0.  */
        fifo_access_status =  _ux_dcd_rz_fifo_port_change(dcd_rz, ed, 0);
    
    /* Check Status.  */
    if (fifo_access_status == UX_ERROR)
        
        /* We have an error. Abort.  */
        return(UX_ERROR);

    /* Get the data buffer size.  */
    data_buffer_size = _ux_dcd_rz_data_buffer_size(dcd_rz, ed);

    /* Get the max packet size for this endpoint.  */
    max_packet_size = ed -> ux_dcd_rz_ed_endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;
    
    /* Check if this transfer takes more than one packet.  */
    if (ed -> ux_dcd_rz_ed_payload_length <= max_packet_size) 
    {

        /* Set the payload to the TD payload length.  */
        payload_length =  ed -> ux_dcd_rz_ed_payload_length;

        /* Set Status to write ends.  */
        status = UX_RZ_DCD_FIFO_WRITE_END;
        
        /* Check for 0 packet.  */
        if ((payload_length == 0) || (payload_length % max_packet_size ) != 0)

            /* Set Status to write short.  */
            status = UX_RZ_DCD_FIFO_WRITE_SHORT;
    }

    else
    {
        
        /* We are doing a multi write.  */
        status = UX_RZ_DCD_FIFO_WRITING;
                
        /* Payload length is the buffer size.  */
        payload_length = data_buffer_size;
        
    }      
    
    /* Get the payload buffer address.  */
    payload_buffer =  ed -> ux_dcd_rz_ed_payload_address;
    
    /* Memorize packet length.  */
    ed -> ux_dcd_rz_ed_packet_length = payload_length;
    
    /* Select a transfer size.  */
    if ((payload_length & 0x1 ) || ( (ULONG) payload_buffer & 0x1))   
        /* We use 8 bits to transfer.  */
        transfer_length_width = UX_RZ_DCD_DFIFOSEL_MBW_8;
    else if ((payload_length & 0x2 )|| ( (ULONG) payload_buffer & 0x2))
        /* We use 16 bits to transfer.  */
        transfer_length_width = UX_RZ_DCD_DFIFOSEL_MBW_16;
    else
        /* We use 32 bits to transfer.  */
        transfer_length_width = UX_RZ_DCD_DFIFOSEL_MBW_32;

    /* Set the width to x bits. */
    _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_CFIFOSEL, transfer_length_width);
    
    /* Write according to the width.  */
    switch (transfer_length_width)
    {
       case UX_RZ_DCD_DFIFOSEL_MBW_32  : 

            /* Write 32 bits values.  */
            while (payload_length != 0)                  
            {
                /* Write the 32 bit value.  */
                *((ULONG *)UX_RZ_DCD_CFIFO) =  *((ULONG *)payload_buffer);

                /* Update the payload buffer address.  */
                payload_buffer += sizeof(ULONG);
            
                /* And the remaining length.  */
                payload_length -= sizeof(ULONG);
                
            }
        case UX_RZ_DCD_DFIFOSEL_MBW_16  : 

            /* Write 16 bits values.  */
            while (payload_length != 0)                  
            {
                
                /* Write the 16 bit value.  */
                *((ULONG *)UX_RZ_DCD_CFIFO) =  _ux_utility_short_get(payload_buffer);


                /* Update the payload buffer address.  */
                payload_buffer += sizeof(USHORT);
            
                /* And the remaining length.  */
                payload_length -= sizeof(USHORT);
            }
            break;
            

        case UX_RZ_DCD_DFIFOSEL_MBW_8  : 

            /* Write 8 bits values.  */
            while (payload_length != 0)                  
            {
                /* Write the 8 bit value.  */
                *((ULONG *)UX_RZ_DCD_CFIFO) =  *payload_buffer;

                /* Update the payload buffer address.  */
                payload_buffer += sizeof(UCHAR);
            
                /* And the remaining length.  */
                payload_length -= sizeof(UCHAR);
                
            }
    }

    /* Check status. If we have a short packet, we need to set 
       the BVAL flag  in the Fifo controller.  */
    if (status != UX_RZ_DCD_FIFO_WRITING)
    {

        /* Read the current FIFO control register.  */
        rz_register = _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_CFIFOCTR);

        /* Check if the BVAL bit is already set.  */
        if ((rz_register & UX_RZ_DCD_FIFOCTR_BVAL) == 0)

            /* No so set it.  This will enable the Buffer Memory Valid flag.  
               This bit is set to 1 when data has been written to the FIFO and
               this is a short packet or zero packet or a full packet but not the
               end of the transmission.  */
            _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_CFIFOCTR, UX_RZ_DCD_FIFOCTR_BVAL);
    }       

    /* Return status about buffer transfer.  */
    return(status);
}

