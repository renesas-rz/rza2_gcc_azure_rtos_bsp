#include "r_intc_lld_rza2m.h"        /* INTC Driver Header   */
#include "iodefine.h"

/* Access to peripherals and board defines. */
#include "r_usbh0_basic_if.h"
#include "r_usbh0_hhid.h"
#include "r_usbh0_hhid_if.h"
#include "r_usbh0_hhid_config.h"
#include "r_usbh0_hhci.h"
#include "r_usbh0_hohci_typedef.h"

#include "r_usbh1_basic_if.h"
#include "r_usbh1_hhid.h"
#include "r_usbh1_hhid_if.h"
#include "r_usbh1_hhid_config.h"
#include "r_usbh1_hhci.h"
#include "r_usbh1_hohci_typedef.h"

/* Include files of USBX */
#include "ux_api.h"

#include "ux_rz_driver.h"

#include "ux_hcd_ehci.h"
#include "ux_hcd_ohci.h"


extern uint32_t usbh0_hstd_ohci_physical_address_of(void *data);
extern uint32_t usbh1_hstd_ohci_physical_address_of(void *data);

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
#define USB00_WAKEON_INT                0x00000010  /* Interrupt */
#define USB00_UCOM_INT                  0x00000008  /* Interrupt */
#define USB00_USBH_INTB                 0x00000004  /* EHCI interrupt */
#define USB00_USBH_INTA                 0x00000002  /* OHCI interrupt */
#define USB00_AHB_INT                   0x00000001  /* Bus master error interrupt */

#define USB10_WAKEON_INT                0x00000010  /* Interrupt */
#define USB10_UCOM_INT                  0x00000008  /* Interrupt */
#define USB10_USBH_INTB                 0x00000004  /* EHCI interrupt */
#define USB10_USBH_INTA                 0x00000002  /* OHCI interrupt */
#define USB10_AHB_INT                   0x00000001  /* Bus master error interrupt */

/* Time-out setting (msec) */
#define USBH0_OHCI_TIMEOUT              (3000u)
#define USBH1_OHCI_TIMEOUT              (3000u)

/***********************************************************************************************************************
 Private global variables and functions
 **********************************************************************************************************************/
static void _ux_rz_usbh0_cstd_PortInit(void);
static void _ux_rz_usbh1_cstd_PortInit(void);

static void _ux_rz_usbh0_ahb_pci_bridge_init(void);
static void _ux_rz_usbh1_ahb_pci_bridge_init(void);

static void _ux_rz_USBH0_Init(void);
static void _ux_rz_USBH1_Init(void);

static void _ux_rz_usbh0_module_init(void);
static void _ux_rz_usbh1_module_init(void);

static void _ux_rz_r_usbh0_hstd_hci_wait_time(uint32_t ms);
static void _ux_rz_r_usbh1_hstd_hci_wait_time(uint32_t ms);

static uint32_t _ux_rz_usbh0_hstd_ohci_hw_reset(void);
static uint32_t _ux_rz_usbh1_hstd_ohci_hw_reset(void);

static void _ux_rz_usbh0_hstd_ohci_hw_setup(void);
static void _ux_rz_usbh1_hstd_ohci_hw_setup(void);

static void _ux_rz_usbh0_hstd_hci_interrupt_handler(void);
static void _ux_rz_usbh1_hstd_hci_interrupt_handler(void);

static void _ux_rz_usbh0_hstd_ohci_interrupt_handler(void);
static void _ux_rz_usbh1_hstd_ohci_interrupt_handler(void);

static void _ux_rz_usbh0_hstd_ehci_interrupt_handler(void);
static void _ux_rz_usbh1_hstd_ehci_interrupt_handler(void);

static void _ux_rz_USBH0_isr(uint32_t int_sense);
static void _ux_rz_USBH1_isr(uint32_t int_sense);


#ifdef RZA2_USBH0_OHCI_IP
static void _ux_rz_usbh0_port_clean_seq(void);
#endif
#ifdef RZA2_USBH1_OHCI_IP
static void _ux_rz_usbh1_port_clean_seq(void);
#endif

/***********************************************************************************************************************
 * Function Name: ux_rz_usbh0_initialize
 * Description  : Initialize of USB0 host
 * Arguments    : none
 * Return Value : UX_SUCCESS
 **********************************************************************************************************************/
UINT ux_rz_usbh0_initialize(VOID)
{
	UINT rtn = UX_SUCCESS;

    _ux_rz_usbh0_cstd_PortInit();      /* Initialize USB Port0 */
    _ux_rz_USBH0_Init();
	_ux_rz_usbh0_hstd_ohci_hw_setup();

	return rtn;
}

/***********************************************************************************************************************
 * Function Name: ux_rz_usbh1_initialize
 * Description  : Initialize of USB1 host
 * Arguments    : none
 * Return Value : UX_SUCCESS
 **********************************************************************************************************************/
UINT ux_rz_usbh1_initialize(VOID)
{
	UINT rtn = UX_SUCCESS;

    _ux_rz_usbh1_cstd_PortInit();      /* Initialize USB Port1 */
    _ux_rz_USBH1_Init();
	_ux_rz_usbh1_hstd_ohci_hw_setup();

	return rtn;
}

