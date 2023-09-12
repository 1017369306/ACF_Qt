#ifndef IBREAKPAD_H
#define IBREAKPAD_H

#include "breakpadPlus_global.h"
#include <string.h>
#include <vector>
#include <iostream>
#include <cstdio>
#include <functional>
#include <stdlib.h>
#include <fstream>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(_WIN64)
#include <direct.h>
#include <io.h>
#endif

#ifdef _MSC_VER
//#include <include/windows/dirent.h>
#include <string>

//解决中文乱码
#pragma execution_character_set("utf-8")
#else
#include <unistd.h>
#include <dirent.h>
#endif

using namespace std;

/**
 * @brief The ExceptionRegisters struct CONTEXT *的一些关键属性
 */
struct BREAKPADPLUS_EXPORT ExceptionRegisters{
    std::string EAX;
    std::string EBX;
    std::string ECX;
    std::string EDX;
    std::string ESI;
    std::string EDI;
    std::string CS_EIP;
    std::string SS_ESP;
    std::string EBP;
    std::string DS;
    std::string ES;
    std::string FS;
    std::string GS;
    std::string Flags;

    /**
     * @brief toString
     * @return
     */
    std::string toString(){
        std::string retStr;
        retStr.append("EAX: " + EAX + "\n");
        retStr.append("EBX: " + EBX + "\n");
        retStr.append("ECX: " + ECX + "\n");
        retStr.append("EDX: " + EDX + "\n");
        retStr.append("ESI: " + ESI + "\n");
        retStr.append("EDI: " + EDI + "\n");
        retStr.append("CS_EIP: " + CS_EIP + "\n");
        retStr.append("SS_ESP: " + SS_ESP + "\n");
        retStr.append("EBP: " + EBP + "\n");
        retStr.append("DS: " + DS + "\n");
        retStr.append("ES: " + ES + "\n");
        retStr.append("FS: " + FS + "\n");
        retStr.append("GS: " + GS + "\n");
        retStr.append("Flags: " + Flags + "\n");
        return retStr;
    }
};

/**
 * @brief The ExceptionItem struct 异常子项
 */
struct BREAKPADPLUS_EXPORT ExceptionItem
{
    /**
     * @brief Address 内存地址
     */
    std::string Address;

    /**
     * @brief Module 模块
     */
    std::string Module;

    /**
     * @brief File 文件
     */
    std::string File;

    /**
     * @brief Function 函数/方法
     */
    std::string Function;

    /**
     * @brief DecodeError 崩溃分析，定位失败信息
     */
    std::string DecodeError;

    /**
     * @brief Line 行号
     */
    int Line = 0;

    /**
     * @brief toString
     * @return
     */
    std::string toString(){
        std::string retStr;
        retStr.append("Address: " + Address + "\n");
        retStr.append("Module: " + Module + "\n");
        retStr.append("File: " + File + "\n");
        retStr.append("Function: " + Function + "\n");
        retStr.append("Line: " + to_string(Line) + "\n");
        retStr.append("DecodeError: " + DecodeError + "\n");
        return retStr;
    }

};

/**
 * @brief The ExceptionInfo struct 崩溃信息结构体
 */
struct BREAKPADPLUS_EXPORT ExceptionInfo
{
    /**
     * @brief ApplicationName 异常的程序名
     */
    std::string ApplicationName;

    /**
     * @brief ApplicationDirPath 异常的程序运行全路径（程序目录+程序名）
     */
    std::string ApplicationDirPath;

    /**
     * @brief CrashLastTime 异常的最后时间
     */
    std::string CrashLastTime;

    /**
     * @brief ExceptionCode 异常编号
     */
    std::string ExceptionCode;

    /**
     * @brief FaultAddress 失败/错误时的内存地址
     */
    std::string FaultAddress;

    /**
     * @brief Registers CONTEXT *的一些关键属性
     */
    ExceptionRegisters Registers;

    /**
     * @brief Items 异常的所有子项
     */
    vector<ExceptionItem> Items;

public:

    /**
     * @brief toString
     * @return
     */
    std::string toString(){
        std::string retStr;
        retStr.append("//=====================================================\n");
        retStr.append("ApplicationName: " + ApplicationName + "\n");
        retStr.append("ApplicationDirPath: " + ApplicationDirPath + "\n");
        retStr.append("Crash Last Time: " + CrashLastTime + "\n");
        retStr.append("Exception code: " + ExceptionCode + "\n");
        retStr.append("Fault address: " + FaultAddress + "\n");
        retStr.append("\nRegisters: \n" + Registers.toString() + "\n");
        retStr.append("Call stack: \n");
        for(int i = 0; i < Items.size(); i++){
            ExceptionItem item = Items.at(i);
            retStr.append(item.toString() + "\n");
        }
        return retStr;
    }

};

