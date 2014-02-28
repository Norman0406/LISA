import qbs

Project {
    name: "Libs"
    references: [
        "aggregation/aggregation.qbs",
        "extensionsystem/extensionsystem.qbs",
        "utils/utils.qbs",
    ].concat(project.additionalLibs)
}