/***********************************************************************************************************************
 * Function Name: ux_rz_usbh0_start
 * Description  : Start of USB0
 * Arguments    : none
 * Return Value : UX_SUCCESS
 **********************************************************************************************************************/
UINT ux_rz_usbh0_start(VOID)
{
    R_INTC_Enable(INTC_ID_USB_USBHI0);

	return UX_SUCCESS;
}

/***********************************************************************************************************************
 * Function Name: ux_rz_usbh1_start
 * Description  : Start of USB1
 * Arguments    : none
 * Return Value : UX_SUCCESS
 **********************************************************************************************************************/
UINT ux_rz_usbh1_start(VOID)
{
    R_INTC_Enable(INTC_ID_USB_USBHI1);

	return UX_SUCCESS;
}

/***********************************************************************************************************************
 * Function Name: ux_rz_usbh0_stop
 * Description  : Stop of USB0
 * Arguments    : none
 * Return Value : UX_SUCCESS
 **********************************************************************************************************************/
UINT ux_rz_usbh0_stop(VOID)
{
    R_INTC_Disable(INTC_ID_USB_USBHI0);

	return UX_SUCCESS;
}

/***********************************************************************************************************************
 * Function Name: ux_rz_usbh1_stop
 * Description  : Stop of USB1
 * Arguments    : none
 * Return Value : UX_SUCCESS
 **********************************************************************************************************************/
UINT ux_rz_usbh1_stop(VOID)
{
    R_INTC_Disable(INTC_ID_USB_USBHI1);

	return UX_SUCCESS;
}

/******************************************************************************
Function Name   : _ux_rz_usbh0_cstd_PortInit
Description     : Port Initialize
Argument        : none
Return          : none
******************************************************************************/
static void _ux_rz_usbh0_cstd_PortInit(void)
{
    /* VBUSEN0 : P1_0 *//* OVRCUR0 : PJ_5 */
    /* ID : PL2 *//* INT_N : PL3 */
    /* ---- PFS Register Write Protect : OFF  ---- */
    GPIO.PWPR.BIT.B0WI = 0;
    GPIO.PWPR.BIT.PFSWE = 1;

    GPIO.P10PFS.BIT.PSEL = 0x05;   /* ISEL = b'0, PSEL = b'101 */
    GPIO.PJ5PFS.BIT.PSEL = 0x03;   /* ISEL = b'0, PSEL = b'011 */

    /* ---- PFS Register Write Protect : ON ---- */
    GPIO.PWPR.BIT.PFSWE = 0x0;
    GPIO.PWPR.BIT.B0WI = 0x1;

    PORT1.PMR.BIT.PMR0 = 0x1;
    PORTJ.PMR.BIT.PMR5 = 0x1;
} /* End of function _ux_rz_usbh0_cstd_PortInit() */

/******************************************************************************
Function Name   : _ux_rz_usbh1_cstd_PortInit
Description     : Port Initialize
Argument        : none
Return          : none
******************************************************************************/
static void _ux_rz_usbh1_cstd_PortInit(void)
{
    /* VBUSEN1 : P1_2 *//* OVRCUR1 : PH_5(IRQ5) */
    /* ---- PFS Register Write Protect : OFF  ---- */
    GPIO.PWPR.BIT.B0WI = 0;
    GPIO.PWPR.BIT.PFSWE = 1;

    GPIO.P12PFS.BIT.PSEL = 0x05;   /* ISEL = b'0, PSEL = b'101 */
    GPIO.PH5PFS.BIT.PSEL = 0x05;   /* ISEL = b'0, PSEL = b'101 */

    /* ---- PFS Register Write Protect : ON ---- */
    GPIO.PWPR.BIT.PFSWE = 0x0;
    GPIO.PWPR.BIT.B0WI = 0x1;

    PORT1.PMR.BIT.PMR2 = 0x1;
    PORTH.PMR.BIT.PMR5 = 0x1;
} /* End of function _ux_rz_usbh1_cstd_PortInit() */

/******************************************************************************
* Function Name: _ux_rz_usbh0_ahb_pci_bridge_init
* Description  :
* Arguments    : none
* Return Value : none
******************************************************************************/
static void _ux_rz_usbh0_ahb_pci_bridge_init(void)
{
    USB00.INT_ENABLE.BIT.WAKEON_INTEN   = 1;
    USB00.INT_ENABLE.BIT.UCOM_INTEN     = 1;
    USB00.INT_ENABLE.BIT.USBH_INTBEN    = 1;
    USB00.INT_ENABLE.BIT.USBH_INTAEN    = 1;
    USB00.INT_ENABLE.BIT.AHB_INTEN      = 1;

    USB00.AHB_BUS_CTR.BIT.PROT_TYPE     = 0;
    USB00.AHB_BUS_CTR.BIT.PROT_MODE     = 0;
    USB00.AHB_BUS_CTR.BIT.ALIGN_ADDRESS = 0;
    USB00.AHB_BUS_CTR.BIT.MAX_BURST_LEN = 0;
} /* End of function _ux_rz_usbh0_ahb_pci_bridge_init() */

