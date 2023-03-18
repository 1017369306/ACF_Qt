TEMPLATE = subdirs
CONFIG += ordered \
    qt \
    qt
SUBDIRS += \
    library \
    plugins \
    modules\
    mainDemo

QT += widgets
