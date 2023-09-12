QT       += core gui network widgets concurrent xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG -= debug_and_release

#自动拷贝文件
CONFIG += file_copies

# 提供控制台显示功能（可在release模式运行时查看控制台输出）
#CONFIG += console

include($$PWD/../plugins/QtitanDocking/src/shared/qtitandocking.pri)

#拷贝文件或库
message("starting copy icons")
#files中，最后加/，表示copy此目录中的所有文件，无/表示仅创建空目录；文件或最后一级目录copy到path目录中
iconFiles.files = $$PWD/appPics/icons/
#iconFiles.files += $$PWD/../library/globalResource/svgs/
iconFiles.path = $$OUT_PWD/appPics
COPIES += iconFiles
message("icons copy completed")

message("starting copy svgs")
svgFiles.files = $$PWD/../library/globalResource/svgs/theme/
svgFiles.path = $$OUT_PWD/appPics/svgs
COPIES += svgFiles
message("svgs copy completed")


#要拷贝QtitanDocking的动态库到输出目录，不然调试要异常
message("starting copy QtitanDocking dll")
#QtitanDocking_copy.files += $$PWD/../plugins/QtitanDocking/bin/
QtitanDocking_copy.files += $$PWD/../plugins/QtitanDocking/bin/QtitanBased2.dll
QtitanDocking_copy.files += $$PWD/../plugins/QtitanDocking/bin/QtitanDockingd4.dll
QtitanDocking_copy.files += $$PWD/../plugins/QtitanDocking/bin/QtitanStyled2.dll
QtitanDocking_copy.path = $$OUT_PWD
COPIES += QtitanDocking_copy
message("QtitanDocking dll copy completed")


#CONFIG += file_copies
#svgFiles.files = $$PWD/appPics/svgs/*
#svgFiles.path = $$OUT_PWD/../mainDemo/appPics/svgs
#COPIES += svgFiles

############ 异常捕获、分析和定位支持 ############

#启用多线程、异常、RTTI、STL支持
CONFIG += thread exceptions rtti stl
macx: LIBS += -framework AppKit

msvc {
    #解决msvc乱码
    QMAKE_CXXFLAGS += -execution-charset:utf-8
    QMAKE_CXXFLAGS += -source-charset:utf-8
    QMAKE_CXXFLAGS_WARN_ON += -wd4819

    #生成*.pdb文件
    CONFIG += force_debug_info
    CONFIG += separate_debug_info

}
else {
    #在release下可以生成调试信息

    #禁用优化 ，启用生成调试信息
    QMAKE_LFLAGS_RELEASE = /INCREMENTAL:NO /DEBUG
    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

    QMAKE_CXXFLAGS_RELEASE += -g
    QMAKE_CFLAGS_RELEASE += -g
    #禁止优化
    QMAKE_CFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE -= -O2
    #release在最后link时默认有"-s”参数，表示"忽略输出文件中的所有符号信息"，因此要去掉该参数
    QMAKE_LFLAGS_RELEASE = -mthreads -W

}

INCLUDEPATH += $$PWD/../lib/breakpadPlus/include \
               $$PWD/../lib/hookPlus/include
DEPENDPATH += $$PWD/../lib/breakpadPlus/include \
              $$PWD/../lib/hookPlus/include

#INCLUDEPATH += $$PWD/../lib/hookPlus/include
#DEPENDPATH += $$PWD/../lib/hookPlus/include

