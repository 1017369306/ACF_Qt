QT -= gui
QT += network

#大于Qt4的版本才包含widgets模块
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#生成的动态库的名字
TARGET = globalInterface
#模板类型，选择动态库模板
TEMPLATE = lib
#定义动态库的宏，我们需要在输出的地方使用这个宏来定义，比如一个类，一个函数等。
#注意：有时候我们看到文件输出了.dll，但是没有文件.lib，就是没有使用这个宏去输出我们的文件。
DEFINES += GLOBALINTERFACE_LIBRARY

CONFIG += c++11
CONFIG -= debug_and_release
#指定只生成静态库
#CONFIG += staticlib

msvc {
    #解决msvc乱码
    QMAKE_CXXFLAGS += -execution-charset:utf-8
    QMAKE_CXXFLAGS += -source-charset:utf-8
    QMAKE_CXXFLAGS_WARN_ON += -wd4819

    #生成*.pdb文件
    CONFIG += force_debug_info
    CONFIG += separate_debug_info

}
else {
    #在release下可以生成调试信息

    #禁用优化 ，启用生成调试信息
    QMAKE_LFLAGS_RELEASE = /INCREMENTAL:NO /DEBUG
    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

    QMAKE_CXXFLAGS_RELEASE += -g
    QMAKE_CFLAGS_RELEASE += -g
    #禁止优化
    QMAKE_CFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE -= -O2
    #release在最后link时默认有"-s”参数，表示"忽略输出文件中的所有符号信息"，因此要去掉该参数
    QMAKE_LFLAGS_RELEASE = -mthreads -W

}

#定义编译选项。QT_DEPRECATED_WARNINGS表示当Qt的某些功能被标记为过时的，那么编译器会发出警告。
DEFINES += QT_DEPRECATED_WARNINGS

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    globalInterface_global.h \
    IPlugIn.h \
    ITask.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

SOURCES += \
    IPlugIn.cpp \
    ITask.cpp