/**
 * @brief ExceptionCrashedCallBack 发生了异常，并分析、定位到完整的异常信息后的回调函数
 */
typedef std::function<void(ExceptionInfo)> ExceptionCrashedCallBack;

/**
 * @brief The IBreakpad class 崩溃分析、定位组件的接口
 * 作者 张洋
 * 创建日期 2023-08-28
 */
class BREAKPADPLUS_EXPORT IBreakpad
{
    //接口属性定义
public:

    /**
     * @brief IBreakpad 构造函数
     * @param appPath 运行全路径（程序目录+程序名）
     */
    IBreakpad(std::string appPath){
        m_appPath = appPath;
        int indexTemp = m_appPath.find_last_of('/');
        if(indexTemp > 0){
            m_appDir = m_appPath.substr(0, indexTemp);
            m_appName = m_appPath.substr(indexTemp + 1);
        }
    }

    /**
     * @brief instance 子类实现，返回真正处理异常的类指针
     * @return
     */
    virtual IBreakpad *instance()=0;

    /**
     * @brief getAppPath 获取程序运行全路径（程序目录+程序名）
     * @return
     */
    std::string getAppPath() const {return m_appPath;}

    /**
     * @brief getAppName 获取程序名
     * @return
     */
    std::string getAppName() const {return m_appName;}

    /**
     * @brief getAppDir 获取程序目录
     * @return
     */
    std::string getAppDir() const {return m_appDir;}

    /**
     * @brief setExceptionCrashedCallBack 设置异常回调函数
     * @param callBack
     */
    void setExceptionCrashedCallBack(ExceptionCrashedCallBack callBack)
    {
        m_exceptionCrashedCallBack = std::move(callBack);
    }
    ExceptionCrashedCallBack getExceptionCrashedCallBack() const{
        return m_exceptionCrashedCallBack;
    }

public:

    static void GetNowTime(struct tm& nowTime){
        memset(&nowTime, 0, sizeof(struct tm));
        time_t t = time(NULL);
        struct tm* pTime = localtime(&t);
        if (pTime)
        {
            nowTime.tm_sec = pTime->tm_sec;
            nowTime.tm_min = pTime->tm_min;
            nowTime.tm_hour = pTime->tm_hour;
            nowTime.tm_mday = pTime->tm_mday;
            nowTime.tm_mon = pTime->tm_mon;
            nowTime.tm_year = pTime->tm_year;
            nowTime.tm_wday = pTime->tm_wday;
            nowTime.tm_yday = pTime->tm_yday;
            nowTime.tm_isdst = pTime->tm_isdst;
        }
    }

    /**
     * @brief accessHelper 获取文件夹或文件的权限的帮助函数
     * @param path
     * @param mode 00表示只判断是否存在，02判断写权限， 04判断读权限，06判断读和写权限
     * @return
     */
    static bool accessHelper(std::string path, int mode){
        int ret = access(path.c_str(), mode);
        return (ret == 0);
    }

    /**
     * @brief getDirectoryExist 判断文件夹是否存在
     * @param path
     * @return
     */
    static bool getDirectoryExist(std::string path){
        return accessHelper(path, 0);
    }

    /**
     * @brief createDirectory 创建文件夹
     * @param path
     * @return
     */
    static bool createDirectory(std::string path)
    {
        bool exist = getDirectoryExist(path.c_str());
        if (exist)
        {
            return true;
        }
        int ret = -1;

        umask(0);

#if defined(_WIN64)
        ret = mkdir(path.c_str());
#else
        ret = mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);//0x777);
#endif

        return (ret == 0);
    }

    /**
    * 字符串分割函数
    * @param str 字符串
    * @param pattern 分隔符
    * @return
    */
    static std::vector<std::string> split(std::string str, std::string pattern){
        std::string::size_type pos;
        std::vector<std::string> result;
        str += pattern;//扩展字符串以方便操作
        int size = str.size();
        for (int i = 0; i < size; i++)
        {
            pos = str.find(pattern, i);
            if (pos < size)
            {
                std::string s = str.substr(i, pos - i);
                result.push_back(s);
                i = pos + pattern.size() - 1;
            }
        }
        return result;
    }

private:

    /**
     * @brief m_appPath 程序运行全路径（程序目录+程序名）
     */
    std::string m_appPath;

    /**
     * @brief m_appName 程序名
     */
    std::string m_appName;

    /**
     * @brief m_appDir 程序目录
     */
    std::string m_appDir;

    /**
     * @brief m_exceptionCrashedCallBack 发生了异常，并分析、定位到完整的异常信息后的回调函数
     */
    ExceptionCrashedCallBack m_exceptionCrashedCallBack;

};

#endif // IBREAKPAD_H
