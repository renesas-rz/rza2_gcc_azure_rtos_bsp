/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : syscalls.c
* Device(s)    : RZ/A2M
* Tool-Chain   : Arm Development Studio 2019.0-1
*              : Arm Compiler 6.12
* OS           : FreeRTOS Version See src/FreeRTOS/include/FreeRTOS.h
* H/W Platform : RZ/A2M Evaluation Board
* Description  :
******************************************************************************/
/******************************************************************************
* History      : DD.MM.YYYY Version Description
*              : 30.04.2020         ARM compatible
******************************************************************************/

#if defined( __ARMCC_VERSION )

#include <string.h>
#include <rt_sys.h>
#include "r_devlink_wrapper.h"
#include "r_typedefs.h"

#define STDIN_HANDLE   (0)
#define STDOUT_HANDLE  (1)
#define STDERR_HANDLE  (2)
#define INVALID_HANDLE (-1)

/*
 * These names are special strings which will be recognized by
 * _sys_open and will cause it to return the standard I/O handles, instead
 * of opening a real file.
 */
const char __stdin_name[]  = "\\\\.\\stdin";
const char __stdout_name[] = "\\\\.\\stdout";
const char __stderr_name[] = "\\\\.\\stderr";

static int monitor_stdin;
static int monitor_stdout;
static int monitor_stderr;

#ifndef _PARAMS
#define _PARAMS(paramlist)		paramlist
#endif

/* Forward prototypes.  */
void    initialise_monitor_handles _PARAMS ((void));

FILEHANDLE _sys_open(const char* name, int openmode);
FILEHANDLE open(const char* name, int openmode);
int _sys_close(FILEHANDLE fh);
int close(FILEHANDLE fh);
int _sys_write(FILEHANDLE fh, const unsigned char* buf, unsigned len, int mode);
int write(FILEHANDLE fh, const unsigned char* buf, unsigned len, int mode);
int _sys_read(FILEHANDLE fh, unsigned char* buf, unsigned len, int mode);
int read(FILEHANDLE fh, unsigned char* buf, unsigned len, int mode);
int _sys_istty(FILEHANDLE fh);
int _sys_seek(FILEHANDLE fh, long pos);
void _ttywrch(int ch);
int _sys_ensure(FILEHANDLE fh);
long _sys_flen(FILEHANDLE fh);
int _sys_tmpnam(char * name, int sig, unsigned maxlen);
void _sys_exit(int returncode);
char_t * _sys_command_string(char_t * cmd, int_t len);

static int _swiopen (const char * path, int flags);
static int_t get_file_handle(const char* name);
static int file_handle_convert_for_sys(FILEHANDLE fh);

void initialise_monitor_handles (void)
{
    /* Open the standard file descriptors by opening the special
     * teletype device, ":tt", read-only to obtain a descritpor for
     * standard input and write-only to obtain a descriptor for standard
     * output. Finally, open ":tt" in append mode to obtain a descriptor
     * for standard error. Since this is a write mode, most kernels will
     * probably return the same value as for standard output, but the
     * kernel can differentiate the two using the mode flag and return a
     * different descriptor for standard error.
     */

    int fh;
    fh = low_open((char *)__stdin_name, O_RDWR);
    monitor_stdin = fh;
    fh = low_open((char *)__stdout_name, O_WRONLY);
    monitor_stdout = fh;
    fh = low_open((char *)__stderr_name, O_WRONLY);
    monitor_stderr = fh;
    
    /* If we failed to open stderr, redirect to stdout. */
    if (monitor_stderr == -1)
    {
        monitor_stderr = monitor_stdout;
    }
}

static int _swiopen (const char * path, int flags)
{
  int aflags = 0, fh;

  /* The flags are Unix-style, so we need to convert them. */
#ifdef O_BINARY
  if (flags & O_BINARY)
    aflags |= 1;
#endif

  /* In O_RDONLY we expect aflags == 0. */
  if (flags & O_RDWR) {
    aflags |= O_RDWR;
  }
  if (flags & O_WRONLY) {
    aflags |= O_WRONLY;
  }
  if (flags & O_CREAT){
    aflags |= O_CREAT;
  }
  if (flags & O_TRUNC){
    aflags |= O_TRUNC;
  }
  if (flags & O_WRONLY){
    aflags |= O_WRONLY;
  }
  if (flags & O_APPEND){
    aflags |= O_APPEND;
  }

    fh = low_open((char *)path, flags);

    /* Return a user file descriptor or an error. */
    if (fh >= 0)
    {
      return (fh);
    }
    return (-1);
}


