#include "EditPane.h"
#include "EditPaneTabs.h"
#include "FScintilla.h"
#include "TabContext.h"
#include "LexerPicker.h"
#include "FileFilters.h"

#include <QVBoxLayout>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QScrollBar>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QMimeData>
// #include <QDebug>

#include <Qsci/qscilexer.h>

namespace fedup {

int EditPane::_nextNewIndex = 1;

EditPane::EditPane(QWidget *parent) : QWidget(parent), _tabs(NULL)
{
	setAcceptDrops(true);

	QVBoxLayout * const vbox = new QVBoxLayout(this);
	vbox->setSpacing(0);
	vbox->setContentsMargins(0, 0, 0, 0);
	vbox->addWidget(_tabs = new EditPaneTabs);
	vbox->addWidget(_editor = new FScintilla);

	connect(_tabs, &EditPaneTabs::tabCloseRequested, this, &EditPane::_slot_TabCloseRequested);
	connect(_tabs, &EditPaneTabs::tabCloseAllButRequested, this, &EditPane::_slot_TabCloseAllButRequested);
	connect(_tabs, &EditPaneTabs::tabRemoved, this, &EditPane::_slot_TabRemoved);
	connect(_tabs, &EditPaneTabs::tabChanged, this, &EditPane::_slot_TabChanged);
	connect(_editor, &FScintilla::modificationChanged, _tabs, &EditPaneTabs::slot_ModificationChanged);

	_editor->setVisible(false);
}

EditPane::~EditPane()
{
	for (FilePathToContextMap::iterator it = _openFiles.begin(); it != _openFiles.end(); ++it)
	{
		delete it.value();
	}
}

void EditPane::loadSession(QSettings &settings)
{
	const int recentArraySize = settings.beginReadArray("open");
	for (int i = 0; i < recentArraySize; i++)
	{
		settings.setArrayIndex(i);
		const QString filePath = settings.value("filepath").toString();
		const OpenResult openResult = open(filePath);
		if (openResult == OpenSucceeded || openResult == OpenAlreadyOpen)
		{
			_editor->verticalScrollBar()->setValue(settings.value("vertical_scrollbar_pos", 0).toInt());
			_editor->horizontalScrollBar()->setValue(settings.value("horizontal_scrollbar_pos", 0).toInt());
			TabContext * const context = _tabs->tabContext(_tabs->currentIndex());
			context->save(_editor); // HACK to make it store the new scroll bar positions
		}
	}
	settings.endArray();
}

void EditPane::saveSession(QSettings &settings) const
{
	if (_tabs->currentIndex() != -1)
	{
		TabContext * const context = _tabs->tabContext(_tabs->currentIndex());
		context->save(_editor); // HACK to make it store the new scroll bar positions
	}
	settings.beginWriteArray("open");
	int j = 0;
	for (int i = 0; i < _tabs->count(); i++)
	{
		TabContext * const context = _tabs->tabContext(i);
		if (context->filePath.size() == 0)
			continue;
		const QString absoluteFilePath = QFileInfo(context->filePath).absoluteFilePath();
		settings.setArrayIndex(j);
		settings.setValue("filepath", absoluteFilePath);
		settings.setValue("vertical_scrollbar_pos", context->verticalScrollBarPosition);
		settings.setValue("horizontal_scrollbar_pos", context->horizontalScrollBarPosition);
		j++;
	}
	settings.endArray();
}

void EditPane::openNew()
{
	const int newIndex = _tabs->addTab("new " + QString::number(_nextNewIndex++), new TabContext);
	_tabs->setCurrentIndex(newIndex);
}

OpenResult EditPane::open(const QString &filePath)
{
	const QFileInfo info(filePath);
	const QString absoluteFilePath = info.absoluteFilePath();
	FilePathToContextMap::const_iterator it = _openFiles.find(absoluteFilePath);
	if (it != _openFiles.end())
	{
		int i = 0;
		for (; i < _tabs->count(); i++)
		{
			if (_tabs->tabContext(i) == *it)
				break;
		}
		_tabs->setCurrentIndex(i);
		return OpenAlreadyOpen;
	}
	else
	{
		TabContext * const context = new TabContext;
		context->filePath = absoluteFilePath;
		_openFiles.insert(absoluteFilePath, context);
		const QString filename = info.fileName();
		const int newIndex = _tabs->addTab(filename, context);
		_tabs->setCurrentIndex(newIndex);
		QFile file(absoluteFilePath);
		if (!file.open(QIODevice::ReadOnly))
		{
			_tabs->removeTab(newIndex);
			if (!file.exists())
				return OpenDoesntExist;
			else
				return OpenAccessDenied;
		}
		const bool succeeded = _editor->read(&file);
		if (!succeeded)
		{
			_tabs->removeTab(newIndex);
			return OpenReadError;
		}
		_editor->setModified(false); // TODO figure out why this makes it not modified, but then makes the undo option available unto a context switch :-/
		_editor->setDocument(_editor->document()); // HACK to correct the above glitch
		if (!absoluteFilePath.isEmpty())
			_editor->setLexer(LexerPicker::chooseLexer(filename));
		return OpenSucceeded;
	}
}

SaveResult EditPane::saveAs(const QString &filePath)
{
	QFileInfo info(filePath);
	const QString absoluteFilePath = info.absoluteFilePath();

	if (_tabs->count() == 0)
		return SaveNothingToSave;
	TabContext * const context = _tabs->tabContext(_tabs->currentIndex());

	QFile file(absoluteFilePath);
	if (!file.open(QIODevice::WriteOnly))
	{
		if (!info.dir().exists())
			return SaveDirectoryDoesntExist;
		else
			return SaveAccessDenied;
	}
	if (!_editor->write(&file))
		return SaveWriteError;
	const QString oldFilePath = context->filePath;
	if (absoluteFilePath != context->filePath)
	{
		// TODO emit a signal?
		_openFiles.remove(context->filePath, context);
		context->filePath = absoluteFilePath;
		_openFiles.insert(absoluteFilePath, context);
		_tabs->setTabText(_tabs->currentIndex(), info.fileName());
	}
	_editor->setModified(false);
	const QString filename = info.fileName();
	if (!absoluteFilePath.isEmpty())
	{
		QFileInfo oldInfo(oldFilePath);
		if (oldInfo.suffix().compare(info.suffix(), Qt::CaseInsensitive) != 0)
			_editor->setLexer(LexerPicker::chooseLexer(filename));
	}
	return SaveSucceeded;
}

void EditPane::closeTab()
{
	_tabs->removeTab(_tabs->currentIndex());
}

void EditPane::closeAll()
{
	for (int i = _tabs->count()-1; i >= 0; i--)
		_tabs->removeTab(i);
}

void EditPane::reload()
{
	if (_tabs->count() > 0)
	{
		TabContext * const context = _tabs->tabContext(_tabs->currentIndex());

		QFile file(context->filePath);
		if (!file.open(QIODevice::ReadOnly))
			return; // TODO informative error message

		// TODO possibly preserve the original contents?
		const bool succeeded = _editor->read(&file);
		if (!succeeded)
			return;
		_editor->setModified(false); // TODO figure out why this makes it not modified, but then makes the undo option available unto a context switch :-/
		_editor->setDocument(_editor->document()); // HACK to correct the above glitch
	}
}

void EditPane::_slot_TabCloseRequested(int index)
{
	_TryClosingTab(index);
}

void EditPane::_slot_TabCloseAllButRequested(int index)
{
	for (int i = _tabs->count()-1; i >= 0; i--)
	{
		if (i == index)
			continue;
		if (!_TryClosingTab(i))
			return;
	}
}

bool EditPane::_TryClosingTab(int index)
{
	// TODO consolidate the this code with MainWindow's, and also make it use _workingDirectory
	TabContext * const context = _tabs->tabContext(index);
	QScopedPointer<FScintilla> edit(new FScintilla);
	edit->setDocument(context->document);
	if (edit->isModified())
	{
		const QMessageBox::StandardButton result = QMessageBox::question(this, "Save?", "Save file \"" + ((context->filePath.size() > 0) ? context->filePath : _tabs->tabText(index)) + "\"", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
		if (result == QMessageBox::Cancel)
			return false;
		if (result == QMessageBox::Save)
		{
			QString filePath = context->filePath;
			if (filePath.size() == 0)
			{
				filePath = QFileDialog::getSaveFileName(this, "Save As...", QString(), FILE_FILTERS); // TODO we don't have access to _currentDirectory in this class
				if (filePath.size() == 0)
					return false;
				QFileInfo info(filePath);
				filePath = info.absoluteFilePath();
			}

			const int oldIndex = _tabs->currentIndex();
			_tabs->setCurrentIndex(index);
			const SaveResult saveResult = saveAs(filePath);
			_tabs->setCurrentIndex(oldIndex);
			switch (saveResult)
			{
				case SaveNothingToSave:
				// NOTE: this shouldn't happen normally unless saveAs is programatically called rather than from an action
				break;

				case SaveSucceeded:
				// _currentDirectory = info.absolutePath();
				break;

				case SaveAccessDenied:
				QMessageBox::warning(this, "Access denied", "Error opening \"" + filePath + "\" for writing");
				return false;

				case SaveDirectoryDoesntExist:
				QMessageBox::warning(this, "Directory doesn't exist", "Error opening \"" + filePath + "\" for writing");
				return false;

				case SaveWriteError:
				QMessageBox::warning(this, "Write error", "Error writing \"" + filePath + "\"");
				return false;
			}
		}
	}
	_tabs->removeTab(index);
	return true;
}

void EditPane::_slot_TabRemoved(TabContext *context)
{
	//qDebug() << ">> Tab removed:" << context;
	if (!context->filePath.isEmpty())
		_openFiles.remove(context->filePath, context);
	delete context;
}

void EditPane::_slot_TabChanged(TabContext *context, TabContext *oldContext)
{
	//qDebug() << ">> Tab changed from:" << oldContext << "to:" << context;
	if (oldContext)
		oldContext->save(_editor);
	else if (_editor->lexer())
	{
		QsciLexer * const lexer = _editor->lexer();
		_editor->setLexer(NULL);
		delete lexer;
	}
	if (context)
		context->restore(_editor);
	else
	{
		_editor->setFilePath(QString()); // TODO do _editor->setContext in either case
		_editor->setDocument(QsciDocument()); // TODO do _editor->setContext in either case
		QsciLexer * const lexer = _editor->lexer();
		_editor->setLexer(NULL);
		delete lexer;
	}
	_editor->setVisible(context);
}

void EditPane::dragEnterEvent(QDragEnterEvent *event)
{
	// qDebug() << event->mimeData()->formats();
	if (event->mimeData()->hasFormat("text/uri-list"))
		event->acceptProposedAction();
}

void EditPane::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasUrls())
	{
		const QList<QUrl> urlList = event->mimeData()->urls();

		for (QList<QUrl>::const_iterator it = urlList.begin(); it != urlList.end(); ++it)
		{
			const QString filePath = it->toLocalFile();
			const QFileInfo info(filePath);
			if (info.isFile())
				open(filePath);
		}
	}
	event->acceptProposedAction();
}

} // namespace fedup
