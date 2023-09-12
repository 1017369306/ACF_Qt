#ifndef LOGGERHELPER_H
#define LOGGERHELPER_H

#include <QString>
#include <QObject>
#include <IPlugIn.h>
#include "log4qt/logger.h"
#include "log4qt/basicconfigurator.h"
#include "log4qt/propertyconfigurator.h"
#include "log4qt/logmanager.h"
#include <QApplication>
#include <globalEnums.h>
#include <acfproperty.h>
#include <defaultplugin.h>

using namespace Log4Qt;

#define Logger_Trace "Trace"
#define Logger_Debug "Debug"
#define Logger_Info "Info"
#define Logger_Warn "Warn"
#define Logger_Error "Error"
#define Logger_Fatal "Fatal"

/**
 * @brief Log4Qt日志插件的实现
 */
class Logger4Qt : public DefaultPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_iid FILE "logLib.json")
    Q_INTERFACES(IPlugIn)
public:

    Logger4Qt();
    ~Logger4Qt();

     /**
     * @brief initModule 初始化模块
     * @return true：初始化成功；false:初始化失败
     */
    int initModule() override;

     /**
     * @brief disModule 销毁模块
     */
    void disModule() override;

    /**
     * @brief sendData 插件运行起来后，给其发送数据
     * @param data
     */
    void sendData(QVariant data = QVariant::Invalid) override {
        if(data.isValid() && data.canConvert<LoggerBaseStruct>()){
            LoggerBaseStruct logger = data.value<LoggerBaseStruct>();
            //1、如果此日志等级小于系统日志等级，则忽略
            if(logger.level < ACFProperty::instance()->getLogLevel()){
                return;
            }
            //2、如果日志等级大于等于TRACE，都先往TRACE中写
            if(logger.level >= LoggerBaseLevel::TRACE_LogLevel){
                this->writeTrace(logger.message);
            }
            //3、根据日志等级，写入指定的日志文件
            QString logLevel = "";
            switch (logger.level) {
            case LoggerBaseLevel::DEBUG_LogLevel:
            {
                logLevel = Logger_Debug;
                this->writeDebug(logger.message);
                break;
            }
            case LoggerBaseLevel::INFO_LogLevel:
            {
                logLevel = Logger_Info;
                this->writeInfo(logger.message);
                break;
            }
            case LoggerBaseLevel::WARN_LogLevel:
            {
                logLevel = Logger_Warn;
                this->writeWarn(logger.message);
                break;
            }
            case LoggerBaseLevel::ERROR_LogLevel:
            {
                logLevel = Logger_Error;
                this->writeError(logger.message);
                break;
            }
            case LoggerBaseLevel::FATAL_LogLevel:
            {
                logLevel = Logger_Fatal;
                this->writeFatal(logger.message);
                break;
            }
            }

            if(ACFProperty::instance()->getOutputText()){
                ACFProperty::instance()->getOutputText()->append(logLevel + QString(": ") + QString(logger.message.c_str()));
            }
        }
    }

private:

    /**
    * @brief writeTrace 写Trace等级的日志，子类需实现
    */
    void writeTrace(const std::basic_string<char> &message);

    /**
    * @brief writeDebug 写Debug等级的日志，子类需实现
    */
    void writeDebug(const std::basic_string<char> &message);

    /**
    * @brief writeInfo 写Info等级的日志，子类需实现
    */
    void writeInfo(const std::basic_string<char> &message);

    /**
    * @brief writeWarn 写Warn等级的日志，子类需实现
    */
    void writeWarn(const std::basic_string<char> &message);

    /**
    * @brief writeError 写Error等级的日志，子类需实现
    */
    void writeError(const std::basic_string<char> &message);

    /**
    * @brief writeFatal 写Fatal等级的日志，子类需实现
    */
    void writeFatal(const std::basic_string<char> &message);

private:

    Logger *m_traceLogger = nullptr;
    Logger *m_debugLogger = nullptr;
    Logger *m_infoLogger = nullptr;
    Logger *m_warnLogger = nullptr;
    Logger *m_errorLogger = nullptr;
    Logger *m_fatalLogger = nullptr;

};


#endif // LOGGERHELPER_H
