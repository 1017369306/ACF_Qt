#include "moduleBase.h"
#include <QJsonObject>

ModuleBase::ModuleBase(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);

    this->m_guid = QUuid::createUuid(); //new guid
}

ModuleBase::~ModuleBase()
{
    if(m_PopMenu != nullptr)
    {
        this->m_PopMenu->clear();
        delete m_PopMenu;
        m_PopMenu = nullptr;
    }
}

int ModuleBase::initModule(){
    m_moduleProperty = ModuleBase::convertFrom(getPlugInProperty());
    return GlobalInfo::GlobalInfoEnum::DealSuccess;
}

ModuleProperty ModuleBase::convertFrom(const PlugInProperty &plugInProperty){
    ModuleProperty property;
    property.name = plugInProperty.name;
    property.displayName = plugInProperty.displayName;
    property.description = plugInProperty.description;
    property.icon = plugInProperty.icon;

    QJsonObject json = plugInProperty.customData.toJsonObject();
    if(!json.isEmpty()){
        if(json.contains(PluginJson_MultipleInstance)){
            property.multipleInstance = json.value(PluginJson_MultipleInstance).toInt();
        }
        if(json.contains(PluginJson_CanClose)){
            property.canClose = json.value(PluginJson_CanClose).toBool();
        }
        if(json.contains(PluginJson_Group)){
            property.group = (ModuleGroup)json.value(PluginJson_Group).toInt();
        }
        if(json.contains(PluginJson_Index)){
            property.index = json.value(PluginJson_Index).toInt();
        }
    }
    return property;
}

TaskStatus ModuleBase::getTaskRuning(){
    if(this->getTaskObject() == nullptr)
        return TaskStatus::Stoped;
    return this->getTaskObject()->getTaskState();
}

int ModuleBase::uiOrder(const CommandType &order, QVariant tag)
{
    //TODO
//    hrlogger::WriteLog(hrlogger::DebugLevel, "用户操作了(ToolBarMenuItem)：" + QString("%1").arg((int)order));

    int orderResult = GlobalInfo::GlobalInfoEnum::DealFatal;
    switch (order)
    {
    case CommandType::Measure_Run:
    {
        orderResult = this->StartTask();
        break;
    }
    case CommandType::Measure_Stop:
    {
        if(tag.isNull())
        {
            orderResult = this->StopTask(StopReason::ClickByUser);
        }
        else{
            bool isInt = false;
            int index = tag.toInt(&isInt);
            if(isInt){
                orderResult = this->StopTask((StopReason)index);
            }
        }
        break;
    }
    case CommandType::Measure_Pause:
    {
        orderResult = this->PauseTask();
        break;
    }
    case CommandType::Measure_Continue:
    {
        orderResult = this->ContinueTask();
        break;
    }
    default:
    {
        orderResult = otherOrdering();
        if(orderResult){
            orderResult = this->m_taskobj->otherOrder(order, tag);
            if(orderResult){
                otherOrdered();
            }
        }
        break;
    }
    }
    //TODO
    //write logger
    return orderResult;
}

int ModuleBase::StartTask()
{
    int orderResult = GlobalInfo::GlobalInfoEnum::DealFatal;
    if(this->getTaskRuning() != TaskStatus::Started)
    {
        //实现类此方法内部发生异常，作为框架如何处理异常？如何让程序不崩溃？
        orderResult = starting();
        if(!orderResult)
        {
            //TODO
//            hrlogger::WriteLog(hrlogger::DebugLevel, QString("任务在%1处启动失败！").arg("starting"));
        }
        else
        {
            //开始任务
            orderResult = this->m_taskobj->startTask();
            if(!orderResult)
            {
                //TODO
//                hrlogger::WriteLog(hrlogger::DebugLevel, QString("任务在%1处启动失败！").arg("Taskobj->runTask"));
            }
            else{
                //TODO
                //实现类此方法内部发生异常，作为框架如何处理异常？如何让程序不崩溃？
                started();
            }
        }
        if(orderResult){
            //TODO
//            RunInfoPanelMgr::GetInstance()->Notice(RunInfoPanel::SuccessLevel, tr("任务启动成功！"));
        }
        else{
            //TODO
//            RunInfoPanelMgr::GetInstance()->Notice(RunInfoPanel::ErrorLevel, tr("任务启动失败！"));
        }
    }
    return orderResult;
}

