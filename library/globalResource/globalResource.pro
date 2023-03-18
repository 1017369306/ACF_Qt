QT -= gui

TEMPLATE = lib
DEFINES += GLOBALRESOURCE_LIBRARY
#DESTDIR = $$OUT_PWD/../../mainDemo

CONFIG += c++11 resources_big

#复制defaultColors.ini配置文件到指定路径
CONFIG += file_copies
DefaultThemeIni.files = $$PWD/themeFiles/defaultColors.ini
DefaultThemeIni.path = $$OUT_PWD/../../mainDemo/config/themes
COPIES += DefaultThemeIni

#复制darkColors.ini配置文件到指定路径
CONFIG += file_copies
DarkThemeIni.files = $$PWD/themeFiles/darkColors.ini
DarkThemeIni.path = $$OUT_PWD/../../mainDemo/config/themes
COPIES += DarkThemeIni

#自动拷贝文件
CONFIG += file_copies
svgFiles.files = $$PWD/svgs/*
svgFiles.path = $$OUT_PWD/../../mainDemo/appPics/svgs
COPIES += svgFiles

#这种方式如果没有svg目录，它不会新建
#src_file = $$PWD/svg/*
#dst_file = $$DESTDIR/svg
#win32 {
##转换斜杠 / 到 \\
#src_file ~= s,/,\\,g
#dst_file ~= s,/,\\,g
#dst_file2 ~= s,/,\\,g
#QMAKE_POST_LINK += copy $$src_file $$dst_file
#}
#unix {
#QMAKE_POST_LINK += cp -r -f $$src_file $$dst_file
#}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    globalColors.cpp \
    globalSizes.cpp

HEADERS += \
    globalResource_global.h \
    globalColors.h \
    globalSizes.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc

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
