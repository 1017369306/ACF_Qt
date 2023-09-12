#include "systeminfohelper.h"

SystemInfo SystemInfoHelper::getSystemInfo(){
    SystemInfo systemInfo = SystemInfo();
    systemInfo.productType = QSysInfo::productType();
    systemInfo.productVersion = QSysInfo::productVersion();
    systemInfo.kernelType = QSysInfo::kernelType();
    systemInfo.kernelVersion = QSysInfo::kernelVersion();
    systemInfo.currentCpuArchitecture = QSysInfo::currentCpuArchitecture();
    systemInfo.buildCpuArchitecture = QSysInfo::buildCpuArchitecture();
    systemInfo.buildAbi = QSysInfo::buildAbi();

    return systemInfo;
}
