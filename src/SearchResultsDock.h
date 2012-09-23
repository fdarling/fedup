#ifndef FEDUP_SEARCHRESULTSDOCK_H
#define FEDUP_SEARCHRESULTSDOCK_H

#include <QDockWidget>

// QT_BEGIN_NAMESPACE
// QT_END_NAMESPACE

#include <Qsci/qsciscintilla.h>

namespace fedup {

class SearchResultsDock : public QDockWidget
{
	Q_OBJECT
public:
	SearchResultsDock(QWidget *parent = NULL);
	~SearchResultsDock();

	void clear();
signals:
	void requestOpenFileLine(const QString &filePath, int line);
public slots:
	void startSearch(const QString &term);
	void addResult(const QString &filePath, int line, const QString &result, int highlightStart, int highlightLength);
	void endSearch();
protected:
	bool eventFilter(QObject *obj, QEvent *event);
	void _FinishFile();
	QsciScintilla *_editor;
	QString _lastFilePath;
	int _filePathLine;
	int _currentLine;
	int _fileCount;
	int _hitCount;
	int _totalHits;
};

} // namespace fedup

#endif