/******************************************************************************
* Function Name: _ux_rz_usbh1_ahb_pci_bridge_init
* Description  :
* Arguments    : none
* Return Value : none
******************************************************************************/
static void _ux_rz_usbh1_ahb_pci_bridge_init(void)
{

    USB10.INT_ENABLE.BIT.WAKEON_INTEN   = 1;
    USB10.INT_ENABLE.BIT.UCOM_INTEN     = 1;
    USB10.INT_ENABLE.BIT.USBH_INTBEN    = 1;
    USB10.INT_ENABLE.BIT.USBH_INTAEN    = 1;
    USB10.INT_ENABLE.BIT.AHB_INTEN      = 1;

    USB10.AHB_BUS_CTR.BIT.PROT_TYPE     = 0;
    USB10.AHB_BUS_CTR.BIT.PROT_MODE     = 0;
    USB10.AHB_BUS_CTR.BIT.ALIGN_ADDRESS = 0;
    USB10.AHB_BUS_CTR.BIT.MAX_BURST_LEN = 0;
} /* End of function _ux_rz_usbh1_ahb_pci_bridge_init() */

/***********************************************************************************************************************
Function Name   : _ux_rz_USBH0_Init
Description     :
Arguments       : none
Return          : none
***********************************************************************************************************************/
static void _ux_rz_USBH0_Init(void)
{
    _ux_rz_usbh0_module_init();        /* Start USB Port0 module */

#if(BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB)
    r_usbh0_tusb320hi_host_setting();    /* USB Port0 Setting */
#endif /* (BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB) */
} /* End of function _ux_rz_USBH0_Init() */

/***********************************************************************************************************************
Function Name   : _ux_rz_USBH1_Init
Description     :
Arguments       : none
Return          : none
***********************************************************************************************************************/
static void _ux_rz_USBH1_Init(void)
{
    _ux_rz_usbh1_module_init();        /* Start USB Port0 module */

#if(BSP_CFG_BOARD == BSP_CFG_BOARD_GR_MANGO)
    r_usbh1_tusb320_host_setting();    /* USB Port1 Setting */
#endif /* (BSP_CFG_BOARD == BSP_CFG_BOARD_GR_MANGO) */
} /* End of function _ux_rz_USBH1_Init() */

/***********************************************************************************************************************
* Function Name: _ux_rz_usbh0_module_init
* Description  :
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
static void _ux_rz_usbh0_module_init(void)
{
    volatile unsigned long  dummy;
    volatile uint8_t        dummy_buf;

    CPG.STBCR6.BYTE  &= 0xFD;                       /* Release module standby */
    dummy_buf   = CPG.STBCR6.BYTE;

#if(BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB)
    USB00.PHYCLK_CTRL.LONG = 0x00000001;            /* USBPHY select : USB_X1 */
#else
    USB00.PHYCLK_CTRL.LONG = 0x00000000;            /* USBPHY select : EXTAL */
#endif /* (BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB) */

    for(dummy = 0; dummy < (450 * 500); dummy++);   /* Wait 500us */

    USB00.COMMCTRL.BIT.OTG_PERI = 0;                /* Operation mode setting (0 : Host, 1 : Peri) */

    USB00.USBCTR.LONG &= ~0x00000002;               /* UTMI+PHY reset release */

    for(dummy = 0; dummy < (450 * 100); dummy++);   /* Wait 100us */

    /* AHB-PCI bridge Register setting */
    _ux_rz_usbh0_ahb_pci_bridge_init();

    /* Core defined register setting */
    USB00.REGEN_CG_CTRL.BIT.NONUSE_CLK_MSK  = 1;
    USB00.REGEN_CG_CTRL.BIT.HOST_CLK_MSK    = 0;
    USB00.REGEN_CG_CTRL.BIT.PERI_CLK_MSK    = 0;

    USB00.SPD_CTRL.BIT.SUSPENDM_ENABLE      = 0;
    USB00.SPD_CTRL.BIT.SLEEPM_ENABLE        = 0;
    USB00.SPD_CTRL.BIT.WKCNNT_ENABLE        = 0;
    USB00.SPD_CTRL.BIT.GLOBAL_SUSPENDM_P1   = 0;

    /* Interrupt setting */
    R_INTC_RegistIntFunc(INTC_ID_USB_USBHI0, _ux_rz_USBH0_isr);
    R_INTC_SetPriority(INTC_ID_USB_USBHI0, 5);
} /* End of function _ux_rz_usbh0_module_init() */

/***********************************************************************************************************************
* Function Name: _ux_rz_usbh1_module_init
* Description  :
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
static void _ux_rz_usbh1_module_init(void)
{
    volatile unsigned long  dummy;
    volatile uint8_t        dummy_buf;

    CPG.STBCR6.BYTE  &= 0xFE;                       /* Release module standby */
    dummy_buf   = CPG.STBCR6.BYTE;

#if(BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB)
    USB10.PHYCLK_CTRL.LONG = 0x00000001;            /* USBPHY select : USB_X1 */
#else
    USB10.PHYCLK_CTRL.LONG = 0x00000000;            /* USBPHY select : EXTAL */
