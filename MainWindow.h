#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void    onSaveImage();
    void    onLoadFile();
    void    onPlayPause();
    void    onStop();
    void    onResizeImage();
    void    onLoadScene();

    void    onRenderingStarted();
    void    onRenderingFinished();

    void    onUpdateProgress();

private:
    void    _loadScene(QString const& scene);

    void    _loadConfig();
    void    _setConfig();

    Ui::MainWindow* _ui;
    QString _nextScene;
};

#endif // MAINWINDOW_H
