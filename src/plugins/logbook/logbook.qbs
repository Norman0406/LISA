import qbs.base 1.0
import LISAPlugin

LISAPlugin {
    name: "Logbook"

    Depends { name: "Core" }
    Depends { name: "Qt"; submodules: ["widgets", "xml", "network", "script", "multimedia"] }
    Depends { name: "cpp" }

    files: [
        "*.h", "*.cpp", "*.ui", "*.qrc"
    ]
}

