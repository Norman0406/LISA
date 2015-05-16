macx {
    QMAKE_LFLAGS += -Wl,-rpath,@loader_path/../,-rpath,@executable_path/../
} else:linux-* {
    #do the rpath by hand since it's not possible to use ORIGIN in QMAKE_RPATHDIR
    # this expands to $ORIGIN (after qmake and make), it does NOT read a qmake var
    QMAKE_RPATHDIR += \$\$ORIGIN
    QMAKE_RPATHDIR += \$\$ORIGIN/..
    QMAKE_RPATHDIR += \$\$ORIGIN/../$$LISA_LIBRARY_BASENAME/lisa
    LISA_PLUGIN_RPATH = $$join(QMAKE_RPATHDIR, ":")

    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${LISA_PLUGIN_RPATH}\'
    QMAKE_RPATHDIR =
}
