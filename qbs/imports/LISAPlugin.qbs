import qbs.base 1.0
import qbs.FileInfo
import LISAFunctions

Product {
    type: ["dynamiclibrary", "pluginSpec"]
    //property string provider: 'QtProject'
    property var pluginspecreplacements
    property var pluginRecommends: []

    Depends { name: "Qt.core" }
    property string minimumQtVersion: "4.8"
    condition: LISAFunctions.versionIsAtLeast(Qt.core.version, minimumQtVersion)

    targetName: LISAFunctions.lisaLibraryName(qbs, name)
    destinationDirectory: project.lisa_plugin_path + '/'// + provider

    Depends { name: "ExtensionSystem" }
    Depends { name: "pluginspec" }
    Depends { name: "cpp" }
    Depends {
        condition: project.testsEnabled
        name: "Qt.test"
    }

    cpp.defines: project.generalDefines.concat([name.toUpperCase() + "_LIBRARY"])
    cpp.installNamePrefix: "@rpath/PlugIns/"// + provider + "/"
    cpp.rpaths: qbs.targetOS.contains("osx") ? ["@loader_path/../..", "@executable_path/.."]
                                      : ["$ORIGIN", "$ORIGIN/..", "$ORIGIN/../.."]
    cpp.linkerFlags: {
        if (qbs.buildVariant == "release" && (qbs.toolchain.contains("gcc") || qbs.toolchain.contains("mingw")))
            return ["-Wl,-s"]
        else if (qbs.buildVariant == "debug" && qbs.toolchain.contains("msvc"))
            return ["/INCREMENTAL:NO"] // Speed up startup time when debugging with cdb
    }

    property string pluginIncludeBase: ".." // #include <plugin/header.h>
    cpp.includePaths: [pluginIncludeBase]

    Group {
        name: "PluginSpec"
        files: [ product.name + ".pluginspec.in" ]
        fileTags: ["pluginSpecIn"]
    }

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.lisa_plugin_path// + "/"// + provider
    }

    Export {
        Depends { name: "ExtensionSystem" }
        Depends { name: "cpp" }
        cpp.includePaths: [pluginIncludeBase]
    }
}
