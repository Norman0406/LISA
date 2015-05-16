!isEmpty(LISA_PRI_INCLUDED):error("lisa.pri already included")
LISA_PRI_INCLUDED = 1

LISA_VERSION = 0.0.1
LISA_COMPAT_VERSION = 0.0.1
BINARY_ARTIFACTS_BRANCH = master

# enable c++11
CONFIG += c++11

defineReplace(qtLibraryName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
              else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

defineTest(minQtVersion) {
    maj = $$1
    min = $$2
    patch = $$3
    isEqual(QT_MAJOR_VERSION, $$maj) {
        isEqual(QT_MINOR_VERSION, $$min) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$min) {
            return(true)
        }
    }
    greaterThan(QT_MAJOR_VERSION, $$maj) {
        return(true)
    }
    return(false)
}

# For use in custom compilers which just copy files
defineReplace(stripSrcDir) {
    return($$relative_path($$absolute_path($$1, $$OUT_PWD), $$_PRO_FILE_PWD_))
}

isEmpty(LISA_LIBRARY_BASENAME) {
    LISA_LIBRARY_BASENAME = lib
}

equals(TEST, 1) {
    QT +=testlib
    DEFINES += WITH_TESTS
}

LISA_SOURCE_TREE = $$PWD
isEmpty(LISA_BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    LISA_BUILD_TREE = $$clean_path($$OUT_PWD)
    LISA_BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}
LISA_APP_PATH = $$LISA_BUILD_TREE/bin
macx {
    LISA_APP_TARGET   = "LISA"
    LISA_LIBRARY_PATH = $$LISA_APP_PATH/$${LISA_APP_TARGET}.app/Contents/Frameworks
    LISA_PLUGIN_PATH  = $$LISA_APP_PATH/$${LISA_APP_TARGET}.app/Contents/PlugIns
    LISA_LIBEXEC_PATH = $$LISA_APP_PATH/$${LISA_APP_TARGET}.app/Contents/Resources
    LISA_DATA_PATH    = $$LISA_APP_PATH/$${LISA_APP_TARGET}.app/Contents/Resources
    LISA_BIN_PATH     = $$LISA_APP_PATH/$${LISA_APP_TARGET}.app/Contents/MacOS
    copydata = 1
} else {
    contains(TEMPLATE, vc.*):vcproj = 1
    LISA_APP_TARGET   = lisa
    LISA_LIBRARY_PATH = $$LISA_BUILD_TREE/$$LISA_LIBRARY_BASENAME/lisa
    LISA_PLUGIN_PATH  = $$LISA_LIBRARY_PATH/plugins
    LISA_LIBEXEC_PATH = $$LISA_APP_PATH # FIXME
    LISA_DATA_PATH    = $$LISA_BUILD_TREE/share/lisa
    LISA_BIN_PATH     = $$LISA_APP_PATH
    !isEqual(LISA_SOURCE_TREE, $$LISA_BUILD_TREE):copydata = 1
}

INCLUDEPATH += \
    $$LISA_BUILD_TREE/src \ # for <app/app_version.h>
    $$LISA_SOURCE_TREE/src/libs \
    $$LISA_SOURCE_TREE/tools

LISA_PLUGIN_DIRS_FROM_ENVIRONMENT = $$(LISA_PLUGIN_DIRS)
LISA_PLUGIN_DIRS += $$split(LISA_PLUGIN_DIRS_FROM_ENVIRONMENT, $$QMAKE_DIRLIST_SEP)
LISA_PLUGIN_DIRS += $$LISA_SOURCE_TREE/src/plugins
for(dir, LISA_PLUGIN_DIRS) {
    INCLUDEPATH += $$dir
}

CONFIG += \
    depend_includepath \
    no_include_pwd

LIBS += -L$$LISA_LIBRARY_PATH

!isEmpty(vcproj) {
    DEFINES += LISA_LIBRARY_BASENAME=\"$$LISA_LIBRARY_BASENAME\"
} else {
    DEFINES += LISA_LIBRARY_BASENAME=\\\"$$LISA_LIBRARY_BASENAME\\\"
}

DEFINES += LISA QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII
!macx:DEFINES += QT_USE_FAST_OPERATOR_PLUS QT_USE_FAST_CONCATENATION

unix {
    CONFIG(debug, debug|release):OBJECTS_DIR = $${OUT_PWD}/.obj/debug-shared
    CONFIG(release, debug|release):OBJECTS_DIR = $${OUT_PWD}/.obj/release-shared

    CONFIG(debug, debug|release):MOC_DIR = $${OUT_PWD}/.moc/debug-shared
    CONFIG(release, debug|release):MOC_DIR = $${OUT_PWD}/.moc/release-shared

    RCC_DIR = $${OUT_PWD}/.rcc
    UI_DIR = $${OUT_PWD}/.uic
}

win32-msvc* {
    #Don't warn about sprintf, fopen etc being 'unsafe'
    DEFINES += _CRT_SECURE_NO_WARNINGS
    QMAKE_CXXFLAGS_WARN_ON *= -w44996
    # Speed up startup time when debugging with cdb
    QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO
}

qt {
    contains(QT, core): QT += concurrent
    contains(QT, gui): QT += widgets
}

!isEmpty(LISA_PLUGIN_DEPENDS):LIBS *= -L$$LISA_PLUGIN_PATH
# recursively resolve plugin deps
done_plugins =
for(ever) {
    isEmpty(LISA_PLUGIN_DEPENDS): \
        break()
    done_plugins += $$LISA_PLUGIN_DEPENDS
    for(dep, LISA_PLUGIN_DEPENDS) {
        dependencies_file =
        for(dir, LISA_PLUGIN_DIRS) {
            exists($$dir/$$dep/$${dep}_dependencies.pri) {
                dependencies_file = $$dir/$$dep/$${dep}_dependencies.pri
                break()
            }
        }
        isEmpty(dependencies_file): \
            error("Plugin dependency $$dep not found")
        include($$dependencies_file)
        LIBS += -l$$qtLibraryName($$LISA_PLUGIN_NAME)
    }
    LISA_PLUGIN_DEPENDS = $$unique(LISA_PLUGIN_DEPENDS)
    LISA_PLUGIN_DEPENDS -= $$unique(done_plugins)
}

# recursively resolve library deps
done_libs =
for(ever) {
    isEmpty(LISA_LIB_DEPENDS): \
        break()
    done_libs += $$LISA_LIB_DEPENDS
    for(dep, LISA_LIB_DEPENDS) {
        include($$PWD/src/libs/$$dep/$${dep}_dependencies.pri)
        LIBS += -l$$qtLibraryName($$LISA_LIB_NAME)
    }
    LISA_LIB_DEPENDS = $$unique(LISA_LIB_DEPENDS)
    LISA_LIB_DEPENDS -= $$unique(done_libs)
}
