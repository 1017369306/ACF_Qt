#ifndef DEFAULTPLUGIN_H
#define DEFAULTPLUGIN_H

#include "mainSystemLib_global.h"
#include "IPlugIn.h"
#include <globalInfoEnum.h>
#include <globalEnums.h>
#include <acfproperty.h>

/**
 * @brief 日志抽象类，日志插件必须继承此类
 * 作者 张洋
 * 创建日期 2022-12-05
 */
class MAINSYSTEMLIB_EXPORT DefaultPlugin : public QObject, public IPlugIn
{
    Q_OBJECT
public:
    DefaultPlugin(){
        this->m_guid = QUuid::createUuid(); //new guid
    }
    ~DefaultPlugin(){}

    //接口属性定义
public:

    /**
     * @brief getGUID 获取唯一识别编号
     * @return 返回唯一识别编号
     */
    virtual QUuid getGUID() override {return m_guid;}

    /**
    * @brief getWidget 获得作用的窗体。注意：作为独立窗体显示：setWindowFlags(Qt::Window);
    * 不阻塞的模态窗体，如果内嵌窗体已经设置了父窗体：setWindowModality(Qt::WindowModal);
    * 不阻塞的模态窗体，如果内嵌窗体未设置了父窗体：setWindowModality(Qt::ApplicationModal);
    * @param parent 父窗体
    * @return
    */
    virtual QWidget *getWidget(QWidget *parent = nullptr) {return nullptr;}

    /**
     * @brief createNewPlugin 创建新的插件实例
     * @return
     */
    virtual IPlugIn *createNewPlugin(){return this;}

    /**
     * @brief initModule 初始化模块
     * @return true：初始化成功；false:初始化失败
     */
    virtual int initModule() override {return 0;}

    /**
     * @brief run 运行插件
     * @return
     */
    virtual int run() override {return GlobalInfo::GlobalInfoEnum::DealSuccess;}

    /**
     * @brief stop 停止插件
     * @return
     */
    virtual int stop() override {return GlobalInfo::GlobalInfoEnum::DealSuccess;}

    /**
     * @brief sendData 插件运行起来后，给其发送数据
     * @param data
     */
    virtual void sendData(QVariant data = QVariant::Invalid) override {}

    /**
     * @brief disModule 销毁模块
     */
    virtual void disModule() override {}

    /**
     * @brief ThemeChanged 主题改变通知
     */
    virtual void cssStyleChanged() override {}

    /**
     * @brief LanguageChanged 语言改变通知
     */
    virtual void LanguageChanged() override {}

private:

    /**
     * @brief m_str_GUID 功能组件的唯一识别ID
     */
    QUuid m_guid;

};


#endif // DEFAULTPLUGIN_H
