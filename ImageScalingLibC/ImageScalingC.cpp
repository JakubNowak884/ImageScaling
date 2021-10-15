#include "pch.h"

#include <stdio.h>
#include <stdlib.h>

#pragma warning(disable:4996)

extern "C"
{
    __declspec(dllexport) int scaleImage2()
    {
        FILE* f = fopen("Sample.bmp", "rb");
        unsigned char info[54];

        fread(info, sizeof(unsigned char), 54, f);

        int width = *(int*)&info[18];
        int height = *(int*)&info[22];

        int size = 3 * width * height;
        unsigned char* data = (unsigned char*)malloc(size * sizeof(unsigned char));

        fread(data, sizeof(unsigned char), size, f);
        fclose(f);

        FILE* output = fopen("Output.bmp", "wb");
        fwrite(info, sizeof(unsigned char), 54, output);
        fwrite(data, sizeof(unsigned char), size, output);

        fclose(output);

        return 1;
    }
}