
include($$PWD/qtitanbase.pri)

python:QTN_LIB_NAME=Python.QtitanStyle
else:QTN_LIB_NAME=QtitanStyle

QTITAN_LIB_PATH = $$QTN_OUTPUT_DIR

QTN_COMPONENT_VERSION = $$fromfile($$PWD/../base/base.pri, QTN_COMPONENT_VERSION)
QTN_COMPONENT_VERSION_LIST = $$split(QTN_COMPONENT_VERSION, ".")
DEFINES += QTN_VERSION_STYLE=$$QTN_COMPONENT_VERSION

if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
    QTN_LIB_NAME = $$QTN_LIB_NAME"d"
} else {
}

win32|qtitan_staticlib {
    QTN_LIB_NAME = $$QTN_LIB_NAME$$member(QTN_COMPONENT_VERSION_LIST, 0)
}

qtitan_staticlib {
    QTN_LIB_NAME = $$QTN_LIB_NAME"_static"
    DEFINES += QTITAN_LIBRARY_STATIC
    macx:LIBS += -framework AppKit
    win32:LIBS += -luser32 -lgdi32 -ladvapi32 -lshlwapi
}

unix|win32-g++ {
    android:QTN_LIB_NAME = "$$QTN_LIB_NAME"_$$ANDROID_TARGET_ARCH
    LIBS += -L$$QTITAN_LIB_PATH
    LIBS += -l$$QTN_LIB_NAME
} else {
    LIBS += $$QTITAN_LIB_PATH/$$QTN_LIB_NAME".lib"
}

greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
