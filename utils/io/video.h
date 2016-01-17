// Created by alvaregd on 07/12/15.

#ifndef WATER_REFLECTION_VIDEO_H
#define WATER_REFLECTION_VIDEO_H

#include <platform/glfw_launcher.h>

#define SCREENSHOT_FILE "./build/screenshot.png"

/** Holds Information about how to capture video */
struct Video{
    unsigned char* g_video_memory_start = NULL; //pointer to start to of video buffer
    unsigned char* g_video_memory_ptr = NULL;   //pointer to any frame in the video
    int g_video_seconds_total = 10;              //total frame time
    int g_video_fps = 25;                       //number of frames per second

    //timers
    bool dump_video = false;
    double video_timer = 0.0; // time video has been recording
    double video_dump_timer = 0.0; // timer for next frame grab
    double frame_time = 0.04; // 1/25 seconds of time

};
/** Reserve a chunk of memory to hold frames for the video */
void reserve_video_memory (Video* video, Window * hardware);

/** Create a video frame from the framebuffer **/
void grab_video_frame (Video *video, Window * hardware) ;

/** multi-threaded version of grab_videp_frame **/
bool dump_video_frame_parallel(Window *window, long int frameNumber, unsigned char* address);

/** generate a frame file **/
bool dump_video_frame(Video *video, Window *hardware);

/** Wrapper function to generate many frames files  **/
bool dump_video_frames(Video *video, Window *window);

/** Take a snapshot frame and create a file immediately **/
bool screencapture (Window * hardware) ;

/** update the frame count information**/
bool videoUpdateTimer(Video* video,double* elapsed_seconds );

#endif //WATER_REFLECTION_VIDEO_H
