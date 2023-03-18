#ifndef ILOGGER_H
#define ILOGGER_H

#include "IPlugIn.h"
//#include <log4cplus/log4cplus.h>
//#include <log4cplus/logger.h>

//using namespace log4cplus;

// global object
//extern Logger traceLogger;
//extern ILogger debugLogger;
//extern ILogger infoLogger;
//extern ILogger warnLogger;
//extern ILogger errorLogger;
//extern ILogger fatalLogger;

/**
 * @brief The LoggerBase class 日志抽象类，日志插件必须继承此类
 * 作者 张洋
 * 创建日期 2022-12-05
 */
class LoggerBase : public QObject, public IPlugIn
{
    Q_OBJECT
public:
    LoggerBase(){
        this->m_guid = QUuid::createUuid(); //new guid
    }
    ~LoggerBase(){}

    static LoggerBase *instance(){
        return m_loggerBase;
    }

  //接口属性定义
public:

    /**
     * @brief getGUID 获取唯一识别编号
     * @return 返回唯一识别编号
     */
    virtual QUuid getGUID() override {return m_guid;}

    /**
     * @brief getPlugInType 获取此插件的类型
     * @return 插件的类型
     */
    PlugInType getPlugInType() override {return PlugInType::PlugIn_Logger;}

     /**
     * @brief initModule 初始化模块
     * @return true：初始化成功；false:初始化失败
     */
    virtual int initModule() override {return 0;}

     /**
     * @brief disModule 销毁模块
     */
    virtual void disModule() override {}

    /**
     * @brief ThemeChanged 主题改变通知
     */
    virtual void ThemeChanged() override {}

    /**
     * @brief LanguageChanged 语言改变通知
     */
    virtual void LanguageChanged() override {}

public:

    /**
    * @brief writeTrace 写Trace等级的日志，子类需实现
    */
    virtual void writeTrace(const std::basic_string<char> &message);

    /**
    * @brief writeDebug 写Debug等级的日志，子类需实现
    */
    virtual void writeDebug(const std::basic_string<char> &message);

    /**
    * @brief writeInfo 写Info等级的日志，子类需实现
    */
    virtual void writeInfo(const std::basic_string<char> &message);

    /**
    * @brief writeWarn 写Warn等级的日志，子类需实现
    */
    virtual void writeWarn(const std::basic_string<char> &message);

    /**
    * @brief writeError 写Error等级的日志，子类需实现
    */
    virtual void writeError(const std::basic_string<char> &message);

    /**
    * @brief writeFatal 写Fatal等级的日志，子类需实现
    */
    virtual void writeFatal(const std::basic_string<char> &message);

protected:

    static LoggerBase *m_loggerBase;

private:

    /**
     * @brief m_str_GUID 功能组件的唯一识别ID
     */
    QUuid m_guid;

};

// define some macros for simplicity
#define LOG_TRACE(logInfo)                          \
    if(LoggerBase::instance() != nullptr){          \
        LoggerBase::instance()->writeTrace(logInfo); \
    }                                               \

#define LOG_DEBUG(logInfo)                          \
    if(LoggerBase::instance() != nullptr){          \
        LoggerBase::instance()->writeDebug(logInfo); \
    }                                               \

#define LOG_INFO(logInfo)                           \
    if(LoggerBase::instance() != nullptr){          \
        LoggerBase::instance()->writeInfo(logInfo);  \
    }                                               \

#define LOG_WARN(logInfo)                           \
    if(LoggerBase::instance() != nullptr){          \
        LoggerBase::instance()->writeWarn(logInfo);  \
    }                                               \

#define LOG_ERROR(logInfo)                          \
    if(LoggerBase::instance() != nullptr){          \
        LoggerBase::instance()->writeError(logInfo); \
    }                                               \

#define LOG_FATAL(logInfo)                          \
    if(LoggerBase::instance() != nullptr){          \
        LoggerBase::instance()->writeFatal(logInfo); \
    }                                               \

#endif // ILOGGER_H
