#include "MainWindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QTimer>

#include "ui_MainWindow.h"

#include "Config.h"
#include "SceneGenerator.h"
#include "Logger.h"
#include "Camera.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), _ui(new Ui::MainWindow), _nextScene(QString::null)
{
    _ui->setupUi(this);

    Camera* camera = new Camera;
    camera->set(config->defaultCameraVerticalFOV(), config->defaultCameraAspectRatio());
    camera->setAperture(config->defaultCameraAperture());
    camera->setFocalPlane(config->defaultCameraFocusPlane());

    _ui->renderer->setCamera(camera);
    _setConfig();
    _ui->sceneNames->addItems(sceneGenerator->scenes());
    _ui->sceneNames->setCurrentIndex(0);
    logger->setConsole(_ui->console);
    logger->setStatusBar(_ui->statusBar);

    onLoadScene();

    connect(_ui->action_Save, SIGNAL(triggered()), SLOT(onSaveImage()));
    connect(_ui->action_Open, SIGNAL(triggered()), SLOT(onLoadFile()));
    connect(_ui->action_Play_Pause, SIGNAL(triggered()), SLOT(onPlayPause()));
    connect(_ui->action_Stop, SIGNAL(triggered()), SLOT(onStop()));
    connect(_ui->action_Resize, SIGNAL(triggered()), SLOT(onResizeImage()));

    connect(_ui->sceneNames, SIGNAL(currentIndexChanged(QString)), SLOT(onLoadScene()));

    connect(_ui->renderer, SIGNAL(renderingFinished()), SLOT(onRenderingFinished()));
    connect(_ui->renderer, SIGNAL(renderingStarted()), SLOT(onRenderingStarted()));
    connect(_ui->renderer, SIGNAL(clicked(Intersection)), SLOT(onClick(Intersection)));

    QTimer* progressTimer = new QTimer(this);
    connect(progressTimer, SIGNAL(timeout()), SLOT(onUpdateProgress()));
    progressTimer->start(35);
}

MainWindow::~MainWindow()
{
    delete _ui;
}

void MainWindow::onSaveImage()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Image", config->rootDir(), "Image Files (*.bmp *.jpg *.png);;All Files (*.*)");

    if (!filename.isEmpty()) {
        _ui->renderer->saveImage(filename);
    }
}

void MainWindow::onLoadFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Scene", config->rootDir(), "Scene Files (*.obj *.3ds);;All Files (*.*)");

    if (!filename.isEmpty()) {
        sceneGenerator->loadFile(filename, _ui->renderer);
    }
}

void MainWindow::onPlayPause()
{
    if (!_ui->renderer->isRendering()) {
        _loadConfig();
        _ui->renderer->startRendering();
    }
}

void MainWindow::onStop()
{
    _ui->renderer->stopRendering();
    _ui->action_Play_Pause->setText("Play");
}

void MainWindow::onResizeImage()
{
    QSize current = _ui->renderer->imageSize();
    bool ok;
    int w = QInputDialog::getInt(this, "Image width", "Enter the new image width", current.width(), 1, 1e5, 1, &ok);
    if (ok) {
        int h = QInputDialog::getInt(this, "Image height", "Enter the new image height", current.height(), 1, 1e5, 1, &ok);
        if (ok) {
            _ui->renderer->stopRendering();
            _ui->renderer->setImageSize(w, h);
        }
    }
}

void MainWindow::onLoadScene()
{
    onStop();
    if (_ui->renderer->isRendering()) {
        _nextScene = _ui->sceneNames->currentText();
    } else {
        _loadScene(_ui->sceneNames->currentText());
        onPlayPause();
    }
}

void MainWindow::onRenderingStarted()
{
    logger->showMessage("Rendering...");
}

void MainWindow::onRenderingFinished()
{
    logger->writeSuccess(QString("Rendering finished : %1 ms").arg(QString::number(_ui->renderer->elapsedTime())));
    logger->clearMessage();
    if (!_nextScene.isNull()) {
        _loadScene(_nextScene);
        onPlayPause();
    }
}

