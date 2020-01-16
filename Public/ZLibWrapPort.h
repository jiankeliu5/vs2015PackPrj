#pragma once

#include "../ZLibWrapLib/ZLib/zlib.h"
#include "../ZLibWrapLib/ZLibWrapLib.h"



#ifdef _X86_

#ifdef _DEBUG
	#pragma comment(lib,"../Debug/ZLibWrapLib.lib")
#else
	#pragma comment(lib,"../Release/ZLibWrapLib.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib,"../x64/Debug/ZLibWrapLib.lib")
#else
#pragma comment(lib,"../x64/Release/ZLibWrapLib.lib")
#endif
#endif


