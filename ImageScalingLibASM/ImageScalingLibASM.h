// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the IMAGESCALINGLIBASM_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// IMAGESCALINGLIBASM_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef IMAGESCALINGLIBASM_EXPORTS
#define IMAGESCALINGLIBASM_API __declspec(dllexport)
#else
#define IMAGESCALINGLIBASM_API __declspec(dllimport)
#endif

// This class is exported from the dll
class IMAGESCALINGLIBASM_API CImageScalingLibASM {
public:
	CImageScalingLibASM(void);
	// TODO: add your methods here.
};

extern IMAGESCALINGLIBASM_API int nImageScalingLibASM;

IMAGESCALINGLIBASM_API int fnImageScalingLibASM(void);
