#include "notify2class.h"
#include <globalInfoEnum.h>
#include <QMessageBox>
#include <globalhelper.h>
#include <loggerBase.h>
#include <globalSizes.h>
#include <globalColors.h>
#include <frameworktool.h>

Notify2Class::Notify2Class() : DefaultPlugin() {

}

Notify2Class::~Notify2Class(){

}

 /**
 * @brief initModule 初始化模块
 * @return true：初始化成功；false:初始化失败
 */
int Notify2Class::initModule(){
    int result = GlobalInfo::GlobalInfoEnum::DealFatal;

    // 可建立全局实例
    if(manager == nullptr){
        manager = new NotifyManager(qApp);
    }

    if(!frameworkTool::getCustomCssList().contains(":/qss/notifyTheme.css")){
        frameworkTool::appendCustomCss(":/qss/notifyTheme.css");
        frameworkTool::reLoadCssStyle();

        this->cssStyleChanged();
    }

//    manager->setStyleSheet("#notify-background {....", "自定义主题名称"); // 添加自定义主题样式表，默认样式主题名为default

//    // 基本用法
//    manager->notify("消息标题", "消息主体");

//    // 追加扩展数据
//    QVariantMap data;
//    data["icon"] = ":/messageXXX.png"; // 自定义消息图标，也可传入QPixmap
//    //data["icon"] = QPixmap(xxx);
//    data["name"] = "张三";
//    data["location"] = "XX市XX路";
//    data["money"] = 30.59;
//    data["theme"] = "black"; // 自定义主题样式，除默认样式，其它需预先添加后使用
//    // 也可直接自定义样式表如下
//    // data["styleSheet"] = "#notify-background {...."
//    data["displayTime"] = 30*1000; // 自定义显示时间（毫秒）
//    manager->notify("支付信息", "XXX商家XXX商品", data);

    // 点击消息处理
    disconnect(manager, &NotifyManager::notifyDetail, this, &Notify2Class::slot_notifyDetail);
    connect(manager, &NotifyManager::notifyDetail, this, &Notify2Class::slot_notifyDetail);

    return result;
}

void Notify2Class::cssStyleChanged(){
    SvgHelper::updateSvg("./appPics/svgs/theme/info.svg", GlobalColors::instance()->InfoColor());
    SvgHelper::updateSvg("./appPics/svgs/theme/success.svg", GlobalColors::instance()->SuccessColor());
    SvgHelper::updateSvg("./appPics/svgs/theme/warn.svg", GlobalColors::instance()->WarningColor());
    SvgHelper::updateSvg("./appPics/svgs/theme/error.svg", GlobalColors::instance()->DangerColor());
    SvgHelper::updateSvg("./appPics/svgs/theme/fatal.svg", GlobalColors::instance()->PrimaryTextColor());

    // 可选修改默认参数
    manager->setMaxCount(3); // 最大显示消息数，默认5
    manager->setDisplayTime(5000); // 显示时间，默认10000（毫秒）
    manager->setAnimateTime(300); // 动画时间，默认300（毫秒）
    manager->setSpacing(GlobalSizes::instance()->DefaultControlPadding().width() / 2.0); // 消息框间距，默认10px
    manager->setCornerMargins(GlobalSizes::instance()->DefaultControlPadding().width(), GlobalSizes::instance()->DefaultControlPadding().width()); // 右下角边距，默认10, 10
    manager->setNotifyWndSize(GlobalSizes::instance()->DefaultControlHeight() * 9, GlobalSizes::instance()->DefaultControlHeight() * 3.2); // 消息框大小，默认300, 60
    manager->setDefaultIcon("./appPics/svgs/theme/info.svg"); // 消息图标，默认":/message.png"
    manager->setShowQueueCount(true); // 是否显示超出最大数未显示的消息数量，默认true
    manager->setIconSize(QSize(GlobalSizes::instance()->DefaultIconHeight(), GlobalSizes::instance()->DefaultIconHeight()));//设置左上角图标大小

    manager->setStyleSheet(frameworkTool::getAppCss(), "default");
}

void Notify2Class::slot_notifyDetail(const QVariantMap &data){
    QWidget *widget = GlobalMethods::getMainWindow();
    if(widget != nullptr){
        QMessageBox::information(widget, data.value(def_notifyTitle).toString(), data.value(def_notifyBody).toString());
    }
}

 /**
 * @brief disModule 销毁模块
 */
void Notify2Class::disModule(){
    if(manager != nullptr){
        delete manager;
        manager = nullptr;
    }
}
