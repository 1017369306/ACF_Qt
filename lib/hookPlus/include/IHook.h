#ifndef IHook_H
#define IHook_H

#include "hookPlus_global.h"
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
#include <string>

//解决中文乱码
#pragma execution_character_set("utf-8")
#else
#include <unistd.h>
#include <dirent.h>
#endif

using namespace std;

#ifdef __GNUC__
#    define GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > (x) || __GNUC__ == (x) && __GNUC_MINOR__ >= (y))
#else
#    define GCC_VERSION_AT_LEAST(x,y) 0
#endif

#if GCC_VERSION_AT_LEAST(3,1)
#    define attribute_deprecated __attribute__((deprecated))
#elif defined(_MSC_VER)
#    define attribute_deprecated __declspec(deprecated)
#else
#    define attribute_deprecated
#endif

//#define WH_JOURNALRECORD 0
//#define WH_JOURNALPLAYBACK 1
//#define WH_KEYBOARD 2
//#define WH_GETMESSAGE 3
//#define WH_CALLWNDPROC 4
//#define WH_CBT 5
//#define WH_SYSMSGFILTER 6
//#define WH_MOUSE 7
//#define WH_HARDWARE 8
//#define WH_DEBUG 9
//#define WH_SHELL 10
//#define WH_FOREGROUNDIDLE 11
//#define WH_CALLWNDPROCRET 12

//#define WH_KEYBOARD_LL 13
//#define WH_MOUSE_LL 14

/**
 * @brief 注册的钩子类型
 */
enum HookModel
{
    Hook_NULL = -1,
    /**
     * @brief 键盘
     */
    Hook_KEYBOARD = 2,
    /**
     * @brief 鼠标
     */
    Hook_MOUSE = 7
};

/**
 * @brief 行为类型
 */
enum BehaviorTypeEnum{
    /**
     * @brief 未知的类型
     */
    UnKnown = 0,
    /**
     * @brief 鼠标左键按下
     */
    MouseLeftDown,
    /**
     * @brief 鼠标左键释放
     */
    MouseLeftUp,
    /**
     * @brief 鼠标右键按下
     */
    MouseRightDown,
    /**
     * @brief 鼠标右键释放
     */
    MouseRightUp,
    /**
     * @brief 鼠标滚轮向下
     */
    RollDown,
    /**
     * @brief 鼠标滚轮向上
     */
    RollUp,
    /**
     * @brief 按键按下
     */
    KeyDown,
    /**
     * @brief 按键释放
     */
    KeyUp
//    /**
//     * @brief 鼠标移动
//     */
//    MouseMove

};

//#pragma pack(1)

/**
 * @brief 用户行为信息结构体
 */
struct HOOKPLUS_EXPORT BehaviorInfo
{
    /**
     * @brief 最后时间的长度
     */
    int LastTimelength = 0;
    /**
     * @brief 最后时间
     */
    char* LastTime = nullptr;

    /**
     * @brief 行为类型
     */
    BehaviorTypeEnum BehaviorType;

    /**
     * @brief 枚举值的长度
     */
    int CodeLength = 0;

    /**
     * @brief 枚举值
     */
    int* Code = nullptr;

    /**
     * @brief 值的长度
     */
    int ValueLength = 0;

    /**
     * @brief 值
     */
    char* Value = nullptr;

    /**
     * @brief 触发时的坐标X值
     */
    int X = 0;

    /**
     * @brief 触发时的坐标Y值
     */
    int Y = 0;

public:

    /**
     * @brief 获取此结构体的数据长度
     * @return
     */
    int getLength() const {
        return LastTimelength + sizeof (BehaviorTypeEnum) + CodeLength + ValueLength + sizeof (int) * 2;
    }

    /**
     * @brief toString
     * @return
     */
    std::string toString(){
        std::string retStr;
        retStr.append("//=====================================================\n");
        retStr.append("LastTime: " + std::string(LastTime) + "\n");
        retStr.append("BehaviorType: " + to_string(BehaviorType) + "\n");
        retStr.append("Code: ");
        for(int i = 0; i < CodeLength; i++){
            if(i == 0){
                retStr.append(to_string(Code[i]));
            }
            else{
                retStr.append(", " + to_string(Code[i]));
            }
        }
        retStr.append("\n");
        retStr.append("Value: " + std::string(Value) + "\n");
        retStr.append("X: " + to_string(X) + "\n");
        retStr.append("Y: " + to_string(Y) + "\n");
        return retStr;
    }

