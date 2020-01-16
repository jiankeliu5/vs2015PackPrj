// Global.h: interface for the CRscObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_GLOBAL_)
#define _GLOBAL_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef ISEC_STATIC_LIBRARY
	#define _AA_DLL_EXPORT_
	#define _AA_DLL_EXPORT_C_
#else
	#ifndef UNUSER_EXPORT
		#ifdef  _AA_DLL_
			#define _AA_DLL_EXPORT_		__declspec(dllexport)
			#define _AA_DLL_EXPORT_C_ 	extern "C" __declspec(dllexport)
		#else
			#define _AA_DLL_EXPORT_		__declspec(dllimport)
			#define _AA_DLL_EXPORT_C_	extern "C" __declspec(dllimport)
		#endif
	#else
		#define  _AA_DLL_EXPORT_
	#endif
#endif

#include "glKeyDef.h"


#endif // !defined(_GLOBAL_)
