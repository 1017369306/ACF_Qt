QT += widgets network
CONFIG += c++11
CONFIG -= debug_and_release

TEMPLATE = lib          #表明这个makefile是一个lib的makefile
CONFIG += plugin        #应用程序是一个插件
TARGET = homePage        #插件名称
#拷贝编译库文件到执行目录
DESTDIR = PWD/../../../mainDemo/plugins/modules/homePage

#拷贝编译库文件到执行目录
#DESTDIR = PWD/../../../WXDH_Client/modules/level

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

FORMS += \
    homepage.ui

HEADERS += \
    homepage.h

SOURCES += \
    homepage.cpp

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    homePage.json

unix|win32: LIBS += -L$$PWD/../../lib/framework/globalInterface/lib/ -lglobalInterface

INCLUDEPATH += $$PWD/../../lib/framework/globalInterface/include
DEPENDPATH += $$PWD/../../lib/framework/globalInterface/include

unix|win32: LIBS += -L$$PWD/../../lib/framework/mainSystemLib/lib/ -lmainSystemLib

INCLUDEPATH += $$PWD/../../lib/framework/mainSystemLib/include
DEPENDPATH += $$PWD/../../lib/framework/mainSystemLib/include
