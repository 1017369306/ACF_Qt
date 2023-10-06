QT -= gui
QT += network widgets xml
CONFIG += c++11

TEMPLATE = lib          #表明这个makefile是一个lib的makefile
CONFIG += plugin        #应用程序是一个插件
TARGET = notifyLib        #插件名称

include($$PWD/qtnotify2/qtnotify2.pri)

#拷贝编译库文件到执行目录
DESTDIR = PWD/../../../mainDemo/plugins/notifyLib

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    notify2class.h

SOURCES += \
    notify2class.cpp

DISTFILES += \
    notifyLib.json

RESOURCES += \
    notifyTheme.qrc


unix|win32: LIBS += -L$$PWD/../../lib/framework/globalInterface/lib/ -lglobalInterface

INCLUDEPATH += $$PWD/../../lib/framework/globalInterface/include
DEPENDPATH += $$PWD/../../lib/framework/globalInterface/include

unix|win32: LIBS += -L$$PWD/../../lib/framework/mainSystemLib/lib/ -lmainSystemLib

INCLUDEPATH += $$PWD/../../lib/framework/mainSystemLib/include
DEPENDPATH += $$PWD/../../lib/framework/mainSystemLib/include

unix|win32: LIBS += -L$$PWD/../../lib/framework/globalResource/lib/ -lglobalResource

INCLUDEPATH += $$PWD/../../lib/framework/globalResource/include
DEPENDPATH += $$PWD/../../lib/framework/globalResource/include
