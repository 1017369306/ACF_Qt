QT -= gui

TEMPLATE = lib
DEFINES += GLOBALRESOURCE_LIBRARY

CONFIG += c++11 resources_big
CONFIG -= debug_and_release

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

#拷贝编译库文件到代码仓库中的相对目录
DESTDIR += $$PWD/../../lib/framework/globalResource/lib
lessThan(QT_MAJOR_VERSION, 5) {
    win32:DLLDESTDIR += $$PWD/../../lib/framework/globalResource/lib
}
#拷贝头文件到代码仓库中的相对目录
CONFIG += file_copies
header.files = $$HEADERS
header.path = $$PWD/../../lib/framework/globalResource/include
COPIES += header
