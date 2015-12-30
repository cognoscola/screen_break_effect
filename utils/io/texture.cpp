//
// Created by alvaregd on 06/12/15.
//
#include "texture.h"
#include "stb_image.h"

void loadImageFile(const char* filename, bool flip, unsigned char ** data, int* width, int* height){

    int x, y, n;
    int force_channels = 4;
    unsigned char *image_data = stbi_load(filename, &x, &y, &n, force_channels);
    if (!image_data) {
        fprintf(stderr, "ERROR: could not load %s\n", filename);
    }
    if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
        fprintf(stderr, "WARNING:texture %s is not a power of 2 dimensions\n", filename);
    }
    printf("Found texture, size:%dx%d\n", x,y);

    //flip the image
    if(flip){
        int width_in_bytes = x *4;
        unsigned char *top = NULL;
        unsigned char *bottom = NULL;
        unsigned char temp = 0;
        int half_height = y/2;

        for (int row = 0; row < half_height; row++) {
            top = image_data + row * width_in_bytes;
            bottom = image_data + (y - row - 1) * width_in_bytes;
            for (int col = 0; col < width_in_bytes; col++) {
                temp = *top;
                *top = *bottom;
                *bottom = temp;
                top++;
                bottom++;
            }
        }

    }

    *data = image_data;
    *width = x;
    *height = y;
}
