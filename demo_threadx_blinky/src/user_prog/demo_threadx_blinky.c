/* This is a small demo(blinky) of the high-performance ThreadX kernel.  */

#include <stdio.h>
#include "tx_api.h"

#include "r_typedefs.h"
#include "iodefine.h"
#include "r_gpio_drv_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define MAIN_PRV_LED_ON     (1)
#define MAIN_PRV_LED_OFF    (0)

/**********************************************************************************************************************
 Private global variables and functions
 *********************************************************************************************************************/
static uint32_t s_main_led_flg;      /* LED lighting/turning off */
static int_t s_my_gpio_handle;
static st_r_drv_gpio_pin_rw_t s_p82_hi =
{
    GPIO_PORT_8_PIN_2,
    GPIO_LEVEL_HIGH,
    GPIO_SUCCESS
};
static st_r_drv_gpio_pin_rw_t s_p82_lo =
{
    GPIO_PORT_8_PIN_2,
    GPIO_LEVEL_LOW,
    GPIO_SUCCESS
};
static const r_gpio_port_pin_t s_led_pin_list[] =
{
    GPIO_PORT_8_PIN_2,
};

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


/* Define the test threads.  */
void    thread_0_entry(ULONG thread_input)
{
    int_t err;
    st_r_drv_gpio_pin_list_t pin_led;

    /* For information only
     * Use stdio calls to open drivers once  the kernel is initialised
     *
     * i.e.
     * int_t ostm3_handle;
     * ostm3_handle = open (DEVICE_INDENTIFIER "ostm2", O_RDWR);
     * close (ostm3_handle);
     */

    s_my_gpio_handle = open(DEVICE_INDENTIFIER "gpio", O_RDWR);

    /* On error */
    if (s_my_gpio_handle < 0)
    {
        /* stop execute */
        while (1)
        {
	        ;
        }
    }

    /**************************************************
     * Initialise P6_0 pin parameterised in GPIO_SC_TABLE_MANUAL
     **************************************************/
    pin_led.p_pin_list = s_led_pin_list;
    pin_led.count = (sizeof(s_led_pin_list)) / (sizeof(s_led_pin_list[0]));
    err = direct_control(s_my_gpio_handle, CTL_GPIO_INIT_BY_PIN_LIST, &pin_led);

    /* On error */
    if (err < 0)
    {
        /* stop execute */
        while (1)
        {
            ;
        }
    }

	while(1)
	{
        /* ==== LED blink ==== */
        s_main_led_flg ^= 1;

        if (MAIN_PRV_LED_ON == s_main_led_flg)
        {
            direct_control(s_my_gpio_handle, CTL_GPIO_PIN_WRITE, &s_p82_hi);
        }
        else
        {
            direct_control(s_my_gpio_handle, CTL_GPIO_PIN_WRITE, &s_p82_lo);
        }

        /* Sleep for 50 ticks.  */
        tx_thread_sleep(50);
    }
}

