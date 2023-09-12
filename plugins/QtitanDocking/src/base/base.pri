QTN_SOLUTION_VERSION = 2022.1.0

QTN_COMPONENT_VERSION = 2.0.0

CONFIG+=opengl_disabled

INCLUDEPATH += $$quote($$PWD)

INCLUDE_HEADERS = \
    $$PWD/../../include/QtitanBase.h \
    $$PWD/../../include/QtitanDef.h \
    $$PWD/../../include/QtnWindowTitleBar.h \
    $$PWD/../../include/QtnMaterialWidget.h \
    $$PWD/../../include/QtnCommonStyle.h \
    $$PWD/../../include/QtnSegoeMDL2Assets.h \
    $$PWD/../../include/QtnWindowsColor.h \
    $$PWD/../../include/QtitanMigration.h \
    $$PWD/../../include/QtnToolTip.h \
#    $$PWD/../../include/QtnCalculatorWidget.h \
    $$PWD/../../include/QtnToggleSwitch.h \
    $$PWD/../../include/QtnColorButton.h \
    $$PWD/../../include/QtnPopupMenu.h

PUBLIC_HEADERS = \
    $$PWD/QtitanDef.h \
    $$PWD/QtnPopupMenu.h \
    $$PWD/QtnWindowTitleBar.h \
    $$PWD/QtnMaterialWidget.h \
    $$PWD/QtnCommonStyle.h \
    $$PWD/QtnSegoeMDL2Assets.h \
    $$PWD/QtnWindowsColor.h \
    $$PWD/QtnToolTip.h \
#    $$PWD/QtnCalculatorWidget.h \
    $$PWD/QtnToggleSwitch.h \
    $$PWD/QtnColorButton.h

PRIVATE_HEADERS = \
    $$PWD/QtitanMSVSDebug.h \
    $$PWD/QtnPlatform.h \
    $$PWD/QtnCustomLayout.h \
    $$PWD/QtnPopupMenuPrivate.h \
    $$PWD/QtnScrollWidgetBar.h \
    $$PWD/QtnBackstageWidget.h \
    $$PWD/QtnWindowTitleBarPrivate.h \
    $$PWD/QtnCommonStylePrivate.h \
    $$PWD/QtnStyleHelperPrivate.h \
#    $$PWD/QtnCalculatorWidgetPrivate.h \
    $$PWD/QtnAbstractTabBar.h \
    $$PWD/QtnStyleOption.h

ALL_SOURCES = \
    $$PWD/QtnPlatform.cpp \
    $$PWD/QtnCustomLayout.cpp \
    $$PWD/QtnPopupMenu.cpp \
    $$PWD/QtnScrollWidgetBar.cpp \
    $$PWD/QtnBackstageWidget.cpp \
    $$PWD/QtnWindowTitleBar.cpp \
    $$PWD/QtnMaterialWidget.cpp \
    $$PWD/QtnSegoeMDL2Assets.cpp \
    $$PWD/QtnWindowsColor.cpp \
    $$PWD/QtnCommonStyle.cpp \
    $$PWD/QtnStyleHelper.cpp \
    $$PWD/QtnStyleOption.cpp \
    $$PWD/QtnToolTip.cpp \
#    $$PWD/QtnCalculatorWidget.cpp \
    $$PWD/QtnToggleSwitch.cpp \
    $$PWD/QtnAbstractTabBar.cpp \
    $$PWD/QtnColorButton.cpp

WINDOWS_HEADERS = \
    $$PWD/QtnPlatform_win.h \
    $$PWD/QtnWindowTitleBar_win.h \
    $$PWD/QtnWindowTitleBarGlow_win.h

WINDOWS_SOURCES = \
    $$PWD/QtnPlatform_win.cpp \
    $$PWD/QtnWindowTitleBar_win.cpp \
    $$PWD/QtnCommonStyle_win.cpp \
    $$PWD/QtnWindowTitleBarGlow_win.cpp

LINUX_HEADERS = 
LINUX_SOURCES = \ 
    $$PWD/QtnPlatform_linux.cpp \
    $$PWD/QtnCommonStyle_linux.cpp

MACOSX_HEADERS = 
MACOSX_SOURCES = \
    $$PWD/QtnPlatform_mac.mm \
    $$PWD/QtnCommonStyle_mac.mm

RESOURCES += \
    $$PWD/QtnBaseResources.qrc

!opengl_disabled {
    PRIVATE_HEADERS += \
        $$PWD/QtnOpenGLEffect.h
    ALL_SOURCES += \
        $$PWD/QtnOpenGLEffect.cpp
    RESOURCES += \
        $$PWD/QtnOpenGLShaders.qrc

    QT += opengl

  #LIBS += -lopengl32
}

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

SOURCES += \
    $$ALL_SOURCES

win32 {
  HEADERS += $$WINDOWS_HEADERS
  SOURCES += $$WINDOWS_SOURCES

  DEFINES += _WINDOWS
  LIBS += -luser32 -lgdi32 -lshell32 -lcomctl32
}

linux* {
  HEADERS += $$LINUX_HEADERS
  SOURCES += $$LINUX_SOURCES
}

macx {
  HEADERS += $$MACOSX_HEADERS
  OBJECTIVE_SOURCES += $$MACOSX_SOURCES
  LIBS += -framework AppKit -framework Carbon
}

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

private_headers {
  QT += core-private widgets-private gui-private
}
DEFINES += QT_NO_DEPRECATED_WARNINGS
