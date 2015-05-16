include(../../lisa.pri)

TEMPLATE = app
TARGET = phony_target
CONFIG -= qt sdk separate_debug_info gdb_dwarf_index
QT =
LIBS =
macx:CONFIG -= app_bundle

isEmpty(vcproj) {
    QMAKE_LINK = @: IGNORE THIS LINE
    OBJECTS_DIR =
    win32:CONFIG -= embed_manifest_exe
} else {
    CONFIG += console
    PHONY_DEPS = .
    phony_src.input = PHONY_DEPS
    phony_src.output = phony.c
    phony_src.variable_out = GENERATED_SOURCES
    phony_src.commands = echo int main() { return 0; } > phony.c
    phony_src.name = CREATE phony.c
    phony_src.CONFIG += combine
    QMAKE_EXTRA_COMPILERS += phony_src
}

DATA_DIRS = \
    themes \

for(data_dir, DATA_DIRS) {
    files = $$files($$PWD/$$data_dir/*, true)
    # Info.plist.in are handled below
    for(file, files):!contains(file, ".*/Info\\.plist\\.in$"):!exists($$file/*):FILES += $$file
}

# conditionally deployed data
!isEmpty(copydata) {
    copy2build.input = FILES
    copy2build.output = $$LISA_DATA_PATH/${QMAKE_FUNC_FILE_IN_stripSrcDir}
    isEmpty(vcproj):copy2build.variable_out = PRE_TARGETDEPS
    win32:copy2build.commands = $$QMAKE_COPY \"${QMAKE_FILE_IN}\" \"${QMAKE_FILE_OUT}\"
    unix:copy2build.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    copy2build.name = COPY ${QMAKE_FILE_IN}
    copy2build.CONFIG += no_link
    QMAKE_EXTRA_COMPILERS += copy2build
}

!macx {
    for(data_dir, DATA_DIRS) {
        eval($${data_dir}.files = $$quote($$PWD/$$data_dir))
        eval($${data_dir}.path = $$LISA_PREFIX/share/lisa)
        INSTALLS += $$data_dir
    }
} else {
   # do version magic for app bundles
   dumpinfo.input = qml/qmldump/Info.plist.in
   dumpinfo.output = $$LISA_DATA_PATH/qml/qmldump/Info.plist
   QMAKE_SUBSTITUTES += dumpinfo
   puppetinfo.input = qml/qmlpuppet/qmlpuppet/Info.plist.in
   puppetinfo.output = $$LISA_DATA_PATH/qml/qmlpuppet/qmlpuppet/Info.plist
   QMAKE_SUBSTITUES += puppetinfo
   puppet2info.input = qml/qmlpuppet/qml2puppet/Info.plist.in
   puppet2info.output = $$LISA_DATA_PATH/qml/qmlpuppet/qml2puppet/Info.plist
   QMAKE_SUBSTITUES += puppet2info
}
