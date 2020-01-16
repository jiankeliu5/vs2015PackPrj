#ifndef _XX_SQLITE__XXX_NET
#define _XX_SQLITE__XXX_NET

#include "../public/include/sqlite3.h"
#ifdef _DEBUG
#pragma comment(lib,"../public/lib/debug/SQLite.lib")   //debug版本 有问题
#else
#pragma comment(lib,"../public/lib/release/SQLite.lib")
#endif

#endif 