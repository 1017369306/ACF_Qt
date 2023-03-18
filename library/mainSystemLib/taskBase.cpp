#include "taskBase.h"
#include <QDir>

taskBase::taskBase()
{
    this->m_guid = QUuid::createUuid();
}

taskBase::~taskBase()
{

}

int taskBase::startTask(){
    if(this->m_taskStatus != TaskStatus::Stoped)
        return GlobalInfo::GlobalInfoEnum::TaskIsNotStop;
    int flag = starting();
    if(flag != GlobalInfo::GlobalInfoEnum::DealSuccess)
    {
        return flag;
    }
    this->m_taskStatus = TaskStatus::Started;
    this->m_taskStartTime = QDateTime::currentDateTime();
    started();
    return GlobalInfo::GlobalInfoEnum::DealSuccess;
}

int taskBase::stopTask(const StopReason &stopReason){
    if(this->m_taskStatus == TaskStatus::Stoped)
        return GlobalInfo::GlobalInfoEnum::TaskIsStop;
    int flag = stoping();
    if(flag != GlobalInfo::GlobalInfoEnum::DealSuccess)
    {
        return flag;
    }
    this->m_taskStatus = TaskStatus::Stoped;
    stoped();
    return GlobalInfo::GlobalInfoEnum::DealSuccess;
}

int taskBase::pauseTask(){
    if(this->m_taskStatus != TaskStatus::Started)
        return GlobalInfo::GlobalInfoEnum::TaskIsNotStart;
    int flag = pausing();
    if(flag != GlobalInfo::GlobalInfoEnum::DealSuccess)
    {
        return flag;
    }
    this->m_taskStatus = TaskStatus::Paused;
    paused();
    return GlobalInfo::GlobalInfoEnum::DealSuccess;
}

int taskBase::continueTask(){
    if(this->m_taskStatus != TaskStatus::Paused)
        return GlobalInfo::GlobalInfoEnum::TaskIsNotPause;
    int flag = continuing();
    if(flag != GlobalInfo::GlobalInfoEnum::DealSuccess)
    {
        return flag;
    }
    this->m_taskStatus = TaskStatus::Started;
    continued();
    return GlobalInfo::GlobalInfoEnum::DealSuccess;
}

int taskBase::otherOrder(const CommandType &type, const QVariant &tag){
    int flag = otherOrdering(type, tag);
    if(flag != GlobalInfo::GlobalInfoEnum::DealSuccess)
    {
        return flag;
    }
    otherOrdered(type, tag);
    return GlobalInfo::GlobalInfoEnum::DealSuccess;
}



