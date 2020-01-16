#pragma once

//json库的编译参数要求与CloudPrintRouterDemo的一致。【/MD】和【在共享DLL中使用MFC】

#include "../Public/jsoncpp-1.8.4/include/json/json.h"
using namespace Json;


#ifdef _DEBUG
#pragma comment(lib,"../Public/jsoncpp-1.8.4/lib/debug/lib_json.lib")
#else
#pragma comment(lib,"../Public/jsoncpp-1.8.4/lib/release/lib_json.lib")
#endif
