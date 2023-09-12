#include "mainwindow.h"

#include <QApplication>
#include <stdio.h>
#include <iostream>
#include <QProcess>
#include <vector>

#if defined(_WIN64)
#include <winbreakpad.h>
#include <winHook.h>
//#include "../lib/hookPlus/include/winHook.h"
#elif defined(__linux__)
#include <linuxbreakpad.h>
#include <linuxhook.h>
#endif

using namespace std;

static const char black[]  = {0x1b, '[', '1', ';', '3', '0', 'm', 0};
static const char red[]    = {0x1b, '[', '1', ';', '3', '1', 'm', 0};
static const char green[]  = {0x1b, '[', '1', ';', '3', '2', 'm', 0};
static const char yellow[] = {0x1b, '[', '1', ';', '3', '3', 'm', 0};
static const char blue[]   = {0x1b, '[', '1', ';', '3', '4', 'm', 0};
static const char purple[] = {0x1b, '[', '1', ';', '3', '5', 'm', 0};
static const char normal[] = {0x1b, '[', '0', ';', '3', '9', 'm', 0};

/**
 * @brief getVert 判断cpu架构
 * @return
 */
std::string getVert()
{
#ifdef __x86_64__
    return "x86_64";
#elif defined __aarch64__
    return "aarch64";
#else
    return "unknown platform";
#endif
}

/**
 * @brief getSystem 判断操作系统
 * @return
 */
std::string getSystem()
{

#ifdef Q_OS_MAC
    return "mac";
#elif defined (__linux__)
    return "linux";
#elif defined (_WIN32)
    return "win32";
#elif defined (_WIN64)
    return "win64;
#else
    return "unknown system";
#endif

}

std::string getMake(){
#if defined(Q_OS_UNIX)
    return "linux";
#elif defined(Q_CC_MSVC)
    return "win-MSVC";
#elif defined(Q_CC_MINGW)
    return "win-MINGW";
#endif
}

void onExceptionCrashed(ExceptionInfo exceptionInfo){
    std::cout << exceptionInfo.toString() << std::endl;
    //TODO

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString appPath = QApplication::applicationDirPath();

    //注冊异常捕获函数（需要在MainWindow构造前注册）
    IBreakpad *breakpad = nullptr;
#if defined(_WIN64)

    AllocConsole();
    freopen("CONOUT$", "w+t", stdout);

    breakpad = new winBreakpad(appPath.toStdString());
    breakpad->instance()->setExceptionCrashedCallBack(std::bind(onExceptionCrashed, std::placeholders::_1));
#elif defined(__linux__)
    breakpad = new LinuxBreakpad(appPath.toStdString());
    breakpad->instance()->setExceptionCrashedCallBack(std::bind(onExceptionCrashed, std::placeholders::_1));
#endif

#if defined(_WIN64)
    bool installFlag = false;

    //1、远程线程注入，通过dll注入到目标进程中，目的：可以执行某段恶意代码，窃取密码,提权、进行HOOK等（目前注入成功，但HOOK不生效）
    //    bool addDebugFlag = WinHook::instance()->AddDebugPrivilege(); ///< 把自己的进程添加Debug权限
    //    DWORD PID = ProcessPidHelper::GetProcessPID(TEXT("breakpadPlusTest.exe"));
    //    installFlag = WinHook::instance()->RemoteInject(PID, TEXT("D:\\sourceCode\\Qt\\winBreakpad_test\\remoteHookPlus.dll"));

    //2、引用dll方式注入到本程序中，进行HOOK
//    vector<HookModel> models = {HookModel::Hook_MOUSE, HookModel::Hook_KEYBOARD};
//    installFlag = WinHook::instance()->installHook(models);

    if(installFlag){
//        WinHook::instance()->setExceptionCrashedCallBack(std::bind(&MainWindow::slot_hookChanged, this, std::placeholders::_1));
//        ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(
//            LoggerBaseLevel::INFO_LogLevel, "InstallHook success!")));
    }
    else{
//        DWORD lastError = GetLastError();
//        ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(
//            LoggerBaseLevel::INFO_LogLevel, QString("InstallHook error: %1").arg(lastError))));
    }

#elif defined(__linux__)
    LinuxHook::instance()->setExceptionCrashedCallBack(std::bind(onHookChanged, std::placeholders::_1));
#endif

//    std::cout << black << "[DEBUG]" << normal << ' ';

//    char *msg = new char[100];
//    size_t totalLength = 0;
//    size_t length = strlen(red);
//    memcpy(msg, red, length);
//    totalLength += length;

//    size_t length1 = strlen("[DEBUG]");
//    memcpy(msg + totalLength, "[DEBUG]", length1);
//    totalLength += length1;

//    size_t length2 = strlen(normal);
//    memcpy(msg + totalLength, normal, length2);
//    totalLength += length2;

//    char *temp = new char{' '};
//    size_t length3 = strlen(temp);
//    memcpy(msg + totalLength, temp, length3);
//    totalLength += length3;

//    size_t n = fwrite(msg, 1, totalLength, stdout);

    MainWindow w;
    w.show();
    int flag = a.exec();

    if(breakpad != nullptr){
        delete breakpad;
        breakpad = nullptr;
    }

    if(flag == CODE_RESTART)
    {
        QProcess::startDetached(qApp->applicationFilePath(), QStringList());
        return 0;
    }
    return flag;
}
