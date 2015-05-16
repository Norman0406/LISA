include(../../lisa.pri)

TEMPLATE  = subdirs

SUBDIRS   = \
    aggregation \
    extensionsystem \
    utils

for(l, SUBDIRS) {
    LISA_LIB_DEPENDS =
    include($$l/$${l}_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$LISA_LIB_DEPENDS
}

SUBDIRS += \
    utils/process_stub.pro

win32:SUBDIRS += utils/process_ctrlc_stub.pro
