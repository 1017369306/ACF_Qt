#pragma once
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;
using namespace log4cplus::helpers;

// global object
extern Logger traceLogger;
extern Logger debugLogger;
extern Logger infoLogger;
extern Logger warnLogger;
extern Logger errorLogger;
extern Logger fatalLogger;

// define some macros for simplicity
#define LOG_TRACE(logEvent)			LOG4CPLUS_TRACE(traceLogger, logEvent)
#define LOG_DEBUG(logEvent)			LOG4CPLUS_DEBUG(debugLogger, logEvent)
#define LOG_DEBUG_F(...)            LOG4CPLUS_DEBUG_FMT(debugLogger, __VA_ARGS__)
#define LOG_INFO(logEvent)			LOG4CPLUS_INFO(infoLogger, logEvent)
#define LOG_WARN(logEvent)			LOG4CPLUS_WARN(warnLogger, logEvent)
#define LOG_ERROR(logEvent)			LOG4CPLUS_ERROR(errorLogger, logEvent)
#define LOG_FATAL(logEvent)			LOG4CPLUS_FATAL(fatalLogger, logEvent)

extern void initLogger(const std::string &appPath);

extern void shutDown();

extern void close();
