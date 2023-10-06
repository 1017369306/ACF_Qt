#ifndef SYSTEMINFOHELPER_H
#define SYSTEMINFOHELPER_H

#include "mainSystemLib_global.h"
#include <QObject>
#include <QSysInfo>

/**
 * @brief The SystemInfo struct 系统信息
 */
struct MAINSYSTEMLIB_EXPORT SystemInfo
{
    /**
     * @brief productType 当前系统的产品类型
     */
    QString productType;
    /**
     * @brief productVersion 当前系统的产品版本
     */
    QString productVersion;
    /**
     * @brief kernelType 当前系统的内核类型
     */
    QString kernelType;
    /**
     * @brief kernelVersion 当前系统的内核版本号
     */
    QString kernelVersion;
    /**
     * @brief currentCpuArchitecture 当前系统的CPU架构类型
     */
    QString currentCpuArchitecture;
    /**
     * @brief buildCpuArchitecture Qt库的编译CPU架构类型
     */
    QString buildCpuArchitecture;
    /**
     * @brief buildAbi Qt库的ABI（应用程序二进制接口）类型
     */
    QString buildAbi;
};

/**
 * @brief The SystemInfoHelper class 获取系统信息的帮助类
 */
class MAINSYSTEMLIB_EXPORT SystemInfoHelper
{
public:

    /**
     * @brief getSystemInfo 获取系统信息
     * @return
     */
    static SystemInfo getSystemInfo();

};

#endif // SYSTEMINFOHELPER_H
