include(../../lisaplugin.pri)

QT += sql network xml

HEADERS += logbookplugin.h \
    logbookwindow.h \
    database.h \
    generalsettings.h \
    logbookconstants.h \
    logbookmode.h \
    logbookproxymodel.h \
    logbooktoolbar.h \
    qsoentry.h \
    logbookform.h \
    logbookformdialog.h \
    callsignlookup/callsignlookup.h \
    callsignlookup/callsignlookupqrzcom.h \
    callsignlookup/callsigndata.h \
    callsignlookup/callsignlookupmanager.h \
    callsignlookup/callsignlookupsettingspage.h

SOURCES += logbookplugin.cpp \
    logbookwindow.cpp \
    database.cpp \
    generalsettings.cpp \
    logbookmode.cpp \
    logbookproxymodel.cpp \
    logbooktoolbar.cpp \
    qsoentry.cpp \
    logbookform.cpp \
    logbookformdialog.cpp \
    callsignlookup/callsignlookup.cpp \
    callsignlookup/callsignlookupqrzcom.cpp \
    callsignlookup/callsigndata.cpp \
    callsignlookup/callsignlookupmanager.cpp \
    callsignlookup/callsignlookupsettingspage.cpp

FORMS += \
    generalsettings.ui \
    logbookformdialog.ui \
    callsignlookup/callsignlookupsettingspage.ui
