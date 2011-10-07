#include "EditPane.h"
#include "EditPaneTabs.h"
#include "FScintilla.h"
#include "TabContext.h"
#include "LexerPicker.h"

#include <QVBoxLayout>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QSettings>
//#include <QDebug>

#include <Qsci/qscilexer.h>

namespace fedup {

int EditPane::_nextNewIndex = 1;

EditPane::EditPane(QWidget *parent) : QWidget(parent), _tabs(NULL)
{
	QVBoxLayout * const vbox = new QVBoxLayout(this);
	vbox->setSpacing(0);
	vbox->setContentsMargins(0, 0, 0, 0);
	vbox->addWidget(_tabs = new EditPaneTabs(this));
	vbox->addWidget(_editor = new FScintilla(this));

	connect(_tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(_slot_TabCloseRequested(int)));
	connect(_tabs, SIGNAL(tabRemoved(TabContext*)), this, SLOT(_slot_TabRemoved(TabContext*)));
	connect(_tabs, SIGNAL(tabChanged(TabContext*, TabContext*)), this, SLOT(_slot_TabChanged(TabContext*, TabContext*)));
	connect(_editor, SIGNAL(modificationChanged(bool)), _tabs, SLOT(slot_ModificationChanged(bool)));

	_editor->setVisible(false);
}

EditPane::~EditPane()
{
}

void EditPane::loadSession(QSettings &settings)
{
	const int recentArraySize = settings.beginReadArray("open");
	for (int i = 0; i < recentArraySize; i++)
	{
		settings.setArrayIndex(i);
		const QString filePath = settings.value("filepath").toString();
		open(filePath);
	}
	settings.endArray();
}

void EditPane::saveSession(QSettings &settings) const
{
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
		if (absoluteFilePath.size() != 0)
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
	if (absoluteFilePath != context->filePath)
	{
		// TODO emit a signal?
		_openFiles.remove(context->filePath, context);
		context->filePath = absoluteFilePath;
		_openFiles.insert(absoluteFilePath, context);
		_tabs->setTabText(_tabs->currentIndex(), info.fileName());
	}
	_editor->setModified(false);
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

void EditPane::_slot_TabCloseRequested(int index)
{
	_tabs->removeTab(index);
}

void EditPane::_slot_TabRemoved(TabContext *context)
{
	//qDebug() << ">> Tab removed:" << context;
	if (context->filePath.size() != 0)
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
		_editor->setDocument(QsciDocument()); // TODO do _editor->setContext in either case
		QsciLexer * const lexer = _editor->lexer();
		_editor->setLexer(NULL);
		delete lexer;
	}
	_editor->setVisible(context);
}

} // namespace fedup
