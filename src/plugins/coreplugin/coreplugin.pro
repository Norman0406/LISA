DEFINES += CORE_LIBRARY
QT += help \
    network \
    printsupport \
    qml \
    sql

# embedding build time information prevents repeatedly binary exact versions from same source code
isEmpty(LISA_SHOW_BUILD_DATE): LISA_SHOW_BUILD_DATE = $$(LISA_SHOW_BUILD_DATE)
!isEmpty(LISA_SHOW_BUILD_DATE): DEFINES += LISA_SHOW_BUILD_DATE

include(../../lisaplugin.pri)
win32-msvc*:QMAKE_CXXFLAGS += -wd4251 -wd4290 -wd4250
SOURCES += \
    mainwindow.cpp \
    tabpositionindicator.cpp \
    fancyactionbar.cpp \
    fancytabwidget.cpp \
    generalsettings.cpp \
    themesettings.cpp \
    themesettingswidget.cpp \
    id.cpp \
    icontext.cpp \
    messagemanager.cpp \
    messageoutputwindow.cpp \
    outputpane.cpp \
    outputwindow.cpp \
    statusbarmanager.cpp \
    versiondialog.cpp \
    actionmanager/actionmanager.cpp \
    actionmanager/command.cpp \
    actionmanager/commandbutton.cpp \
    actionmanager/actioncontainer.cpp \
    actionmanager/commandsfile.cpp \
    dialogs/settingsdialog.cpp \
    actionmanager/commandmappings.cpp \
    statusbarwidget.cpp \
    coreplugin.cpp \
    modemanager.cpp \
    plugindialog.cpp \
    manhattanstyle.cpp \
    minisplitter.cpp \
    styleanimator.cpp \
    fileiconprovider.cpp \
    icore.cpp \
    infobar.cpp \
    dialogs/ioptionspage.cpp \
    settingsdatabase.cpp \
    imode.cpp \
    outputpanemanager.cpp \
    variablechooser.cpp \
    featureprovider.cpp \
    icorelistener.cpp \
    ioutputpane.cpp \
    windowsupport.cpp \
    themeeditor/themecolors.cpp \
    themeeditor/themecolorstableview.cpp \
    themeeditor/colorvariable.cpp \
    themeeditor/themeeditorwidget.cpp \
    themeeditor/colorrole.cpp \
    themeeditor/themesettingstablemodel.cpp \
    themeeditor/sectionedtablemodel.cpp \
    themeeditor/themesettingsitemdelegate.cpp \
    messagebox.cpp \
    dialogs/shortcutsettings.cpp

HEADERS += \
    mainwindow.h \
    tabpositionindicator.h \
    fancyactionbar.h \
    fancytabwidget.h \
    generalsettings.h \
    themesettings.h \
    themesettingswidget.h \
    id.h \
    messagemanager.h \
    messageoutputwindow.h \
    outputpane.h \
    outputwindow.h \
    statusbarmanager.h \
    actionmanager/actioncontainer.h \
    actionmanager/actionmanager.h \
    actionmanager/command.h \
    actionmanager/commandbutton.h \
    actionmanager/actionmanager_p.h \
    actionmanager/command_p.h \
    actionmanager/actioncontainer_p.h \
    actionmanager/commandsfile.h \
    dialogs/settingsdialog.h \
    actionmanager/commandmappings.h \
    dialogs/ioptionspage.h \
    icontext.h \
    icore.h \
    infobar.h \
    imode.h \
    ioutputpane.h \
    coreconstants.h \
    icorelistener.h \
    versiondialog.h \
    core_global.h \
    statusbarwidget.h \
    coreplugin.h \
    modemanager.h \
    plugindialog.h \
    manhattanstyle.h \
    minisplitter.h \
    styleanimator.h \
    fileiconprovider.h \
    settingsdatabase.h \
    designmode.h \
    outputpanemanager.h \
    variablechooser.h \
    featureprovider.h \
    windowsupport.h \
    themeeditor/themecolors.h \
    themeeditor/themecolorstableview.h \
    themeeditor/colorvariable.h \
    themeeditor/themeeditorwidget.h \
    themeeditor/colorrole.h \
    themeeditor/themesettingstablemodel.h \
    themeeditor/sectionedtablemodel.h \
    themeeditor/themesettingsitemdelegate.h \
    messagebox.h \
    dialogs/shortcutsettings.h

FORMS += \
    generalsettings.ui \
    themesettings.ui \
    themeeditor/themeeditorwidget.ui

RESOURCES += core.qrc \
    fancyactionbar.qrc

win32 {
    QT += gui-private # Uses QPlatformNativeInterface.
    LIBS += -lole32 -luser32
}
else:macx {
    LIBS += -framework AppKit
}
else:unix {
    IMAGE_SIZE_LIST = 16 24 32 48 64 128 256 512

    for(imagesize, IMAGE_SIZE_LIST) {
        eval(image$${imagesize}.files = images/logo/$${imagesize}/QtProject-qtcreator.png)
        eval(image$${imagesize}.path = $$LISA_PREFIX/share/icons/hicolor/$${imagesize}x$${imagesize}/apps)
        INSTALLS += image$${imagesize}
    }
}

equals(TEST, 1) {
    SOURCES += testdatadir.cpp
    HEADERS += testdatadir.h
}
