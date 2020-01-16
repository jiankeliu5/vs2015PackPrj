#pragma once
#include "../log4zLib/log4z.h"

#ifdef _DEBUG
#pragma comment(lib,"../Debug/log4zLib.lib")
#else
#pragma comment(lib,"../Release/log4zLib.lib")
#endif
using namespace zsummer::log4z;

#endif

