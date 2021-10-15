// ImageScalingLibASM.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "ImageScalingLibASM.h"


// This is an example of an exported variable
IMAGESCALINGLIBASM_API int nImageScalingLibASM=0;

// This is an example of an exported function.
IMAGESCALINGLIBASM_API int fnImageScalingLibASM(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CImageScalingLibASM::CImageScalingLibASM()
{
    return;
}
