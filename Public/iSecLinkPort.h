#pragma once

#include "../iSecLink/Link.h"
#include "../iSecLink/LinkTcp.h"
#include "../iSecLink/LinkTh.h"
#include "../iSecLink/LinkRabbitMQTh.h"
#include "../iSecLink/LinkHttp.h"
#include "../iSecLink/LinkHttpTh.h"
#include "../iSecLink/XabMsg.h"

#ifdef _DEBUG
#pragma comment(lib,"..\\Debug\\iSecLink.lib")
#else
#pragma comment(lib,"..\\Release\\iSecLink.lib")
#endif