    /**
     * @brief 释放内存
     */
    void dispose(){
        if(LastTime != nullptr){
            delete[] LastTime;
            LastTime = nullptr;
        }
        if(Code != nullptr){
            delete[] Code;
            Code = nullptr;
        }
        if(Value != nullptr){
            delete[] Value;
            Value = nullptr;
        }
        LastTimelength = 0;
        CodeLength = 0;
        ValueLength = 0;
        BehaviorType = BehaviorTypeEnum::UnKnown;
        X = 0;
        Y = 0;
    }

};

//#pragma pack()

/**
 * @brief 特殊码的状态（指示灯开启、处于按下状态？）
 */
struct HOOKPLUS_EXPORT SpecialCodeStatus
{
    bool CapOn;
    bool CapDown;
    bool ShiftOn;
    bool ShiftDown;
    bool CtrlOn;
    bool CtrlDown;
    bool AltOn;
    bool AltDown;

public:

    /**
     * @brief toString
     * @return
     */
    std::string toString(){
        std::string retStr;
        retStr.append("//=====================SpecialCodeStatus========================\n");
        retStr.append("CapOn: " + boolToString(CapOn) + "\n");
        retStr.append("CapDown: " + boolToString(CapDown) + "\n");
        retStr.append("ShiftOn: " + boolToString(ShiftOn) + "\n");
        retStr.append("ShiftDown: " + boolToString(ShiftDown) + "\n");
        retStr.append("CtrlOn: " + boolToString(CtrlOn) + "\n");
        retStr.append("CtrlDown: " + boolToString(CtrlDown) + "\n");
        retStr.append("AltOn: " + boolToString(AltOn) + "\n");
        retStr.append("AltDown: " + boolToString(AltDown) + "\n");
        return retStr;
    }

private:

    std::string boolToString(bool value){
        return value ? "true" : "false";
    }

};

/**
 * @brief 捕获到行为时的回调函数
 */
typedef std::function<void(BehaviorInfo)> BehaviorCatchedCallBack;

/**
 * @brief 用户行为记录组件的接口
 * 作者 张洋
 * 创建日期 2023-08-28
 */
class HOOKPLUS_EXPORT IHook
{
    //接口属性定义
public:

    /**
     * @brief 构造函数
     * @param 运行全路径（程序目录+程序名）
     */
    IHook(){}

    /**
     * @brief 安装钩子函数
     * @param model 钩子类型
     */
    virtual bool installHook(vector<HookModel> models)=0;

    /**
     * @brief 卸载钩子
     * @param model 钩子类型
     */
    virtual void unInstallHook(vector<HookModel> models)=0;

    /**
     * @brief 设置回调函数
     * @param
     */
    void setExceptionCrashedCallBack(BehaviorCatchedCallBack callBack)
    {
        m_exceptionCrashedCallBack = std::move(callBack);
    }
    BehaviorCatchedCallBack getExceptionCrashedCallBack() const{
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

    static std::string GetNowTime(){
        struct tm nowTime;
        GetNowTime(nowTime);
        char Date_Key[32] = {0};
        sprintf(Date_Key, "%04d-%02d-%02d %02d:%02d:%02d", nowTime.tm_year + 1900,
                                                     nowTime.tm_mon + 1, nowTime.tm_mday, nowTime.tm_hour,
                                                     nowTime.tm_min, nowTime.tm_sec);
        std::string retTime = std::string(Date_Key);
        return retTime;
    }

    /**
     * @brief 获取文件夹或文件的权限的帮助函数
     * @param
     * @param 00表示只判断是否存在，02判断写权限， 04判断读权限，06判断读和写权限
     * @return
     */
    static bool accessHelper(std::string path, int mode){
        int ret = access(path.c_str(), mode);
        return (ret == 0);
    }

    /**
     * @brief 判断文件夹是否存在
     * @param
     * @return
     */
    static bool getDirectoryExist(std::string path){
        return accessHelper(path, 0);
    }

    /**
     * @brief 创建文件夹
     * @param
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
    * @param 字符串
    * @param 分隔符
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
     * @brief 捕获到行为时的回调函数
     */
    BehaviorCatchedCallBack m_exceptionCrashedCallBack;

};

#endif // IHook_H
