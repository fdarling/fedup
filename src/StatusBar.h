#ifndef FEDUP_STATUSBAR_H
#define FEDUP_STATUSBAR_H

#include <QStatusBar>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace fedup {

class StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    StatusBar(QWidget *parent = NULL);
    ~StatusBar();
public slots:
    void slot_SetLineCount(int lineCount);
    void slot_SetLength(int length);
    void slot_SetCursorPosition(int line, int index);
    void slot_SetSelectionLength(int length);
protected:
    void _RefreshLengthLabel();
    void _RefreshPositionLabel();

    QLabel *_lengthLabel;
    QLabel *_positionLabel;
    int _currentLine;
    int _currentColumn;
    int _totalLines;
    int _totalSize;
    int _selectionSize;
};

} // namespace fedup

#endif
