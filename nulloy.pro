TEMPLATE = subdirs
CONFIG += ordered

widgets.subdir = src/widgetCollection
SUBDIRS += widgets

# qmake -config plugins_gstreamer
plugins_gstreamer {
	gstreamer_playback.subdir = src/plugins/playbackEngineGstreamer
	gstreamer_waveform.subdir = src/plugins/waveformBuilderGstreamer
	SUBDIRS += gstreamer_playback gstreamer_waveform
}

# qmake -config plugins_phonon
plugins_phonon {
	phonon_playback.subdir = src/plugins/playbackEnginePhonon
	phonon_waveform.subdir = src/plugins/waveformBuilderPhonon
	SUBDIRS += phonon_playback phonon_waveform
}

# qmake -config plugins_vlc
plugins_vlc {
	vlc_playback.subdir = src/plugins/playbackEngineVlc
	vlc_waveform.subdir = src/plugins/waveformBuilderVlc
	SUBDIRS += vlc_playback vlc_waveform
}

# qmake -config no_plugins
!no_plugins:!plugins_gstreamer:!plugins_phonon:!plugins_vlc {
	plugins.subdir = src/plugins
	SUBDIRS += plugins
}

src.file = src/player.pro
SUBDIRS += src

unix {
	include(src/version.pri)
	release.commands =	rm -rf $${TARGET}-$${VERSION} && \
						mkdir $${TARGET}-$${VERSION} && \
						GIT_WORK_TREE=$${TARGET}-$${VERSION} git checkout -f && \
						cd $${TARGET}-$${VERSION} && \
						sed -i \'s/$${LITERAL_HASH}\\(VERSION=\\)_N_VERS_/\\1$${VERSION}/g\' src/version.sh && \
						find obs -type f -exec sed -i \'s/_N_VERS_/$${VERSION}/g\' {} + && \
						rm -f .gitignore && \
						src/changelog.sh -i ChangeLog -c \"Sergey Vlasov <sergey@vlasov.me>\" -p nulloy -r obs/nulloy.changes -d obs/debian.changelog && \
						cd - && \
						tar zcpf $${TARGET}-$${VERSION}.tar.gz $${TARGET}-$${VERSION} && \
						rm -rf $${TARGET}-$${VERSION}
	QMAKE_EXTRA_TARGETS += release
}

# vim: set ts=4 sw=4: #
