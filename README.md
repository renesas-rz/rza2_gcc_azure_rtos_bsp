# RZ/A2M Microsoft Azure SDK

This is the RZ/A2M Software Development Kit with Microsoft Azure Support. The SDK contains the RZ/A2M Board Support Package and drivers for the RZ/A2M target Processors, the Asure RTOS ( Threadx ), Azure middleware ( USB Host, USB Device, Network, IoT, Filesystem and GUI Development ). In driver GPIO code generation has been integrated with the RZ/A2M Smart Configurator.

## Microsoft Azure Libraries

The [Microsoft Azure software](https://azure.microsoft.com/en-us/free/search/?&ef_id=EAIaIQobChMIo6et7eiu8AIVAh-tBh2tygWBEAAYAiAAEgIHcPD_BwE:G:s&OCID=AID2100131_SEM_EAIaIQobChMIo6et7eiu8AIVAh-tBh2tygWBEAAYAiAAEgIHcPD_BwE:G:s&gclid=EAIaIQobChMIo6et7eiu8AIVAh-tBh2tygWBEAAYAiAAEgIHcPD_BwE) is supported by the Azure Microsoft Team.  This repository includes the library debug binaries. The full source code for the listed libraries can be found [here](https://github.com/azure-rtos/).

License

- **Threadx** 
  <dt>This is the Microsoft Azure RTOS</dt>
  <dt>To learn more about [Azure RTOS ThreadX](https://docs.microsoft.com/azure/rtos/threadx/).</dt>

- **USBx**

  <dt>This is the USB middleware. It support both Host and Device. USB Class support include HID, MSC, CDC,   HUB, UVC, Audio, and printer.</dt>
  <dt>To learn more about [Azure RTOS USBX](https://docs.microsoft.com/azure/rtos/usbx/).</dt>

- **GUIx**

  <dt>This the GUI middleware. It supports 16, 24 and 32 bit color space. Resolution is dependent on the amount of internal memory ( i.e. 4MB RZ/A2M ) or external memory. </dt>
  <dt>To learn more about [Azure RTOS GUIX](https://docs.microsoft.com/azure/rtos/guix/).</dt>

- **FileX**

  <dt>This is the RTOS file system. Supports RAM disk, USB and SD.</dt>
  <dt>To learn more about [Azure RTOS FileX](https://docs.microsoft.com/azure/rtos/filex/).</dt>

- **NetXDuo**

  <dt>This middware supports the network for IP4, Ip6, TCP, UDP, HTTP, Secure Socket Layer, Crypto libraries, and many more.</dt>
  <dt>To learn more about [Azure RTOS NetX Duo](https://docs.microsoft.com/azure/rtos/netx-duo/).</dt>


- **Azure IoT**

  As part of the NetxDuo the Azure IoT Cloud connectivity is included.

## Microsoft Azure Demos

- **demo_filex**

  <dt>The "demo_filex" is a sample project to check the function of FileX.</dt>
  <dt>This sample project outputs the status to the console while writing a file called "TEST.TXT" to the media when the media is inserted into the Micro SD Card Slot of GR-MANGO.</dt>

- **demo_guix_home_automation**

  <dt>The "demo_guix_home_automation" is a sample project of GUIX and USBX. </dt>
  <dt>Please use the lower part of the GR-MANGO USB connector Type-A port (CN5).</dt>
  <dt>Also, connect the HDMI port of GR-MANGO to the monitor and check the display.</dt>

- **demo_guix_widget_types**

  <dt>The "demo_guix_widget_types" is a sample project of GUIX and USBX.</dt>
  <dt>Please use the lower part of the GR-MANGO USB connector Type-A port (CN5).</dt>
  <dt>Also, connect the HDMI port of GR-MANGO to the monitor and check the display.</dt>

- **demo_guix_washing_machine**

  <dt>The "demo_guix_washing_machine" is a sample project of GUIX and USBX.</dt>
  <dt>Please use the lower part of the GR-MANGO USB connector Type-A port (CN5).</dt>
  <dt>Also, connect the HDMI port of GR-MANGO to the monitor and check the display.</dt>

- **demo_netx**

  <dt>The "demo_netx" is a sample project of NetXDuo. Returns a response to a ping request from ping command.</dt>

  <dt>Note :</dt>
  <dt>The IP address of the board is set to 192.168.2.120 and the netmask is set to 255.255.255.0.
      If the operating network environment does not match the set IP address, change the IP address set with nx_ip_create () or change the IP address on your PC side.</dt>

- **demo_netx_http**

  <dt>The "demo_netx_http" is a sample project of NetX Duo HTTP server. Responds to HTTP requests from the browser.</dt>

  <dt>Note :</dt>
  <dt>The IP address of the board is set to 192.168.2.120 and the netmask is set to 255.255.255.0.
      If the operating network environment does not match the set IP address, change the IP address set with nx_ip_create () or change the IP address on your PC side.</dt>

- **demo_threadx**

  <dt>The "demo_threadx" is a sample project to check the operation of ThreadX.</dt>
  <dt>This sample project outputs a message to the console while switching between multiple threads.</dt>

- **demo_threadx_blinky**

  <dt>The "demo_threadx_blinky" is a sample project to check the operation of ThreadX.</dt>
  <dt>This sample project blinks the LEDs on the board at 0.5 second intervals.</dt>

- **demo_usbx_device_cdc_acm**

  <dt>The "demo_usbx_device_cdc_acm" is a sample project to confirm the CDC ACM function of USBX device.</dt>
  <dt>This sample project works as a CDC ACM function of USB, is recognized as a serial port by Windows, and
  echoes back the input characters. If the entered character is a line feed code, a message will be output to
  the console.</dt>

- **demo_usbx_host_hid**

  <dt>The "demo_usbx_host_hid" is a sample project of HID function of USBX host.</dt>
  <dt>Please use the lower part of the GR-MANGO USB connector Type-A port (CN5).</dt>

- **demo_usbx_host_mass**

  <dt>The "demo_usbx_host_mass" is a sample project of MSC function of USBX host.</dt>
  <dt>Please use the lower part of the GR-MANGO USB connector Type-A port (CN5).</dt>

- **demo_usbx_host_uvc**

  <dt>The "demo_usbx_host_uvc" is a USB Video Class (UVC) sample project for a USB X host.</dt>
  <dt>Please use the upper part of the GR-MANGO USB connector Type-A port (CN5).</dt>
  <dt>You can check the camera image with the browser on your PC.

  <dt>Note :</dt>
  <dt>The IP address of the board is set to 192.168.2.120 and the netmask is set to 255.255.255.0.
      If the operating network environment does not match the set IP address, change the IP address set with nx_ip_create () or change the IP address on your PC side.</dt>

- **demo_cam_and_disp**

  <dt>The Ågdemo_cam_and_dispÅh is a sample project to capture an image from the MIPI camera and display the captured image.</dt>
  <dt>Also, this sample project adjusts an image from the camera by the CUI (Character User Interface) tool.</dt>

- **demo_drp_basic_sample**
  <dt>The Ågdemo_drp_basic_sampleÅh is a sample project to check the basic function of DRP. </dt>
  <dt>This sample project converts the input image from MIPI camera to grayscale image using DRP Library and outputs to display.</dt>

- **demo_drp_dynamic_sample1**
  <dt>The Ågdemo_drp_dynamic_sample1Åh is a sample project to check the dynamic loading of DRP.</dt>
  <dt>This sample project detects the edges of the input image from MIPI camera by Canny method using DRP Library and outputs to display.</dt>

- **demo_drp_dynamic_sample2**
  <dt>The Ågdemo_drp_dynamic_sample2Åh is a sample project to check the dynamic loading of DRP.</dt>
  <dt>This sample project detects the corners of the input image from MIPI camera by the Harris corner detector using DRP Library and outputs to display.</dt>

- **demo_drp_dynamic_sample3**
  <dt>The Ågdemo_drp_dynamic_sample3Åh is a sample project to check the dynamic loading of DRP.</dt>
  <dt>This sample project performs contour detection using find contours application after detecting the edges of the input image from MIPI camera by Canny method using DRP Library. The results are output to display.</dt>

- **demo_drp_parallel_sample**
  <dt>The Ågdemo_drp_parallel_sampleÅh is a sample project to check the parallel operation of DRP.</dt>
  <dt>This sample project converts the input image from MIPI camera to grayscale image at high-speed using parallel operation function of DRP Library and outputs to display.</dt>

- **demo_drp_simple_isp_sample1**
  <dt>The Ågdemo_drp_simple_isp_sample1Åh is a sample project to check the ISP (Image Signal Processor) of DRP.</dt>
  <dt>This sample project corrects the color and noise of the input image from MIPI camera using DRP Library, and the image with high color reproducibility is output to the Display.</dt>


## Renesas RZ/A2M Microoft Azure SDK Board Configuration

This SDK is set up for the [GR-MANGO](https://www.renesas.com/jp/en/products/gadget-renesas/boards/gr-mango) board with RZ/A2M.

  <dt>Note :</dt>
  <dt>GR-MANGO is produced by CORE CORPORATION.</dt>

## Getting Started

[Demos Getting Started Guide](https://github.com/renesas-rz/rza2_gcc_azure_rtos_bsp/tree/main/docs)

Please refer to the following documents in the DOCS folder.

- r01qs0062jj0101-rza2m.pdf (JPN)
- r01qs0062ej0101-rza2m.pdf (ENG)
