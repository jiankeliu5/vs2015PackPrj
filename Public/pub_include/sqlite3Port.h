#pragma once

#include "../include/sqlite3/sqlite3.h"

#ifdef _DEBUG
	#pragma comment(lib, "../../Debug/sqlite3Lib.lib")
#else
	#pragma comment(lib, "../../Release/sqlite3Lib.lib")
#endif // _DEBUG

