#include <stdio.h>
#include <math.h>
#include "ImageScalingC.h"

void scaleImage(unsigned char* pixels, unsigned char* newPixels, int oldWidth, int newWidth, double x_ratio, double y_ratio, int partSize, int totalSize)
{
    double px, py;
    
    for (int j = 0; j < partSize; j++)
    {
        int currentSize = (totalSize + j) / 3;
        int row = (currentSize / (newWidth / 3));
        int column = (currentSize % (newWidth / 3));
        px = floor(column * x_ratio);
        py = floor(row * y_ratio);
        px *= 3;
        newPixels[totalSize + j] = pixels[(int)((py * oldWidth) + px)];
        if (++j == partSize - 1)
            break;
        newPixels[totalSize + j] = pixels[(int)((py * oldWidth) + px) + 1];
        if (++j == partSize - 1)
            break;
        newPixels[totalSize + j] = pixels[(int)((py * oldWidth) + px) + 2];
    }
}