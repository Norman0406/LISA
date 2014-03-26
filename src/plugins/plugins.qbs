import qbs

Project {
    name: "Plugins"

    references: [
        "coreplugin/coreplugin.qbs",
        "coreplugin/images/logo/logo.qbs",
        "helloworld/helloworld.qbs",
        "digital/digital.qbs",
        "logbook/logbook.qbs"
    ].concat(project.additionalPlugins)
}
