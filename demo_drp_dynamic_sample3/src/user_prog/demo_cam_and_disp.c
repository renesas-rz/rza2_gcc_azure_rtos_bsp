/* This is a small demo DRP Dynamic Loading Sample Program 3.  */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "r_typedefs.h"
#include "iodefine.h"
#include "r_cpg_drv_api.h"
#include "r_ostm_drv_api.h"
#include "r_scifa_drv_api.h"
#include "r_gpio_drv_api.h"
#include "r_startup_config.h"
#include "compiler_settings.h"
#include "main.h"
#include "r_os_abstraction_api.h"
#include "version.h"
#include "tx_api.h"


/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define MAIN_PRV_LED_ON     (1)
#define MAIN_PRV_LED_OFF    (0)

/**********************************************************************************************************************
 Private global variables and functions
 *********************************************************************************************************************/
#define DEMO_STACK_SIZE         1024
#define DEMO_BYTE_POOL_SIZE     9120

/* Define the ThreadX object control blocks...  */
TX_THREAD               thread_0;
TX_BYTE_POOL            byte_pool_0;


/* Define thread prototypes.  */
void    thread_0_entry(ULONG thread_input);


/* Define what the initial system looks like.  */
void    tx_application_define(void *first_unused_memory)
{

CHAR    *pointer = TX_NULL;


    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", first_unused_memory, DEMO_BYTE_POOL_SIZE);

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    /* Allocate the stack for thread 0.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
}


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern void sample_main(void);

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static int_t gs_my_gpio_handle;

/* Terminal window escape sequences */
static const char_t * const sp_clear_screen = "\x1b[2J";
static const char_t * const sp_cursor_home  = "\x1b[H";

/* Define the test threads.  */
void    thread_0_entry(ULONG thread_input)
{
    st_os_abstraction_info_t ver_info;

    /* For information only
     * Use stdio calls to open drivers once  the kernel is initialised
     *
     * i.e.
     * int_t ostm3_handle;
     * ostm3_handle = open (DEVICE_INDENTIFIER "ostm2", O_RDWR);
     * close (ostm3_handle);
     */

    gs_my_gpio_handle = open(DEVICE_INDENTIFIER "gpio", O_RDWR);

    /* On error */
    if (gs_my_gpio_handle < 0)
    {
        /* stop execute */
        while (1)
        {
	        ;
        }
    }


    /* ==== Output banner message ==== */
    printf("%s%s", sp_clear_screen, sp_cursor_home);
    printf("RZ/A2M sample for GCC Ver. %u.%u\r\n", APPLICATION_INFO_VERSION, APPLICATION_INFO_RELEASE);
    printf("Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.\r\n");
    printf("Build Info Date %s at %s \r\n", __DATE__, __TIME__);

    if(R_OS_GetVersion(&ver_info) == 0)
    {
        printf("%s Version %d.%d\r\n", ver_info.p_szdriver_name,
                ver_info.version.sub.major, ver_info.version.sub.minor);
    }

    sample_main();

}

