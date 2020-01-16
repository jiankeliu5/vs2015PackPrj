#pragma once

#include "logTypes.h"

_XABLOG_BEGIN

//! log4z class
class ILog4zManager
{
public:
	ILog4zManager() {};
	virtual ~ILog4zManager() {};

	
	//! Config or overwrite configure
	//! Needs to be called before ILog4zManager::Start,, OR Do not call.
	virtual bool config(const char * configPath) = 0;
	virtual bool configFromString(const char * configContent) = 0;

	//! Create or overwrite logger.
	//! Needs to be called before ILog4zManager::Start, OR Do not call.
	virtual LoggerId createLogger(const char* key) = 0;

	//! Start Log Thread. This method can only be called once by one process.
	virtual bool start() = 0;

	//! Default the method will be calling at process exit auto.
	//! Default no need to call and no recommended.
	virtual bool stop() = 0;

	//! Find logger. thread safe.
	virtual LoggerId findLogger(const char* key) = 0;

	//pre-check the log filter. if filter out return false. 
	virtual bool prePushLog(LoggerId id, int level) = 0;
	//! Push log, thread safe.
	virtual bool pushLog(LoggerId id, int level, const char * log, const char * file = nullptr, int line = 0) = 0;

	//! set logger's attribute, thread safe.
	virtual bool enableLogger(LoggerId id, bool enable) = 0; // immediately when enable, and queue up when disable. 
	virtual bool setLoggerName(LoggerId id, const char * name) = 0;
	virtual bool setLoggerPath(LoggerId id, const char * path) = 0;
	virtual bool setLoggerLevel(LoggerId id, int nLevel) = 0; // immediately when enable, and queue up when disable. 
	virtual bool setLoggerFileLine(LoggerId id, bool enable) = 0;
	virtual bool setLoggerDisplay(LoggerId id, bool enable) = 0;
	virtual bool setLoggerOutFile(LoggerId id, bool enable) = 0;
	virtual bool setLoggerLimitsize(LoggerId id, unsigned int limitsize) = 0;// limit file's size, unit Million byte.
	virtual bool setLoggerMonthdir(LoggerId id, bool enable) = 0;


	//! Update logger's attribute from config file, thread safe.
	virtual bool setAutoUpdate(int interval/*per second, 0 is disable auto update*/) = 0;
	virtual bool updateConfig() = 0;

	//! Log4z status statistics, thread safe.
	virtual bool isLoggerEnable(LoggerId id) = 0;
	virtual unsigned long long getStatusTotalWriteCount() = 0;
	virtual unsigned long long getStatusTotalWriteBytes() = 0;
	virtual unsigned long long getStatusWaitingCount() = 0;
	virtual unsigned int getStatusActiveLoggers() = 0;
};

_XABLOG_END
