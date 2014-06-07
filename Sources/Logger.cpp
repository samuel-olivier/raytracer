#include "Logger.h"

#include <QStatusBar>
#include <QTimer>
#include <QDebug>

#include "Console.h"

Logger* Logger::_instance = 0;

Logger *Logger::instance()
{
    if (!_instance) {
        _instance = new Logger;
    }
    return _instance;
}

Console *Logger::console() const
{
    return _console;
}

void Logger::setConsole(Console *console)
{
    _console = console;
}

QStatusBar *Logger::statusBar() const
{
    return _statusBar;
}

void Logger::setStatusBar(QStatusBar *statusBar)
{
    _statusBar = statusBar;
}

void Logger::writeInfo(const QString &message)
{
    QMutexLocker m(&_consoleMutex);
    _consoleActions.append(QPair<ConsoleAction, QString>(Info, message));
}

void Logger::writeDebug(const QString &message)
{
    QMutexLocker m(&_consoleMutex);
    _consoleActions.append(QPair<ConsoleAction, QString>(Debug, message));
}

void Logger::writeSuccess(const QString &message)
{
    QMutexLocker m(&_consoleMutex);
    _consoleActions.append(QPair<ConsoleAction, QString>(Success, message));
}

void Logger::writeError(const QString &message)
{
    QMutexLocker m(&_consoleMutex);
    _consoleActions.append(QPair<ConsoleAction, QString>(Error, message));
}

void Logger::clearConsole()
{
    QMutexLocker m(&_consoleMutex);
    _consoleActions.append(QPair<ConsoleAction, QString>(ClearConsole, ""));
}

void Logger::writeSeparator()
{
    QMutexLocker m(&_consoleMutex);
    _consoleActions.append(QPair<ConsoleAction, QString>(Separator, ""));
}

void Logger::showMessage(const QString &message, int timeout)
{
    QMutexLocker m(&_statusMessageMutex);
    _nextAction = Show;
    _statusMessageString = message;
    _statusMessageTimeout = timeout;
}

void Logger::clearMessage()
{
    _nextAction = ClearMessage;
}

void Logger::onRefresh()
{
    if (_statusBar && _nextAction != Status_None) {
        QMutexLocker m(&_statusMessageMutex);
        if (_nextAction == Show) {
            _statusBar->showMessage(_statusMessageString, _statusMessageTimeout);
        } else {
            _statusBar->clearMessage();
        }
        _nextAction = Status_None;
    }
    if (_console) {
        QMutexLocker m(&_consoleMutex);

        for (QPair<ConsoleAction, QString> const& current : _consoleActions) {
            if (current.first == Info) {
                _console->writeInfo(current.second);
            } else if (current.first == Debug) {
                _console->writeDebug(current.second);
            } else if (current.first == Success) {
                _console->writeSuccess(current.second);
            } else if (current.first == Error) {
                _console->writeError(current.second);
            } else if (current.first == ClearConsole) {
                _console->clear();
            } else if (current.first == Separator) {
                _console->writeSeparator();
            }
        }
        _consoleActions.clear();
    }
}

Logger::Logger()
    : _console(0), _statusBar(0), _nextAction(Status_None)
{
    QTimer* refreshTimer = new QTimer(this);
    connect(refreshTimer, SIGNAL(timeout()), SLOT(onRefresh()));
    refreshTimer->start(60);
}

Logger::~Logger()
{
}
