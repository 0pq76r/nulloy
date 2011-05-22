/********************************************************************
**  Nulloy Music Player, http://nulloy.com
**  Copyright (C) 2010-2011 Sergey Vlasov <sergey@vlasov.me>
**
**  This program can be distributed under the terms of the GNU
**  General Public License version 3.0 as published by the Free
**  Software Foundation and appearing in the file LICENSE.GPL3
**  included in the packaging of this file.  Please review the
**  following information to ensure the GNU General Public License
**  version 3.0 requirements will be met:
**
**  http://www.gnu.org/licenses/gpl-3.0.html
**
*********************************************************************/

#ifndef N_WAVEFORM_BUILDER_VLC_H
#define N_WAVEFORM_BUILDER_VLC_H

#include <vlc/vlc.h>
#include <QTimer>
#include "waveformBuilderInterface.h"
#include "pluginInterface.h"

class NWaveformBuilderVlc : public NWaveformBuilderInterface, public NPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(NWaveformBuilderInterface NPluginInterface)

private:
	libvlc_instance_t *m_vlcInstance;
	libvlc_media_player_t *m_mediaPlayer;
	QString m_currentFile;

	QByteArray m_pcmBuffer;
	QTimer *m_timer;
	qreal position();
	void start();

public:
	NWaveformBuilderVlc(QObject *parent = NULL) : NWaveformBuilderInterface(parent) {}
	~NWaveformBuilderVlc();
	void init();
	QString identifier() { return "Nulloy/Waveform/Vlc/0.1"; }
	QString interface() { return WAVEFORM_INTERFACE; }

	void startFile(const QString &file);
	void stop();
	void prepareBuffer(uint8_t **pcmBuffer, unsigned int size);
	void handleBuffer(uint8_t *pcmBuffer, unsigned int nChannels, unsigned int nSamples);

private slots:
	void update();
};

#endif

/* vim: set ts=4 sw=4: */
