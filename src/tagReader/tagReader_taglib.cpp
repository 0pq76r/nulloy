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

#include "tagReader.h"

#include <taglib/tag.h>
#include <taglib/fileref.h>

#include <QCoreApplication>
#include <QFileInfo>

class NTagReaderPrivate
{
public:
	NTagReaderPrivate() {}
	~NTagReaderPrivate() {}

	QString m_path;
	TagLib::FileRef *m_tagRef;
};

NTagReader::NTagReader(const QString &file) : d_ptr(new NTagReaderPrivate())
{
	Q_D(NTagReader);
	d->m_path = file;
	d->m_tagRef = new TagLib::FileRef(file.toUtf8().data());
}

NTagReader::~NTagReader()
{
	Q_D(NTagReader);
	delete d->m_tagRef;
}

bool NTagReader::isValid()
{
	Q_D(NTagReader);
	return d->m_tagRef->file() && d->m_tagRef->file()->isValid();
}

QString NTagReader::toString(const QString &format)
{
	Q_D(NTagReader);

	if (format.isEmpty())
		return "";

	if (!d->m_tagRef->file()->isValid())
		return "NTagReader::InvalidFile";

	TagLib::Tag *tag = d->m_tagRef->tag();
	TagLib::AudioProperties *prop = d->m_tagRef->audioProperties();

	int seconds_total = prop->length();

	QString res;
	for (int i = 0; i < format.size(); ++i) {
		if (format.at(i) == '%') {
			++i;
			QChar ch = format.at(i);
			if (ch == 'a') {
				QString str = TStringToQString(tag->artist());
				if (str.isEmpty())
					str = "<Unknown artist>";
				res += str;
			} else if (ch == 't') {
				QString str = TStringToQString(tag->title());
				if (str.isEmpty())
					str = QFileInfo(d->m_path).baseName();
				res += str;
			} else if (ch == 'A') {
				QString str = TStringToQString(tag->album());
				if (str.isEmpty())
					str = "<Unknown album>";
				res += str;
			} else if (ch == 'c') {
				QString str = TStringToQString(tag->comment());
				if (str.isEmpty())
					str = "<Empty comment>";
				res += str;
			} else if (ch == 'g') {
				QString str = TStringToQString(tag->genre());
				if (str.isEmpty())
					str = "<Unknown genre>";
				res += str;
			} else if (ch == 'y') {
				QString str = QString::number(tag->year());
				if (str == "0")
					str = "<Unknown year>";
				res += str;
			} else if (ch == 'n') {
				QString str = QString::number(tag->track());
				if (str == "0")
					str = "<Unknown track number>";
				res += str;
			} else if (ch == 'd') {
				QString duration;
				if (seconds_total > 0) {
					int seconds = seconds_total % 60;
					int minutes = (seconds_total - seconds) / 60;
					int hours = minutes / 60;
					minutes = minutes % 60;
					if (hours > 0)
						duration.sprintf("%d:%02d:%02d", hours, minutes, seconds);
					else
						duration.sprintf("%d:%02d", minutes, seconds);
				} else {
					duration = "<Unknown duration>";
				}
				res += duration;
			} else if (ch == 'D') {
				QString duration;
				if (seconds_total == 0)
					duration = "<Unknown duration>";
				else
					duration = QString::number(seconds_total);
				res += duration;
			} else if (ch == 'B') {
				QString str = QString::number(prop->bitrate() / 1000);
				if (str == "0")
					str = "<Unknown bitrate>";
				res += str;
			} else if (ch == 's') {
				QString str = QString::number(prop->sampleRate());
				if (str == "0")
					str = "<Unknown sample rate>";
				res += str;
			} else if (ch == 'C') {
				QString str = QString::number(prop->channels());
				if (str == "0")
					str = "<Unknown channels number>";
				res += str;
			} else if (ch == 'f') {
				res += QFileInfo(d->m_path).baseName();
			} else if (ch == 'F') {
				res += QFileInfo(d->m_path).fileName();
			} else if (ch == 'p') {
				res += QFileInfo(d->m_path).absoluteFilePath();
			} else if (ch == 'v') {
				res += QCoreApplication::applicationVersion();
			} else {
				res += ch;
			}
		} else {
			res += format.at(i);
		}
	}

	return res;
}

/* vim: set ts=4 sw=4: */
