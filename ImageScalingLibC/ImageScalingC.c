#include <stdio.h>
#include <math.h>
#include "ImageScalingC.h"

void scaleImage(unsigned char* pixels, unsigned char* newPixels, int oldWidth, int newWidth, double x_ratio, double y_ratio, int size, int totalSize, int scale)
{
    double px, py;
    int counter = 0;
    int t = scale;
    
    for (int j = 0; j < size; j += 3)
    {
        int currentSize = (totalSize + j) / 3;
        int row = (currentSize / (newWidth / 3));
        int column = (currentSize % (newWidth / 3));
        px = floor(column * x_ratio);
        py = floor(row * y_ratio);
        px *= 3;
        newPixels[totalSize + j] = pixels[(int)((py * oldWidth) + px)];
        newPixels[totalSize + j + 1] = pixels[(int)((py * oldWidth) + px) + 1];
        newPixels[totalSize + j + 2] = pixels[(int)((py * oldWidth) + px) + 2];
        /* int currentSize = (totalSize + j);
       float row = (currentSize / newWidth);
       counter = row * oldWidth;
       counter -= ceil(((row - 1) / 2)) * newWidth;

       int p = (totalSize + (j / 3)) / scale;
       p *= 3;
       p -= counter;
       newPixels[totalSize + j] = pixels[p];
       newPixels[totalSize + j + 1] = pixels[p + 1];
       newPixels[totalSize + j + 2] = pixels[p + 2];*/
    }
}