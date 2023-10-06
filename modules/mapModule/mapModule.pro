QT += core gui network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
win32{
    message("enter win32")
    msvc {
        message("enter msvc")
        QT += webenginewidgets
    }
    else {
        message("enter wingw")
        QT += axcontainer
    }
}
#为什么else和else:unix都不行，下面单独unix就可以？
#else:unix
#{
#    message("enter unix")
#    QT += webenginewidgets
#}

unix{
    message("enter unix")
    QT += webenginewidgets
}

CONFIG += c++11
CONFIG -= debug_and_release

TEMPLATE = lib          #表明这个makefile是一个lib的makefile
CONFIG += plugin        #应用程序是一个插件
TARGET = mapModule        #插件名称
#拷贝编译库文件到执行目录
DESTDIR = PWD/../../../mainDemo/plugins/modules/mapModule

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    mapModule.json

FORMS += \
    mainview.ui

HEADERS += \
    mainview.h

SOURCES += \
    mainview.cpp


unix|win32: LIBS += -L$$PWD/../../lib/framework/globalInterface/lib/ -lglobalInterface

INCLUDEPATH += $$PWD/../../lib/framework/globalInterface/include
DEPENDPATH += $$PWD/../../lib/framework/globalInterface/include

unix|win32: LIBS += -L$$PWD/../../lib/framework/mainSystemLib/lib/ -lmainSystemLib

INCLUDEPATH += $$PWD/../../lib/framework/mainSystemLib/include
DEPENDPATH += $$PWD/../../lib/framework/mainSystemLib/include

unix|win32: LIBS += -L$$PWD/../../lib/framework/globalResource/lib/ -lglobalResource

INCLUDEPATH += $$PWD/../../lib/framework/globalResource/include
DEPENDPATH += $$PWD/../../lib/framework/globalResource/include
