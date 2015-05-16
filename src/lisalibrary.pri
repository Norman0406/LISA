include($$replace(_PRO_FILE_PWD_, ([^/]+$), \\1/\\1_dependencies.pri))
TARGET = $$LISA_LIB_NAME

include(../lisa.pri)

# use precompiled header for libraries by default
isEmpty(PRECOMPILED_HEADER):PRECOMPILED_HEADER = $$PWD/shared/lisa_pch.h

win32 {
    DLLDESTDIR = $$LISA_APP_PATH
}

DESTDIR = $$LISA_LIBRARY_PATH

osx: QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/Frameworks/
include(rpath.pri)

TARGET = $$qtLibraryName($$TARGET)

TEMPLATE = lib
CONFIG += shared dll

contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols

!macx {
    win32 {
        dlltarget.path = $$LISA_PREFIX/bin
        INSTALLS += dlltarget
    } else {
        target.path = $$LISA_PREFIX/$$LISA_LIBRARY_BASENAME/lisa
        INSTALLS += target
    }
}
