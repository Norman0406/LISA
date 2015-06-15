include(../../lisaplugin.pri)

QT += sql network xml

HEADERS += logbookplugin.h \
    logbookwindow.h \
    database.h \
    profilesettingspage.h \
    logbookconstants.h \
    logbookmode.h \
    logbookproxymodel.h \
    logbooktoolbar.h \
    callsignlookup/callsignlookup.h \
    callsignlookup/callsignlookupqrzcom.h \
    callsignlookup/callsigndata.h \
    callsignlookup/callsignlookupmanager.h \
    callsignlookup/callsignlookupsettingspage.h \
    profiledata.h \
    profilenamevalidator.h \
    logbookentrypane.h \
    importexport/exporter.h \
    importexport/importer.h \
    importexport/importeradif.h \
    importexport/exporteradif.h \
    importexport/exporterfactory.h \
    importexport/importerfactory.h \
    logbookentrydelegate.h \
    spinboxrst.h \
    lineeditfrequency.h

SOURCES += logbookplugin.cpp \
    logbookwindow.cpp \
    database.cpp \
    profilesettingspage.cpp \
    logbookmode.cpp \
    logbookproxymodel.cpp \
    logbooktoolbar.cpp \
    callsignlookup/callsignlookup.cpp \
    callsignlookup/callsignlookupqrzcom.cpp \
    callsignlookup/callsigndata.cpp \
    callsignlookup/callsignlookupmanager.cpp \
    callsignlookup/callsignlookupsettingspage.cpp \
    profiledata.cpp \
    profilenamevalidator.cpp \
    logbookentrypane.cpp \
    importexport/exporter.cpp \
    importexport/importer.cpp \
    importexport/exporteradif.cpp \
    importexport/importeradif.cpp \
    importexport/exporterfactory.cpp \
    importexport/importerfactory.cpp \
    logbookentrydelegate.cpp \
    spinboxrst.cpp \
    lineeditfrequency.cpp

FORMS += \
    profilesettingspage.ui \
    logbookformdialog.ui \
    callsignlookup/callsignlookupsettingspage.ui \
    logbookentrywidget.ui
