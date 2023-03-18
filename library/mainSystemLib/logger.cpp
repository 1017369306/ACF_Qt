#define UNICODE
#include <log4cplus/logger.h>
#include <log4cplus/layout.h>
#include <log4cplus/configurator.h>
#include <log4cplus/log4cplus.h>
#include <stdio.h>
#include <bits/basic_string.h>
#include <bits/locale_classes.h>
#include "logger.h"

using namespace log4cplus;

Logger traceLogger = Logger::getInstance(LOG4CPLUS_TEXT("TRACELOG"));
Logger debugLogger = Logger::getInstance(LOG4CPLUS_TEXT("DEBUGLOG"));
Logger infoLogger = Logger::getInstance(LOG4CPLUS_TEXT("INFOLOG"));
Logger warnLogger = Logger::getInstance(LOG4CPLUS_TEXT("WARNLOG"));
Logger errorLogger = Logger::getInstance(LOG4CPLUS_TEXT("ERRORLOG"));
Logger fatalLogger = Logger::getInstance(LOG4CPLUS_TEXT("FATALLOG"));

void initLogger(const std::string &appPath) {
    std::string configPath = appPath + "./log4cplus.conf";
    bool exit = false;
    if (FILE *file = fopen(configPath.c_str(), "r")) {
        fclose(file);
        exit = true;
    }
    else {
        exit = false;
    }
    if(exit){
        PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("./log4cplus.conf"));
    }

}

void shutDown(){
//    traceLogger.shutdown();
//    log4cplus::Logger::shutdown();
}

void close()
{
    log4cplus::deinitialize();
}
