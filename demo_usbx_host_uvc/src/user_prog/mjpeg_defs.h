/* UVC data defines for USBX */

#ifndef __MJPEG_DEFS_H__
#define __MJPEG_DEFS_H__

#define UVC_STREAM_HEADER_HEADERINFO_EOH       (1U << 7)
#define UVC_STREAM_HEADER_HEADERINFO_ERR       (1U << 6)
#define UVC_STREAM_HEADER_HEADERINFO_STI       (1U << 5)
#define UVC_STREAM_HEADER_HEADERINFO_RES       (1U << 4)
#define UVC_STREAM_HEADER_HEADERINFO_SCR       (1U << 3)
#define UVC_STREAM_HEADER_HEADERINFO_PTS       (1U << 2)
#define UVC_STREAM_HEADER_HEADERINFO_EOF       (1U << 1)
#define UVC_STREAM_HEADER_HEADERINFO_FID       (1U << 0)

typedef struct uvc_stream_header_struct
{
    UCHAR bHeaderLength;
    UCHAR bmHeaderInfo;
    UCHAR dwPresentationTime[4];
    UCHAR scrSourceClock_SourceTimeClock[4];
    UCHAR scrSourceClock_SOFCounter[2];
} uvc_stream_header_t;


typedef struct spec_of_camera
{
    int width;
    int height;
    int interval;
} SPEC_OF_CAMERA;


/* Define the number of buffers used in this demo. */ 
#define MAX_NUM_BUFFERS  45

typedef struct
{
    UCHAR* p_frame;
    ULONG length;
} FRAME_DATA;

extern FRAME_DATA frame_data[MAX_NUM_BUFFERS];


VOID uvc_frame_finish(UCHAR* p_frame);
int uvc_camera_spec_get(SPEC_OF_CAMERA** p_camera_spec, int* p_select);
void uvc_camera_spec_set(int select);
void uvc_camera_stop(void);
void uvc_camera_start(void);

ULONG display_image_get(UCHAR** image);
void display_image_release(void);
void mjpeg_thread_entry(ULONG thread_input);

#endif /* __MJPEG_DEFS_H__ */

