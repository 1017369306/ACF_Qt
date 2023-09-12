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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../library/globalInterface/ -lglobalInterface
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../library/globalInterface/ -lglobalInterface
else:unix: LIBS += -L$$OUT_PWD/../../library/globalInterface/ -lglobalInterface

INCLUDEPATH += $$PWD/../../library/globalInterface
DEPENDPATH += $$PWD/../../library/globalInterface

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../library/mainSystemLib/ -lmainSystemLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../library/mainSystemLib/ -lmainSystemLib
else:unix: LIBS += -L$$OUT_PWD/../../library/mainSystemLib/ -lmainSystemLib

INCLUDEPATH += $$PWD/../../library/mainSystemLib
DEPENDPATH += $$PWD/../../library/mainSystemLib

HEADERS += \
    notify2class.h

SOURCES += \
    notify2class.cpp

DISTFILES += \
    notifyLib.json

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../library/globalResource/ -lglobalResource
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../library/globalResource/ -lglobalResource
else:unix: LIBS += -L$$OUT_PWD/../../library/globalResource/ -lglobalResource

INCLUDEPATH += $$PWD/../../library/globalResource
DEPENDPATH += $$PWD/../../library/globalResource

RESOURCES += \
    notifyTheme.qrc
