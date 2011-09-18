#ifndef LEXERPICKER_H
#define LEXERPICKER_H

#include <QtGlobal>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class QsciLexer;

namespace fedup {

class LexerPicker
{
public:
	static QsciLexer * chooseLexer(const QString &filename);
};

} // namespace fedup

#endif
