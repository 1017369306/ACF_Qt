QT -= gui
QT += network widgets
CONFIG += c++11
CONFIG -= debug_and_release

TEMPLATE = lib          #表明这个makefile是一个lib的makefile
CONFIG += plugin        #应用程序是一个插件
TARGET = logLib        #插件名称

include(PWD/../log4qt/log4qt.pri)

#拷贝编译库文件到执行目录
DESTDIR = PWD/../../../mainDemo/plugins/logLib

#复制darkColors.ini配置文件到指定路径
CONFIG += file_copies
log4qtConf.files = $$PWD/log4qt.conf
log4qtConf.path = $$OUT_PWD/../../mainDemo/config
COPIES += log4qtConf

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../library/globalInterface/ -lglobalInterface
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../library/globalInterface/ -lglobalInterface
else:unix: LIBS += -L$$OUT_PWD/../../library/globalInterface/ -lglobalInterface

INCLUDEPATH += $$PWD/../../library/globalInterface
DEPENDPATH += $$PWD/../../library/globalInterface

HEADERS += \
    logger4Qt.h

SOURCES += \
    logger4Qt.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../library/mainSystemLib/ -lmainSystemLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../library/mainSystemLib/ -lmainSystemLib
else:unix: LIBS += -L$$OUT_PWD/../../library/mainSystemLib/ -lmainSystemLib

INCLUDEPATH += $$PWD/../../library/mainSystemLib
DEPENDPATH += $$PWD/../../library/mainSystemLib

DISTFILES += \
    logLib.json
