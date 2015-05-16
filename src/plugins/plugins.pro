include(../../lisa.pri)

TEMPLATE  = subdirs

SUBDIRS   = \
    coreplugin \
    logbook \
    digital

isEmpty(LISA_PACKAGE_MODE) {
    SUBDIRS += \
        helloworld #\
        #updateinfo
#} else:!isEmpty(UPDATEINFO_ENABLE) {
#    SUBDIRS += \
#        updateinfo
}

for(p, SUBDIRS) {
    LISA_PLUGIN_DEPENDS =
    include($$p/$${p}_dependencies.pri)
    pv = $${p}.depends
    $$pv = $$LISA_PLUGIN_DEPENDS
}
