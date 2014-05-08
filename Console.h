#ifndef CONSOLE_H
#define CONSOLE_H

#include <QTextEdit>

class Console : public QTextEdit
{
public:
    Console(QWidget *parent = 0);
    virtual ~Console();

    void    writeInfo(QString const& message);
    void    writeDebug(QString const& message);
    void    writeSuccess(QString const& message);
    void    writeError(QString const& message);
    void    writeSeparator();

private:
    void    _appendMessage(QColor const& color, QString const& header, QString const& message);
};

#endif // CONSOLE_H
