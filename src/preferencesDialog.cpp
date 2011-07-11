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

#include "preferencesDialog.h"

#include "settings.h"
#include "player.h"

#ifndef _N_NO_SKINS_
#include "skinLoader.h"
#endif

#ifndef _N_NO_PLUGINS_
#include "pluginLoader.h"
#endif

#include <QMessageBox>
#include <QPushButton>

#include <QDebug>

NPreferencesDialog::~NPreferencesDialog() {}

NPreferencesDialog::NPreferencesDialog(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	setObjectName("preferencesDialog");

	connect(ui.buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(saveSettings()));
	connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));

	setWindowTitle(QCoreApplication::applicationName() + " Preferences");

#ifdef _N_NO_SKINS_
	ui.skinLabel->hide();
	ui.skinComboBox->hide();
#endif

#ifdef _N_NO_PLUGINS_
	ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.pluginsTab));
#endif
}

void NPreferencesDialog::initShortcuts()
{
	foreach (QWidget *widget, QApplication::topLevelWidgets()) {
		NPlayer *player = qobject_cast<NPlayer *>(widget);
		if (player) {
			QList<NAction *> allActions = player->findChildren<NAction *>();
			for (int i = 0; i < allActions.size(); ++i) {
				NAction *action = allActions.at(i);
				if (action->parent() == player && action->isGlobal())
					m_globalActionList << action;
			}
		}
	}
}

void NPreferencesDialog::showEvent(QShowEvent *event)
{
	loadSettings();
	QDialog::showEvent(event);
}

void NPreferencesDialog::setVersionLabel(QString text)
{
	ui.versionLabel->setText(text);
}

void NPreferencesDialog::on_versionCheckButton_clicked()
{
	ui.versionLabel->setText("Checking...");
	emit versionOnlineRequested();
}

void NPreferencesDialog::loadSettings()
{
	ui.versionLabel->setText("");

	ui.minimizeToTrayCheckBox->setChecked(NSettings::value("GUI/MinimizeToTray").toBool());
	ui.restorePlaybackCheckBox->setChecked(NSettings::value("RestorePlayback").toBool());
	ui.multipleInstansesCheckBox->setChecked(!NSettings::value("SingleInstanse").toBool());
	ui.trayIconCheckBox->setChecked(NSettings::value("GUI/TrayIcon").toBool());
	ui.versionCheckBox->setChecked(NSettings::value("AutoCheckUpdates").toBool());
	ui.displayLogDialogCheckBox->setChecked(NSettings::value("DisplayLogDialog").toBool());

	int index;

#ifndef _N_NO_PLUGINS_
	// plugins
	foreach (QString str, NPluginLoader::pluginIdentifiers()) {
		QString id = str.section('/', 2);
		if (str.split('/').at(1) == "Playback")
			ui.playbackComboBox->addItem(id.replace('/', ' '), id);
		if (str.split('/').at(1) == "Waveform")
			ui.waveformComboBox->addItem(id.replace('/', ' '), id);
	}

	if (ui.playbackComboBox->count() == 1)
		ui.playbackComboBox->setEnabled(FALSE);
	if (ui.waveformComboBox->count() == 1)
		ui.waveformComboBox->setEnabled(FALSE);

	index = ui.playbackComboBox->findData(NSettings::value("Playback"));
	if (index != -1)
		ui.playbackComboBox->setCurrentIndex(index);
	index = ui.waveformComboBox->findData(NSettings::value("Waveform"));
	if (index != -1)
		ui.waveformComboBox->setCurrentIndex(index);
#endif

#ifndef _N_NO_SKINS_
	// skins
	foreach (QString str, NSkinLoader::skinIdentifiers()) {
		QString id = str.section('/', 2);
		ui.skinComboBox->addItem(id.replace('/', ' '), id);
	}

	if (ui.skinComboBox->count() == 1)
		ui.skinComboBox->setEnabled(FALSE);

	index = ui.skinComboBox->findData(NSettings::value("GUI/Skin"));
	if (index != -1)
		ui.skinComboBox->setCurrentIndex(index);
#endif

	// shortcuts
	for (int i = 0; i < m_globalActionList.size(); ++i) {
		QString strSeq = NSettings::value("GlobalShortcuts/" + m_globalActionList.at(i)->objectName()).toString();
		if (!strSeq.isEmpty())
			m_globalActionList.at(i)->setShortcut(QKeySequence(strSeq));
	}
	ui.globalShortcutEditorWidget->init(m_globalActionList);
}

void NPreferencesDialog::saveSettings()
{
	NSettings::setValue("GUI/MinimizeToTray", ui.minimizeToTrayCheckBox->isChecked());
	NSettings::setValue("RestorePlayback", ui.restorePlaybackCheckBox->isChecked());
	NSettings::setValue("SingleInstanse", !ui.multipleInstansesCheckBox->isChecked());
	NSettings::setValue("GUI/TrayIcon", ui.trayIconCheckBox->isChecked());
	NSettings::setValue("AutoCheckUpdates", ui.versionCheckBox->isChecked());
	NSettings::setValue("DisplayLogDialog", ui.displayLogDialogCheckBox->isChecked());

	bool showPluginMessage = FALSE;
	bool showSkinMessage = FALSE;

#ifndef _N_NO_PLUGINS_
	// plugins
	QVariant playbackVariant = ui.playbackComboBox->itemData(ui.playbackComboBox->currentIndex());
	if (NSettings::value("Playback").isValid() && playbackVariant != NSettings::value("Playback"))
		showPluginMessage = TRUE;
	QVariant waveformVariant = ui.waveformComboBox->itemData(ui.waveformComboBox->currentIndex());
	if (NSettings::value("Waveform").isValid() && waveformVariant != NSettings::value("Waveform"))
		showPluginMessage = TRUE;

	NSettings::setValue("Playback", playbackVariant);
	NSettings::setValue("Waveform", waveformVariant);
#endif

#ifndef _N_NO_SKINS_
	// skins
	QVariant skinVariant = ui.skinComboBox->itemData(ui.skinComboBox->currentIndex());
	if (NSettings::value("GUI/Skin").isValid() && skinVariant != NSettings::value("GUI/Skin"))
		showSkinMessage = TRUE;

	NSettings::setValue("GUI/Skin", skinVariant);
#endif

	QString message;
	if (showPluginMessage && showSkinMessage) {
		message = tr("Switching plugins and skins requires restart.");
	} else {
		if (showPluginMessage)
			message = tr("Switching plugins requires restart.");
		if (showSkinMessage)
			message = tr("Switching skins requires restart.");
	}
	if (!message.isEmpty()) {
		QMessageBox box(QMessageBox::Information, windowTitle(), message, QMessageBox::Close, this);
		box.exec();
	}

	// shortcuts
	ui.globalShortcutEditorWidget->applyShortcuts();
	for (int i = 0; i < m_globalActionList.size(); ++i) {
		QList<QKeySequence> shortcut = m_globalActionList.at(i)->shortcuts();
		QStringList strSeqList;
		foreach (QKeySequence seq, shortcut)
			strSeqList << seq.toString();

		if (!strSeqList.isEmpty())
			NSettings::setValue("GlobalShortcuts/" + m_globalActionList.at(i)->objectName(), strSeqList.join(", "));
		else
			NSettings::remove("GlobalShortcuts/" + m_globalActionList.at(i)->objectName());
	}

	emit settingsChanged();
}

/* vim: set ts=4 sw=4: */
