QT += widgets network xml
CONFIG += c++11

TEMPLATE = lib          #表明这个makefile是一个lib的makefile
CONFIG += plugin        #应用程序是一个插件
TARGET = loggerView        #插件名称
#拷贝编译库文件到执行目录
DESTDIR = PWD/../../../mainDemo/plugins/modules/loggerView

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


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../library/globalInterface/release/ -lglobalInterface
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../library/globalInterface/debug/ -lglobalInterface
else:unix: LIBS += -L$$OUT_PWD/../../library/globalInterface/ -lglobalInterface

INCLUDEPATH += $$PWD/../../library/globalInterface
DEPENDPATH += $$PWD/../../library/globalInterface

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../library/mainSystemLib/release/ -lmainSystemLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../library/mainSystemLib/debug/ -lmainSystemLib
else:unix: LIBS += -L$$OUT_PWD/../../library/mainSystemLib/ -lmainSystemLib

INCLUDEPATH += $$PWD/../../library/mainSystemLib
DEPENDPATH += $$PWD/../../library/mainSystemLib

HEADERS += \
    loggerView.h \
    queryHeader.h \
    queryItem.h

SOURCES += \
    loggerView.cpp \
    queryHeader.cpp \
    queryItem.cpp

FORMS += \
    loggerView.ui \
    queryHeader.ui \
    queryItem.ui

DISTFILES += \
    loggerView.json

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../library/globalResource/release/ -lglobalResource
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../library/globalResource/debug/ -lglobalResource
else:unix: LIBS += -L$$OUT_PWD/../../library/globalResource/ -lglobalResource

INCLUDEPATH += $$PWD/../../library/globalResource
DEPENDPATH += $$PWD/../../library/globalResource