/******************************************************************************
* Function Name: _sys_open
* @brief         Open a file.
*
*                Description:<br>
*                May return -1 if the file failed to open.<br>
*                We do not require this function to do anything.<br>
*                Simply return a dummy handle.<br>
* @param         name       :name
* @param         openmode   :mode
* @retval        handle     :>=0 (success) -1 (fail)
******************************************************************************/
FILEHANDLE _sys_open(const char* name, int openmode)
{
    return (FILEHANDLE)get_file_handle(name);
}

FILEHANDLE open(const char* name, int openmode)
{
    return _swiopen(name, openmode);
}


/******************************************************************************
* Function Name: _sys_close
* @brief         Close a file.
*
*                Description:<br>
*                Should return 0 on success or a negative value on error.<br>
*                Not required in this implementation. Always return success.
* @param         fh        :not used
* @retval        0         :success
******************************************************************************/
int _sys_close(FILEHANDLE fh)
{
    int file_handle;

    /* If the file handle is different between the ARM compiler library and devlink,
     * conversion is required.
     */
    file_handle = file_handle_convert_for_sys(fh);
    
    /* Attempt to close the handle. */
    return low_close(file_handle);
}

int close(FILEHANDLE fh)
{
    /* Attempt to close the handle. */
    return low_close(fh);
}


/******************************************************************************
* Function Name: _sys_write
* @brief         Write a file.
*
*                Description:<br>
*                Returns 0 on success, negative on error, and the number<br>
*                of characters _not_ written on partial success.<br>
*                This implementation sends a buffer of size 'len' to the UART.
* @param         fh        :not used
* @param[in]     buf       :buffer address to write string
* @param[in]     len       :buffer length
* @param         mode      :not used
* @retval        0         :success
******************************************************************************/
int _sys_write(FILEHANDLE fh, const unsigned char* buf, unsigned int len, int mode)
{
    int ret;
    int file_handle;
    UNUSED_PARAM(mode);
    
    /* If the file handle is different between the ARM compiler library and devlink,
     * conversion is required.
     */
    file_handle = file_handle_convert_for_sys(fh);
    
    ret = low_write(file_handle, (uint8_t *)buf, (uint16_t)len);
    if(ret >= 0){
        ret = 0;
    }

    return ret;
}

int write(FILEHANDLE fh, const unsigned char* buf, unsigned len, int mode)
{
    UNUSED_PARAM(mode);
    return low_write(fh, (uint8_t *)buf, len);
}


/******************************************************************************
* Function Name: _sys_read
* Description  : ARMCLANG interface to low-level I/O read
* Arguments    : FILEHANDLE fh
*              : const char *buffer
*              : unsigned int n
* Return Value : none
******************************************************************************/
int _sys_read(FILEHANDLE fh, unsigned char* buf, unsigned len, int mode)
{
    int ret;
    int file_handle;
    UNUSED_PARAM(mode);
    
    /* If the file handle is different between the ARM compiler library and devlink,
     * conversion is required.
     */
    file_handle = file_handle_convert_for_sys(fh);
    
    ret = low_read(file_handle, (uint8_t *)buf, (uint32_t)mode);
    if(ret >= 0){
        ret = 0;
    }

    return ret;
}

int read(FILEHANDLE fh, unsigned char* buf, unsigned len, int mode)
{
    UNUSED_PARAM(mode);
    return low_read(fh, (uint8_t *)buf, len);
}


/******************************************************************************
* Function Name: _sys_istty
* @brief         Test whether a filehandle refers to a terminal.
*
*                Description:<br>
*                Return non-zero if the argument file is connected to<br>
*                a terminal.
* @param         fh        :not used
* @retval        1         :filehandle is terminal.
******************************************************************************/
int _sys_istty(FILEHANDLE fh)
{
	UNUSED_PARAM(fh);
    return  1;
}

/******************************************************************************
* Function Name: _sys_seek
* @brief         Move the file position to a given offset from the file start.
*
*                Description:<br>
*                Returns >=0 on success, <0 on failure.<br>
*                Seeking is not supported for the UART.
* @param         fh        :not used
* @param         pos       :not used
* @retval        -1        :failure.
******************************************************************************/
int _sys_seek(FILEHANDLE fh, long pos)
{
	UNUSED_PARAM(fh);
	UNUSED_PARAM(pos);

    return  0;
}

/******************************************************************************
* Function Name: _ttywrch
* @brief         Writes a character to the output channel.
*
*                Description:<br>
*                This function is used for last-resort error message output.
* @param[in]     ch        :a character to write.
* @retval        none
******************************************************************************/
void _ttywrch(int ch)
{
    UNUSED_PARAM(ch);
    return;
}

