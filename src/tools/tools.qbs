import qbs

Project {
    name: "Tools"
    references: [
        "qtcreatorcrashhandler/qtcreatorcrashhandler.qbs"
    ].concat(project.additionalTools)
}
