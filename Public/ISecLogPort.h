#pragma once

#include "../Public/include/ISecLog/ZLogHelper.h"


#ifdef _DEBUG
#pragma comment(lib,"../Public/lib/debug/ISecLogd.lib")
#else
#pragma comment(lib,"../Public/lib/release/ISecLog.lib")
#endif