#endif /* (BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB) */

    for(dummy = 0; dummy < (450 * 500); dummy++);   /* Wait 500us */

    USB10.COMMCTRL.BIT.OTG_PERI = 0;                /* Operation mode setting (0 : Host, 1 : Peri) */

    USB10.USBCTR.LONG &= ~0x00000002;               /* UTMI+PHY reset release */

    for(dummy = 0; dummy < (450 * 100); dummy++);   /* Wait 100us */

    /* AHB-PCI bridge Register setting */
    _ux_rz_usbh1_ahb_pci_bridge_init();

    /* Core defined register setting */
    USB10.REGEN_CG_CTRL.BIT.NONUSE_CLK_MSK  = 1;
    USB10.REGEN_CG_CTRL.BIT.HOST_CLK_MSK    = 0;
    USB10.REGEN_CG_CTRL.BIT.PERI_CLK_MSK    = 0;

    USB10.SPD_CTRL.BIT.SUSPENDM_ENABLE      = 0;
    USB10.SPD_CTRL.BIT.SLEEPM_ENABLE        = 0;
    USB10.SPD_CTRL.BIT.WKCNNT_ENABLE        = 0;
    USB10.SPD_CTRL.BIT.GLOBAL_SUSPENDM_P1   = 0;

    /* Interrupt setting */
    R_INTC_RegistIntFunc(INTC_ID_USB_USBHI1, _ux_rz_USBH1_isr);
    R_INTC_SetPriority(INTC_ID_USB_USBHI1, 5);
} /* End of function _ux_rz_usbh1_module_init() */

/* waittimer_function for usbhstd */
static void _ux_rz_r_usbh0_hstd_hci_wait_time(uint32_t ms)
{
    uint32_t    prv = USB00.HCFMNUMBER.LONG;

    while(ms)
    {
        if ((USB00.HCFMNUMBER.LONG - prv) != 0)
        {
            ms--;
            prv = USB00.HCFMNUMBER.LONG;
        }
    }
} /* End of function r_usbh0_hstd_hci_wait_time() */

/* waittimer_function for usbhstd */
static void _ux_rz_r_usbh1_hstd_hci_wait_time(uint32_t ms)
{
    uint32_t    prv = USB10.HCFMNUMBER.LONG;

    while(ms)
    {
        if ((USB10.HCFMNUMBER.LONG - prv) != 0)
        {
            ms--;
            prv = USB10.HCFMNUMBER.LONG;
        }
    }
} /* End of function r_usbh1_hstd_hci_wait_time() */

/***********************************************************************************************************************
 * Function     : Reset OHCI H/W
 * Declaration  : void _ux_rz_usbh0_hstd_ohci_hw_reset( void )
 * Argument     : void
 * Return       : uint32_t ; status
 * Note         :
***********************************************************************************************************************/
static uint32_t _ux_rz_usbh0_hstd_ohci_hw_reset(void)
{
    uint32_t    timeout;

    USB00.HCCOMMANDSTATUS.LONG = 0x00000001;  /* Reset HC */
    timeout = 0;
    while (USB00.HCCOMMANDSTATUS.LONG & 0x00000001)
    {
        if (timeout > USBH0_OHCI_TIMEOUT)
        {
            return USBH0_ERROR;
        }

        _ux_rz_r_usbh0_hstd_hci_wait_time(1);
        timeout++;
    }

    return USBH0_OK;
} /* End of function _ux_rz_usbh0_hstd_ohci_hw_reset() */

/***********************************************************************************************************************
 * Function     : Reset OHCI H/W
 * Declaration  : void _ux_rz_usbh1_hstd_ohci_hw_reset( void )
 * Argument     : void
 * Return       : uint32_t ; status
 * Note         :
***********************************************************************************************************************/
static uint32_t _ux_rz_usbh1_hstd_ohci_hw_reset(void)
{
    uint32_t    timeout;

    USB10.HCCOMMANDSTATUS.LONG = 0x00000001;  /* Reset HC */
    timeout = 0;
    while (USB10.HCCOMMANDSTATUS.LONG & 0x00000001)
    {
        if (timeout > USBH1_OHCI_TIMEOUT)
        {
            return USBH1_ERROR;
        }

        _ux_rz_r_usbh1_hstd_hci_wait_time(1);
        timeout++;
    }

    return USBH1_OK;
} /* End of function _ux_rz_usbh1_hstd_ohci_hw_reset() */

