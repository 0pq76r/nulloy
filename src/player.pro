TEMPLATE = app
QT += script

unix:TARGET = nulloy
win32:TARGET = Nulloy
DESTDIR = ..

DEPENDPATH += . ux/
INCLUDEPATH += . ux/

HEADERS += *.h ux/*.h
SOURCES += *.cpp ux/*.cpp

FORMS += *.ui

OBJECTS_DIR	= $$TMP_DIR
MOC_DIR		= $$TMP_DIR
RCC_DIR		= $$TMP_DIR
UI_DIR		= $$TMP_DIR

# trash
HEADERS += trash/trash.h
INCLUDEPATH += trash/
win32 {
	SOURCES += trash/trash_win.cpp
	LIBS += -ladvapi32 -lshell32
}
mac {
	OBJECTIVE_SOURCES += trash/trash_mac.mm
	LIBS += -framework Foundation -framework Cocoa
}
unix:!mac {
	SOURCES += trash/trash_x11.cpp
}

# qmake -config no-skins
!no-skins {
	CONFIG += uitools
	INCLUDEPATH += widgetCollection
	LIBS += -LwidgetCollection -lwidget_collection
	unix:PRE_TARGETDEPS += widgetCollection/libwidget_collection.a
	win32:PRE_TARGETDEPS += widgetCollection/widget_collection.lib
	RESOURCES += native-skin-embedded.qrc

	unix {
		SRC_DIR=$$PWD
		silver_skin.target = ../skins/silver.nzs
		silver_skin.depends = skins/silver/*
		silver_skin.commands =	[ -d $$SRC_DIR/../skins ] || mkdir $$SRC_DIR/../skins && \
								cd $$TMP_DIR && cp -r $$SRC_DIR/skins/silver . && \
								cd silver && \
								rm design.svg && \
								zip $$SRC_DIR/../skins/silver.nzs *
		QMAKE_EXTRA_TARGETS += silver_skin
		PRE_TARGETDEPS += $$silver_skin.target
		#dirty hack for install
		system($$silver_skin.commands)
	}
} else {
	DEFINES += _N_NO_SKINS_

	HEADERS -= skinFileSystem.h   skinLoader.h
	SOURCES -= skinFileSystem.cpp skinLoader.cpp
	HEADERS +=	widgetCollection/dropArea.h \
				widgetCollection/label.h \
				widgetCollection/playlistWidget.h \
				widgetCollection/slider.h \
				widgetCollection/waveformSlider.h \
				widgetCollection/playlistItem.h
	SOURCES +=	widgetCollection/dropArea.cpp \
				widgetCollection/label.cpp \
				widgetCollection/playlistWidget.cpp \
				widgetCollection/slider.cpp \
				widgetCollection/waveformSlider.cpp \
				widgetCollection/playlistItem.cpp

	DEPENDPATH += widgetCollection/
	INCLUDEPATH += widgetCollection/

	RESOURCES += no-skins.qrc
	FORMS += skins/native/form.ui
}


# win7 taskbar
win32 {
	LIBS += -lole32 -luser32
} else {
	HEADERS -= ux/w7TaskBar.h
	SOURCES -= ux/w7TaskBar.cpp
}

RESOURCES += icons/icons.qrc
win32:RC_FILE = icons/icon.rc
mac:ICON = icons/icon.icns


include(version.pri)
DEFINES += _N_VERSION_=\""\\\"$${N_VERSION}\\\""\"
win32:DEFINES += _N_TIME_STAMP_=__TIMESTAMP__
unix:DEFINES += _N_TIME_STAMP_=\""\\\"`date +\\\"%a %b %d %T %Y\\\"`\\\""\"
build_pass:CONFIG(static, static|shared) {
	DEFINES += _N_STATIC_BUILD_
} else {
	DEFINES += _N_SHARED_BUILD_
}


include(../3rdParty/qxt-0.6.1~reduced/src/gui/qxtglobalshortcut.pri)
include(../3rdParty/qtsingleapplication-2.6.1/src/qtsingleapplication.pri)
include(../3rdParty/qtiocompressor-2.3.1/src/qtiocompressor.pri)

# qmake -config no-plugins
!no-plugins {
	HEADERS -= pluginLoader.h
	SOURCES -= pluginLoader.cpp
} else {
	DEFINES += _N_NO_PLUGINS_
}


# qmake -config embed-gstreamer
embed-gstreamer|no-plugins {
	include(plugins/gstreamer.pri)
	DEFINES += _N_GSTREAMER_PLUGINS_BUILTIN_
	HEADERS += plugins/waveformBuilderGstreamer/*.h plugins/playbackEngineGstreamer/*.h
	SOURCES += plugins/waveformBuilderGstreamer/*.cpp plugins/playbackEngineGstreamer/*.cpp
	INCLUDEPATH += plugins/waveformBuilderGstreamer plugins/playbackEngineGstreamer
}


# qmake "PREFIX=/usr"
unix:!mac {
	prefix.path = $$PREFIX
	target.path = $$prefix.path/bin

	system(icons/install-icons.sh $$TMP_DIR/icons)
	icons.files = $$TMP_DIR/icons/*
	icons.path = $$prefix.path

	desktop.files = ../nulloy.desktop
	desktop.path = $$prefix.path/share/applications

	INSTALLS += target icons desktop

	!no-skins {
		skins.files = ../skins/*
		skins.path = $$prefix.path/share/nulloy/skins
		INSTALLS += skins
	}

	!no-plugins {
		plugins.files = ../plugins/*
		plugins.path = $$prefix.path/lib/nulloy/plugins
		INSTALLS += plugins
	}
}


mac {
	prefix.path = ../$${TARGET}.app

	!no-skins {
		skins.files = ../skins/*
		skins.path = $$prefix.path/Contents/MacOS/skins
		INSTALLS += skins
	}

	!no-plugins {
		plugins.files = ../plugins/*
		plugins.path = $$prefix.path/Contents/MacOS/plugins
		INSTALLS += plugins
	}
}

# vim: set ts=4 sw=4: #
