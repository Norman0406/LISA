import qbs
import qbs.base 1.0

Project {
    property bool withAutotests: qbs.buildVariant === "debug"
    property string lisa_version_major: '0'
    property string lisa_version_minor: '0'
    property string lisa_version_release: '1'
    property string lisa_version: lisa_version_major + '.' + lisa_version_minor + '.' + lisa_version_release
    property string lisa_compat_version_major: '0'
    property string lisa_compat_version_minor: '0'
    property string lisa_compat_version_release: '1'
    property string lisa_compat_version: lisa_compat_version_major + '.' + lisa_compat_version_minor + '.' + lisa_compat_version_release
    property path lisa_source_tree: path
    property string lisa_app_path: qbs.targetOS.contains("osx") ? "" : "bin"
    property string lisa_app_target: qbs.targetOS.contains("osx") ? "LISA" : "lisa"
    property pathList additionalPlugins: []
    property pathList additionalLibs: []
    property pathList additionalTools: []
    property string lisa_library_path: {
        if (qbs.targetOS.contains("osx"))
            return lisa_app_target + ".app/Contents/PlugIns"
        else if (qbs.targetOS.contains("windows"))
            return lisa_app_path
        else
            return "lib/lisa"
    }
    property string lisa_plugin_path: {
        if (qbs.targetOS.contains("osx"))
            return lisa_library_path
        else if (qbs.targetOS.contains("windows"))
            return "lib/lisa/plugins"
        else
            return lisa_library_path + "/plugins"
    }
    property string lisa_data_path: qbs.targetOS.contains("osx")
            ? lisa_app_target + ".app/Contents/Resources"
            : "share/lisa"
    property string lisa_libexec_path: qbs.targetOS.contains("osx")
            ? lisa_data_path
            : lisa_app_path
    property string lisa_bin_path: qbs.targetOS.contains("osx")
            ? lisa_app_target + ".app/Contents/MacOS"
            : lisa_app_path
    property bool testsEnabled: qbs.getenv("TEST") || qbs.buildVariant === "debug"
    property stringList generalDefines: [
        "LISA",
        'LISA_LIBRARY_BASENAME="lib"',
        "QT_DISABLE_DEPRECATED_BEFORE=0x040900",
        "QT_NO_CAST_TO_ASCII",
        "QT_NO_CAST_FROM_ASCII"
    ].concat(testsEnabled ? ["WITH_TESTS"] : [])
    qbsSearchPaths: "qbs"

    references: [
        "src/src.qbs",
        "external/external.qbs",
        //"share/share.qbs",
        "share/lisa/translations/translations.qbs",
        //"tests/tests.qbs"
    ]
}
