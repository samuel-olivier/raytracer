#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QObject>
#include <QMutex>
#include <atomic>

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

private slots:
    void        onRefresh();

private:
    enum StatusBarAction {
        ClearMessage,
        Show,
        Status_None
    };

    enum ConsoleAction {
        Info,
        Debug,
        Success,
        Error,
        ClearConsole,
        Separator,
        Console_None
    };

    Logger();
    ~Logger();

    static Logger*  _instance;

    Console*    _console;
    QStatusBar* _statusBar;

    std::atomic<StatusBarAction>    _nextAction;
    QMutex                          _statusMessageMutex;
    QString                         _statusMessageString;
    int                             _statusMessageTimeout;

    QMutex                                  _consoleMutex;
    QList<QPair<ConsoleAction, QString>>    _consoleActions;
};

#endif // LOGGER_H