#windows系统
win32{
    # 需要引用的第三方库（windows的系统环境中自带的）
    LIBS += -ldbghelp -limagehlp

    msvc {
        message("enter msvc")

        LIBS += -L$$PWD/../lib/breakpadPlus/lib/x86_64/MSVC/ -lbreakpadPlus

        breakpadPlus_copy.files = $$PWD/../lib/breakpadPlus/lib/x86_64/MSVC/breakpadPlus.dll
        breakpadPlus_copy.path = $$OUT_PWD
        COPIES += breakpadPlus_copy

        LIBS += -L$$PWD/../lib/hookPlus/lib/x86_64/MSVC/ -lhookPlus
        hookPlus_copy.files = $$PWD/../lib/hookPlus/lib/x86_64/MSVC/hookPlus.dll
        hookPlus_copy.path = $$OUT_PWD
        COPIES += hookPlus_copy

#        LIBS += -L$$PWD/../lib/QtitanDocking/lib/x86_64/MSVC/ -lQtitanBase2
#        LIBS += -L$$PWD/../lib/QtitanDocking/lib/x86_64/MSVC/ -lQtitanDocking4
#        LIBS += -L$$PWD/../lib/QtitanDocking/lib/x86_64/MSVC/ -lQtitanStyle2

#        message("start copy QtitanDocking")

#        QtitanDocking_copy.files += $$PWD/../lib/QtitanDocking/lib/x86_64/MSVC/QtitanBase2.dll
#        QtitanDocking_copy.files += $$PWD/../lib/QtitanDocking/lib/x86_64/MSVC/QtitanDocking4.dll
#        QtitanDocking_copy.files += $$PWD/../lib/QtitanDocking/lib/x86_64/MSVC/QtitanStyle2.dll
#        QtitanDocking_copy.path = $$OUT_PWD
#        COPIES += QtitanDocking_copy

        message("dll load completed for msvc")
    }
    else {
        message("enter wingw")

        LIBS += -L$$PWD/../lib/breakpadPlus/lib/x86_64/wingw/ -lbreakpadPlus

        breakpadPlus_copy.files = $$PWD/../lib/breakpadPlus/lib/x86_64/wingw/breakpadPlus.dll
        breakpadPlus_copy.path = $$OUT_PWD
        COPIES += breakpadPlus_copy

        #复制addr2line.exe到编译输出路径
        addr2line_copy.files = $$PWD/../lib/breakpadPlus/lib/x86_64/wingw/addr2line.exe
        addr2line_copy.path = $$OUT_PWD
        COPIES += addr2line_copy

#        LIBS += -L$$PWD/../lib/hookPlus/lib/x86_64/wingw/ -lhookPlus
#        hookPlus_copy.files = $$PWD/../lib/hookPlus/lib/x86_64/wingw/hookPlus.dll
#        hookPlus_copy.path = $$OUT_PWD
#        COPIES += hookPlus_copy
    }

}
#linux系统
unix{
    message("enter unix")
    contains(QT_ARCH, arm64){
        #在这里处理arm64所需
        LIBS += -L$$PWD/../lib/breakpadPlus/lib/arrch64/ -lbreakpadPlus
        breakpadPlus_copy.files = $$PWD/../lib/breakpadPlus/lib/arrch64/*
        breakpadPlus_copy.path = $$OUT_PWD
        COPIES += breakpadPlus_copy

        LIBS += -L$$PWD/../lib/hookPlus/lib/arrch64/ -lhookPlus
        hookPlus_copy.files = $$PWD/../lib/hookPlus/lib/arrch64/*
        hookPlus_copy.path = $$OUT_PWD
        COPIES += hookPlus_copy
    }
    contains(QT_ARCH, x86_64){
        LIBS += -L$$PWD/../lib/breakpadPlus/lib/x86_64/gun/ -lbreakpadPlus
        breakpadPlus_copy.files = $$PWD/../lib/breakpadPlus/lib/x86_64/gun/*
        breakpadPlus_copy.path = $$OUT_PWD
        COPIES += breakpadPlus_copy

        #在这里处理除了arm64的（默认x86_64）
        addr2line_copy.files = $$PWD/../lib/breakpadPlus/lib/x86_64/gun/openCoreDumpInLinux.sh
        addr2line_copy.path = $$OUT_PWD
        COPIES += addr2line_copy

        LIBS += -L$$PWD/../lib/hookPlus/lib/x86_64/gun/ -lbreakpadPlus
        hookPlus_copy.files = $$PWD/../lib/hookPlus/lib/x86_64/gun/*
        hookPlus_copy.path = $$OUT_PWD
        COPIES += hookPlus_copy
    }
}

############ 异常捕获、分析和定位支持 ############

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutDialog.cpp \
    customdockwindowbase.cpp \
    customlistitem.cpp \
    main.cpp \
    maindockwindow.cpp \
    mainviewpanels.cpp \
    mainwindow.cpp \
    popuplistwidget.cpp

HEADERS += \
#    $$PWD/../lib/hookPlus/include/hookPlus_global.h \
#    $$PWD/../lib/hookPlus/include/IHook.h \
#    $$PWD/../lib/hookPlus/include/winHook.h \
#    $$PWD/../lib/hookPlus/include/linuxHook.h \
    aboutDialog.h \
    customdockwindowbase.h \
    customlistitem.h \
    maindockwindow.h \
    mainviewpanels.h \
    mainwindow.h \
    popuplistwidget.h

FORMS += \
    aboutDialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../library/globalResource/ -lglobalResource
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../library/globalResource/ -lglobalResource
else:unix: LIBS += -L$$OUT_PWD/../library/globalResource/ -lglobalResource

INCLUDEPATH += $$PWD/../library/globalResource
DEPENDPATH += $$PWD/../library/globalResource

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../library/mainSystemLib/ -lmainSystemLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../library/mainSystemLib/ -lmainSystemLib
else:unix: LIBS += -L$$OUT_PWD/../library/mainSystemLib/ -lmainSystemLib

INCLUDEPATH += $$PWD/../library/mainSystemLib
DEPENDPATH += $$PWD/../library/mainSystemLib

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../library/globalInterface/ -lglobalInterface
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../library/globalInterface/ -lglobalInterface
else:unix: LIBS += -L$$OUT_PWD/../library/globalInterface/ -lglobalInterface

INCLUDEPATH += $$PWD/../library/globalInterface
DEPENDPATH += $$PWD/../library/globalInterface

RESOURCES += \
    customTheme.qrc

