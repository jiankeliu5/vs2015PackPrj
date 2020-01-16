//Google Protocol Buffer 库的通用头文件

#ifndef _PORTOBUF_PORT_42A527D6_4FB1_4320_AFAE_7F6700CB554E_H_
#define _PORTOBUF_PORT_42A527D6_4FB1_4320_AFAE_7F6700CB554E_H_

#ifdef  _PROTOBUF_DLL_
#define _PROTOBUF_DLL_EXPORT_		__declspec(dllexport)
#define _PROTOBUF_DLL_EXPORT_C_ 	extern "C" __declspec(dllexport)
#else
#define _PROTOBUF_DLL_EXPORT_		__declspec(dllimport)
#define _PROTOBUF_DLL_EXPORT_C_	extern "C" __declspec(dllimport)
#endif


#ifdef _DEBUG
#pragma comment(lib,"..\\public\\protobuf-cpp-3.5.1\\lib\\libprotobufd.lib")
#else
#pragma comment(lib,"..\\public\\protobuf-cpp-3.5.1\\lib\\libprotobuf.lib")
#endif

#endif // _PORTOBUF_PORT_42A527D6_4FB1_4320_AFAE_7F6700CB554E_H_