void MainWindow::onUpdateProgress()
{
    int elaps = _ui->renderer->elapsedTime();
    QTime elapsed(0, 0, 0, 0);
    QTime mean(0, 0, 0, 0);
    int sampleNumber = _ui->renderer->sampleNumber();

    elapsed = elapsed.addMSecs(elaps);
    _ui->elapsedTime->setText(elapsed.toString("mm:ss.zzz"));
    _ui->sampleNumber->setText(QString::number(sampleNumber));
    if (sampleNumber > 0) {
        mean = mean.addMSecs(elaps / sampleNumber);
    }
    _ui->sampleDuration->setText(mean.toString("mm:ss.zzz"));
}

void MainWindow::onClick(Intersection hit)
{
    _ui->focalPlane->setValue(hit.position.distanceToPoint(_ui->renderer->camera()->position()));
}

void MainWindow::_loadScene(const QString &scene)
{
    sceneGenerator->loadScene(scene, _ui->renderer);
    _nextScene = QString::null;
    _ui->epsilon->setValue(Config::Epsilon);
    _ui->verticalFov->setValue(_ui->renderer->camera()->verticalFOV());
    _ui->aspectRatio->setValue(_ui->renderer->camera()->aspectRatio());
    _ui->aperture->setValue(_ui->renderer->camera()->aperture());
    _ui->focalPlane->setValue(_ui->renderer->camera()->focalPlane());
}

void MainWindow::_loadConfig()
{
    config->setShadowEnabled(_ui->shadow->isChecked());
    config->setThreadNumber(_ui->threadNumber->value());
    config->setRenderingTask(_ui->taskNumber->value());
    config->setLightSampleNumber(_ui->lightSampleNumber->value());
    config->setAntialiasingResolution(_ui->antialiasingResolution->value());

    QString v = _ui->antialiasingType->currentText();
    Config::AntialiasingType type;
    if (v == "Uniform") {
        type = Config::Uniform;
    } else if (v == "Random") {
        type = Config::Random;
    } else if (v == "Jittered") {
        type = Config::Jittered;
    } else {
        type = Config::Shirley;
    }
    config->setAntialiasingType(type);

    config->setPathDepth(_ui->pathDepth->value());
    config->setPathSampleNumber(_ui->pathSampleNumber->value());
    config->setRefractionIndex(_ui->refractionIndex->value());

    Config::Epsilon = _ui->epsilon->value();
    _ui->renderer->camera()->set(_ui->verticalFov->value(), _ui->aspectRatio->value());
    _ui->renderer->camera()->setAperture(_ui->aperture->value());
    _ui->renderer->camera()->setFocalPlane(_ui->focalPlane->value());
}

void MainWindow::_setConfig()
{
    _ui->shadow->setChecked(config->shadowEnabled());
    _ui->threadNumber->setValue(config->threadNumber());
    _ui->taskNumber->setValue(config->renderingTaskNumber());
    _ui->lightSampleNumber->setValue(config->lightSampleNumber());
    _ui->antialiasingResolution->setValue(config->antialiasingResolution());

    if (_ui->antialiasingType->count() != 4) {
        _ui->antialiasingType->clear();
        _ui->antialiasingType->addItems(QStringList() << "Uniform" << "Random" << "Jittered" << "Shirley");

        QString v;
        Config::AntialiasingType type = config->antialiasingType();
        if (type == Config::Uniform) {
            v = "Uniform";
        } else if (type == Config::Random) {
            v = "Random";
        } else if (type == Config::Jittered) {
            v = "Jittered";
        } else {
            v = "Shirley";
        }
        _ui->antialiasingType->setCurrentText(v);
    }
    _ui->pathDepth->setValue(config->pathDepth());
    _ui->pathSampleNumber->setValue(config->pathSampleNumber());
    _ui->refractionIndex->setValue(config->refractionIndex());

    _ui->epsilon->setValue(Config::Epsilon);
    _ui->verticalFov->setValue(_ui->renderer->camera()->verticalFOV());
    _ui->aspectRatio->setValue(_ui->renderer->camera()->aspectRatio());
    _ui->aperture->setValue(_ui->renderer->camera()->aperture());
    _ui->focalPlane->setValue(_ui->renderer->camera()->focalPlane());
}