/***********************************************************************************************************************
 * Function     : OHCI H/W Setup
 * Declaration  : uint32_t _ux_rz_usbh0_hstd_ohci_hw_setup(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void _ux_rz_usbh0_hstd_ohci_hw_setup(void)
{
    uint32_t    n;
    uint32_t    portnum;
    uint32_t    numports;
    uint32_t    *p_port_status_reg;
    uint32_t    timeout;
    uint32_t    err;

    /* H/W reset */
    err = _ux_rz_usbh0_hstd_ohci_hw_reset();
    if (USBH0_OK != err)
    {
        return;
    }

    USB00.HCHCCA.LONG = usbh0_hstd_ohci_physical_address_of(usbh0_hstd_ohci_get_hcca_address());


    numports = USB00.HCRHDESCRIPTORA.LONG & 0x000000FF;

    /* Set USB-Operational state */
    USB00.HCCONTROL.LONG = (USB00.HCCONTROL.LONG & ~0x000000C0) | 0x00000080;

    /*  Clear HcInterruptStatus */
    USB00.HCINTERRUPTSTATUS.LONG = 0xFFFFFFFF;

    USB00.HCFMINTERVAL.LONG = 0x27782EDF;
    USB00.HCPERIODICSTART.LONG = 0x2A2F;

    /*  Set PowerControl-related Reg */
    USB00.HCRHDESCRIPTORA.LONG    = 0x02001902;   /* Enable Power-SW, Disable OverCurrent */
    for (n = 0; n < numports; n++)
    {
        USB00.HCRHDESCRIPTORB.LONG |= (0x00020000 << n);
    }

    for (portnum = 0; portnum < numports; portnum++)
    {
        p_port_status_reg = (uint32_t *)&(USB00.HCRHPORTSTATUS1.LONG);
        p_port_status_reg += portnum;

        *p_port_status_reg = 0x00000100;
        timeout = 0;
        while (*p_port_status_reg & 0x00000100 == 0x00000000)
        {
            if (timeout > USBH0_OHCI_TIMEOUT)
            {
                return;
            }

            _ux_rz_r_usbh0_hstd_hci_wait_time(1);
            timeout++;
        }
    }

    /* Enable OverCurrent */
    USB00.HCRHDESCRIPTORA.LONG &= ~0x00001000;

    for (portnum = 0; portnum < numports; portnum++)
    {
        p_port_status_reg = (uint32_t *)&(USB00.HCRHPORTSTATUS1.LONG);
        p_port_status_reg += portnum;
        *p_port_status_reg = 0xFFFF0000;
    }

    /*  Enable MasterInt,RHSC,FNO,WBDH */
    USB00.HCINTERRUPTENABLE.LONG = 0x80000062;

    /* Control,Bulk,Iso,Periodic List Enable */
    USB00.HCCONTROL.LONG |= 0x0000003C;

    return;
} /* End of function _ux_rz_usbh0_hstd_ohci_hw_setup() */

/***********************************************************************************************************************
 * Function     : OHCI H/W Setup
 * Declaration  : uint32_t _ux_rz_usbh1_hstd_ohci_hw_setup(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void _ux_rz_usbh1_hstd_ohci_hw_setup(void)
{
    uint32_t    n;
    uint32_t    portnum;
    uint32_t    numports;
    uint32_t    *p_port_status_reg;
    uint32_t    timeout;
    uint32_t    err;

    /* H/W reset */
    err = _ux_rz_usbh1_hstd_ohci_hw_reset();
    if (USBH1_OK != err)
    {
        return;
    }

    USB10.HCHCCA.LONG = usbh1_hstd_ohci_physical_address_of(usbh1_hstd_ohci_get_hcca_address());


    numports = USB10.HCRHDESCRIPTORA.LONG & 0x000000FF;

    /* Set USB-Operational state */
    USB10.HCCONTROL.LONG = (USB10.HCCONTROL.LONG & ~0x000000C0) | 0x00000080;

    /*  Clear HcInterruptStatus */
    USB10.HCINTERRUPTSTATUS.LONG = 0xFFFFFFFF;

    USB10.HCFMINTERVAL.LONG = 0x27782EDF;
    USB10.HCPERIODICSTART.LONG = 0x2A2F;

    /*  Set PowerControl-related Reg */
    USB10.HCRHDESCRIPTORA.LONG    = 0x02001902;   /* Enable Power-SW, Disable OverCurrent */
    for (n = 0; n < numports; n++)
    {
        USB10.HCRHDESCRIPTORB.LONG |= (0x00020000 << n);
    }

    for (portnum = 0; portnum < numports; portnum++)
    {
        p_port_status_reg = (uint32_t *)&(USB10.HCRHPORTSTATUS1.LONG);
        p_port_status_reg += portnum;

        *p_port_status_reg = 0x00000100;
        timeout = 0;
        while (*p_port_status_reg & 0x00000100 == 0x00000000)
        {
            if (timeout > USBH1_OHCI_TIMEOUT)
            {
                return;
            }

            _ux_rz_r_usbh1_hstd_hci_wait_time(1);
            timeout++;
        }
    }

#if(BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB)
    /* Enable OverCurrent */
    USB10.HCRHDESCRIPTORA.LONG &= ~0x00001000;
#endif /* (BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB) */

    for (portnum = 0; portnum < numports; portnum++)
    {
        p_port_status_reg = (uint32_t *)&(USB10.HCRHPORTSTATUS1.LONG);
        p_port_status_reg += portnum;
        *p_port_status_reg = 0xFFFF0000;
    }

    /*  Enable MasterInt,RHSC,FNO,WBDH */
    USB10.HCINTERRUPTENABLE.LONG = 0x80000062;

    /* Control,Bulk,Iso,Periodic List Enable */
    USB10.HCCONTROL.LONG |= 0x0000003C;

    return;
} /* End of function _ux_rz_usbh1_hstd_ohci_hw_setup() */

/***********************************************************************************************************************
 * Function     : Interrupt Routine for HCI
 * Declaration  : void _ux_rz_usbh0_hstd_hci_interrupt_handler(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void _ux_rz_usbh0_hstd_hci_interrupt_handler(void)
{
    /* Check ehci */
    _ux_rz_usbh0_hstd_ehci_interrupt_handler();

    /* Check ohci */
    _ux_rz_usbh0_hstd_ohci_interrupt_handler();
} /* End of function _ux_rz_usbh0_hstd_hci_interrupt_handler() */

