#pragma once

#include "../../3thLib/jsonLib/value.h"
#include "../../3thLib/jsonLib/reader.h"
#include "../../3thLib/jsonLib/writer.h"

//typedef Json::Reader			JsonReader;
//typedef Json::Writer			JsonWriter;
//typedef Json::FastWriter		JsonFastWriter;
//typedef Json::StyledWriter		JsonStyleWriter;
typedef Json::Value				JsonValue;
typedef Json::Value::Members	JsonValueMembers;

#define JsonObject				Json::objectValue
#define JsonArryValue			Json::arrayValue
#define	JsonNullValue			Json::nullValue

#ifdef _DEBUG
	#pragma comment(lib, "../../Debug/jsonLib.lib")
#else
	#pragma comment(lib, "../../Release/jsonLib.lib")
#endif	// _DEBUG





