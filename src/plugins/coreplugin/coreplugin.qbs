import qbs.base 1.0
import LISAPlugin

LISAPlugin {
    name: "Core"

    Depends {
        name: "Qt"
        submodules: [
            "widgets", "xml", "network", "script", "sql", "help", "printsupport"
        ]
    }
    Depends { name: "Utils" }
    Depends { name: "Aggregation" }
    Depends { name: "app_version_header" }

    cpp.dynamicLibraries: {
        if (qbs.targetOS.contains("windows")) return [
            "ole32",
            "user32"
        ]
    }

    cpp.frameworks: qbs.targetOS.contains("osx") ? ["AppKit"] : undefined

    Group {
        name: "General"
        files: [
            "basefilewizard.cpp", "basefilewizard.h",
            "core.qrc",
            "core_global.h",
            "coreconstants.h",
            "coreplugin.cpp", "coreplugin.h",
            "designmode.cpp", "designmode.h",
            "documentmanager.cpp", "documentmanager.h",
            "editmode.cpp", "editmode.h",
            "editortoolbar.cpp", "editortoolbar.h",
            //"externaltool.cpp", "externaltool.h",
            //"externaltoolmanager.h",
            "fancyactionbar.cpp", "fancyactionbar.h", "fancyactionbar.qrc",
            "fancytabwidget.cpp", "fancytabwidget.h",
            "featureprovider.cpp", "featureprovider.h",
            "fileiconprovider.cpp", "fileiconprovider.h",
            "fileutils.cpp", "fileutils.h",
            "findplaceholder.cpp", "findplaceholder.h",
            "generalsettings.cpp", "generalsettings.h", "generalsettings.ui",
            "generatedfile.cpp", "generatedfile.h",
            "helpmanager.cpp", "helpmanager.h",
            "icontext.cpp", "icontext.h",
            "icore.cpp", "icore.h",
            "icorelistener.cpp", "icorelistener.h",
            "id.cpp", "id.h",
            "idocument.cpp", "idocument.h",
            "idocumentfactory.h",
            "ifilewizardextension.h",
            "imode.cpp", "imode.h",
            "inavigationwidgetfactory.cpp", "inavigationwidgetfactory.h",
            "infobar.cpp", "infobar.h",
            "ioutputpane.h",
            "iversioncontrol.cpp", "iversioncontrol.h",
            "mainwindow.cpp", "mainwindow.h",
            "manhattanstyle.cpp", "manhattanstyle.h",
            "messagemanager.cpp", "messagemanager.h",
            "messageoutputwindow.cpp", "messageoutputwindow.h",
            "mimedatabase.cpp", "mimedatabase.h",
            "mimetypemagicdialog.cpp", "mimetypemagicdialog.h", "mimetypemagicdialog.ui",
            "mimetypesettings.cpp", "mimetypesettings.h",
            "mimetypesettingspage.ui",
            "minisplitter.cpp", "minisplitter.h",
            "modemanager.cpp", "modemanager.h",
            "navigationsubwidget.cpp", "navigationsubwidget.h",
            "navigationwidget.cpp", "navigationwidget.h",
            "outputpane.cpp", "outputpane.h",
            "outputpanemanager.cpp", "outputpanemanager.h",
            "outputwindow.cpp", "outputwindow.h",
            "plugindialog.cpp", "plugindialog.h",
            "removefiledialog.cpp", "removefiledialog.h", "removefiledialog.ui",
            "rightpane.cpp", "rightpane.h",
            "settingsdatabase.cpp", "settingsdatabase.h",
            "sidebar.cpp", "sidebar.h",
            "sidebarwidget.cpp", "sidebarwidget.h",
            "statusbarmanager.cpp", "statusbarmanager.h",
            "statusbarwidget.cpp", "statusbarwidget.h",
            "styleanimator.cpp", "styleanimator.h",
            "tabpositionindicator.cpp", "tabpositionindicator.h",
            "textdocument.cpp", "textdocument.h",
            //"toolsettings.cpp", "toolsettings.h",
            "variablechooser.cpp", "variablechooser.h", "variablechooser.ui",
            "variablemanager.cpp", "variablemanager.h",
            //"vcsmanager.cpp", "vcsmanager.h",
            "versiondialog.cpp", "versiondialog.h",
        ]
    }

    Group {
        name: "Action Manager"
        prefix: "actionmanager/"
        files: [
            "actioncontainer.cpp", "actioncontainer.h", "actioncontainer_p.h",
            "actionmanager.cpp", "actionmanager.h", "actionmanager_p.h",
            "command.cpp", "command.h", "command_p.h",
            "commandbutton.cpp", "commandbutton.h",
            "commandmappings.cpp", "commandmappings.h", "commandmappings.ui",
            "commandsfile.cpp", "commandsfile.h",
        ]
    }

    Group {
        name: "Dialogs"
        prefix: "dialogs/"
        files: [
            //"addtovcsdialog.cpp", "addtovcsdialog.h", "addtovcsdialog.ui",
            //"externaltoolconfig.cpp", "externaltoolconfig.h", "externaltoolconfig.ui",
            "ioptionspage.cpp", "ioptionspage.h",
            "iwizard.cpp", "iwizard.h",
            "newdialog.cpp", "newdialog.h", "newdialog.ui",
            "openwithdialog.cpp", "openwithdialog.h", "openwithdialog.ui",
            "promptoverwritedialog.cpp", "promptoverwritedialog.h",
            "readonlyfilesdialog.cpp", "readonlyfilesdialog.h", "readonlyfilesdialog.ui",
            "saveitemsdialog.cpp", "saveitemsdialog.h", "saveitemsdialog.ui",
            "settingsdialog.cpp", "settingsdialog.h",
            "shortcutsettings.cpp", "shortcutsettings.h",
        ]
    }

    Group {
        name: "Editor Manager"
        prefix: "editormanager/"
        files: [
            "BinFiles.mimetypes.xml",
            "documentmodel.cpp", "documentmodel.h",
            "editormanager.cpp", "editormanager.h",
            "editorview.cpp", "editorview.h",
            "ieditor.cpp", "ieditor.h",
            "ieditorfactory.cpp", "ieditorfactory.h",
            "iexternaleditor.cpp", "iexternaleditor.h",
            "openeditorsview.cpp", "openeditorsview.h",
            "openeditorswindow.cpp", "openeditorswindow.h",
            "systemeditor.cpp", "systemeditor.h",
        ]
    }

    Group {
        name: "Progress Manager"
        prefix: "progressmanager/"
        files: [
            "futureprogress.cpp", "futureprogress.h",
            "progressbar.cpp", "progressbar.h",
            "progressmanager.cpp", "progressmanager.h", "progressmanager_p.h",
            "progressview.cpp", "progressview.h",
        ]
    }

    Group {
        name: "ProgressManager_win"
        condition: qbs.targetOS.contains("windows")
        files: [
            "progressmanager/progressmanager_win.cpp",
        ]
    }

    Group {
        name: "ProgressManager_mac"
        condition: qbs.targetOS.contains("osx")
        files: [
            "macfullscreen.h",
            "macfullscreen.mm",
            "progressmanager/progressmanager_mac.mm",
        ]
    }

    Group {
        name: "ProgressManager_x11"
        condition: qbs.targetOS.contains("unix") && !qbs.targetOS.contains("osx")
        files: [
            "progressmanager/progressmanager_x11.cpp",
        ]
    }

    Group {
        name: "Tests"
        condition: project.testsEnabled
        files: [
            "testdatadir.cpp",
            "testdatadir.h",
        ]
    }

    Export {
        Depends { name: "Aggregation" }
        Depends { name: "Utils" }
    }
}
