import qbs.base 1.0
import LISAPlugin

LISAPlugin {
    name: "Digital"

    Depends { name: "libfftw3-3" }
    Depends { name: "Core" }
    Depends { name: "Qt"; submodules: ["widgets", "xml", "network", "script", "multimedia"] }
    Depends { name: "cpp" }

    Group {
        name: "Audio"
        prefix: "audio/"
        files: [
            "*.h", "*.cpp", "*.ui"
        ]
    }

    Group {
        name: "Signal Processing"
        prefix: "signalprocessing/"
        files:[
            "*.h", "*.cpp", "*.ui"
        ]
    }

    Group {
        name: "Modems"
        prefix: "modems/"
        files: [
            "*.h", "*.cpp", "*.ui"
        ]
    }


    files: [
        "*.h", "*.cpp", "*.ui", "*.qrc"
    ]
}

