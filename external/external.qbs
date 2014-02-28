import qbs

Project {
    name: "External"

    DynamicLibrary {
        name: "libfftw3-3"
        Depends { name: "cpp" }

        Group {
            name: "Include"
            prefix: "include/"
            files: [
                "fftw/fftw3.h"
            ]
        }

        Export {
            Depends { name: "cpp" }
            cpp.includePaths: [ "include/" ]
            cpp.libraryPaths : [ "bin/" ]
            cpp.dynamicLibraries: [
                "libfftw3-3"
            ]
        }

        Group {
            name: "Binary"
            condition: qbs.targetOS == "windows"
            qbs.install: true
            qbs.installDir: lisa_app_path
            files: [
                "bin/libfftw3-3.dll"
            ]
        }
    }
}