/***********************************************************************************************************************
 * Function     : Interrupt Routine for HCI
 * Declaration  : void _ux_rz_usbh1_hstd_hci_interrupt_handler(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void _ux_rz_usbh1_hstd_hci_interrupt_handler(void)
{
    /* Check ehci */
    _ux_rz_usbh1_hstd_ehci_interrupt_handler();

    /* Check ohci */
    _ux_rz_usbh1_hstd_ohci_interrupt_handler();
} /* End of function _ux_rz_usbh1_hstd_hci_interrupt_handler() */


/***********************************************************************************************************************
 * Function     : OHCI Detect interrupt
 * Declaration  : void _ux_rz_usbh0_hstd_ohci_interrupt_handler( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void _ux_rz_usbh0_hstd_ohci_interrupt_handler(void)
{
    uint32_t                        hc_int_enable;
    uint32_t                        hc_int_status;

	_ux_hcd_ohci_interrupt_handler();

#if 1
    hc_int_status = USB00.HCINTERRUPTSTATUS.LONG;
    hc_int_enable = USB00.HCINTERRUPTENABLE.LONG;
    /* It is our interrupt, prevent HC from doing it to us again until we are finished */

    USB00.HCINTERRUPTSTATUS.LONG = (~USBH0_OHCI_IS_WRITEBACKDONEHEAD & hc_int_status);
#endif
} /* End of function _ux_rz_usbh0_hstd_ohci_interrupt_handler() */

/***********************************************************************************************************************
 * Function     : OHCI Detect interrupt
 * Declaration  : void _ux_rz_usbh1_hstd_ohci_interrupt_handler( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void _ux_rz_usbh1_hstd_ohci_interrupt_handler(void)
{
    uint32_t                        hc_int_enable;
    uint32_t                        hc_int_status;

	_ux_hcd_ohci_interrupt_handler();

#if 1
    hc_int_status = USB10.HCINTERRUPTSTATUS.LONG;
    hc_int_enable = USB10.HCINTERRUPTENABLE.LONG;
    /* It is our interrupt, prevent HC from doing it to us again until we are finished */

    USB10.HCINTERRUPTSTATUS.LONG = (~USBH1_OHCI_IS_WRITEBACKDONEHEAD & hc_int_status);
#endif
} /* End of function _ux_rz_usbh1_hstd_ohci_interrupt_handler() */

/***********************************************************************************************************************
 * Function     : EHCI Interrupt Handler
 * Declaration  : void _ux_rz_usbh0_hstd_ehci_interrupt_handler(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void _ux_rz_usbh0_hstd_ehci_interrupt_handler(void)
{
    if (0 != USB00.USBSTS.LONG && 0 != USB00.USBINTR.LONG)
    {
        if ((1 == USB00.USBSTS.BIT.HostSystemError) && (1 == USB00.USBINTR.BIT.HostSystemErrorEnable))
        {
			_ux_hcd_ehci_interrupt_handler();
            /* USB00.USBSTS.BIT.HostSystemError = 1; */
            USB00.USBSTS.LONG &= 0x0001F010;
        }

        if ((1 == USB00.USBSTS.BIT.PortChangeDetect) && (1 == USB00.USBINTR.BIT.PortChangeDetectEnable))
        {
			_ux_hcd_ehci_interrupt_handler();
            /* USB00.USBSTS.BIT.PortChangeDetect = 1; */
            USB00.USBSTS.LONG &= 0x0001F004;
        }

        if ((1 == USB00.USBSTS.BIT.USBERRINT) && (1 == USB00.USBINTR.BIT.USBErrorInterruptEnable))
        {
			_ux_hcd_ehci_interrupt_handler();
            /* USB00.USBSTS.BIT.USBERRINT = 1; */
            USB00.USBSTS.LONG &= 0x0001F0002;
        }

        if ((1 == USB00.USBSTS.BIT.InterruptonAsyncAdvance) && (1 == USB00.USBINTR.BIT.InterruptonAsyncAdvanceEnable))
        {
			_ux_hcd_ehci_interrupt_handler();
            /* USB00.USBSTS.BIT.InterruptonAsyncAdvance = 1; */
            USB00.USBSTS.LONG &= 0x0001F020;
        }

        if ((1 == USB00.USBSTS.BIT.FrameListRollover) && (1 == USB00.USBINTR.BIT.FrameListRolloverEnable))
        {
			_ux_hcd_ehci_interrupt_handler();
            /* USB00.USBSTS.BIT.FrameListRollover = 1; */
            USB00.USBSTS.LONG &= 0x0001F008;
        }

        if ((1 == USB00.USBSTS.BIT.USBINT) && (1 == USB00.USBINTR.BIT.USBInterruptEnable))
        {
			_ux_hcd_ehci_interrupt_handler();
        	/* USB00.USBSTS.BIT.USBINT = 1; */
        	USB00.USBSTS.LONG &= 0x0001F001;
        }
    }
} /* End of function _ux_rz_usbh0_hstd_ehci_interrupt_handler() */

