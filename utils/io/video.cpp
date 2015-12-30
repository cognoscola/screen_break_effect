//
// Created by alvaregd on 07/12/15.
//

#include "video.h"
#include "stb_image_write.h"
#include <pthread.h>

#define NUM_THREADS 8

void reserve_video_memory (Video* video, Window * hardware) {
    // 480 MB at 800x800 resolution 230.4 MB at 640x480 resolution
    video->g_video_memory_ptr = (unsigned char*)malloc (
            hardware->vmode->width * hardware->vmode->height * 3 * video->g_video_fps * video->g_video_seconds_total
    );
    video->g_video_memory_start = video->g_video_memory_ptr;
}

void grab_video_frame (Video* video, Window * hardware) {
    // copy frame-buffer into 24-bit rgbrgb...rgb image
    glReadPixels (
            0, 0, hardware->vmode->width, hardware->vmode->height, GL_RGB, GL_UNSIGNED_BYTE, video->g_video_memory_ptr
    );
    // move video pointer along to the next frame's worth of bytes
    video->g_video_memory_ptr += hardware->vmode->width * hardware->vmode->height * 3;
}

bool dump_video_frame (Video* video,  Window * hardware) {
    static long int frame_number = 0;
    printf ("writing video frame %li\n", frame_number);
    // write into a file
    char name[1024];
    sprintf (name, "/home/alvaregd/Documents/Games/water_reflection/video/video_frame_%03ld.png", frame_number);

    unsigned char* last_row = video->g_video_memory_ptr +
                              (hardware->vmode->width * 3 * (hardware->vmode->height - 1));
    if (!stbi_write_png (
            name, hardware->vmode->width, hardware->vmode->height, 3, last_row, -3 * hardware->vmode->width
    )) {
        fprintf (stderr, "ERROR: could not write video file %s\n", name);
        return false;
    }

    frame_number++;
    return true;
}

bool dump_video_frame_parallel(Window *window, long int frameNumber, unsigned char* address) {

    // write into a file
    char name[1024];
    sprintf (name, "/home/alvaregd/Documents/Games/water_reflection/video/video_frame_%03ld.png", frameNumber);
    unsigned char* last_row = address + (window->vmode->width * 3 * (window->vmode->height - 1));
    if (!stbi_write_png (name, window->vmode->width, window->vmode->height, 3, last_row, -3 * window->vmode->width)) {
        fprintf (stderr, "ERROR: could not write video file %s\n", name);
        return false;
    }
    return true;
}

typedef struct {
    int thread_id;
    int iterations;
    int frameStartId;
    unsigned char* startLocation;
    Window* window;

} thread_data;

void *call_from_thread(void *args) {
    thread_data *data = (thread_data*)args;

    unsigned char* bufferAddress = data->startLocation;
    int frameNumber = data->frameStartId;

    for (int i = 0; i < data->iterations; i++) {
        if (!dump_video_frame_parallel (data->window, frameNumber, bufferAddress)) {
           return false;
        }
        bufferAddress += data->window->vmode->width * data->window->vmode->height * 3;
        frameNumber++;
   }
    return NULL;
}

bool dump_video_frames (Video* video,  Window *window) {
    // reset iterating pointer first
    video->g_video_memory_ptr = video->g_video_memory_start;

    pthread_t t[NUM_THREADS];
    thread_data td[NUM_THREADS];

    //divide the number of frames between the threads
    int totalFrames = video->g_video_seconds_total * video->g_video_fps;
    int remainder =totalFrames % NUM_THREADS;
    unsigned char* assignedAddress = video->g_video_memory_start;
    int assignedFrame = 0;

    for (int j = 0; j < NUM_THREADS; ++j) {

        int iterations = (totalFrames - remainder)/NUM_THREADS;
        if (j < remainder) {iterations ++;}
        td[j].thread_id = j;
        //give each thread the number of iterations it needs to perform
        td[j].iterations= iterations;
        //give the frame start id so that it knows how to form the title
        td[j].frameStartId = assignedFrame;
        //give each thread the location in the video buffer where they will start iterating at.
        td[j].startLocation = assignedAddress;

        printf("Thread %d gets %d iterations starting at Frame %d\n", j, iterations,assignedFrame);
        //update to the new address
        assignedAddress += iterations * (window->vmode->width * window->vmode->height * 3);
        assignedFrame += iterations;

        //pass along the video and window details
        td[j].window = window;
    }

    //Launch a group of threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&t[i], NULL, call_from_thread, (void*)&td[i]);
    }
    printf("Launched by main\n");

    //Join the threads with the main thread
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(t[i], NULL);
    }

    free (video->g_video_memory_start);
    printf ("VIDEO IMAGES DUMPED\n");
    return true;
}

bool screencapture (Window * hardware) {
    unsigned char* buffer = (unsigned char*)malloc (hardware->vmode->width * hardware->vmode->height * 3);
    glReadPixels (0, 0, hardware->vmode->width, hardware->vmode->height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    unsigned char* last_row = buffer + (hardware->vmode->width * 3 * (hardware->vmode->height - 1));
    if (!stbi_write_png (SCREENSHOT_FILE, hardware->vmode->width, hardware->vmode->height, 3, last_row, -3 * hardware->vmode->width)) {
        fprintf (stderr, "ERROR: could not write screenshot file %s\n", SCREENSHOT_FILE);
    }
    free (buffer);
    return true;
}

bool videoUpdateTimer(Video* video,double* elapsed_seconds ){

    if(video->dump_video) {
        // elapsed_seconds is seconds since last loop iteration
        video->video_timer += *elapsed_seconds;
        video->video_dump_timer += *elapsed_seconds;
        // only record 10s of video, then quit
        if (video->video_timer > video->g_video_seconds_total) {
            printf("Finished!\n");
            return true;
        }else{
            return false;
        }
    }
    return false;
}
