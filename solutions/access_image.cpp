#include "image.h"
#include <cstring>
#include <chrono>
#include <iostream>

// HW0 #1
// const Image& im: input image
// int x,y: pixel coordinates
// int ch: channel of interest
// returns the 0-based location of the pixel value in the data array
int pixel_address(const Image &im, int x, int y, int ch) {

    int pixel_idx = 0;
    int ch_size = im.w * im.h;
    pixel_idx = (ch * ch_size) + (y * im.w) + x;
    return pixel_idx;
}

// HW0 #1
// const Image& im: input image
// int x,y,ch: pixel coordinates and channel of interest
// returns the value of the clamped pixel at channel ch
float get_clamped_pixel(const Image &im, int x, int y, int ch) {
    // TODO: clamp the coordinates and return the correct pixel value

    if (x < 0) x = 0;
    if (x >= im.w) x = im.w - 1;
    if (y < 0) y = 0;
    if (y >= im.h) y = im.h - 1;
    if (ch < 0) ch = 0;
    if (ch >= im.c) ch = im.c - 1;

    return im.data[pixel_address(im, x, y, ch)];
}


// HW0 #1
// Image& im: input image
// int x,y,ch: pixel coordinates and channel of interest
void set_pixel(Image &im, int x, int y, int c, float value) {
    if ((x >= 0 && x < im.w) && (y >= 0 && y < im.h) && (c >= 0 && c < im.c))
        im.data[pixel_address(im, x, y, c)] = value;
}


// HW0 #2
// Copies an image
// Image& to: destination image
// const Image& from: source image
void copy_image(Image &to, const Image &from) {
    // allocating data for the new image
    to.data = (float *) calloc(from.w * from.h * from.c, sizeof(float));
    to.c = from.c;
    to.w = from.w;
    to.h = from.h;



    //TODO: check why memcpy is slower
    
    // test with for loop to show the difference in computational time
    auto start_t = std::chrono::system_clock::now();
    memcpy(to.data, from.data, to.size()*sizeof(float));
    auto end_t1 = std::chrono::system_clock::now();
    auto memcpy_time = end_t1 - start_t;
    for (int i = 0; i < to.size(); ++i) {
        to.data[i] = from.data[i];
    }

    auto end_t2 = std::chrono::system_clock::now();
    auto forloop_time = end_t2 - end_t1;
    cout << "memcpy ms: " << memcpy_time.count() << endl;
    cout << "for loop ms: " << forloop_time.count() << endl;
}
