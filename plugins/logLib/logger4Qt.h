#ifndef LOGGERHELPER_H
#define LOGGERHELPER_H

#include <QString>
#include <QObject>
#include <IPlugIn.h>
#include <loggerBase.h>
#include "log4qt/logger.h"
#include "log4qt/basicconfigurator.h"
#include "log4qt/propertyconfigurator.h"
#include "log4qt/logmanager.h"
#include <QApplication>

using namespace Log4Qt;

/**
 * @brief Log4Qt日志插件的实现
 */
class Logger4Qt : public LoggerBase
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

    IPlugIn *createNewPlugin() override {return this;}

    QWidget *getWidget(QWidget *parent = nullptr) override {return nullptr;}

    /**
    * @brief writeTrace 写Trace等级的日志，子类需实现
    */
    void writeTrace(const std::basic_string<char> &message) override;

    /**
    * @brief writeDebug 写Debug等级的日志，子类需实现
    */
    void writeDebug(const std::basic_string<char> &message) override;

    /**
    * @brief writeInfo 写Info等级的日志，子类需实现
    */
    void writeInfo(const std::basic_string<char> &message) override;

    /**
    * @brief writeWarn 写Warn等级的日志，子类需实现
    */
    void writeWarn(const std::basic_string<char> &message) override;

    /**
    * @brief writeError 写Error等级的日志，子类需实现
    */
    void writeError(const std::basic_string<char> &message) override;

    /**
    * @brief writeFatal 写Fatal等级的日志，子类需实现
    */
    void writeFatal(const std::basic_string<char> &message) override;

private:

    Logger *m_traceLogger = nullptr;
    Logger *m_debugLogger = nullptr;
    Logger *m_infoLogger = nullptr;
    Logger *m_warnLogger = nullptr;
    Logger *m_errorLogger = nullptr;
    Logger *m_fatalLogger = nullptr;

};


#endif // LOGGERHELPER_H
