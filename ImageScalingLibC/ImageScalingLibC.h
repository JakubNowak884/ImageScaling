// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the IMAGESCALINGLIBC_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// IMAGESCALINGLIBC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef IMAGESCALINGLIBC_EXPORTS
#define IMAGESCALINGLIBC_API __declspec(dllexport)
#else
#define IMAGESCALINGLIBC_API __declspec(dllimport)
#endif

// This class is exported from the dll
class IMAGESCALINGLIBC_API CImageScalingLibC {
public:
	CImageScalingLibC(void);
	// TODO: add your methods here.
};

extern IMAGESCALINGLIBC_API int nImageScalingLibC;

IMAGESCALINGLIBC_API int fnImageScalingLibC(void);
