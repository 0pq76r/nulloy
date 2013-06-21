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

#include "playlistWidget.h"

#include "settings.h"
#include "core.h"
#include "trash.h"

#include <QtGui>
#include <QProcess>

#include <QDebug>

NPlaylistWidget::NPlaylistWidget(QWidget *parent) : QListWidget(parent)
{
	connect(this, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(on_itemActivated(QListWidgetItem *)));
	setItemDelegate(new NPlaylistItemDelegate(this));
	m_currentItem = NULL;

	QShortcut *revealShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);
	connect(revealShortcut, SIGNAL(activated()), this, SLOT(revealInFileManager()));
	QAction *revealAction = new QAction(QIcon::fromTheme("fileopen",
	                                    style()->standardIcon(QStyle::SP_DirOpenIcon)),
	                                    tr("Reveal in File Manager..."), this);
	revealAction->setShortcut(revealShortcut->key());
	connect(revealAction, SIGNAL(triggered()), this, SLOT(revealInFileManager()));


	QShortcut *removeShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	connect(removeShortcut, SIGNAL(activated()), this, SLOT(removeFromPlaylist()));
	QAction *removeAction = new QAction(QIcon::fromTheme("edit-clear",
	                                    style()->standardIcon(QStyle::SP_DialogResetButton)),
	                                    tr("Remove From Playlist"), this);
	removeAction->setShortcut(removeShortcut->key());
	connect(removeAction, SIGNAL(triggered()), this, SLOT(removeFromPlaylist()));


	QShortcut *trashShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Delete), this);
	connect(trashShortcut, SIGNAL(activated()), this, SLOT(moveToTrash()));
	QAction *trashAction = new QAction(QIcon::fromTheme("trashcan_empty",
	                                   style()->standardIcon(QStyle::SP_TrashIcon)),
	                                   tr("Move To Trash"), this);
	trashAction->setShortcut(trashShortcut->key());
	connect(trashAction, SIGNAL(triggered()), this, SLOT(moveToTrash()));


	m_contextMenu = new QMenu(this);
	m_contextMenu->addAction(revealAction);
	m_contextMenu->addAction(removeAction);
	m_contextMenu->addAction(trashAction);

	m_drag = NULL;
}

void NPlaylistWidget::setTagReader(NTagReaderInterface *tagReader)
{
	m_tagReader = tagReader;
}

void NPlaylistWidget::contextMenuEvent(QContextMenuEvent *event)
{
	if (selectedItems().size() != 0)
		m_contextMenu->exec(mapToGlobal(event->pos()));
	else
		QListWidget::contextMenuEvent(event);
}

void NPlaylistWidget::moveToTrash()
{
	foreach (QListWidgetItem *item, selectedItems()) {
		QString path = QFileInfo(item->data(NPlaylistItem::PathRole).toString()).canonicalFilePath();
		QString error;
		if (NTrash(path, &error) != 0) {
			QMessageBox box(QMessageBox::Warning, "File Delete Error", "", QMessageBox::Yes | QMessageBox::Cancel, this);
			box.setDefaultButton(QMessageBox::Cancel);
			box.setText("Failed to move to Trash \"" + path + "\"" + (error.isEmpty() ? "" : "\n" + error));
			box.setInformativeText("Do you want to delete permanently?");
			if (box.exec() == QMessageBox::Yes) {
				if (!QFile::remove(path)) {
					QMessageBox::critical(this, "File Delete Error", "Failed to delete \"" + path + "\"");
					return;
				}
			} else {
				return;
			}
		}

		QListWidgetItem *takenItem = takeItem(row(item));
		delete takenItem;
	}
	viewport()->update();
}

void NPlaylistWidget::removeFromPlaylist()
{
	foreach (QListWidgetItem *item, selectedItems()) {
		QListWidgetItem *takenItem = takeItem(row(item));
		delete takenItem;
	}
	viewport()->update();
}

void NPlaylistWidget::revealInFileManager()
{
	QFileInfo fileInfo(selectedItems().first()->data(NPlaylistItem::PathRole).toString());

	if (!fileInfo.exists()) {
		QMessageBox::warning(this, "File Manager Error", "File doesn't exist: " + selectedItems().first()->text());
		return;
	}

	QString fileManagerCommand;
	QStringList arguments;
	QString path = fileInfo.canonicalFilePath();
#if defined Q_WS_WIN
	fileManagerCommand = "explorer.exe";
	arguments << "/n,/select,";
	path = path.replace('/', '\\');
#elif defined Q_WS_X11
	QProcess xdg;
	xdg.start("xdg-mime query default inode/directory");
	xdg.waitForStarted();
	xdg.waitForFinished();

	fileManagerCommand = QString::fromUtf8(xdg.readAll()).simplified().remove(".desktop");
#elif defined Q_WS_MAC
	fileManagerCommand = "open";
	arguments << "-R";
#endif

	arguments << path;

	QProcess reveal;
	reveal.start(fileManagerCommand, arguments);
	reveal.waitForStarted();
	reveal.waitForFinished();
}

