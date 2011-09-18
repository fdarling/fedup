#include "LexerPicker.h"

#include <QString>
#include <QFileInfo>

#include <Qsci/qscilexerverilog.h>
#include <Qsci/qscilexervhdl.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexerbatch.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexertcl.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexerxml.h>

#define FOREST_MACRO_SET_LEXER_FONT lexer->setFont(createFont());
#define FOREST_MACRO_CREATE_LEXER(lexerType) lexerType * const lexer = new lexerType;

namespace fedup {

static QFont createFont(bool bold = false)
{
	QFont f("Courier New", 10);
	if (bold)
		f.setWeight(QFont::Bold);
	f.setStyleStrategy(QFont::NoAntialias);
	f.setStyleHint(QFont::TypeWriter);
	return f;
}

static const QColor      KEYWORD_COLOR(  0,   0, 255);
static const QColor     OPERATOR_COLOR(  0,   0, 128);
static const QColor      COMMENT_COLOR(  0, 128,   0);
static const QColor PREPROCESSOR_COLOR(128,  64,   0);
static const QColor       NUMBER_COLOR(255, 128,   0);
static const QColor       STRING_COLOR(128, 128, 128);

static const QColor  SYSTEM_TASK_COLOR(128,   0, 255);

static const QColor   IDENTIFIER_COLOR = SYSTEM_TASK_COLOR;

static const QColor   PURPLE = SYSTEM_TASK_COLOR;

QsciLexer * LexerPicker::chooseLexer(const QString &filename)
{
	if (filename.size() == 0)
		return NULL;

	QFileInfo fileInfo(filename);
	const QString ext = fileInfo.suffix().toLower();

	if (ext == "v" || ext == "vh")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerVerilog)
		// lexer->setFoldPreprocessor(true); // default true?
		lexer->setFoldAtModule(true); // default false
		//lexer->setFoldCompact(true); // default true
		QFont f = createFont();
		lexer->setFont(f);
		f.setBold(true);
		lexer->setFont(f, QsciLexerVerilog::Keyword);
		lexer->setFont(f, QsciLexerVerilog::Operator);
		lexer->setColor(     KEYWORD_COLOR, QsciLexerVerilog::Keyword);
		lexer->setColor(    OPERATOR_COLOR, QsciLexerVerilog::Operator);
		lexer->setColor(     COMMENT_COLOR, QsciLexerVerilog::Comment);
		lexer->setColor(PREPROCESSOR_COLOR, QsciLexerVerilog::Preprocessor);
		lexer->setColor(      NUMBER_COLOR, QsciLexerVerilog::Number);
		lexer->setColor( SYSTEM_TASK_COLOR, QsciLexerVerilog::SystemTask);
		return lexer;
	}
	else if (ext == "vhdl")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerVHDL)
		FOREST_MACRO_SET_LEXER_FONT
		return lexer;
	}
	else if (ext == "cpp" || ext == "c" || ext == "cxx" || ext == "h" || ext == "hpp" || ext == "hxx")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerCPP)
		QFont f = createFont();
		QFont f2 = createFont(true);
		lexer->setFont(f);
		lexer->setFont(f2, QsciLexerCPP::Keyword);
		lexer->setFont(f2, QsciLexerCPP::Operator);
		// f.setBold(true);
		// lexer->setFont(f, QsciLexerCPP::Keyword);
		// lexer->setFont(f, QsciLexerCPP::Operator);
		// lexer->setColor(     KEYWORD_COLOR, QsciLexerCPP::Keyword);
		lexer->setColor(    OPERATOR_COLOR, QsciLexerCPP::Operator);
		lexer->setColor(     COMMENT_COLOR, QsciLexerCPP::Comment);
		lexer->setColor(PREPROCESSOR_COLOR, QsciLexerCPP::PreProcessor);
		lexer->setColor(      NUMBER_COLOR, QsciLexerCPP::Number);
		lexer->setColor(            PURPLE, QsciLexerCPP::Keyword);
		lexer->setColor(     KEYWORD_COLOR, QsciLexerCPP::Keyword);
		lexer->setColor(      STRING_COLOR, QsciLexerCPP::DoubleQuotedString);
		return lexer;
	}
	else if (ext == "sh")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerBash)
		FOREST_MACRO_SET_LEXER_FONT
		return lexer;
	}
	else if (ext == "bat")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerBatch)
		FOREST_MACRO_SET_LEXER_FONT
		return lexer;
	}
	else if (ext == "css")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerCSS)
		FOREST_MACRO_SET_LEXER_FONT
		return lexer;
	}
	else if (ext == "html" || ext == "htm" || ext == "php")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerHTML)
		FOREST_MACRO_SET_LEXER_FONT
		return lexer;
	}
	else if (ext == "pl")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerPerl)
		FOREST_MACRO_SET_LEXER_FONT
		return lexer;
	}
	else if (ext == "py")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerPython)
		FOREST_MACRO_SET_LEXER_FONT
		return lexer;
	}
	else if (ext == "sql")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerSQL)
		FOREST_MACRO_SET_LEXER_FONT
		return lexer;
	}
	else if (ext == "tcl")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerTCL)
		FOREST_MACRO_SET_LEXER_FONT
		return lexer;
	}
	else if (ext == "mak" || fileInfo.fileName().toLower() == "makefile")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerMakefile)
		FOREST_MACRO_SET_LEXER_FONT
		return lexer;
	}
	else if (ext == "xml")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerXML)
		FOREST_MACRO_SET_LEXER_FONT
		return lexer;
	}
	else if (ext == "js")
	{
		FOREST_MACRO_CREATE_LEXER(QsciLexerJavaScript)
		FOREST_MACRO_SET_LEXER_FONT
		return lexer;
	}

	return NULL;
}

} // namespace fedup

#undef FOREST_MACRO_CREATE_LEXER
#undef FOREST_MACRO_SET_LEXER_FONT
