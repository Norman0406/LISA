include(../../../lisa.pri)

LANGUAGES = de

# var, prepend, append
defineReplace(prependAll) {
    for(a,$$1):result += $$2$${a}$$3
    return($$result)
}

XMLPATTERNS = $$shell_path($$[QT_INSTALL_BINS]/xmlpatterns)
LUPDATE = $$shell_path($$[QT_INSTALL_BINS]/lupdate) -locations relative -no-ui-lines -no-sort
LRELEASE = $$shell_path($$[QT_INSTALL_BINS]/lrelease)
LCONVERT = $$shell_path($$[QT_INSTALL_BINS]/lconvert)

wd = $$replace(LISA_SOURCE_TREE, /, $$QMAKE_DIR_SEP)

TRANSLATIONS = $$prependAll(LANGUAGES, $$PWD/lisa_,.ts)

plugin_sources = $$files($$LISA_SOURCE_TREE/src/plugins/*)
plugin_sources ~= s,^$$re_escape($$LISA_SOURCE_TREE/),,g$$i_flag
plugin_sources -= src/plugins/plugins.pro \
    src/plugins/helloworld
shared_sources = $$files($$LISA_SOURCE_TREE/src/shared/*)
shared_sources ~= s,^$$re_escape($$LISA_SOURCE_TREE/),,g$$i_flag
shared_sources -= \
    src/shared/qbs
sources = src/app src/libs $$plugin_sources $$shared_sources

files = $$files($$PWD/*_??.ts) $$PWD/lisa_untranslated.ts
for(file, files) {
    lang = $$replace(file, .*_([^/]*)\\.ts, \\1)
    v = ts-$${lang}.commands
    $$v = cd $$wd && $$LUPDATE $$sources -ts $$file
    v = ts-$${lang}.depends
    $$v = extract
    QMAKE_EXTRA_TARGETS += ts-$$lang
}
ts-all.commands = cd $$wd && $$LUPDATE $$sources -ts $$files
ts-all.depends = extract
QMAKE_EXTRA_TARGETS += ts-all

check-ts.commands = (cd $$replace(PWD, /, $$QMAKE_DIR_SEP) && perl check-ts.pl)
check-ts.depends = ts-all
QMAKE_EXTRA_TARGETS += check-ts

isEqual(QMAKE_DIR_SEP, /) {
    commit-ts.commands = \
        cd $$wd; \
        git add -N share/lisa/translations/*_??.ts && \
        for f in `git diff-files --name-only share/lisa/translations/*_??.ts`; do \
            $$LCONVERT -locations none -i \$\$f -o \$\$f; \
        done; \
        git add share/lisa/translations/*_??.ts && git commit
} else {
    commit-ts.commands = \
        cd $$wd && \
        git add -N share/lisa/translations/*_??.ts && \
        for /f usebackq %%f in (`git diff-files --name-only share/lisa/translations/*_??.ts`) do \
            $$LCONVERT -locations none -i %%f -o %%f $$escape_expand(\\n\\t) \
        cd $$wd && git add share/lisa/translations/*_??.ts && git commit
}
QMAKE_EXTRA_TARGETS += commit-ts

ts.commands = \
    @echo \"The \'ts\' target has been removed in favor of more fine-grained targets.\" && \
    echo \"Use \'ts-<lang>\' instead. To add a language, use \'ts-untranslated\',\" && \
    echo \"rename the file and re-run \'qmake\'.\"
QMAKE_EXTRA_TARGETS += ts

TEMPLATE = app
TARGET = phony_target2
CONFIG -= qt sdk separate_debug_info gdb_dwarf_index
QT =
LIBS =

updateqm.input = TRANSLATIONS
updateqm.output = $$LISA_DATA_PATH/translations/${QMAKE_FILE_BASE}.qm
isEmpty(vcproj):updateqm.variable_out = PRE_TARGETDEPS
updateqm.commands = $$LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.name = LRELEASE ${QMAKE_FILE_IN}
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm

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

qmfiles.files = $$prependAll(LANGUAGES, $$OUT_PWD/lisa_,.qm)
qmfiles.path = $$LISA_PREFIX/share/lisa/translations
qmfiles.CONFIG += no_check_exist
INSTALLS += qmfiles
