#include "logger4Qt.h"
#include <globalInfoEnum.h>
#include <QFileInfo>
#include <QDir>

Logger4Qt::Logger4Qt() : DefaultPlugin(){

}

Logger4Qt::~Logger4Qt(){

}

 /**
 * @brief initModule 初始化模块
 * @return true：初始化成功；false:初始化失败
 */
int Logger4Qt::initModule(){
    int result = GlobalInfo::GlobalInfoEnum::DealFatal;

    //重要：这里要手动创建最外层的logs目录，不然Log4Qt会初始化失败，它也不会帮你创建
    QString logDir = qApp->applicationDirPath() + "/logs/";
    QDir dir(logDir);
    if(!dir.exists()){
        dir.mkdir(logDir);
    }

    QString path = qApp->applicationDirPath() + "/config/log4qt.conf";
    if(QFileInfo::exists(path)){
        //配置文件路径
        bool flag = Log4Qt::PropertyConfigurator::configure(path);
        //将qDebug()调试信息直接记录到日志文件
        Log4Qt::LogManager::setHandleQtMessages(true);

        m_traceLogger = Log4Qt::Logger::logger("traceAppender");
        m_debugLogger = Log4Qt::Logger::logger("debugAppender");
        m_infoLogger = Log4Qt::Logger::logger("infoAppender");
        m_warnLogger = Log4Qt::Logger::logger("warnAppender");
        m_errorLogger = Log4Qt::Logger::logger("errorAppender");
        m_fatalLogger = Log4Qt::Logger::logger("fatalAppender");

        if(flag){
            result = GlobalInfo::GlobalInfoEnum::DealSuccess;
        }
    }

    return result;
}

 /**
 * @brief disModule 销毁模块
 */
void Logger4Qt::disModule(){

}

/**
* @brief writeTrace 写Trace等级的日志，子类需实现
*/
void Logger4Qt::writeTrace(const std::basic_string<char> &message){
    if(m_traceLogger){
        m_traceLogger->trace(message.c_str());
    }
}

/**
* @brief writeDebug 写Debug等级的日志，子类需实现
*/
void Logger4Qt::writeDebug(const std::basic_string<char> &message){
    if(m_debugLogger){
        m_debugLogger->debug(message.c_str());
    }
}

/**
* @brief writeInfo 写Info等级的日志，子类需实现
*/
void Logger4Qt::writeInfo(const std::basic_string<char> &message){
    if(m_infoLogger){
        m_infoLogger->info(message.c_str());
    }
}

/**
* @brief writeWarn 写Warn等级的日志，子类需实现
*/
void Logger4Qt::writeWarn(const std::basic_string<char> &message){
    if(m_warnLogger){
        m_warnLogger->warn(message.c_str());
    }
}

/**
* @brief writeError 写Error等级的日志，子类需实现
*/
void Logger4Qt::writeError(const std::basic_string<char> &message){
    if(m_errorLogger){
        m_errorLogger->error(message.c_str());
    }
}

/**
* @brief writeFatal 写Fatal等级的日志，子类需实现
*/
void Logger4Qt::writeFatal(const std::basic_string<char> &message){
    if(m_fatalLogger){
        m_fatalLogger->fatal(message.c_str());
    }
}
