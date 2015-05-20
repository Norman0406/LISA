include(../../lisaplugin.pri)

QT += sql

HEADERS += logbookplugin.h \
    logbookwindow.h \
    database.h \
    generalsettings.h \
    logbookconstants.h \
    logbookmode.h \
    logbookproxymodel.h \
    logbooktoolbar.h \
    qsoentry.h

SOURCES += logbookplugin.cpp \
    logbookwindow.cpp \
    database.cpp \
    generalsettings.cpp \
    logbookmode.cpp \
    logbookproxymodel.cpp \
    logbooktoolbar.cpp \
    qsoentry.cpp

FORMS += \
    generalsettings.ui \
    logbookformdialog.ui
