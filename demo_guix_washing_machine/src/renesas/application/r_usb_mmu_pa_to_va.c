/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2014(2015) Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_usb_MMU_va_to_pa.c
* Description  : USB Host MSC application code
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
//#include "main.h"
#include "r_typedefs.h"


/***********************************************************************************************************************
Constant macro definitions
***********************************************************************************************************************/
#define	USB_MMU_VIRTUAL_PAGE_0  (0x82000000)  //Virtual address corresponding to 0x80000000
#define	USB_MMU_VIRTUAL_PAGE_1  (0x82100000)  //Virtual address corresponding to 0x80100000
#define	USB_MMU_VIRTUAL_PAGE_2  (0x82200000)  //Virtual address corresponding to 0x80200000
#define USB_MMU_VIRTUAL_PAGE_3  (0x82300000)  //Virtual address corresponding to 0x80300000


#define USB_MMU_MASK_TOP         (0xFFF00000)
#define USB_MMU_MASK_BOTTOM      (0x000FFFFF)


/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

uint32_t r_usbh0_pa_to_va(uint32_t paddr);
uint32_t r_usbh1_pa_to_va(uint32_t paddr);

/******************************************************************************
* Function Name: r_usbh0_pa_to_va
* Description  : Convert physical address to virtual address
* Arguments    : uint32_t   paddr  ; I : physical address to be convert
* Return Value : virtual address
******************************************************************************/
uint32_t r_usbh0_pa_to_va(uint32_t paddr)
{

		uint32_t     vaddr;

		switch(paddr & USB_MMU_MASK_TOP)
		{
		case 0x80000000:
			vaddr = (USB_MMU_VIRTUAL_PAGE_0 & USB_MMU_MASK_TOP) + (paddr & USB_MMU_MASK_BOTTOM);
			break;
		case 0x80100000:
			vaddr = (USB_MMU_VIRTUAL_PAGE_1 & USB_MMU_MASK_TOP) + (paddr & USB_MMU_MASK_BOTTOM);
			break;
		case 0x80200000:
			vaddr = (USB_MMU_VIRTUAL_PAGE_2 & USB_MMU_MASK_TOP) + (paddr & USB_MMU_MASK_BOTTOM);
			break;
		case 0x80300000:
			vaddr = (USB_MMU_VIRTUAL_PAGE_3 & USB_MMU_MASK_TOP) + (paddr & USB_MMU_MASK_BOTTOM);
			break;
		default:
			vaddr = paddr;
			break;
		}

	return vaddr;
} /* End of function r_usbh0_pa_to_va() */


/******************************************************************************
* Function Name: r_usbh1_pa_to_va
* Description  : Convert physical address to virtual address
* Arguments    : uint32_t   paddr  ; I : physical address to be convert
* Return Value : virtual address
******************************************************************************/
uint32_t r_usbh1_pa_to_va(uint32_t paddr)
{
	uint32_t     vaddr;

	switch(paddr & USB_MMU_MASK_TOP)
	{
	case 0x80000000:
		vaddr = (USB_MMU_VIRTUAL_PAGE_0 & USB_MMU_MASK_TOP) + (paddr & USB_MMU_MASK_BOTTOM);
		break;
	case 0x80100000:
		vaddr = (USB_MMU_VIRTUAL_PAGE_1 & USB_MMU_MASK_TOP) + (paddr & USB_MMU_MASK_BOTTOM);
		break;
	case 0x80200000:
		vaddr = (USB_MMU_VIRTUAL_PAGE_2 & USB_MMU_MASK_TOP) + (paddr & USB_MMU_MASK_BOTTOM);
		break;
	case 0x80300000:
		vaddr = (USB_MMU_VIRTUAL_PAGE_3 & USB_MMU_MASK_TOP) + (paddr & USB_MMU_MASK_BOTTOM);
		break;
	default:
		vaddr = paddr;
		break;
	}

	return vaddr;
} /* End of function r_usbh1_pa_to_va() */



/******************************************************************************
End of function usb_serial_input
******************************************************************************/


/***********************************************************************************************************************
End  Of File
***********************************************************************************************************************/
