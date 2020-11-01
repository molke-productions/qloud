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

INCLUDEPATH += /usr/include/qwt \
/usr/include/qt

LIBS += -lsndfile \
-lfftw3 \
-ljack

QMAKE_CXXFLAGS += -std=c++11

INSTALLS += target
target.path = $$PREFIX/bin
