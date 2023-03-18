QT -= gui

QT += network widgets svg xml

TEMPLATE = lib
DEFINES += MAINSYSTEMLIB_LIBRARY

CONFIG += c++11


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    customnativecontrol.h \
    globalInfoEnum.h \
    globalhelper.h \
    loggerBase.h \
    mainSystemLib_global.h \
    moduleBase.h \
    notifybase.h \
    pluginManager.h \
    taskBase.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

SOURCES += \
    customnativecontrol.cpp \
    globalInfoEnum.cpp \
    globalhelper.cpp \
    loggerBase.cpp \
    moduleBase.cpp \
    notifyBase.cpp \
    pluginManager.cpp \
    taskBase.cpp

############ for qBreakpad ############
# qBreakpad中需要使用到network模块
QT += network

# without c++11 & AppKit library compiler can't solve address for symbols
CONFIG += c++11

#在​​debug​​模式下，生成的可执行程序，默认就会带有调试信息。
#但是我们期望的是，在​​release​​​下也能生成，带有调试信息的可执行程序。毕竟交给客户的是​​release​​版，我们大多时候，也只是需要对release版程序进行bug定位。
#所以，需要在​​qBreakpadTest.pro​​​文件中，添加如下内容，让​​release​​版程序带上调试信息：

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../qBreakpad/lib/release/ -lqBreakpad
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../qBreakpad/lib/debug/ -lqBreakpad
else:unix: LIBS += -L$$PWD/../../qBreakpad/lib/ -lqBreakpad

INCLUDEPATH += $$PWD/../../qBreakpad/include
DEPENDPATH += $$PWD/../../qBreakpad/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../qBreakpad/lib/release/libqBreakpad.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../qBreakpad/lib/debug/libqBreakpad.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../qBreakpad/lib/release/qBreakpad.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../qBreakpad/lib/debug/qBreakpad.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../qBreakpad/lib/libqBreakpad.a

############ for qBreakpad ############

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../globalInterface/release/ -lglobalInterface
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../globalInterface/debug/ -lglobalInterface
else:unix: LIBS += -L$$OUT_PWD/../globalInterface/ -lglobalInterface

INCLUDEPATH += $$PWD/../globalInterface
DEPENDPATH += $$PWD/../globalInterface

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../globalResource/release/ -lglobalResource
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../globalResource/debug/ -lglobalResource
else:unix: LIBS += -L$$OUT_PWD/../globalResource/ -lglobalResource

INCLUDEPATH += $$PWD/../globalResource
DEPENDPATH += $$PWD/../globalResource
