import qbs.base 1.0

Application {
    type: "application" // no Mac app bundle
    Depends { name: "cpp" }
    cpp.defines: project.generalDefines
    cpp.linkerFlags: {
        if (qbs.buildVariant == "release" && (qbs.toolchain.contains("gcc") || qbs.toolchain.contains("mingw")))
            return ["-Wl,-s"]
    }

    property string toolInstallDir: project.lisa_libexec_path

    cpp.rpaths: qbs.targetOS.contains("osx")
            ? ["@executable_path/../" + project.lisa_library_path]
            : ["$ORIGIN/../" + project.lisa_library_path]

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: toolInstallDir
    }
}
