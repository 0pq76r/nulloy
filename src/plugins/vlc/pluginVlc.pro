unix:TARGET = plugin_vlc
win32:TARGET = PluginVLC

include(../common.pri)

CONFIG += link_pkgconfig
PKGCONFIG += libvlc vlc-plugin

INCLUDEPATH += ..

HEADERS += *.h
SOURCES += *.cpp ../abstractWaveformBuilder.cpp ../../waveformPeaks.cpp

