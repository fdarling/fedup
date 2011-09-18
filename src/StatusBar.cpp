#include "StatusBar.h"
#include <QLabel>

#include <QTextStream>

namespace fedup {

StatusBar::StatusBar(QWidget *parent) : QStatusBar(parent),
	_lengthLabel(NULL), _positionLabel(NULL),
	_currentLine(0), _currentColumn(0),
	_totalLines(0), _totalSize(0),
	_selectionSize(0)
{
	addPermanentWidget(_lengthLabel = new QLabel(this));
	addPermanentWidget(_positionLabel = new QLabel(this));

	_RefreshLengthLabel();
	_RefreshPositionLabel();
}

StatusBar::~StatusBar()
{
}

void StatusBar::slot_SetLineCount(int lineCount)
{
	_totalLines = lineCount;
	_RefreshLengthLabel();
}

void StatusBar::slot_SetLength(int length)
{
	_totalSize = length;
	_RefreshLengthLabel();
}

void StatusBar::slot_SetCursorPosition(int line, int index)
{
	_currentLine = line + 1;
	_currentColumn = index + 1;
	_RefreshPositionLabel();
}

void StatusBar::slot_SetSelectionLength(int length)
{
	_selectionSize = length;
	_RefreshPositionLabel();
}

void StatusBar::_RefreshLengthLabel()
{
	QString buffer;
	buffer.reserve(40);
	QTextStream stream(&buffer);
	stream << "length : " << _totalSize << "   lines : " << _totalLines << '\t';
	_lengthLabel->setText(buffer);
}

void StatusBar::_RefreshPositionLabel()
{
	QString buffer;
	buffer.reserve(40);
	QTextStream stream(&buffer);
	stream << "Ln : " << _currentLine << "   Col : " << _currentColumn << "   Sel : " << _selectionSize << '\t';
	_positionLabel->setText(buffer);
}

} // namespace fedup
