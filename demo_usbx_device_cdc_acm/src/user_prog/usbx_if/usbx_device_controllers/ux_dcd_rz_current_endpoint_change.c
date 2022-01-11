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
#include "ux_system.h"
#include "ux_utility.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_dcd_rz_current_endpoint_change                  PORTABLE C      */ 
/*                                                           5.8          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function change the endpoint in the FIFO                       */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_rz                              Pointer to RX controller        */ 
/*    ed                                    Register to the ed            */ 
/*    direction                             Direction to switch           */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */ 
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
VOID  _ux_dcd_rz_current_endpoint_change(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed, ULONG direction)
{

ULONG   rz_register;
ULONG   rz_register_check;

ULONG   volatile loop_index;

    /* Select the fifo.  */
    switch (ed -> ux_dcd_rz_ed_fifo_index)
    {

        case UX_RZ_DCD_FIFO_D0      :
            
            /* Set the current endpoint index (pipe) in the D0FIFOSEL register.  */
            _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_D0FIFOSEL, ed -> ux_dcd_rz_ed_index);
            break;        
        
        
        case UX_RZ_DCD_FIFO_D1      :
        
            /* Set the current endpoint index (pipe) in the D1FIFOSEL register.  */
            _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_D1FIFOSEL, ed -> ux_dcd_rz_ed_index);
            break;        

        case UX_RZ_DCD_FIFO_C       :

            /* Read the current CFIFOSEL register.  */
            rz_register =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_CFIFOSEL);

            /* Clear the ISEL field, the reading/writing width and the previous endpoint index.  */
            rz_register &= ~(UX_RZ_DCD_CFIFOSEL_CURPIPE_MASK | UX_RZ_DCD_CFIFOSEL_ISEL | UX_RZ_DCD_CFIFOSEL_MBW_MASK);

            /* Write back the register.  */
            _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_CFIFOSEL, rz_register);

            do {

                /* Read the CFIFOSEL register again.  */
                rz_register_check =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_CFIFOSEL);

                /* Mask the required values.  */
                rz_register_check &= ( UX_RZ_DCD_CFIFOSEL_CURPIPE_MASK | UX_RZ_DCD_CFIFOSEL_ISEL);
            } while (rz_register_check != rz_register & (UX_RZ_DCD_CFIFOSEL_CURPIPE_MASK | UX_RZ_DCD_CFIFOSEL_ISEL));

            /* Add the current direction and the new endpoint index.  */
            rz_register |= (ed -> ux_dcd_rz_ed_index | direction);

            /* Write back the register.  */
            _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_CFIFOSEL, rz_register);

            /* Read the CFIFOSEL register again.  */
            rz_register_check =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_CFIFOSEL);

            /* Isolate curpipe and isel.  */
            rz_register_check &= ( UX_RZ_DCD_CFIFOSEL_CURPIPE_MASK | UX_RZ_DCD_CFIFOSEL_ISEL);
            rz_register &= ( UX_RZ_DCD_CFIFOSEL_CURPIPE_MASK | UX_RZ_DCD_CFIFOSEL_ISEL);

            break;

        default                     :
            break;
        
    }
    
    /* According to the RZ spec, we need to wait for 450 ns now.  
       Reading a register takes 60 ns, so do this 8 times. */
        
    for (loop_index = 0; loop_index < 25; loop_index++) 
    {

        /* Just waste a little time ...  */
        rz_register =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_SYSCFG);
        rz_register =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_SYSSTS0);
    }

    return;
}