/***********************************************************************************************************************
 * Function     : EHCI Interrupt Handler
 * Declaration  : void _ux_rz_usbh1_hstd_ehci_interrupt_handler(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void _ux_rz_usbh1_hstd_ehci_interrupt_handler(void)
{
    if (0 != USB10.USBSTS.LONG && 0 != USB10.USBINTR.LONG)
    {
        if ((1 == USB10.USBSTS.BIT.HostSystemError) && (1 == USB10.USBINTR.BIT.HostSystemErrorEnable))
        {
			_ux_hcd_ehci_interrupt_handler();
            /* USB10.USBSTS.BIT.HostSystemError = 1; */
            USB10.USBSTS.LONG &= 0x0001F010;
        }

        if ((1 == USB10.USBSTS.BIT.PortChangeDetect) && (1 == USB10.USBINTR.BIT.PortChangeDetectEnable))
        {
			_ux_hcd_ehci_interrupt_handler();
            /* USB10.USBSTS.BIT.PortChangeDetect = 1; */
            USB10.USBSTS.LONG &= 0x0001F004;
        }

        if ((1 == USB10.USBSTS.BIT.USBERRINT) && (1 == USB10.USBINTR.BIT.USBErrorInterruptEnable))
        {
			_ux_hcd_ehci_interrupt_handler();
            /* USB10.USBSTS.BIT.USBERRINT = 1; */
            USB10.USBSTS.LONG &= 0x0001F0002;
        }

        if ((1 == USB10.USBSTS.BIT.InterruptonAsyncAdvance) && (1 == USB10.USBINTR.BIT.InterruptonAsyncAdvanceEnable))
        {
			_ux_hcd_ehci_interrupt_handler();
            /* USB10.USBSTS.BIT.InterruptonAsyncAdvance = 1; */
            USB10.USBSTS.LONG &= 0x0001F020;
        }

        if ((1 == USB10.USBSTS.BIT.FrameListRollover) && (1 == USB10.USBINTR.BIT.FrameListRolloverEnable))
        {
			_ux_hcd_ehci_interrupt_handler();
            /* USB10.USBSTS.BIT.FrameListRollover = 1; */
            USB10.USBSTS.LONG &= 0x0001F008;
        }

        if ((1 == USB10.USBSTS.BIT.USBINT) && (1 == USB10.USBINTR.BIT.USBInterruptEnable))
        {
			_ux_hcd_ehci_interrupt_handler();
        	/* USB10.USBSTS.BIT.USBINT = 1; */
        	USB10.USBSTS.LONG &= 0x0001F001;
        }
    }
} /* End of function _ux_rz_usbh1_hstd_ehci_interrupt_handler() */

/***********************************************************************************************************************
 * Function Name: _ux_rz_USBH0_isr
 * Description  : Interrupt service routine of USB host
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
static void _ux_rz_USBH0_isr(uint32_t int_sense)
{
    uint32_t    int_state;

    int_state = USB00.INT_STATUS.LONG;
    if (USB00_AHB_INT == (int_state & USB00_AHB_INT))
    {
        USB00.INT_STATUS.LONG |= USB00_AHB_INT;
    }
    else if ((USB00_USBH_INTA == (int_state & USB00_USBH_INTA)) || (USB00_USBH_INTB == (int_state & USB00_USBH_INTB)))  /* EHCI or OHCI interrupt */
    {
        _ux_rz_usbh0_hstd_hci_interrupt_handler();
    }
    else if (USB00_UCOM_INT == (int_state & USB00_UCOM_INT))
    {
    }
    else if (USB00_WAKEON_INT == (int_state & USB00_WAKEON_INT))
    {
        USB00.INT_STATUS.LONG |= USB00_WAKEON_INT;
    }
    else
    {
    }
} /* End of function _ux_rz_USBH0_isr() */

/***********************************************************************************************************************
 * Function Name: _ux_rz_USBH1_isr
 * Description  : Interrupt service routine of USB host
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
static void _ux_rz_USBH1_isr(uint32_t int_sense)
{
    uint32_t    int_state;

    int_state = USB10.INT_STATUS.LONG;
    if (USB10_AHB_INT == (int_state & USB10_AHB_INT))
    {
        USB10.INT_STATUS.LONG |= USB10_AHB_INT;
    }
    else if ((USB10_USBH_INTA == (int_state & USB10_USBH_INTA)) || (USB10_USBH_INTB == (int_state & USB10_USBH_INTB)))  /* EHCI or OHCI interrupt */
    {
        _ux_rz_usbh1_hstd_hci_interrupt_handler();
    }
    else if (USB10_UCOM_INT == (int_state & USB10_UCOM_INT))
    {
    }
    else if (USB10_WAKEON_INT == (int_state & USB10_WAKEON_INT))
    {
        USB10.INT_STATUS.LONG |= USB10_WAKEON_INT;
    }
    else
    {
    }
} /* End of function _ux_rz_USBH1_isr() */



