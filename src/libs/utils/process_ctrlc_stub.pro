!win32: error("process_ctrlc_stub is Windows only")

CONFIG    -= qt
CONFIG    += console warn_on

include(../../../lisa.pri)

TEMPLATE  = app
TARGET    = lisa_ctrlc_stub
DESTDIR   = $$LISA_LIBEXEC_PATH

SOURCES   += process_ctrlc_stub.cpp
LIBS      += -luser32 -lshell32

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

target.path  = $$LISA_PREFIX/bin # FIXME: libexec, more or less
INSTALLS    += target