NPlaylistWidget::~NPlaylistWidget() {}

void NPlaylistWidget::setCurrentItem(NPlaylistItem *item)
{
	QString file = item->data(NPlaylistItem::PathRole).toString();
	QString fileName = QFileInfo(file).fileName();
	if (fileName.endsWith(".m3u") || fileName.endsWith(".m3u8")) {
		int index = row(item);
		int index_bkp = index;
		QList<NM3uItem> m3uItems = NM3uPlaylist::read(file);

		QListWidgetItem *takenItem = takeItem(row(item));
		delete takenItem;

		foreach (NM3uItem m3uItem, m3uItems) {
			NPlaylistItem *newItem = createItemFromM3uItem(m3uItem);
			insertItem(index, newItem);
			++index;
		}

		setCurrentItem(NPlaylistWidget::item(index_bkp));
		return;
	}

	// trying to read tags
	m_tagReader->setSource(file);
	if (m_tagReader->isValid()) {
		item->setText(m_tagReader->toString(NSettings::instance()->value("PlaylistTrackInfo").toString()));
		item->setData(NPlaylistItem::DurationRole, m_tagReader->toString("%D").toInt());
	} else {
		item->setText(fileName);
	}

	// reset failed role
	item->setData(NPlaylistItem::FailedRole, FALSE);

	// setting currently playing font to bold, colors set in delegate
	QFont f = item->font();
	if (m_currentItem) { // reset old item to defaults
		f.setBold(FALSE);
		m_currentItem->setFont(f);
	}
	f.setBold(TRUE);
	item->setFont(f);

	scrollToItem(item);
	m_currentItem = item;update();

	emit mediaSet(file);
}

void NPlaylistWidget::setCurrentFailed()
{
	m_currentItem->setData(NPlaylistItem::FailedRole, TRUE);
}

int NPlaylistWidget::currentRow()
{
	if (m_currentItem)
		return row(m_currentItem);
	else
		return -1;
}

QModelIndex NPlaylistWidget::currentIndex() const
{
	if (m_currentItem)
		return indexFromItem(m_currentItem);
	else
		return QModelIndex();
}

QString NPlaylistWidget::currentTitle()
{
	if (m_currentItem)
		return m_currentItem->text();
	else
		return "";
}

void NPlaylistWidget::setCurrentRow(int row)
{
	if (row < 0)
		return;

	if (row < count())
		setCurrentItem(item(row));
}

void NPlaylistWidget::activateRow(int row)
{
	if (row > -1)
		activateItem(item(row));
}

void NPlaylistWidget::activateItem(NPlaylistItem *item)
{
	if (count() > 0)
		emit itemActivated(item);
	else
		emit activateEmptyFail();
}

void NPlaylistWidget::on_itemActivated(QListWidgetItem *item)
{
	NPlaylistItem *item2 = dynamic_cast<NPlaylistItem *>(item);
	if (m_currentItem != item2)
		setCurrentItem(item2);
	emit currentActivated();
}

QStringList NPlaylistWidget::mediaList()
{
	QStringList list;
	for (int i = 0; i < count(); ++i)
		list << item(i)->data(NPlaylistItem::PathRole).toString();

	return list;
}

void NPlaylistWidget::appendMediaList(const QStringList &pathList)
{
	foreach (QString path, pathList)
		addItem(createItemFromPath(path));
}

void NPlaylistWidget::setMediaList(const QStringList &pathList)
{
	clear();
	m_currentItem = NULL;
	foreach (QString path, pathList)
		addItem(createItemFromPath(path));
}

void NPlaylistWidget::setMediaListFromPlaylist(const QString &file)
{
	clear();
	m_currentItem = NULL;

	QList<NM3uItem> m3uItems = NM3uPlaylist::read(file);
	for (int i = 0; i < m3uItems.count(); ++i) {
		addItem(createItemFromM3uItem(m3uItems.at(i)));
	}
}

void NPlaylistWidget::writePlaylist(const QString &file)
{
	QList<NM3uItem> m3uItems;
	for (int i = 0; i < count(); ++i) {
		NM3uItem m3uItem = {"", "", 0, 0};

		m3uItem.path =  item(i)->data(NPlaylistItem::PathRole).toString();
		m3uItem.title = item(i)->text();
		m3uItem.duration = item(i)->data(NPlaylistItem::DurationRole).toInt();

		if (!item(i)->data(NPlaylistItem::FailedRole).toBool())
			m3uItem.position = 0;
		else
			m3uItem.position = -1;

		m3uItems << m3uItem;
	}
	NM3uPlaylist::write(file, m3uItems);
}

void NPlaylistWidget::activateNext()
{
	int row = currentRow();
	if (row < count() - 1) {
		activateItem(item(row + 1));
	} else if (NSettings::instance()->value("LoadNext").toBool()) {
		QDir::SortFlag flag = (QDir::SortFlag)NSettings::instance()->value("LoadNextSort").toInt();
		QString file = m_currentItem->data(NPlaylistItem::PathRole).toString();
		QString path = QFileInfo(file).path();
		QStringList entryList = QDir(path).entryList(QDir::Files | QDir::NoDotAndDotDot, flag);
		int index = entryList.indexOf(QFileInfo(file).fileName());
		if (index != -1 && entryList.size() > index + 1) {
			addItem(createItemFromPath(path + "/" + entryList.at(index + 1)));
			activateItem(item(row + 1));
		}
	}
}

