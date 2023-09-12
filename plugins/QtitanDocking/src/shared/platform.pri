QTITAN_ROOT = $$PWD/../..
INCLUDEPATH += $$QTITAN_ROOT/include

QT_VERSION = $$[QT_VERSION]
QT_VERSION = $$split(QT_VERSION, ".")
QT_VER_MAJ = $$member(QT_VERSION, 0)
QT_VER_MIN = $$member(QT_VERSION, 1)

QTN_OPENGL_DISABLED=1

!qtitan_no_private {
  greaterThan(QT_MAJOR_VERSION, 4):CONFIG+=private_headers
}

private_headers {
  DEFINES += QTN_PRIVATE_HEADERS
}

equals(QTN_OPENGL_DISABLED, 1) {
  CONFIG+=opengl_disabled
}

!opengl_disabled {
  DEFINES += QTN_OPENGL_ENABLED
}

defineReplace(toNativeSeparators) {
    return($$replace(1, /, $$QMAKE_DIR_SEP))
}
defineReplace(fromNativeSeparators) {
    return($$replace(1, \\\\, /))
} 

defineReplace(cleanPath) {
    win32:1 ~= s|\\\\|/|g
    contains(1, ^/.*):pfx = /
    else:pfx =
    segs = $$split(1, /)
    out =
    for(seg, segs) {
        equals(seg, ..):out = $$member(out, 0, -2)
        else:!equals(seg, .):out += $$seg
    }
    return($$join(out, /, $$pfx))
} 

RCC = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/rcc))
LRELEASE = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/lrelease))
LUPDATE = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/lupdate))
LCONVERT = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/lconvert))
QMAKE_BINARY = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/qmake))

win32 {
    RCC = $${RCC}.exe
    LRELEASE = $${LRELEASE}.exe
    LUPDATE = $${LUPDATE}.exe
    LCONVERT = $${LCONVERT}.exe
    QMAKE_BINARY = $${QMAKE_BINARY}.exe
} 

win32 {
    DEFINES += _WINDOWS
}

!no_mp {
    unix|win32-g++ {
        #QMAKE_CXXFLAGS += -j8
    } else {
        QMAKE_CXXFLAGS += -MP
    }
}

DEFINES += QTN_NAMESPACE

unix|win32-g++ {
    #QMAKE_CXXFLAGS += -Wsuggest-override
    QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override
}

python {
    include($$PWD/python.pri)
    python_debug:QTN_OUTPUT_DIR=$$PYSIDE_DIR/../DevMachines
    else:QTN_OUTPUT_DIR=$$PWD/../../site-packages/DevMachines
} else {
    QTN_OUTPUT_DIR=$$PWD/../../bin$$QTITAN_LIB_PREFIX
}

remove_rpath {
    linux {
        #QMAKE_RPATHDIR = $ORIGIN
        QMAKE_LFLAGS_RPATH=
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
    }
    macx {
        QMAKE_LFLAGS_RPATH=
        QMAKE_LFLAGS += "-Wl,-rpath,@loader_path"
        QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
    }
}

designer_rpath {
    linux {
        #QMAKE_RPATHDIR = $ORIGIN../../lib
        QMAKE_LFLAGS_RPATH=
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../../lib\'"
    }
    macx {
        QMAKE_LFLAGS_RPATH=
        QMAKE_LFLAGS += "-Wl,-rpath,@executable_path/Frameworks"
        QMAKE_LFLAGS += "-Wl,-rpath,@loader_path/../../lib"
    }
}
