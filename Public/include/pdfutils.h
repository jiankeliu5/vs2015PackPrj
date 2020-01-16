#pragma once

#ifdef DLLEXPORT
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif

//路径中使用反斜杠
extern "C" DLL int WINAPI ConvertToPDF(int count, char* pdfpath, ...);
//路径中使用反斜杠
extern "C" DLL int WINAPI ConvertToPDF2(int arraysize, char* pdfpath, char* jpgs[]);
