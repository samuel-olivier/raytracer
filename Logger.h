#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QObject>

#define logger Logger::instance()

class Console;
class QStatusBar;

class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger*  instance();

    Console*    console() const;
    void        setConsole(Console* console);

    QStatusBar* statusBar() const;
    void        setStatusBar(QStatusBar* statusBar);

public slots:
    void        writeInfo(QString const& message);
    void        writeDebug(QString const& message);
    void        writeSuccess(QString const& message);
    void        writeError(QString const& message);
    void        clearConsole();
    void        writeSeparator();

    void        showMessage(QString const& message, int timeout = 0);
    void        clearMessage();

private:
    Logger();
    ~Logger();

    static Logger*  _instance;

    Console*    _console;
    QStatusBar* _statusBar;
};

#endif // LOGGER_H
