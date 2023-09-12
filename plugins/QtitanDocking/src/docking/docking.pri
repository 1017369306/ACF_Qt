QTN_COMPONENT_VERSION = 4.0.0
DEFINES += QTN_VERSION_DOCKING=$$QTN_COMPONENT_VERSION

INCLUDEPATH += $$quote($$PWD/../../include)
INCLUDEPATH += $$quote($$PWD/../base)

ALL_SOURCES = \
    $$PWD/QtnDockDef.cpp \
    $$PWD/QtnDockMainWindow.cpp \
    $$PWD/QtnDockBarBase.cpp \
    $$PWD/QtnDockToolBar.cpp \
    $$PWD/QtnDockToolBarLayoutPrivate.cpp \
    $$PWD/QtnDockToolBarExtensionPrivate.cpp \
    $$PWD/QtnDockMainWindowLayout.cpp \
    $$PWD/QtnDockBarSite.cpp \
    $$PWD/QtnDockBarManager.cpp \
    $$PWD/QtnDockPanelManager.cpp \
    $$PWD/QtnDockLayoutPanel.cpp \
    $$PWD/QtnDockWidgetDragDrop.cpp \
    $$PWD/QtnDockPanelBase.cpp \
    $$PWD/QtnDockPanelSplitter.cpp \
    $$PWD/QtnDockWidgetPanel.cpp \
    $$PWD/QtnDockDocumentPanel.cpp \
    $$PWD/QtnDockWindow.cpp \
    $$PWD/QtnDockTitleBar.cpp

INCLUDE_HEADERS = \
    $$PWD/../../include/QtitanDef.h \
    $$PWD/../../include/QtnDockDef.h \
    $$PWD/../../include/QtitanDocking.h \
    $$PWD/../../include/QtnDockWindow.h \
    $$PWD/../../include/QtnDockMainWindow.h \
    $$PWD/../../include/QtnDockPanelManager.h \
    $$PWD/../../include/QtnDockBarManager.h \
    $$PWD/../../include/QtnDockWidgetPanel.h \
    $$PWD/../../include/QtnDockDocumentPanel.h \
    $$PWD/../../include/QtnDockLayoutPanel.h \
    $$PWD/../../include/QtnDockBarBase.h \
    $$PWD/../../include/QtnDockToolBar.h

PRIVATE_HEADERS = \
    $$PWD/QtnDockToolBarPrivate.h \
    $$PWD/QtnDockLayoutPrivate.h \
    $$PWD/QtnDockBarSitePrivate.h \
    $$PWD/QtnDockBarBasePrivate.h \
    $$PWD/QtnDockToolBarLayoutPrivate.h \
    $$PWD/QtnDockToolBarExtensionPrivate.h \
    $$PWD/QtnDockMainWindowLayout.h \
    $$PWD/QtnDockMainWindowPrivate.h \
    $$PWD/QtnDockWidgetPanelPrivate.h \
    $$PWD/QtnDockDocumentPanelPrivate.h \
    $$PWD/QtnDockBarManagerPrivate.h \
    $$PWD/QtnDockPanelManagerPrivate.h \
    $$PWD/QtnDockLayoutPanelPrivate.h \
    $$PWD/QtnDockWidgetDragDrop.h \
    $$PWD/QtnDockPanelSplitter.h \
    $$PWD/QtnDockWindowPrivate.h \
    $$PWD/QtnDockTitleBarPrivate.h \
    $$PWD/QtnDockTitleBar.h

PUBLIC_HEADERS = \
    $$PWD/QtnDockPanelBase.h \
    $$PWD/QtnDockBarBase.h \
    $$PWD/QtnDockWidgetPanel.h \
    $$PWD/QtnDockDocumentPanel.h \
    $$PWD/QtnDockLayoutPanel.h \
    $$PWD/QtnDockPanelManager.h \
    $$PWD/QtnDockBarManager.h \
    $$PWD/QtnDockMainWindow.h \
    $$PWD/QtnDockWindow.h \
    $$PWD/QtnDockBarSite.h \
    $$PWD/QtnDockToolBar.h \
    $$PWD/QtnDockDef.h

WINDOWS_HEADERS =
WINDOWS_SOURCES =
LINUX_HEADERS =
LINUX_SOURCES =
MACOSX_HEADERS =
MACOSX_SOURCES =


HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

SOURCES += \
    $$ALL_SOURCES

win32 {
  HEADERS += $$WINDOWS_HEADERS
  SOURCES += $$WINDOWS_SOURCES
}

linux* {
  HEADERS += $$LINUX_HEADERS
  SOURCES += $$LINUX_SOURCES
}

macx {
  HEADERS += $$MACOSX_HEADERS
  OBJECTIVE_SOURCES += $$MACOSX_SOURCES
}

RESOURCES += \
    $$PWD/QtnDockResources.qrc

win32 {
    DEFINES += _WINDOWS
    LIBS += -luser32 -lgdi32 -lshell32
}
DEFINES += QT_NO_DEPRECATED_WARNINGS
