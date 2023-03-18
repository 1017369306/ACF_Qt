#include "loggerBase.h"

LoggerBase *LoggerBase::m_loggerBase = nullptr;

/**
* @brief writeTrace 写Trace等级的日志，子类需实现
*/
void LoggerBase::writeTrace(const std::basic_string<char> &message){

}

/**
* @brief writeDebug 写Debug等级的日志，子类需实现
*/
void LoggerBase::writeDebug(const std::basic_string<char> &message){

}

/**
* @brief writeInfo 写Info等级的日志，子类需实现
*/
void LoggerBase::writeInfo(const std::basic_string<char> &message){

}

/**
* @brief writeWarn 写Warn等级的日志，子类需实现
*/
void LoggerBase::writeWarn(const std::basic_string<char> &message){

}

/**
* @brief writeError 写Error等级的日志，子类需实现
*/
void LoggerBase::writeError(const std::basic_string<char> &message){

}

/**
* @brief writeFatal 写Fatal等级的日志，子类需实现
*/
void LoggerBase::writeFatal(const std::basic_string<char> &message){

}
