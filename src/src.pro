include(../config.pri)

SOURCES += main.cpp \
 CapThread.cpp \
 Capture.cpp \
 Excitation.cpp \
 ExcitCfg.cpp \
 ExcitForm.cpp \
 ExcitThread.cpp \
 FileFft.cpp \
 GenThread.cpp \
 HarmData.cpp \
 Harmonics.cpp \
 HarmPlot.cpp \
 IR.cpp \
 IRInfo.cpp \
 IRPlot.cpp \
 IRPPlot.cpp \
 IrsForm.cpp \
 IrsModel.cpp \
 IrsView.cpp \
 JackWrap.cpp \
 LineEditDelegate.cpp \
 Plotter.cpp \
 PlotWindow.cpp \
 QLCfg.cpp \
 QLUtl.cpp \
 QLWin.cpp \
 SplPlot.cpp \
 StepPlot.cpp \
 TickPoster.cpp \
 WavIn.cpp \
 WavInfo.cpp \
 WavOut.cpp \
 Weights.cpp

CONFIG += warn_on \
 thread \
 qt \
 exceptions \
 build_all \
 release

HEADERS += CapThread.h \
 Capture.h \
 Excitation.h \
 ExcitCfg.h \
 ExcitForm.h \
 ExcitThread.h \
 FileFft.h \
 GenThread.h \
 HarmData.h \
 Harmonics.h \
 HarmPlot.h \
 IR.h \
 IRInfo.h \
 IRPlot.h \
 IRPPlot.h \
 IrsForm.h \
 IrsModel.h \
 IrsView.h \
 JackInfo.h \
 JackWrap.h \
 LineEditDelegate.h \
 Plotter.h \
 PlotWindow.h \
 QLCfg.h \
 QLE.h \
 QLUtl.h \
 QLWin.h \
 SplPlot.h \
 StepPlot.h \
 TickPoster.h \
 WavInfo.h \
 WavIn.h \
 WavOut.h \
 Weights.h

QT += xml \
 widgets \
 charts \
 printsupport

TEMPLATE = app
TARGET = ../bin/qloud
OBJECTS_DIR = ../obj
MOC_DIR = ../moc
DESTDIR = .

CONFIG += debug

INCLUDEPATH += /usr/include/qt

LIBS += -lsndfile \
-lfftw3 \
-ljack

INSTALLS += target
target.path = $$PREFIX/bin

TRANSLATIONS += \
    qloud_en.ts \
    qloud_fr.ts

LOCALE_DIR = ../locale
updateqm.input = TRANSLATIONS
updateqm.output = $$LOCALE_DIR/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} \
    -qm $$LOCALE_DIR/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm

win32-g++:HOST = __mswin
unix:HOST = __unix

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

config.input = config.h.in
config.output = config.h
QMAKE_SUBSTITUTES += config

