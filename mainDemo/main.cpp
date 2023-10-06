#include "mainwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <stdio.h>
#include <iostream>
#include <QProcess>
#include <vector>
#include <QThread>
#include <QGuiApplication>
#include <QScreen>

#include "login.h"
#include <logintool.h>
#include <acfproperty.h>

#if defined(_WIN64)
#include <winbreakpad.h>
#include <winHook.h>
#include <frameworktool.h>
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
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling );
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);
    QString appPath = QApplication::applicationDirPath();

    //注冊异常捕获函数（需要在MainWindow构造前注册）
    IBreakpad *breakpad = nullptr;
#if defined(_WIN64)

//    AllocConsole();
//    freopen("CONOUT$", "w+t", stdout);

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

    qDebug() << "UI threadID: " << QThread::currentThreadId();
    LoginTool::instance()->resetProcess();

    //要在加载前获取到样式表（含login窗体的自定义样式），在显示login窗体前设置其样式
    FrameworkTool::initResolution();
    FrameworkTool::appendCustomCss(":/qss/customTheme.css");
    QString css = FrameworkTool::getCssStyle();

    QHash<int, QString> processHash;
    //每一项表示此业务完成时的进度
    processHash.insert(10, "加载中...");
    processHash.insert(20, "读取系统配置...");
    processHash.insert(50, "初始化系统框架...");
    processHash.insert(60, "加载插件/工具模块...");
    processHash.insert(90, "加载主界面...");
    processHash.insert(100, "加载完成，等待进入系统...");

    double loginWidth = QGuiApplication::primaryScreen()->geometry().width() * 0.5;
    double loginHeight = QGuiApplication::primaryScreen()->geometry().height() * 0.4;
    double xTemp = (QGuiApplication::primaryScreen()->geometry().width() - loginWidth) / 2.0;
    double yTemp = (QGuiApplication::primaryScreen()->geometry().height() - loginHeight) / 2.0;

    QPixmap pixmap(":/appPics/login/background.jpg");
    pixmap = pixmap.scaled(loginWidth, loginHeight, Qt::KeepAspectRatioByExpanding);

    Login *login = new Login(pixmap);
    //居中
    login->setGeometry(xTemp, yTemp, loginWidth, loginHeight);
    login->setFixedSize(loginWidth, loginHeight);
    login->setProcessHash(processHash);
    login->show();
    login->init();
    //设置自定义样式（奇怪，这里要在setProperty后再设置setStyleSheet，为啥？）
    if(!css.isEmpty()){
        login->setStyleSheet(css);
    }

    //1、加载中
    //TODO
    LoginTool::instance()->setMilestoneProcess(10);
    //初始化框架功级参数类单例
    ACFProperty::instance();
    QThread::msleep(100);
    //强制立即处理所有的事件，达到立即更新UI的效果
    QCoreApplication::processEvents();
    LoginTool::instance()->setCurrentProcess(10);

    //2、读取系统配置
    //TODO
    LoginTool::instance()->setMilestoneProcess(20);
    QThread::msleep(100);
    QCoreApplication::processEvents();
    LoginTool::instance()->setCurrentProcess(20);

    //3、初始化系统框架
    //TODO
    LoginTool::instance()->setMilestoneProcess(50);
    for(int i = 0; i < 2; i++){
        QThread::msleep(100);
        QCoreApplication::processEvents();
    }
    LoginTool::instance()->setCurrentProcess(50);

    //4、加载插件/工具模块
    //TODO
    LoginTool::instance()->setMilestoneProcess(60);
    QString path = qApp->applicationDirPath() + "/plugins";
    //设置插件的路径
    PluginManager::instance()->setPluginDir(path);
    //获取此路径下的所有插件的信息
    PluginManager::instance()->loadAllPlugins();
//    for(int i = 0; i < 2; i++){
//        QThread::msleep(100);
//        QCoreApplication::processEvents();
//    }
    LoginTool::instance()->setCurrentProcess(60);

    //5、加载主界面
    //TODO
    LoginTool::instance()->setMilestoneProcess(90);
    MainWindow w;
    //主窗体先设置透明，加载完成后再恢复
    w.setWindowOpacity(0);
    //初始化中执行耗时业务
//    w.init();
    //show也会占用一点时间
    w.show();
    LoginTool::instance()->setCurrentProcess(90);

    //6、加载完成，等待进入系统
    //TODO
    LoginTool::instance()->setMilestoneProcess(100);
    for(int i = 0;i < 2; i++){
        QThread::msleep(100);
        QCoreApplication::processEvents();
    }
    LoginTool::instance()->setCurrentProcess(100);

    //加载完成后再恢复
    w.setWindowOpacity(1);
    login->setParent(nullptr);
    delete login;
//    login->deleteLater();

//    MainWindow w;
//    w.show();

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