#ifdef RZA2_USBH0_OHCI_IP
static void _ux_rz_usbh0_port_clean_seq(void)
{
	if(0 == USB00.HCRHDESCRIPTORA.BIT.NPS)
	{
		if(USB00.HCRHDESCRIPTORA.BIT.PSM & USB00.HCRHDESCRIPTORB.BIT.PPCM)
		{
			/* A */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000200; /* LSDA = 1 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000008; /* POCI = 1 */
			USB00.PORTSC1.LONG         |= 0x00002000; /* PortOwner = 1 */
			USB00.PORTSC1.LONG         &= 0xFFFFDFFF; /* PortOwner = 0 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000100; /* POCI = 1 */
		}
		else
		{
			/* B */
			USB00.HCRHSTATUS.LONG      |= 0x00000001; /* PPS  = 1 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000008; /* POCI = 1 */
			USB00.PORTSC1.LONG         |= 0x00002000; /* PortOwner = 1 */
			USB00.PORTSC1.LONG         &= 0xFFFFDFFF; /* PortOwner = 0 */
			USB00.HCRHSTATUS.LONG      |= 0x00010000; /* LPSC = 1 */
		}
	}
	else
	{
		if(USB00.HCRHDESCRIPTORA.BIT.PSM & USB00.HCRHDESCRIPTORB.BIT.PPCM)
		{
			/* C */
			USB00.HCRHDESCRIPTORA.LONG &= 0xFFFFFDFF; /* NPS  = 0 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000200; /* LSDA = 1 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000008; /* POCI = 1 */
			USB00.PORTSC1.LONG         |= 0x00002000; /* PortOwner = 1 */
			USB00.PORTSC1.LONG         &= 0xFFFFDFFF; /* PortOwner = 0 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000100; /* POCI = 1 */
			USB00.HCRHDESCRIPTORA.LONG |= 0x00000200; /* NPS  = 0 */
		}
		else
		{
			/* D */
			USB00.HCRHDESCRIPTORA.LONG &= 0xFFFFFDFF; /* NPS  = 0 */
			USB00.HCRHSTATUS.LONG      |= 0x00000001; /* PPS  = 1 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000008; /* POCI = 1 */
			USB00.PORTSC1.LONG         |= 0x00002000; /* PortOwner = 1 */
			USB00.PORTSC1.LONG         &= 0xFFFFDFFF; /* PortOwner = 0 */
			USB00.HCRHSTATUS.LONG      |= 0x00010000; /* LPSC = 1 */
			USB00.HCRHDESCRIPTORA.LONG |= 0x00000200; /* NPS  = 0 */
		}
	}
}
#endif /* RZA2_USBH0_OHCI_IP */

#ifdef RZA2_USBH1_OHCI_IP
static void _ux_rz_usbh1_port_clean_seq(void)
{
	if(0 == USB10.HCRHDESCRIPTORA.BIT.NPS)
	{
		if(USB10.HCRHDESCRIPTORA.BIT.PSM & USB10.HCRHDESCRIPTORB.BIT.PPCM)
		{
			/* A */
			USB10.HCRHPORTSTATUS1.LONG |= 0x00000200; /* LSDA = 1 */
			USB10.HCRHPORTSTATUS1.LONG |= 0x00000008; /* POCI = 1 */
			USB10.PORTSC1.LONG         |= 0x00002000; /* PortOwner = 1 */
			USB10.PORTSC1.LONG         &= 0xFFFFDFFF; /* PortOwner = 0 */
			USB10.HCRHPORTSTATUS1.LONG |= 0x00000100; /* POCI = 1 */
		}
		else
		{
			/* B */
			USB10.HCRHSTATUS.LONG      |= 0x00000001; /* PPS  = 1 */
			USB10.HCRHPORTSTATUS1.LONG |= 0x00000008; /* POCI = 1 */
			USB10.PORTSC1.LONG         |= 0x00002000; /* PortOwner = 1 */
			USB10.PORTSC1.LONG         &= 0xFFFFDFFF; /* PortOwner = 0 */
			USB10.HCRHSTATUS.LONG      |= 0x00010000; /* LPSC = 1 */
		}
	}
	else
	{
		if(USB10.HCRHDESCRIPTORA.BIT.PSM & USB10.HCRHDESCRIPTORB.BIT.PPCM)
		{
			/* C */
			USB10.HCRHDESCRIPTORA.LONG &= 0xFFFFFDFF; /* NPS  = 0 */
			USB10.HCRHPORTSTATUS1.LONG |= 0x00000200; /* LSDA = 1 */
			USB10.HCRHPORTSTATUS1.LONG |= 0x00000008; /* POCI = 1 */
			USB10.PORTSC1.LONG         |= 0x00002000; /* PortOwner = 1 */
			USB10.PORTSC1.LONG         &= 0xFFFFDFFF; /* PortOwner = 0 */
			USB10.HCRHPORTSTATUS1.LONG |= 0x00000100; /* POCI = 1 */
			USB10.HCRHDESCRIPTORA.LONG |= 0x00000200; /* NPS  = 0 */
		}
		else
		{
			/* D */
			USB10.HCRHDESCRIPTORA.LONG &= 0xFFFFFDFF; /* NPS  = 0 */
			USB10.HCRHSTATUS.LONG      |= 0x00000001; /* PPS  = 1 */
			USB10.HCRHPORTSTATUS1.LONG |= 0x00000008; /* POCI = 1 */
			USB10.PORTSC1.LONG         |= 0x00002000; /* PortOwner = 1 */
			USB10.PORTSC1.LONG         &= 0xFFFFDFFF; /* PortOwner = 0 */
			USB10.HCRHSTATUS.LONG      |= 0x00010000; /* LPSC = 1 */
			USB10.HCRHDESCRIPTORA.LONG |= 0x00000200; /* NPS  = 0 */
		}

	}
}
#endif /* RZA2_OHCI_IP */

