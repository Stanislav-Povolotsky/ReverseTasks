// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MYLIB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MYLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MYLIB_EXPORTS
#define MYLIB_API __declspec(dllexport) 
#else
#define MYLIB_API __declspec(dllimport)
#endif

//// This class is exported from the mylib.dll
//class MYLIB_API Cmylib {
//public:
//	Cmylib(void);
//	// TODO: add your methods here.
//};

extern MYLIB_API int nmylib;

MYLIB_API int fnmylib(void);
