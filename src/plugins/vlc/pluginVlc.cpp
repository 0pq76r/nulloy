/********************************************************************
**  Nulloy Music Player, http://nulloy.com
**  Copyright (C) 2010-2012 Sergey Vlasov <sergey@vlasov.me>
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

#include "pluginVlc.h"
#include "playbackEngineVlc.h"
#include "waveformBuilderVlc.h"

NPluginVlc::NPluginVlc(QObject *parent) : QObject(parent)
{
	m_elements << new NPlaybackEngineVlc()
				<< new NWaveformBuilderVlc();
}

NPluginVlc::~NPluginVlc()
{
	foreach (QObject *obj, m_elements)
		delete obj;
}

QObjectList NPluginVlc::elements()
{
	return m_elements;
}

Q_EXPORT_PLUGIN2(plugin_vlc, NPluginVlc)
