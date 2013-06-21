/********************************************************************
**  Nulloy Music Player, http://nulloy.com
**  Copyright (C) 2010-2013 Sergey Vlasov <sergey@vlasov.me>
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

#include "settings.h"

#include "core.h"

#include <QDir>
#include <QCoreApplication>
#include <QDesktopServices>

#include <QDebug>

#define MIN_VERSION "0.4.5"

NSettings *NSettings::m_instance = NULL;

NSettings::NSettings(QObject *parent)
: QSettings(NCore::rcDir() + "/" +
            NCore::applicationBinaryName() + ".cfg",
            QSettings::IniFormat, parent)
{
	Q_ASSERT_X(!m_instance, "NSettings", "NSettings instance already exists.");
	m_instance = this;

	QString version = value("SettingsVersion").toString();
	if (version.isEmpty() || version < MIN_VERSION) {
		foreach (QString key, allKeys())
			remove(key);
		setValue("SettingsVersion", MIN_VERSION);
	}

	initValue("Shortcuts/playAction", QStringList() << "X" << "C" << "Space");
	initValue("Shortcuts/stopAction", "V");
	initValue("Shortcuts/prevAction", "Z");
	initValue("Shortcuts/nextAction", "B");

	initValue("PlaylistTrackInfo", "%F (%d)");
	initValue("WindowTitleTrackInfo","\"{%a - %t|%F}\" - " + QCoreApplication::applicationName() + " %v");

	initValue("MinimizeToTray", FALSE);
	initValue("TrayIcon", FALSE);
	initValue("AlwaysOnTop", FALSE);
	initValue("WhilePlayingOnTop", FALSE);
	initValue("RestorePlayback", TRUE);
	initValue("SingleInstanse", TRUE);
	initValue("AutoCheckUpdates", TRUE);
	initValue("DisplayLogDialog", TRUE);
	initValue("LastDirectory", QDesktopServices::storageLocation(QDesktopServices::MusicLocation));
	initValue("LoadNext", FALSE);
	initValue("LoadNextSort", QDir::Name);
	initValue("Volume", 0.8);

	initValue("TrackInfo/TopLeft", "{%B kbps/%s kHz|}");
	initValue("TrackInfo/MiddleCenter", "{%a - %t|%F}");
	initValue("TrackInfo/BottomRight", "%T/%d");
}

NSettings::~NSettings()
{
	m_instance = NULL;
}

NSettings* NSettings::instance()
{
	Q_ASSERT_X(m_instance, "NSettings", "NSettings instance has not been created yet.");
	return m_instance;
}

void NSettings::initShortcuts(QObject *instance)
{
	foreach (NAction *action, instance->findChildren<NAction *>()) {
		if (action->isCustomizable())
			m_actionList << action;
	}
}

void NSettings::loadShortcuts()
{
	struct _local
	{
		static QList<QKeySequence> strToSeq(const QStringList &strList)
		{
			QList<QKeySequence> shortcuts;
			foreach (QString str, strList)
				shortcuts << QKeySequence(str);
			return shortcuts;
		}
	};

	foreach (NAction *action, m_actionList) {
		action->setShortcuts(_local::strToSeq(value("Shortcuts/" + action->objectName()).toStringList()));
		action->setGlobalShortcuts(_local::strToSeq(value("GlobalShortcuts/" + action->objectName()).toStringList()));
	}
}

void NSettings::saveShortcuts()
{
	struct _local
	{
		static void save(NSettings *settings, QList<QKeySequence> keys, QString name)
		{
			QStringList keyStrings;
			foreach (QKeySequence seq, keys) {
				if (!seq.isEmpty())
					keyStrings << seq.toString();
			}
			settings->setValue(name, keyStrings);
		}
	};

	foreach (NAction *action, m_actionList) {
		if (action->objectName().isEmpty() || !action->isCustomizable())
			continue;
		_local::save(this, action->shortcuts(), "Shortcuts/" + action->objectName());
		_local::save(this, action->globalShortcuts(), "GlobalShortcuts/" + action->objectName());
	}
}

QList<NAction *> NSettings::shortcuts()
{
	return m_actionList;
}

QVariant NSettings::value(const QString &key, const QVariant &defaultValue)
{
	QVariant value = QSettings::value(key, defaultValue);
	return value;
}

void NSettings::setValue(const QString &key, const QVariant &value)
{
	QSettings::setValue(key, value);
	emit valueChanged(key, value);
}

void NSettings::initValue(const QString &key, const QVariant &defaultValue)
{
	setValue(key, value(key, defaultValue));
}

void NSettings::remove(const QString &key)
{
	QSettings::remove(key);
	emit valueChanged(key, QString());
}

