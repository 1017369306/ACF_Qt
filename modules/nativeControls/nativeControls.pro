QT += widgets network xml
CONFIG += c++11
CONFIG -= debug_and_release

TEMPLATE = lib          #表明这个makefile是一个lib的makefile
CONFIG += plugin        #应用程序是一个插件
TARGET = nativeControls        #插件名称
#拷贝编译库文件到执行目录
DESTDIR = PWD/../../../mainDemo/plugins/modules/nativeControls

#拷贝编译库文件到执行目录
#DESTDIR = PWD/../../../WXDH_Client/modules/level

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    nativeControls.json

FORMS += \
    nativecontrolsview.ui

HEADERS += \
    nativecontrolsview.h

SOURCES += \
    nativecontrolsview.cpp


unix|win32: LIBS += -L$$PWD/../../lib/framework/globalInterface/lib/ -lglobalInterface

INCLUDEPATH += $$PWD/../../lib/framework/globalInterface/include
DEPENDPATH += $$PWD/../../lib/framework/globalInterface/include

unix|win32: LIBS += -L$$PWD/../../lib/framework/mainSystemLib/lib/ -lmainSystemLib

INCLUDEPATH += $$PWD/../../lib/framework/mainSystemLib/include
DEPENDPATH += $$PWD/../../lib/framework/mainSystemLib/include

unix|win32: LIBS += -L$$PWD/../../lib/framework/globalResource/lib/ -lglobalResource

INCLUDEPATH += $$PWD/../../lib/framework/globalResource/include
DEPENDPATH += $$PWD/../../lib/framework/globalResource/include
