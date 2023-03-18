#include "globalInfoEnum.h"

GlobalInfo *GlobalInfo::m_globalInfo = nullptr;

GlobalInfo::GlobalInfo()
{

}

GlobalInfo::~GlobalInfo(){

}

QString GlobalInfo::getInfo(const int &value){
    if(m_allInfos.contains(value)){
        return m_allInfos.value(value);
    }
    return QString("没有找到[%1]对应的字符信息，请联系开发人员！").arg(value);
}
