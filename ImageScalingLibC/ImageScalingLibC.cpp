// ImageScalingLibC.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "ImageScalingLibC.h"


// This is an example of an exported variable
IMAGESCALINGLIBC_API int nImageScalingLibC=0;

// This is an example of an exported function.
IMAGESCALINGLIBC_API int fnImageScalingLibC(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CImageScalingLibC::CImageScalingLibC()
{
    return;
}
