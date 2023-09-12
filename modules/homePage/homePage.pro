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

DISTFILES += \
    homePage.json