/******************************************************************************
* Function Name: _sys_ensure
* @brief         Flush any OS buffers associated with fh,<br>
*                ensuring that the file is up to date on disk.
*                Description:<br>
*                Result is >=0 if OK, negative for an error.
* @param         fh        :not used
* @retval        0         :success.
******************************************************************************/
int _sys_ensure(FILEHANDLE fh)
{
    UNUSED_PARAM(fh);
    /* success */
    return 0;
}

/******************************************************************************
* Function Name: _sys_flen
* @brief         Return the current length of a file.
*                Description:<br>
*                Returns <0 if an error occurred.<br>
*                _sys_flen is allowed to reposition the file pointer<br>
*                (so Unix can implement it with a single lseek,<br>
*                for example), since it is only called when processing<br>
*                SEEK_END relative fseeks, and therefore a call to<br>
*                _sys_flen is always followed by a call to _sys_seek.
* @param         fh        :not used
* @retval        0         :success.
******************************************************************************/
long _sys_flen(FILEHANDLE fh)
{
    UNUSED_PARAM(fh);
    return 0;
}

/******************************************************************************
* Function Name: _sys_tmpnam
* @brief         Return the name for temporary file number sig<br>
*                in the buffer name.
*                Description:<br>
*                Returns 0 on failure.<br>
*                maxlen is the maximum name length allowed.
* @param         name      :not used
* @param         sig       :not used
* @param         maxlen    :not used
* @retval        0         :failure.
******************************************************************************/
int _sys_tmpnam(char * name, int sig, unsigned maxlen)
{
    UNUSED_PARAM(* name);
    UNUSED_PARAM(sig);
    UNUSED_PARAM(maxlen);
    /* fail, not supported */
    return 0;
}

/******************************************************************************
* Function Name: _sys_exit
* @brief         Terminate the program, passing a return code back to the user.
*                Description:<br>
*                This function may not return.
* @param         returncode :not used
* @retval        none
******************************************************************************/
void _sys_exit(int returncode)
{
    UNUSED_PARAM(returncode);
    while(1) {};
}

/******************************************************************************
* Function Name: _sys_command_string
* Description  : Return a pointer to the command line used to invoke the program.
*              : The supplied buffer may be used to store the string, but need
*              : not be.
* Arguments    : char_t * cmd
*              : int_t len
* Return Value : char_t *
******************************************************************************/
char_t * _sys_command_string(char_t * cmd, int_t len)
{
    UNUSED_PARAM(len);
    return cmd;
}

/******************************************************************************
* Function Name: R_SYS_GetFileHandle
* @brief         Get File Handle
*
*                Description:<br>
*                Since the R_DEVLINK_FilePtrDescriptor cannot be used in 
*                the ARM compiler library, an API for obtaining the File Handle
*                from the syscall is prepared separately. 
* @param         name      :name
* @retval        handle    :File Handle 
******************************************************************************/
int_t R_SYS_GetFileHandle(const char* name)
{
    return get_file_handle(name);
}

/******************************************************************************
* Function Name: get_file_handle
* @brief         Get File Handle
*
*                Description:<br>
*                File handle acquisition function for stdio, stdout, stderr.
*                Handles other than the above are invalid.
* @param         name      :name
* @retval        handle    :File Handle 
******************************************************************************/
static int_t get_file_handle(const char* name)
{
    if(strcmp(name, __stdin_name) == 0){
        return STDIN_HANDLE;
    }
    else if(strcmp(name, __stdout_name) == 0){
        return STDOUT_HANDLE;
    }
    else if(strcmp(name, __stderr_name) == 0){
        return STDERR_HANDLE;
    }
    else{
        return INVALID_HANDLE;
    }
}


/******************************************************************************
* Function Name: file_handle_convert_for_sys
* @brief         Convert File Handle.
*
*                Description:<br>
*                If the stdin, stdout, and stderr file handles differ between
*                the ARM compiler library and devlink, replace them with
*                the devlink file handles.
* @param         fh        :File Handle
* @retval        handle    :File Handle after Convert
******************************************************************************/
static int file_handle_convert_for_sys(FILEHANDLE fh)
{
    int file_handle; 

    if((fh == 0) && (fh != monitor_stdin)){
        /* ARM compiler library:0, devlink: not 0 */
        file_handle = monitor_stdin; 
    }
    else if((fh == 1) && (fh != monitor_stdout)){
        /* ARM compiler library:1, devlink: not 1 */
        file_handle = monitor_stdout; 
    }
    else if((fh == 2) && (fh != monitor_stderr)){
        /* ARM compiler library:2, devlink: not 2 */
        file_handle = monitor_stderr; 
    }
    else{
        /* Other than above */
        file_handle = fh;
    }

    return file_handle;
}

/* End of File */

#endif // defined( __ARMCC_VERSION )
