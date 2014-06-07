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
#include "Integrator.h"

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
    connect(_ui->action_Play_Pause, SIGNAL(triggered()), SLOT(onPlayPause()));
    connect(_ui->action_Stop, SIGNAL(triggered()), SLOT(onStop()));

    connect(_ui->sceneNames, SIGNAL(currentIndexChanged(QString)), SLOT(onLoadScene()));
    connect(_ui->integrator, SIGNAL(currentTextChanged(QString)), SLOT(onIntegratorChanged()));

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
    QString filename = QFileDialog::getSaveFileName(this, "Save Image", config->outputDir(), "Image Files (*.bmp *.jpg *.png);;All Files (*.*)");

    if (!filename.isEmpty()) {
        _ui->renderer->saveImage(filename);
    }
}

void MainWindow::onPlayPause()
{
    if (!_ui->renderer->isRendering()) {
        _launchRenderer();
        _ui->action_Play_Pause->setToolTip("Pause");
        _ui->action_Play_Pause->setIcon(QIcon(":/Resources/pause.png"));
        logger->showMessage("Rendering...");
    } else if (_ui->renderer->isPaused()) {
        _ui->renderer->play();
        _ui->action_Play_Pause->setToolTip("Pause");
        _ui->action_Play_Pause->setIcon(QIcon(":/Resources/pause.png"));
        logger->showMessage("Rendering...");
    } else {
        _ui->renderer->pause();
        _ui->action_Play_Pause->setToolTip("Play");
        _ui->action_Play_Pause->setIcon(QIcon(":/Resources/play.png"));
        logger->showMessage("Rendering paused");
    }
    _ui->action_Stop->setEnabled(true);
}

void MainWindow::onStop()
{
    _ui->renderer->stopRendering();
    _ui->action_Play_Pause->setToolTip("Render");
    _ui->action_Play_Pause->setIcon(QIcon(":/Resources/play.png"));
    _ui->action_Stop->setEnabled(false);
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
    _ui->elapsedTime->setText(elapsed.toString("hh:mm:ss.zzz"));
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

void MainWindow::onIntegratorChanged()
{
    QString v = _ui->integrator->currentText();
    _ui->photonMapping->setVisible(false);
    _ui->pathtracing->setVisible(false);
    if (v == "Photon Mapping") {
        _ui->photonMapping->setVisible(true);
    } else {
        _ui->pathtracing->setVisible(true);
    }
}

void MainWindow::_loadScene(const QString &scene)
{
    sceneGenerator->loadScene(scene, _ui->renderer);
    _ui->renderer->setIntegrator(0);

    _nextScene = QString::null;
    _ui->epsilon->setValue(Config::Epsilon);
    _ui->verticalFov->setValue(_ui->renderer->camera()->verticalFOV());
    _ui->aspectRatio->setValue(_ui->renderer->camera()->aspectRatio());
    _ui->aperture->setValue(_ui->renderer->camera()->aperture());
    _ui->focalPlane->setValue(_ui->renderer->camera()->focalPlane());
    _ui->imageWidth->setValue(_ui->renderer->imageSize().width());
    _ui->imageHeight->setValue(_ui->renderer->imageSize().height());

    _ui->photonMappingPhotonNumber->setValue(config->photonMappingPhotonNumber());
    _ui->photonMappingNearestPhotons->setValue(config->photonMappingNumberNearestPhoton());
    _ui->photonMappingSearchRadius->setValue(config->photonMappingMaximumSearchRadius());
}

void MainWindow::_launchRenderer()
{
    _loadConfig();
    Integrator* integrator = _ui->renderer->integrator();
    if (!integrator || integrator->type() != config->integrator()) {
        _ui->renderer->setIntegrator(Integrator::instanciateIntegrator(config->integrator()));
    }
    _ui->renderer->render();
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

    config->setRefractionIndex(_ui->refractionIndex->value());

    Config::Epsilon = _ui->epsilon->value();
    _ui->renderer->camera()->set(_ui->verticalFov->value(), _ui->aspectRatio->value());
    _ui->renderer->camera()->setAperture(_ui->aperture->value());
    _ui->renderer->camera()->setFocalPlane(_ui->focalPlane->value());

    _ui->renderer->setImageSize(_ui->imageWidth->value(), _ui->imageHeight->value());

    v = _ui->integrator->currentText();
    Integrator::Type iType;
    if (v == "Photon Mapping") {
        iType = Integrator::PhotonMapping;
    } else {
        iType = Integrator::Pathtracing;
    }
    config->setIntegrator(iType);

    config->setPathtracingPathDepth(_ui->pathtracingPathDepth->value());

    config->setPhotonMappingMapDepth(_ui->photonMappingMapDepth->value());
    config->setPhotonMappingPhotonNumber(_ui->photonMappingPhotonNumber->value());
    config->setPhotonMappingRayDepth(_ui->photonMappingRayDepth->value());
    config->setPhotonMappingNumberNearestPhoton(_ui->photonMappingNearestPhotons->value());
    config->setPhotonMappingMaximumSearchRadius(_ui->photonMappingSearchRadius->value());
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
    }
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

    _ui->refractionIndex->setValue(config->refractionIndex());

    _ui->epsilon->setValue(Config::Epsilon);
    _ui->verticalFov->setValue(_ui->renderer->camera()->verticalFOV());
    _ui->aspectRatio->setValue(_ui->renderer->camera()->aspectRatio());
    _ui->aperture->setValue(_ui->renderer->camera()->aperture());
    _ui->focalPlane->setValue(_ui->renderer->camera()->focalPlane());

    _ui->imageWidth->setValue(_ui->renderer->imageSize().width());
    _ui->imageHeight->setValue(_ui->renderer->imageSize().height());

    if (_ui->integrator->count() != 2) {
        _ui->integrator->clear();
        _ui->integrator->addItems(QStringList() << "Pathtracing" << "Photon Mapping");
    }
    Integrator::Type iType = config->integrator();
    if (iType == Integrator::PhotonMapping) {
        v = "Photon Mapping";
    } else {
        v = "Pathtracing";
    }
    _ui->integrator->setCurrentText(v);
    onIntegratorChanged();

    _ui->pathtracingPathDepth->setValue(config->pathtracingPathDepth());

    _ui->photonMappingMapDepth->setValue(config->photonMappingMapDepth());
    _ui->photonMappingPhotonNumber->setValue(config->photonMappingPhotonNumber());
    _ui->photonMappingRayDepth->setValue(config->photonMappingRayDepth());
    _ui->photonMappingNearestPhotons->setValue(config->photonMappingNumberNearestPhoton());
    _ui->photonMappingSearchRadius->setValue(config->photonMappingMaximumSearchRadius());
}