int ModuleBase::StopTask(StopReason stopReason)
{
    int orderResult = GlobalInfo::GlobalInfoEnum::DealFatal;
    if(this->getTaskRuning() != TaskStatus::Stoped)
    {
        //实现类此方法内部发生异常，作为框架如何处理异常？如何让程序不崩溃？
        orderResult = stoping();
        if(!orderResult)
        {
            //TODO
//            hrlogger::WriteLog(hrlogger::DebugLevel, QString("任务在%1处启动失败！").arg("starting"));
        }
        else
        {
            //开始任务
            orderResult = this->m_taskobj->stopTask(stopReason);
            if(!orderResult)
            {
                //TODO
//                hrlogger::WriteLog(hrlogger::DebugLevel, QString("任务在%1处启动失败！").arg("Taskobj->runTask"));
            }
            else{
                //TODO
                //实现类此方法内部发生异常，作为框架如何处理异常？如何让程序不崩溃？
                stoped();
            }
        }
        if(orderResult){
            //TODO
//            RunInfoPanelMgr::GetInstance()->Notice(RunInfoPanel::SuccessLevel, tr("任务启动成功！"));
        }
        else{
            //TODO
//            RunInfoPanelMgr::GetInstance()->Notice(RunInfoPanel::ErrorLevel, tr("任务启动失败！"));
        }
    }
    return orderResult;
}

int ModuleBase::PauseTask()
{
    int orderResult = GlobalInfo::GlobalInfoEnum::DealFatal;
    if(this->getTaskRuning() == TaskStatus::Started)
    {
        //实现类此方法内部发生异常，作为框架如何处理异常？如何让程序不崩溃？
        orderResult = pausing();
        if(!orderResult)
        {
            //TODO
//            hrlogger::WriteLog(hrlogger::DebugLevel, QString("任务在%1处启动失败！").arg("starting"));
        }
        else
        {
            //开始任务
            orderResult = this->m_taskobj->pauseTask();
            if(!orderResult)
            {
                //TODO
//                hrlogger::WriteLog(hrlogger::DebugLevel, QString("任务在%1处启动失败！").arg("Taskobj->runTask"));
            }
            else{
                //TODO
                //实现类此方法内部发生异常，作为框架如何处理异常？如何让程序不崩溃？
                paused();
            }
        }
        if(orderResult){
            //TODO
//            RunInfoPanelMgr::GetInstance()->Notice(RunInfoPanel::SuccessLevel, tr("任务启动成功！"));
        }
        else{
            //TODO
//            RunInfoPanelMgr::GetInstance()->Notice(RunInfoPanel::ErrorLevel, tr("任务启动失败！"));
        }
    }
    return orderResult;
}

int ModuleBase::ContinueTask()
{
    int orderResult = GlobalInfo::GlobalInfoEnum::DealFatal;
    if(this->getTaskRuning() == TaskStatus::Paused)
    {
        //实现类此方法内部发生异常，作为框架如何处理异常？如何让程序不崩溃？
        orderResult = continuing();
        if(!orderResult)
        {
            //TODO
//            hrlogger::WriteLog(hrlogger::DebugLevel, QString("任务在%1处启动失败！").arg("starting"));
        }
        else
        {
            //开始任务
            orderResult = this->m_taskobj->continueTask();
            if(!orderResult)
            {
                //TODO
//                hrlogger::WriteLog(hrlogger::DebugLevel, QString("任务在%1处启动失败！").arg("Taskobj->runTask"));
            }
            else{
                //TODO
                //实现类此方法内部发生异常，作为框架如何处理异常？如何让程序不崩溃？
                continued();
            }
        }
        if(orderResult){
            //TODO
//            RunInfoPanelMgr::GetInstance()->Notice(RunInfoPanel::SuccessLevel, tr("任务启动成功！"));
        }
        else{
            //TODO
//            RunInfoPanelMgr::GetInstance()->Notice(RunInfoPanel::ErrorLevel, tr("任务启动失败！"));
        }
    }
    return orderResult;
}

int ModuleBase::starting(){
    return GlobalInfo::GlobalInfoEnum::DealSuccess;
}

int ModuleBase::stoping(){
    return GlobalInfo::GlobalInfoEnum::DealSuccess;
}

int ModuleBase::pausing(){
    return GlobalInfo::GlobalInfoEnum::DealSuccess;
}

int ModuleBase::continuing(){
    return GlobalInfo::GlobalInfoEnum::DealSuccess;
}

int ModuleBase::otherOrdering(){
    return GlobalInfo::GlobalInfoEnum::DealSuccess;
}

void ModuleBase::started(){

}

void ModuleBase::stoped(){

}

void ModuleBase::paused(){

}

void ModuleBase::continued(){

}

void ModuleBase::otherOrdered(){

}
