TEMPLATE = app
TARGET = lisa.sh
OBJECTS_DIR =

PRE_TARGETDEPS = $$PWD/lisa.sh

QMAKE_LINK = cp $$PWD/lisa.sh $@ && : IGNORE REST OF LINE:
QMAKE_STRIP =
CONFIG -= qt separate_debug_info gdb_dwarf_index

QMAKE_CLEAN = lisa.sh

target.path  = $$LISA_PREFIX/bin
INSTALLS    += target

DISTFILES = $$PWD/lisa.sh
