QT       += core gui network widgets concurrent xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

#自动拷贝文件
CONFIG += file_copies
iconFiles.files = $$PWD/appPics/icons/*
iconFiles.path = $$OUT_PWD/../mainDemo/appPics/icons
COPIES += iconFiles

#CONFIG += file_copies
#svgFiles.files = $$PWD/appPics/svgs/*
#svgFiles.path = $$OUT_PWD/../mainDemo/appPics/svgs
#COPIES += svgFiles

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
    customlistitem.cpp \
    main.cpp \
    mainwindow.cpp \
    popuplistwidget.cpp \
    widgettemp.cpp

HEADERS += \
    aboutDialog.h \
    customlistitem.h \
    mainwindow.h \
    popuplistwidget.h \
    widgettemp.h

FORMS += \
    aboutDialog.ui \
    mainwindow.ui \
    widgettemp.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../library/globalResource/release/ -lglobalResource
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../library/globalResource/debug/ -lglobalResource
else:unix: LIBS += -L$$OUT_PWD/../library/globalResource/ -lglobalResource

INCLUDEPATH += $$PWD/../library/globalResource
DEPENDPATH += $$PWD/../library/globalResource

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../library/mainSystemLib/release/ -lmainSystemLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../library/mainSystemLib/debug/ -lmainSystemLib
else:unix: LIBS += -L$$OUT_PWD/../library/mainSystemLib/ -lmainSystemLib

INCLUDEPATH += $$PWD/../library/mainSystemLib
DEPENDPATH += $$PWD/../library/mainSystemLib

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../library/globalInterface/release/ -lglobalInterface
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../library/globalInterface/debug/ -lglobalInterface
else:unix: LIBS += -L$$OUT_PWD/../library/globalInterface/ -lglobalInterface

INCLUDEPATH += $$PWD/../library/globalInterface
DEPENDPATH += $$PWD/../library/globalInterface

RESOURCES += \
    customTheme.qrc

