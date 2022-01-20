#include <stdio.h>
#include "ImageScalingC.h"

void scaleImage(unsigned char* pixels, unsigned char* newPixels, int oldWidth, int newWidth, float x_ratio, float y_ratio, int partSize, int totalSize)
{
    int px, py;
    int newWidth3 = newWidth / 3;

    for (int j = 0; j < partSize; j += 3)
    {
        int currentSize = (totalSize + j) / 3;
        int row = (currentSize / newWidth3);
        int column = (currentSize % newWidth3);
        px = x_ratio * column;
        py = y_ratio * row;
        px *= 3;
        newPixels[totalSize + j] = pixels[(int)((py * oldWidth) + px)];
        newPixels[totalSize + j + 1] = pixels[(int)((py * oldWidth) + px) + 1];
        newPixels[totalSize + j + 2] = pixels[(int)((py * oldWidth) + px) + 2];
    }
}