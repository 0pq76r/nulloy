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

#ifndef N_LABEL_H
#define N_LABEL_H

#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPoint>

class NLabel : public QLabel
{
	Q_OBJECT

	Q_PROPERTY(QPoint shadowOffset READ shadowOffset WRITE setShadowOffset DESIGNABLE true)
	Q_PROPERTY(int shadowOffsetX READ shadowOffsetX WRITE setShadowOffsetX DESIGNABLE true)
	Q_PROPERTY(int shadowOffsetY READ shadowOffsetY WRITE setShadowOffsetY DESIGNABLE true)

	Q_PROPERTY(QColor shadowColor READ shadowColor WRITE setShadowColor DESIGNABLE true)
	Q_PROPERTY(bool shadowEnabled READ shadowEnabled WRITE setShadowEnabled DESIGNABLE true)
	Q_PROPERTY(QString text READ text WRITE setText)

private:
	QPoint m_shadowOffset;
	QColor m_shadowColor;
	bool m_enabled;
	Qt::TextElideMode m_elideMode;
	QString m_elidedText;

	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);

	void updateElidedText();

public:
	NLabel(QWidget* parent = 0);

	Q_INVOKABLE void setText(const QString &text);
	void setElideMode(Qt::TextElideMode mode);

	bool shadowEnabled() const;
	void setShadowEnabled(bool enabled);

	QPoint shadowOffset() const;
	void setShadowOffset(const QPoint &offset);
	Q_INVOKABLE inline void setShadowOffset(int dx, int dy) { setShadowOffset(QPoint(dx, dy)); }
	inline void setShadowOffset(int d) { setShadowOffset(QPoint(d, d)); }

	inline int shadowOffsetX() const { return shadowOffset().x(); }
	inline int shadowOffsetY() const { return shadowOffset().y(); }
	inline void setShadowOffsetX(int dx) { setShadowOffset(QPoint(dx, shadowOffsetY())); }
	inline void setShadowOffsetY(int dy) { setShadowOffset(QPoint(shadowOffsetX(), dy)); }

	QColor shadowColor() const;
	void setShadowColor(QColor color);
	Q_INVOKABLE inline void setShadowColor(QString color) { setShadowColor(QColor(color)); }
};

#endif

