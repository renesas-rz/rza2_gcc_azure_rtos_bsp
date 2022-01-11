/* This is a small demo of the high-performance FileX FAT file system on the SD Card on the
   GR-MANGO board. Note that the SD Card should be formated prior to running this
   demonstration.  */

#include <stdio.h>
#include "tx_api.h"
#include "fx_api.h"

#include "fx_rz_sd_driver.h"

#define DEMO_STACK_SIZE         2048


/* Buffer for FileX FX_MEDIA sector cache. This must be large enough for at least one 
   sector, which are typically 512 bytes in size.  */

unsigned char media_memory[512*8];

/* Define FileX global data structures.  */

FX_MEDIA        sdio_disk;
FX_FILE         my_file;

/* Define ThreadX global data structures.  */

TX_THREAD       thread_0;
ULONG           thread_0_counter;

/* Define thread prototypes.  */

void    thread_0_entry(ULONG thread_input);

/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

CHAR    *pointer;

    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + DEMO_STACK_SIZE;

    /* Initialize FileX.  */
    fx_system_initialize();
}


LONG fat_sample_cd_int_cb_function(LONG sd_port, LONG cd)
{

}


void    thread_0_entry(ULONG thread_input)
{

UINT        status;
ULONG       actual;
CHAR        local_buffer[30];

    tx_thread_sleep(300);
    printf("\r\nRZ/A2M FileX Demonstration Program.\r\n\r\n");

    /* Loop to repeat the demo over and over!  */
    do
    {
        printf("\r\n**** FileX Demonstration ****\r\n");

        /* Open the SD.  */
        status =  fx_media_open(&sdio_disk, "RZ SDIO DISK", _fx_rz_sd_driver, 0, media_memory, sizeof(media_memory));

        /* Check the media open status.  */
        if (status != FX_SUCCESS)
        {

            /* Error, break the loop!  */
            break;
        }

        /* Create a file called TEST.TXT in the root directory.  */
        status =  fx_file_create(&sdio_disk, "TEST.TXT");

        /* Check the create status.  */
        if (status != FX_SUCCESS)
        {

            /* Check for an already created status. This is expected on the
               second pass of this loop!  */
            if (status != FX_ALREADY_CREATED)
            {

                /* Create error, break the loop.  */
                break;
            }
        }

        printf("    A \"TEST.TXT\" file created.\r\n");

        /* Open the test file.  */
        status =  fx_file_open(&sdio_disk, &my_file, "TEST.TXT", FX_OPEN_FOR_WRITE);

        /* Check the file open status.  */
        if (status != FX_SUCCESS)
        {

            /* Error opening file, break the loop.  */
            break;
        }

        /* Seek to the beginning of the test file.  */
        status =  fx_file_seek(&my_file, 0);

        /* Check the file seek status.  */
        if (status != FX_SUCCESS)
        {

            /* Error performing file seek, break the loop.  */
            break;
        }

        /* Write a string to the test file.  */
        status =  fx_file_write(&my_file, " ABCDEFGHIJKLMNOPQRSTUVWXYZ\n", 28);

        /* Check the file write status.  */
        if (status != FX_SUCCESS)
        {

            /* Error writing to a file, break the loop.  */
            break;
        }

        printf("    Data has been written to the file.\r\n");

        /* Seek to the beginning of the test file.  */
        status =  fx_file_seek(&my_file, 0);

        /* Check the file seek status.  */
        if (status != FX_SUCCESS)
        {

            /* Error performing file seek, break the loop.  */
            break;
        }

        /* Read the first 28 bytes of the test file.  */
        status =  fx_file_read(&my_file, local_buffer, 28, &actual);

        /* Check the file read status.  */
        if ((status != FX_SUCCESS) || (actual != 28))
        {

            /* Error reading file, break the loop.  */
            break;
        }

        printf("    It checked the data in the file.\r\n");

        /* Close the test file.  */
        status =  fx_file_close(&my_file);

        /* Check the file close status.  */
        if (status != FX_SUCCESS)
        {

            /* Error closing the file, break the loop.  */
            break;
        }

        /* Close the media.  */
        status =  fx_media_close(&sdio_disk);

        /* Check the media close status.  */
        if (status != FX_SUCCESS)
        {

            /* Error closing the media, break the loop.  */
            break;
        }

        /* Increment the thread counter, which represents the number
           of successful passes through this loop.  */
        thread_0_counter++;

		tx_thread_sleep(100);
    } while (1);

    /* If we get here the FileX test failed!  */
    return;
}

