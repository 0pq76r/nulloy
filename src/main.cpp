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

#include "player.h"
#include <qtsingleapplication.h>
#include "skinFileSystem.h"

Q_IMPORT_PLUGIN(widget_collection)

int main(int argc, char *argv[])
{
	QtSingleApplication instance(argc, argv);

	QString msg;
	if (QCoreApplication::arguments().size() > 1) {
		QStringList pathList;
		foreach (QString arg, QCoreApplication::arguments()) {
			if (QFile(arg).exists())
				pathList << arg;
		}
		msg = "files:" + pathList.join("<|>");
	}
	if (instance.sendMessage(msg))
		return 0;

	QApplication::setQuitOnLastWindowClosed(FALSE);

	QCoreApplication::setApplicationName("Nulloy");
	QCoreApplication::setApplicationVersion("0.1 Alpha");

#if !defined WIN32 && !defined _WINDOWS && !defined Q_WS_WIN
	int res;
	res = chdir(QCoreApplication::applicationDirPath().toAscii().data());
#endif

	NSkinFileSystem::init();

	NPlayer p;
	QObject::connect(&instance, SIGNAL(messageReceived(const QString &)),
					&p, SLOT(message(const QString &)));

	return instance.exec();
}

/* vim: set ts=4 sw=4: */
