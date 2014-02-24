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

#include "aboutDialog.h"

#include <QCoreApplication>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QTabWidget>
#include <QLabel>
#include <QTextBrowser>
#include <QTextStream>
#include <QPushButton>
#include <QFile>

NAboutDialog::NAboutDialog(QWidget *parent) : QDialog(parent)
{
	QString aboutHtml = QString() +
#ifdef Q_WS_MAC
		"<span style=\"font-size:14pt;\">" +
#else
		"<span style=\"font-size:9pt;\">" +
#endif
			"<b>" +  QCoreApplication::applicationName() + " Music Player</b>" +
			"<br>" +
			"<a href='http://" + QCoreApplication::organizationDomain() + "'>http://" +
			                     QCoreApplication::organizationDomain() + "</a>" +
		"</span><br><br>" +
#ifdef Q_WS_MAC
		"<span style=\"font-size:10pt;\">" +
#else
		"<span style=\"font-size:8pt;\">" +
#endif
			"Version: " + QCoreApplication::applicationVersion() + "<br>" +
			              (QString(_N_TIME_STAMP_).isEmpty() ? "" : "Build: " + QString(_N_TIME_STAMP_)) +
			"<br><br>" +
			"Copyright (C) 2010-2013  Sergey Vlasov &lt;sergey@vlasov.me&gt;"
		"</span>";

	setWindowTitle(QObject::tr("About ") + QCoreApplication::applicationName());
	setMaximumSize(0, 0);

	QVBoxLayout *layout = new QVBoxLayout;
	setLayout(layout);

	QTabWidget *tabWidget = new QTabWidget(parent);
	layout->addWidget(tabWidget);

	// about tab
	QWidget *aboutTab = new QWidget;
	tabWidget->addTab(aboutTab, tr("About"));
	QVBoxLayout *aboutTabLayout = new QVBoxLayout;
	aboutTab->setLayout(aboutTabLayout);

	QLabel *iconLabel = new QLabel;
	QPixmap pixmap(":icon-96.png");
	iconLabel->setPixmap(pixmap);
#ifdef Q_WS_MAC
	iconLabel->setMask(pixmap.mask());
#endif

	QHBoxLayout *iconLayout = new QHBoxLayout;
	iconLayout->addStretch();
	iconLayout->addWidget(iconLabel);
	iconLayout->addStretch();
	aboutTabLayout->addLayout(iconLayout);

	QTextBrowser *aboutTextBrowser = new QTextBrowser;
	aboutTextBrowser->setObjectName("aboutTextBrowser");
	aboutTextBrowser->setStyleSheet("background: transparent");
	aboutTextBrowser->setFrameShape(QFrame::NoFrame);
	aboutTextBrowser->setMinimumWidth(350);
	aboutTextBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	aboutTextBrowser->setOpenExternalLinks(TRUE);
	aboutTextBrowser->setHtml("<center>" + aboutHtml + "</center>");

	aboutTabLayout->addWidget(aboutTextBrowser);
	//

	// changelog tab
	QWidget *changelogTab = new QWidget;
	tabWidget->addTab(changelogTab, tr("Changelog"));
	QVBoxLayout *changelogTabLayout = new QVBoxLayout;
	changelogTabLayout->setContentsMargins(0, 0, 0, 0);
	changelogTab->setLayout(changelogTabLayout);

	QFile changelogFile( ":/ChangeLog");
	changelogFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream changelogStream(&changelogFile);
	QString changelogHtml = changelogStream.readAll();
	changelogHtml.replace("\n", "<br>\n");
	changelogHtml.replace(QRegExp("(\\*[^<]*)(<br>)"), "<b>\\1</b>\\2");
	changelogFile.close();

	QTextBrowser *changelogTextBrowser = new QTextBrowser;
	changelogTextBrowser->setHtml(changelogHtml);
	changelogTextBrowser->setOpenExternalLinks(TRUE);
	changelogTabLayout->addWidget(changelogTextBrowser);
	//

	// license tab
	QWidget *licenseTab = new QWidget;
	tabWidget->addTab(licenseTab, tr("License"));
	QVBoxLayout *licenseTabLayout = new QVBoxLayout;
	licenseTab->setLayout(licenseTabLayout);

	QString licenseHtml =
#ifdef Q_WS_MAC
		"<span style=\"font-size:10pt;\">"
#else
		"<span style=\"font-size:8pt;\">"
#endif
			"This program is free software: you can redistribute it and/or modify "
			"it under the terms of the GNU General Public License version 3.0 "
			"as published by the Free Software Foundation.<br>"
			"<br>"
			"This program is distributed in the hope that it will be useful, "
			"but <b>WITHOUT ANY WARRANTY</b>; without even the implied warranty of "
			"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
			"GNU General Public License for more details.<br>"
			"<br>"
			"You should have received a copy of the GNU General Public License "
			"along with this program. If not, see "
			"<a href='http://www.gnu.org/licenses/gpl-3.0.html'>http://www.gnu.org/licenses/gpl-3.0.html</a>.";
		"</span>";

	QTextBrowser *licenseTextBrowser = new QTextBrowser;
	licenseTextBrowser->setObjectName("licenseTextBrowser");
	licenseTextBrowser->setStyleSheet("background: transparent");
	licenseTextBrowser->setFrameShape(QFrame::NoFrame);
	licenseTextBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	licenseTextBrowser->setOpenExternalLinks(TRUE);
	licenseTextBrowser->setAlignment(Qt::AlignVCenter);
	licenseTextBrowser->setHtml(licenseHtml);

	licenseTabLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	licenseTabLayout->addWidget(licenseTextBrowser);
	licenseTabLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	//

	QPushButton *closeButton = new QPushButton("Close");
	connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(closeButton);
	buttonLayout->addStretch();
	layout->addLayout(buttonLayout);
}

NAboutDialog::~NAboutDialog() {}

void NAboutDialog::show()
{
	QDialog::show();
	
	// resize according to content
	QTextBrowser *aboutTextBrowser = qFindChild<QTextBrowser *>(parent(), "aboutTextBrowser");
	QSize aboutTextSize = aboutTextBrowser->document()->size().toSize();
	aboutTextBrowser->setMinimumHeight(aboutTextSize.height());

	QTextBrowser *licenseTextBrowser = qFindChild<QTextBrowser *>(parent(), "licenseTextBrowser");
	QSize licenseTextSize = licenseTextBrowser->document()->size().toSize();
	licenseTextBrowser->setMinimumHeight(licenseTextSize.height());
}

