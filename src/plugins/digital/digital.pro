include(../../lisaplugin.pri)

QT += multimedia

HEADERS += digitalplugin.h \
    audiomanager.h \
    colormap.h \
    colormapvisitor.h \
    digitalconstants.h \
    digitalmode.h \
    digitalwindow.h \
    factory.h \
    generalsettings.h \
    messenger.h \
    messengertoolbar.h \
    messengerwindow.h \
    soundsettings.h \
    spectrumgraph.h \
    spectrumtoolbar.h \
    spectrumwaterfall.h \
    spectrumwidget.h \
    spectrumwindow.h \
    audio/audioconsumer.h \
    audio/audioconsumerlist.h \
    audio/audiodevice.h \
    audio/audiodevicein.h \
    audio/audiodevicelist.h \
    audio/audiodeviceout.h \
    audio/audioproducer.h \
    audio/audioproducerlist.h \
    audio/audioringbuffer.h \
    modems/modem.h \
    modems/modemconfig.h \
    modems/modemfactory.h \
    modems/modemmanager.h \
    modems/modemreceiver.h \
    modems/modemrtty.h \
    modems/modemrttyconfig.h \
    modems/modemtransmitter.h \
    modems/modemworker.h \
    signalprocessing/fftfilter.h \
    signalprocessing/fftspectrum.h \
    signalprocessing/fftspectrumworker.h \
    signalprocessing/filters.h \
    signalprocessing/misc.h

SOURCES += digitalplugin.cpp \
    audiomanager.cpp \
    colormap.cpp \
    colormapvisitor.cpp \
    digitalmode.cpp \
    digitalwindow.cpp \
    generalsettings.cpp \
    messenger.cpp \
    messengertoolbar.cpp \
    messengerwindow.cpp \
    soundsettings.cpp \
    spectrumgraph.cpp \
    spectrumtoolbar.cpp \
    spectrumwaterfall.cpp \
    spectrumwidget.cpp \
    spectrumwindow.cpp \
    audio/audioconsumer.cpp \
    audio/audioconsumerlist.cpp \
    audio/audiodevice.cpp \
    audio/audiodevicein.cpp \
    audio/audiodevicelist.cpp \
    audio/audiodeviceout.cpp \
    audio/audioproducer.cpp \
    audio/audioproducerlist.cpp \
    audio/audioringbuffer.cpp \
    modems/modem.cpp \
    modems/modemconfig.cpp \
    modems/modemmanager.cpp \
    modems/modemreceiver.cpp \
    modems/modemrtty.cpp \
    modems/modemrttyconfig.cpp \
    modems/modemtransmitter.cpp \
    modems/modemworker.cpp \
    signalprocessing/fftfilter.cpp \
    signalprocessing/fftspectrum.cpp \
    signalprocessing/fftspectrumworker.cpp \
    signalprocessing/filters.cpp \
    signalprocessing/misc.cpp

FORMS += \
    generalsettings.ui \
    messenger.ui \
    soundsettings.ui

win32 {
    INCLUDEPATH += $$LISA_SOURCE_TREE/external/include/fftw
    LIBS += $$LISA_SOURCE_TREE/external/bin/libfftw3-3.dll

    COPYTARGET = $${LISA_SOURCE_TREE}/external/bin/libfftw3-3.dll
    COPYTARGET ~= s,/,\\,g
    COPYDEST = $${LISA_APP_PATH}
    COPYDEST ~= s,/,\\,g

    copyData.commands = $${QMAKE_COPY} $$quote($$COPYTARGET) $$quote($$COPYDEST) $$escape_expand(\\n\\t)
    first.depends = $(first)copyData
    export(first.depends)
    export(copyData.commands)
    QMAKE_EXTRA_TARGETS += first copyData
}

unix {
    LIBS += -L/usr/lib/x86_64-linux-gnu -lfftw3
    INCLUDEPATH += /usr/include
}
