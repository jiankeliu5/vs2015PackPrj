/**
	\file ZLogHelper.h
	\brief 该文件是信安宝公司对第三方开源库 log4z(VERSION 3.0) 的修改
	
			1, 改为动态库版本, 这么改的原因是如果一个项目中需要加载多个dll, 而每个dll 都使用静态版本的日志库
				, 而这个日志库内部启用线程, 这样就造成了资源的浪费, 没有必要为日志输出开启多个线程.
			2, 修复bug, 在获得实例时候由函数内部的静态局部变量改为静态全局变量, 因为要确保日志的生命周期比程序任何变量长
			3, 修复bug, 当日志太长造成日志无法输出, 目前采取对字符串进行截断的方式(超过最大长度, 则只显示到最大长度的字符串)
			4, 修改日志文件名的生成, 日志名只精确到天, 而不是每个进程产生一个文件(同时每个文件名加上进程的id)

	\author zhoumanman888@126.com
	\date 2018.10.29
	\note 改成动态库版本后, 由于会有多个某块加载, 所以日志启动只需要一个即可, 一般在主程序中. 另外, 竟量统一文件输出编码方式
		, 建议所有某块以utf-8 作为输出编码.  如 u8"中国"
*/

#pragma once
#include "ILog4zManager.h"
#include "ZLog4zStream.h"

#ifdef ISECLOG_EXPORTS
#define ISECLOG_API __declspec(dllexport)
#else
#define ISECLOG_API __declspec(dllimport)
#endif

/**
	唯一导出函数, 其他宏只是方便用户使用
*/
extern ISECLOG_API XabLog::ILog4zManager* getLog4zMgr();


//! base macro., 修复一个bug, 日志内容太长导致无法显示, modified by zmm, 2018.8.17
#define LOG_STREAM(id, level, log)\
do{\
    if (getLog4zMgr()->prePushLog(id,level)) \
    {\
        char logBuf[XabLog::LOG4Z_LOG_BUF_SIZE] = "";\
        XabLog::ZLog4zStream ssLog4zStream(logBuf, XabLog::LOG4Z_LOG_BUF_SIZE - 1);\
        ssLog4zStream << log;\
        getLog4zMgr()->pushLog(id, level, logBuf, __FILE__, __LINE__);\
    }\
} while (0)


//! fast macro
#define LOG_TRACE(id, log) LOG_STREAM(id, XabLog::LOG_LEVEL_TRACE, log)
#define LOG_DEBUG(id, log) LOG_STREAM(id, XabLog::LOG_LEVEL_DEBUG, log)
#define LOG_INFO(id, log)  LOG_STREAM(id, XabLog::LOG_LEVEL_INFO, log)
#define LOG_WARN(id, log)  LOG_STREAM(id, XabLog::LOG_LEVEL_WARN, log)
#define LOG_ERROR(id, log) LOG_STREAM(id, XabLog::LOG_LEVEL_ERROR, log)
#define LOG_ALARM(id, log) LOG_STREAM(id, XabLog::LOG_LEVEL_ALARM, log)
#define LOG_FATAL(id, log) LOG_STREAM(id, XabLog::LOG_LEVEL_FATAL, log)

//! super macro.
#define LOGT( log ) LOG_TRACE(XabLog::LOG4Z_MAIN_LOGGER_ID, log )
#define LOGD( log ) LOG_DEBUG(XabLog::LOG4Z_MAIN_LOGGER_ID, log )
#define LOGI( log ) LOG_INFO(XabLog::LOG4Z_MAIN_LOGGER_ID, log )
#define LOGW( log ) LOG_WARN(XabLog::LOG4Z_MAIN_LOGGER_ID, log )
#define LOGE( log ) LOG_ERROR(XabLog::LOG4Z_MAIN_LOGGER_ID, log )
#define LOGA( log ) LOG_ALARM(XabLog::LOG4Z_MAIN_LOGGER_ID, log )
#define LOGF( log ) LOG_FATAL(XabLog::LOG4Z_MAIN_LOGGER_ID, log )


