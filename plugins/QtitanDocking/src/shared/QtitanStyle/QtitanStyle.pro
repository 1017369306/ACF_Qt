TEMPLATE = lib
python:TARGET = Python.QtitanStyle
else:TARGET = QtitanStyle
python:CONFIG += remove_rpath

include($$PWD/../platform.pri)
include($$PWD/../../styles/styles.pri)
contains(CONFIG, demo) {
    include($$PWD/../qtitandemo.pri)
}

QTN_COMPONENT_VERSION = $$fromfile($$PWD/../../base/base.pri, QTN_COMPONENT_VERSION)
VERSION = $$QTN_COMPONENT_VERSION
QTN_COMPONENT_VERSION_LIST = $$split(QTN_COMPONENT_VERSION, ".")

qtitan_staticlib:CONFIG+=staticlib
staticlib:CONFIG+=qtitan_staticlib

win32 {
    lessThan(QT_MAJOR_VERSION, 5) {
        VERSION=
    } else {
        CONFIG+=skip_target_version_ext
    }
}

if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
    TARGET = $$member(TARGET, 0)d
} else {
}

win32|qtitan_staticlib {
    TARGET = $$member(TARGET, 0)$$member(QTN_COMPONENT_VERSION_LIST, 0)
}

qtitan_staticlib {
  TARGET = $$member(TARGET, 0)_static
  DEFINES += QTITAN_LIBRARY_STATIC
} else {
  DEFINES += QTITAN_LIBRARY
}
DEFINES += QT_NO_CAST_FROM_ASCII

DESTDIR = $$QTN_OUTPUT_DIR
lessThan(QT_MAJOR_VERSION, 5) {
win32:DLLDESTDIR = $$QTN_OUTPUT_DIR
}

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets
}

QMAKE_TARGET_COMPANY = Developer Machines
QMAKE_TARGET_PRODUCT = QtitanStyle
QMAKE_TARGET_DESCRIPTION = QtitanStyle set of styles for Qt.C++
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2009-2021 Developer Machines

include($$PWD/../qtitanbase.pri)
