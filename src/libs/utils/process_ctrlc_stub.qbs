import qbs.base 1.0
import QtcTool

QtcTool {
    name: "lisa_ctrlc_stub"
    consoleApplication: true
    condition: qbs.targetOS.contains("windows")


    files: [ "process_ctrlc_stub.cpp" ]

    cpp.dynamicLibraries: [
        "user32",
        "shell32"
    ]
}
