#include "Console.h"

Console::Console(QWidget* parent)
    : QTextEdit(parent)
{
}

Console::~Console()
{
}

void Console::writeInfo(const QString &message)
{
    _appendMessage(QColor(0, 0, 0), "Info", message);
}

void Console::writeDebug(const QString &message)
{
    _appendMessage(QColor(0, 0, 255), "DEBUG", message);
}

void Console::writeSuccess(const QString &message)
{
    _appendMessage(QColor(14, 170, 0), "Success", message);
}

void Console::writeError(const QString &message)
{
    _appendMessage(QColor(222, 15, 0), "Error", message);
}

void Console::writeSeparator()
{
    moveCursor(QTextCursor::MoveOperation::End);
    insertHtml("<div>-----------------------------------<br></div>");
    QTextEdit::ensureCursorVisible();
}

void Console::_appendMessage(const QColor &color, const QString &header, const QString &message)
{
    moveCursor(QTextCursor::MoveOperation::End);
    insertHtml(QString("<div style=\"color: %1;\"><b>%2 : </b><span>%3</span><br></div>").arg(color.name(), header, message));
    QTextEdit::ensureCursorVisible();
}
