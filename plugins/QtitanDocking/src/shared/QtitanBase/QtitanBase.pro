TEMPLATE = lib
python:TARGET = Python.QtitanBase
else:TARGET = QtitanBase
python:CONFIG += remove_rpath

include($$PWD/../platform.pri)
include($$PWD/../../base/base.pri)
contains(CONFIG, demo) {
    include($$PWD/../qtitandemo.pri)
}

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
  message("enter QTITAN_LIBRARY_STATIC")
  TARGET = $$member(TARGET, 0)_static
  DEFINES += QTITAN_LIBRARY_STATIC
} else {
  message("enter QTITAN_BASE_LIBRARY")
  DEFINES += QTITAN_BASE_LIBRARY
}
DEFINES += QT_NO_CAST_FROM_ASCII

DESTDIR = $$QTN_OUTPUT_DIR
lessThan(QT_MAJOR_VERSION, 5) {
win32:DLLDESTDIR = $$QTN_OUTPUT_DIR
}

QMAKE_TARGET_COMPANY = Developer Machines
QMAKE_TARGET_PRODUCT = QtitanBase
QMAKE_TARGET_DESCRIPTION = QtitanBase Utilities classes for Qtitan Components
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2009-2021 Developer Machines

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets
}



