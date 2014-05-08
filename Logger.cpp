#include "Logger.h"

#include <QStatusBar>

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
    if (_console) {
        _console->writeInfo(message);
    }
}

void Logger::writeDebug(const QString &message)
{
    if (_console) {
        _console->writeDebug(message);
    }
}

void Logger::writeSuccess(const QString &message)
{
    if (_console) {
        _console->writeSuccess(message);
    }
}

void Logger::writeError(const QString &message)
{
    if (_console) {
        _console->writeError(message);
    }
}

void Logger::clearConsole()
{
    if (_console) {
        _console->clear();
    }
}

void Logger::writeSeparator()
{
    if (_console) {
        _console->writeSeparator();
    }
}

void Logger::showMessage(const QString &message, int timeout)
{
    if (_statusBar) {
        _statusBar->showMessage(message, timeout);
    }
}

void Logger::clearMessage()
{
    if (_statusBar) {
        _statusBar->clearMessage();
    }
}

Logger::Logger()
    : _console(0), _statusBar(0)
{
}

Logger::~Logger()
{
}
