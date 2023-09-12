QT -= gui

QT += network widgets svg xml axcontainer

TEMPLATE = lib
DEFINES += MAINSYSTEMLIB_LIBRARY

CONFIG += c++11
CONFIG -= debug_and_release

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    acfproperty.h \
    customnativecontrol.h \
    defaultplugin.h \
    frameworktool.h \
    globalEnums.h \
    globalInfoEnum.h \
    globalhelper.h \
#    loggerBase.h \
    mainSystemLib_global.h \
    moduleBase.h \
    pluginManager.h \
    systeminfohelper.h \
    taskBase.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

SOURCES += \
    acfproperty.cpp \
    customnativecontrol.cpp \
    frameworktool.cpp \
    globalInfoEnum.cpp \
    globalhelper.cpp \
#    loggerBase.cpp \
    moduleBase.cpp \
    pluginManager.cpp \
    systeminfohelper.cpp \
    taskBase.cpp

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../globalInterface/ -lglobalInterface
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../globalInterface/ -lglobalInterface
#else:unix: LIBS += -L$$OUT_PWD/../globalInterface/ -lglobalInterface

#INCLUDEPATH += $$PWD/../globalInterface
#DEPENDPATH += $$PWD/../globalInterface

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../globalResource/ -lglobalResource
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../globalResource/ -lglobalResource
else:unix: LIBS += -L$$OUT_PWD/../globalResource/ -lglobalResource

INCLUDEPATH += $$PWD/../globalResource
DEPENDPATH += $$PWD/../globalResource


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../globalInterface/ -lglobalInterface
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../globalInterface/ -lglobalInterface
else:unix: LIBS += -L$$OUT_PWD/../globalInterface/ -lglobalInterface

INCLUDEPATH += $$PWD/../globalInterface
DEPENDPATH += $$PWD/../globalInterface
