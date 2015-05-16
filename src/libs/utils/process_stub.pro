CONFIG          += warn_on console use_c_linker
CONFIG          -= qt app_bundle

include(../../../lisa.pri)

TEMPLATE        = app
TARGET          = lisa_process_stub
DESTDIR         = $$LISA_LIBEXEC_PATH

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

unix {
    SOURCES += process_stub_unix.c
    solaris-.*: LIBS += -lsocket
} else {
    SOURCES += process_stub_win.c
    LIBS += -lshell32
}

target.path  = $$LISA_PREFIX/bin # FIXME: libexec, more or less
INSTALLS    += target
