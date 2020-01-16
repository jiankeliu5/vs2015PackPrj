#pragma once

#ifdef _AA_DLL_
	#define _AA_DLL_EXPORT_		_declspec(dllexport)
	#define _AA_DLL_EXPORT_C_	extern "C" _declspec(dllexport)
#else
	#define _AA_DLL_EXPORT_		_declspec(dllimport)
    #define _AA_DLL_EXPORT_C_	extern "C" _declspec(dllimport)
#endif // _AA_DLL


#define PRODUCT_REGISTY_KEY			_T("SOFTWARE\\")		//注册表键值
#define PROJECT_PATH				_T("\\test")
#define PROJECT_REGISTY_KEY			_T("")

#define REMOTE_START_PORT			9000