//! check VC VERSION. DO NOT TOUCH
//! format micro cannot support VC6 or VS2003, please use stream input log, like LOGI, LOGD, LOG_DEBUG, LOG_STREAM ...
#if _MSC_VER >= 1400 //MSVC >= VS2005
#define LOG4Z_FORMAT_INPUT_ENABLE
#endif

#ifndef WIN32
#define LOG4Z_FORMAT_INPUT_ENABLE
#endif


//! format input log.
#ifdef LOG4Z_FORMAT_INPUT_ENABLE
	#ifdef WIN32
		#define LOG_FORMAT(id, level, logformat, ...) \
		do{ \
			if (getLog4zMgr()->prePushLog(id,level)) \
			{\
				char logbuf[XabLog::LOG4Z_LOG_BUF_SIZE]; \
				_snprintf_s(logbuf, XabLog::LOG4Z_LOG_BUF_SIZE, _TRUNCATE, logformat, ##__VA_ARGS__); \
				getLog4zMgr()->pushLog(id, level, logbuf, __FILE__, __LINE__); \
			}\
		} while (0)
	#else
		#define LOG_FORMAT(id, level, logformat, ...) \
		do{ \
			if (getLog4zMgr()->prePushLog(id,level)) \
			{\
				char logbuf[XabLog::LOG4Z_LOG_BUF_SIZE]; \
				snprintf(logbuf, XabLog::LOG4Z_LOG_BUF_SIZE,logformat, ##__VA_ARGS__); \
				getLog4zMgr()->pushLog(id, level, logbuf, __FILE__, __LINE__); \
			} \
		}while(0)
	#endif

	//!format string
	#define LOGFMT_TRACE(id, fmt, ...)  LOG_FORMAT(id, XabLog::LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)
	#define LOGFMT_DEBUG(id, fmt, ...)  LOG_FORMAT(id, XabLog::LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
	#define LOGFMT_INFO(id, fmt, ...)  LOG_FORMAT(id, XabLog::LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
	#define LOGFMT_WARN(id, fmt, ...)  LOG_FORMAT(id, XabLog::LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
	#define LOGFMT_ERROR(id, fmt, ...)  LOG_FORMAT(id, XabLog::LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
	#define LOGFMT_ALARM(id, fmt, ...)  LOG_FORMAT(id, XabLog::LOG_LEVEL_ALARM, fmt, ##__VA_ARGS__)
	#define LOGFMT_FATAL(id, fmt, ...)  LOG_FORMAT(id, XabLog::LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)
	#define LOGFMTT( fmt, ...) LOGFMT_TRACE(XabLog::LOG4Z_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
	#define LOGFMTD( fmt, ...) LOGFMT_DEBUG(XabLog::LOG4Z_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
	#define LOGFMTI( fmt, ...) LOGFMT_INFO(XabLog::LOG4Z_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
	#define LOGFMTW( fmt, ...) LOGFMT_WARN(XabLog::LOG4Z_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
	#define LOGFMTE( fmt, ...) LOGFMT_ERROR(XabLog::LOG4Z_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
	#define LOGFMTA( fmt, ...) LOGFMT_ALARM(XabLog::LOG4Z_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
	#define LOGFMTF( fmt, ...) LOGFMT_FATAL(XabLog::LOG4Z_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
#else
	inline void empty_log_format_function1(XabLog::LoggerId id, const char*, ...) {}
	inline void empty_log_format_function2(const char*, ...) {}
	#define LOGFMT_TRACE empty_log_format_function1
	#define LOGFMT_DEBUG LOGFMT_TRACE
	#define LOGFMT_INFO LOGFMT_TRACE
	#define LOGFMT_WARN LOGFMT_TRACE
	#define LOGFMT_ERROR LOGFMT_TRACE
	#define LOGFMT_ALARM LOGFMT_TRACE
	#define LOGFMT_FATAL LOGFMT_TRACE
	#define LOGFMTT empty_log_format_function2
	#define LOGFMTD LOGFMTT
	#define LOGFMTI LOGFMTT
	#define LOGFMTW LOGFMTT
	#define LOGFMTE LOGFMTT
	#define LOGFMTA LOGFMTT
	#define LOGFMTF LOGFMTT
#endif