void NPlaylistWidget::activatePrev()
{
	int row = currentRow();
	if (row > 0)
		activateItem(item(row - 1));
}

void NPlaylistWidget::activateFirst()
{
	activateItem(item(0));
}

void NPlaylistWidget::activateCurrent()
{
	if (m_currentItem)
		activateItem(m_currentItem);
	else activateFirst();
}

void NPlaylistWidget::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
	Q_UNUSED(parent);
	for (int i = start; i < end + 1; ++i) {
		if (item(i) == m_currentItem) {
			m_currentItem = NULL;
			break;
		}
	}
}

NPlaylistItem* NPlaylistWidget::createItemFromPath(const QString &file)
{
	NPlaylistItem *item = new NPlaylistItem();
	item->setData(NPlaylistItem::PathRole, file);
	item->setText(QFileInfo(file).fileName());
	return item;
}

NPlaylistItem* NPlaylistWidget::createItemFromM3uItem(NM3uItem m3uItem)
{
	NPlaylistItem *item = new NPlaylistItem();
	item->setData(NPlaylistItem::PathRole, m3uItem.path);
	item->setText(m3uItem.title);
	if (m3uItem.position == -1)
		item->setData(NPlaylistItem::FailedRole, TRUE);
	return item;
}

NPlaylistItem* NPlaylistWidget::item(int row)
{
	return dynamic_cast<NPlaylistItem *>(QListWidget::item(row));
}


// DRAG & DROP >>

bool NPlaylistWidget::dropMimeData(int index, const QMimeData *data, Qt::DropAction action)
{
	Q_UNUSED(action);
	foreach (QUrl url, data->urls()) {
		foreach (QString file, NCore::dirListRecursive(url.toLocalFile())) {
			NPlaylistItem *item = createItemFromPath(file);
			insertItem(index, item);
			++index;
		}
	}

	m_drag = NULL;
	return TRUE;
}

QStringList NPlaylistWidget::mimeTypes() const
{
	QStringList qstrList;
	qstrList.append("text/uri-list");
	return qstrList;
}

#ifdef Q_WS_MAC
Qt::DropActions NPlaylistWidget::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}
#endif

QMimeData* NPlaylistWidget::mimeData(const QList<NPlaylistItem *> items) const
{
	QList<QUrl> urls;
	foreach (NPlaylistItem *item, items)
		urls << QUrl::fromLocalFile(item->data(NPlaylistItem::PathRole).toString());

	QPointer<QMimeData> data = new QMimeData();
	data->setUrls(urls);

	return data;
}

void NPlaylistWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return;

	if (!itemAt(event->pos())) {
		selectionModel()->clearSelection();
		return;
	}

	if (selectedItems().isEmpty())
		return;

	QList<QUrl> urls;
	urls << QUrl::fromLocalFile(currentItem()->data(NPlaylistItem::PathRole).toString());

	QMimeData *mimeData = new QMimeData;
	mimeData->setUrls(urls);
	m_mimeDataUrls.clear();

	m_drag = new QDrag(this);
	m_drag->setMimeData(mimeData);
	// restrct to move action
	m_drag->start(Qt::MoveAction);
}

void NPlaylistWidget::dropEvent(QDropEvent *event)
{
	if (m_drag) // moving withing playlist
		event->setDropAction(Qt::MoveAction);
	else // dropping from file manager
		event->setDropAction(Qt::CopyAction);

	QListWidget::dropEvent(event);
}

void NPlaylistWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if (m_drag && !m_mimeDataUrls.isEmpty())
		m_drag->mimeData()->setUrls(m_mimeDataUrls); // recover old data

	// change to move action
	event->setDropAction(Qt::MoveAction);
	QListWidget::dragEnterEvent(event);
}

void NPlaylistWidget::dragMoveEvent(QDragMoveEvent *event)
{
	// change to move action
	event->setDropAction(Qt::MoveAction);
	QListWidget::dragMoveEvent(event);
}

void NPlaylistWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
	if (m_drag) {
		m_mimeDataUrls = m_drag->mimeData()->urls(); // backup

		// forbid drag outside, set dummy mime data
		m_drag->mimeData()->clear();
	}
	event->ignore();
}

// << DRAG & DROP


// STYLESHEET PROPERTIES >>

void NPlaylistWidget::setCurrentTextColor(QColor color)
{
	m_currentTextColor = color;
}

QColor NPlaylistWidget::getCurrentTextColor() const
{
	return m_currentTextColor;
}

void NPlaylistWidget::setFailedTextColor(QColor color)
{
	m_failedTextColor = color;
}

QColor NPlaylistWidget::getFailedTextColor() const
{
	return m_failedTextColor;
}

// << STYLESHEET PROPERTIES

