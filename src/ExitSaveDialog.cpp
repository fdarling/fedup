#include "ExitSaveDialog.h"
#include "MainWindow.h"
#include "EditPane.h"
#include "EditPaneTabs.h"
#include "TabContext.h"
#include "FScintilla.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>

namespace fedup {

ExitSaveDialog::ExitSaveDialog(QWidget *parent) : QDialog(parent), _table(NULL), _mainWindow(NULL), _editPane(NULL)
{
	_table = new QTableWidget(this);
	{
		_table->setWordWrap(false);
		_table->setSortingEnabled(true);
		_table->setCornerButtonEnabled(false);
		_table->verticalHeader()->hide();
		_table->horizontalHeader()->setHighlightSections(false);
		// _table->horizontalHeader()->setStretchLastSection(true);
		// _table->setSelectionBehavior(QAbstractItemView::SelectRows);
		// _table->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
		_table->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
		_table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
		_table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
		_table->setSelectionMode(QAbstractItemView::ContiguousSelection);
		_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}
	{
		_table->setColumnCount(1);
		_table->horizontalHeader()->hide();
		/*QStringList labels;
		labels << "File Path";
		_table->setColumnCount(labels.size());
		_table->setHorizontalHeaderLabels(labels);*/
	}
	connect(_table, SIGNAL(activated(const QModelIndex &)), this, SLOT(_slot_RowActivated(const QModelIndex &)));

	setWindowTitle("Unsaved tabs");

	QHBoxLayout * const hbox = new QHBoxLayout(this);
	hbox->addWidget(_table);

	{
		QVBoxLayout * const vbox = new QVBoxLayout;
		QPushButton * const         saveButton = new QPushButton("Save");
		QPushButton * const         skipButton = new QPushButton("Skip");
		QPushButton * const    selectAllButton = new QPushButton("Select All");
		QPushButton * const  unselectAllButton = new QPushButton("Clear Selection");
		QPushButton * const saveSelectedButton = new QPushButton("Save Selected");
		QPushButton * const           okButton = new QPushButton("Exit Anyways");
		QPushButton * const       cancelButton = new QPushButton("Cancel");
		vbox->addWidget(saveButton);
		vbox->addWidget(skipButton);
		vbox->addWidget(selectAllButton);
		vbox->addWidget(unselectAllButton);
		vbox->addWidget(saveSelectedButton);
		vbox->addWidget(okButton);
		vbox->addWidget(cancelButton);

		connect(        saveButton, SIGNAL(clicked()), this, SLOT(_slot_Save()));
		connect(        skipButton, SIGNAL(clicked()), this, SLOT(_slot_Skip()));
		connect(   selectAllButton, SIGNAL(clicked()), this, SLOT(_slot_CheckAll()));
		connect( unselectAllButton, SIGNAL(clicked()), this, SLOT(_slot_UncheckAll()));
		connect(saveSelectedButton, SIGNAL(clicked()), this, SLOT(_slot_SaveSelected()));
		connect(          okButton, SIGNAL(clicked()), this, SLOT(accept()));
		connect(      cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

		hbox->addLayout(vbox);
	}
}

ExitSaveDialog::~ExitSaveDialog()
{
}

bool ExitSaveDialog::shouldClose(MainWindow *mainWindow, EditPane *editPane)
{
	{
		QScopedPointer<FScintilla> tempEditor(new FScintilla);
		_table->setRowCount(0);
		int rowIndex = 0;
		for (int i = 0; i < editPane->tabs()->count(); i++)
		{
			tempEditor->setDocument(editPane->tabs()->tabContext(i)->document);
			if (tempEditor->isModified())
			// if (1)
			{
				_table->setRowCount(rowIndex+1);
				QTableWidgetItem * const filePathItem = new QTableWidgetItem((editPane->tabs()->tabContext(i)->filePath.size() != 0) ? editPane->tabs()->tabContext(i)->filePath : editPane->tabs()->tabText(i));
				filePathItem->setData(Qt::UserRole, i);
				filePathItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
				filePathItem->setCheckState(Qt::Unchecked);
				// QTableWidgetItem * const checkboxItem = new QTableWidgetItem;
				_table->setItem(rowIndex, 0, filePathItem);
				// _table->setItem(rowIndex, 1, checkboxItem);
				rowIndex++;
			}
		}
		_table->setCurrentCell(0, 0);
		_table->resizeColumnsToContents();
		_table->resizeRowsToContents();
	}
	if (_table->rowCount() == 0)
		return true;
	_mainWindow = mainWindow;
	_editPane = editPane;
	const int result = exec();
	_mainWindow = NULL;
	_editPane = NULL;
	return result == Accepted;
}

void ExitSaveDialog::_slot_Save()
{
	const int nextRow = _table->currentRow()+1;
	const int tabIndex = _table->item(_table->currentRow(), 0)->data(Qt::UserRole).toInt();
	_editPane->tabs()->setCurrentIndex(tabIndex);
	if (!_mainWindow->save())
		return;
	if (nextRow >= _table->rowCount())
		accept();
	_table->setCurrentCell(_table->currentRow()+1, _table->currentColumn());
}

void ExitSaveDialog::_slot_Skip()
{
	const int nextRow = _table->currentRow()+1;
	if (nextRow >= _table->rowCount())
		accept();
	_table->setCurrentCell(_table->currentRow()+1, _table->currentColumn());
}

void ExitSaveDialog::_slot_CheckAll()
{
	for (int i = 0; i < _table->rowCount(); i++)
		_table->item(i, 0)->setCheckState(Qt::Checked);
}

void ExitSaveDialog::_slot_UncheckAll()
{
	for (int i = 0; i < _table->rowCount(); i++)
		_table->item(i, 0)->setCheckState(Qt::Unchecked);
}

void ExitSaveDialog::_slot_SaveSelected()
{
	for (int i = 0; i < _table->rowCount(); i++)
	{
		if (_table->item(i, 0)->checkState() == Qt::Checked)
		{
			const QString filePath = _table->item(i, 0)->text();
			const int tabIndex = _table->item(i, 0)->data(Qt::UserRole).toInt();
			_editPane->tabs()->setCurrentIndex(tabIndex);
			if (!_mainWindow->save())
				return;
		}
	}
	accept();
}

void ExitSaveDialog::_slot_RowActivated(const QModelIndex &index)
{
	if (!_editPane)
		return;
	const int tabIndex = _table->item(index.row(), 0)->data(Qt::UserRole).toInt();
	_editPane->tabs()->setCurrentIndex(tabIndex);
}

} // namespace fedup
