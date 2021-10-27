#include<stdio.h>
#include "ImageScalingC.h"

void scaleImage(unsigned char* pixels, unsigned char* newPixels, int oldWidth, int newWidth, double x_ratio, double y_ratio, int size, int totalSize)
{
    double px, py;

    for (int j = 0; j < size; j++)
    {
        int currentSize = (totalSize + j);
        int row = currentSize % newWidth;
        px = floor(currentSize * x_ratio);
        py = floor(row * y_ratio);
        newPixels[j] = pixels[(int)((py * oldWidth) + px)];
    }
}