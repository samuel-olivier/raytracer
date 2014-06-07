#include "Renderer.h"

#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QtMath>
#include <QMouseEvent>

#include <thread>
#include <algorithm>

#include "Camera.h"
#include "Scene.h"
#include "Intersection.h"
#include "Node.h"
#include "Config.h"
#include "Material.h"
#include "Logger.h"
#include "Integrator.h"

Renderer::Renderer(QWidget *parent)
    : QWidget(parent), _camera(0), _scene(0), _raytracerThread(0), _integrator(0)
{
    setImageSize(config->defaultImageWidth(), config->defaultImageHeight());
    QTimer* updator = new QTimer(this);
    connect(updator, SIGNAL(timeout()), SLOT(repaint()));
    connect(this, SIGNAL(renderingFinished()), SLOT(_onStop()));
    updator->start(1000/60);

    _stopThread = false;
    _isRendering = false;
    _isPaused = false;
}

Renderer::~Renderer()
{
    stopRendering();
}

void Renderer::setImageSize(int width, int height)
{
    _image = QImage(width, height, QImage::Format_RGB32);
    _image.fill(QColor(0, 0, 0));
}

QSize Renderer::imageSize()
{
    QSize res = _image.size();
    return res;
}

Camera *Renderer::camera() const
{
    return _camera;
}

void Renderer::setCamera(Camera *camera)
{
    _camera = camera;
}

Scene *Renderer::scene() const
{
    return _scene;
}

void Renderer::setScene(Scene *scene)
{
    _scene = scene;
}

Integrator *Renderer::integrator() const
{
    return _integrator;
}

void Renderer::setIntegrator(Integrator *integrator)
{
    _integrator = integrator;
}

void Renderer::render()
{
    if (!_camera || !_scene) {
        return ;
    }
    stopRendering();
    _image.fill(Qt::black);
    _stopThread = false;
    _isRendering = true;
    _isPaused = false;
    _raytracerThread = new std::thread(&Renderer::_computeImage, this);
}

void Renderer::play()
{
    if (_isRendering && _isPaused) {
        _pauseMutex.unlock();
        _isPaused = false;
    }
}

void Renderer::pause()
{
    if (_isRendering && !_isPaused) {
        _pauseMutex.lock();
        _isPaused = true;
    }
}

void Renderer::stopRendering()
{
    if (_isRendering) {
        _stopThread = true;
        if (_isPaused) {
            play();
        }
    }
}

bool Renderer::isRendering() const
{
    return _isRendering;
}

bool Renderer::isPaused() const
{
    return _isPaused;
}

void Renderer::saveImage(const QString &filename)
{
    _image.save(filename);
}

int Renderer::elapsedTime() const
{
    if (_isRendering && !_isPaused) {
        return _elapsedTime + _renderingTime.elapsed();
    }
    return _elapsedTime;
}

int Renderer::sampleNumber() const
{
    return _sampleNumber;
}

void Renderer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    float winRatio = float(width()) / height();
    float imgRatio = float(_image.width()) / _image.height();

    QRect rect(0, 0, 0, 0);
    if (winRatio > imgRatio) {
        int h = qMin(height(), _image.height());
        rect.setHeight(h);
        rect.setWidth(h * imgRatio);
    } else {
        int w = qMin(width(), _image.width());
        rect.setWidth(w);
        rect.setHeight(w / imgRatio);
    }
    rect.moveCenter(QPoint(width() / 2, height() / 2));
    painter.drawImage(rect, _image);
}

void Renderer::mouseReleaseEvent(QMouseEvent *e)
{
    if (!_integrator) {
        return ;
    }
    float winRatio = float(width()) / height();
    float imgRatio = float(_image.width()) / _image.height();

    QRect rect(0, 0, 0, 0);
    if (winRatio > imgRatio) {
        int h = qMin(height(), _image.height());
        rect.setHeight(h);
        rect.setWidth(h * imgRatio);
    } else {
        int w = qMin(width(), _image.width());
        rect.setWidth(w);
        rect.setHeight(w / imgRatio);
    }
    rect.moveCenter(QPoint(width() / 2, height() / 2));
    if (rect.contains(e->pos())) {
        Intersection    hit;
        Ray             ray;

        ray.depth = 0;
        _camera->notSampledRay(float(e->pos().x() - rect.left() + 0.5f) / rect.width(), -float(e->pos().y() - rect.top() + 0.5f) / rect.height(), ray);
        _integrator->compute(ray, hit);
        emit clicked(hit);
    }
}

void Renderer::_onStop()
{
    _raytracerThread->join();
    delete _raytracerThread;
    _raytracerThread = 0;
    _stopThread = false;
    _isRendering = false;
}

void Renderer::_computeImage()
{
    if (!_integrator) {
        return ;
    }
    int s = qSqrt(config->renderingTaskNumber());

    QSize imageSize = _image.size();
    QSize sectionSize = imageSize / s;

    sectionSize.setWidth(qMax(sectionSize.width(), 1));
    sectionSize.setHeight(qMax(sectionSize.height(), 1));

    QList<QRect> tasks;
    for (int x = 0; x < imageSize.width(); x += sectionSize.width()) {
        for (int y = 0; y < imageSize.height(); y += sectionSize.height()) {
            QRect newTask(QPoint(x, y), sectionSize);

            newTask.setRight(qMin(newTask.right(), imageSize.width() - 1));
            newTask.setBottom(qMin(newTask.bottom(), imageSize.height() - 1));
            tasks.append(newTask);
        }
    }

    _imageColors.resize(imageSize.width() * imageSize.height());
    _imageColors.fill(Color::BLACK);

    _elapsedTime = 0;
    _sampleNumber = 0;
    _renderingTime.restart();

    _integrator->preprocess(_scene, _camera);
//    if (config->usePhotonMapping()) {
//        QTime time;
//        time.restart();
//        logger->showMessage("Building Global Photon Map...");
//        _globalPhotonMap = new PhotonMap(_scene);
//        _globalPhotonMap->build(PhotonMap::Global);
//        logger->writeSuccess(QString("Global Photon Map built : %1 ms").arg(QString::number(time.elapsed())));
//        time.restart();
//        logger->showMessage("Building Caustic Photon Map...");
//        _causticPhotonMap = new PhotonMap(_scene);
//        _causticPhotonMap->build(PhotonMap::Caustic);
//        logger->clearMessage();
//        logger->writeSuccess(QString("Global Caustic Map built : %1 ms").arg(QString::number(time.elapsed())));
//    }

    _renderingTime.restart();

    emit renderingStarted();

    QTime saveTime;
    saveTime.restart();
    while (_stopThread == false) {
        _renderingTasks = tasks;

        std::random_shuffle(_renderingTasks.begin(), _renderingTasks.end());

        for (int i = 0; i < config->threadNumber(); ++i) {
            _computingThreads.append(new std::thread(&Renderer::_computeSections, this));
        }

        for (std::thread* computingThread : _computingThreads) {
            computingThread->join();
            delete computingThread;
        }
        _computingThreads.clear();
        _renderingTasks.clear();
        ++_sampleNumber;
        _elapsedTime += _renderingTime.elapsed();
        if (saveTime.elapsed() > 5000) {
            saveTime.restart();
            _image.save(config->outputDir() + "/last.bmp");
        }
        _pauseMutex.lock();
        _pauseMutex.unlock();
        _renderingTime.restart();
    }
    emit renderingFinished();

    _isRendering = false;
}

void Renderer::_computeSections()
{
    QSize imageSize = _image.size();
    Ray ray;

    ray.type = Ray::Primary;
    while (1) {
        _renderingTasksMutex.lock();
        if (_renderingTasks.isEmpty()) {
            _renderingTasksMutex.unlock();
            return ;
        }
        QRect task = _renderingTasks.takeFirst();
        _renderingTasksMutex.unlock();
        for (int x = 0; x < task.width(); ++x) {
            for (int y = 0; y < task.height(); ++y) {

                Color pixel = Color::BLACK;
                int resolution = config->antialiasingResolution();
                float subSize = 1.0f / resolution;
                Config::AntialiasingType type = config->antialiasingType();

                for (float x1 = 0.0f; x1 < 1.0f; x1 += subSize) {
                    for (float y1 = 0.0f; y1 < 1.0f; y1 += subSize) {
                        float vx = 0;
                        float vy = 0;

                        if (type == Config::Random) {
                            vx = float(qrand()) / RAND_MAX;
                            vy = float(qrand()) / RAND_MAX;
                        } else if (type == Config::Uniform) {
                            vx = x1 + subSize / 2.0f;
                            vy = y1 + subSize / 2.0f;
                        } else if (type == Config::Jittered) {
                            vx = x1 + subSize * float(qrand()) / RAND_MAX;
                            vy = y1 + subSize * float(qrand()) / RAND_MAX;
                        } else if (type == Config::Shirley) {
                            float vx1 = x1 + subSize * float(qrand()) / RAND_MAX;
                            float vy1 = y1 + subSize * float(qrand()) / RAND_MAX;
                            vx = (vx1 < 0.5) ? (-0.5f + qSqrt(2.0f * vx1)) : (1.5f - qSqrt(2.0f - 2.0f * vx1));
                            vy = (vy1 < 0.5) ? (-0.5f + qSqrt(2.0f * vy1)) : (1.5f - qSqrt(2.0f - 2.0f * vy1));
                        }
                        vx = (vx + x + task.left()) / imageSize.width();
                        vy = (vy + y + task.top()) / imageSize.height();

                        Intersection hit;

                        _camera->ray(vx, -vy, ray);
                        ray.time = float(qrand()) / RAND_MAX;
                        _integrator->compute(ray, hit);
                        pixel.AddScaled(hit.shade, 1.0f / (resolution * resolution));
                    }
                }
                _setPixel(x + task.left(), y + task.top(), pixel);
            }
        }
    }
}

//void Renderer::_pathTrace(const Ray &ray, Intersection &hit)
//{
//    hit.shade = Color::BLACK;
//    if (_scene->intersect(ray, hit)) {
//        if (hit.light != 0) {
//            if (ray.type != Ray::Diffused) {
//                hit.light->intersectionColor(hit.shade);
//            }
//            return ;
//        }
//        if (hit.material == 0) {
//            return ;
//        }
//        if (QVector3D::dotProduct(hit.normal, ray.direction) > 0) {
//            hit.normal *= -1;
//            hit.u *= -1;
//            hit.v *= -1;
//        }
//        hit.material->applyTransformation(hit);
//        for (Light* light : _scene->lights()) {
//            int sampleNumber = light->sampleNumber();
//            for (int i = 0; i < sampleNumber; ++i) {
//                Color color;
//                QVector3D toLight;
//                QVector3D lightPos;
//                float bright = light->illuminate(hit.position, color, toLight, lightPos) / sampleNumber;
//                float cosTheta = QVector3D::dotProduct(toLight, hit.normal);
//                if (cosTheta >= Config::Epsilon && bright >= Config::Epsilon) {
//                    if (!config->shadowEnabled() || !_isShaded(hit.position, toLight, lightPos, ray.time)) {
//                        Color materialColor = Color::BLACK;
//                        hit.material->computeReflectance(materialColor, toLight, ray, hit);
//                        materialColor.Scale(cosTheta);
//                        color.Multiply(materialColor);
//                        hit.shade.AddScaled(color, bright);
//                    }
//                }
//            }
//        }
//        if (ray.depth < config->pathDepth()) {
//            int sampleNumber = config->pathSampleNumber();
//            for (int s = 0; s < sampleNumber; ++s) {
//                Ray newRay;
//                Color c;

//                newRay.type = Ray::Reflected;
//                if (hit.material->sampleRay(ray, hit, newRay, c)) {
//                    newRay.depth = ray.depth + 1;
//                    newRay.time = ray.time;
//                    Intersection newHit;
//                    _pathTrace(newRay, newHit);
//                    newHit.shade.Multiply(c);
//                    newHit.shade.Scale(1.0f / sampleNumber);
//                    hit.shade.Add(newHit.shade);
//                }
//            }
//        }
//    } else {
//        _scene->sky()->evaluate(ray.direction, hit.shade);
//    }
//}

//void Renderer::_rayTrace(const Ray &ray, Intersection &hit, bool searchDiffuse)
//{
//    hit.shade = Color::BLACK;
//    if (_scene->intersect(ray, hit)) {
//        if (hit.light != 0 && searchDiffuse == false) {
//            hit.light->intersectionColor(hit.shade);
//            return ;
//        }
//        if (!hit.material) {
//            return ;
//        }
//        if (QVector3D::dotProduct(hit.normal, ray.direction) > 0) {
//            hit.normal *= -1;
//            hit.u *= -1;
//            hit.v *= -1;
//        }
//        hit.material->applyTransformation(hit);

//        Ray newRay;
//        Color c;

//        bool sample = hit.material->sampleRay(ray, hit, newRay, c);

//        if (ray.depth < config->pathDepth()) {
//            if (sample && (searchDiffuse == false || newRay.type != Ray::Diffused)) {
//                newRay.depth = ray.depth + 1;
//                Intersection newHit;
//                newRay.time = ray.time;
//                _rayTrace(newRay, newHit, searchDiffuse || newRay.type == Ray::Diffused);
//                newHit.shade.Multiply(c);
//                hit.shade.Add(newHit.shade);
//            }
//        }
//        if (searchDiffuse == false) {
//            for (Light* light : _scene->lights()) {
//                int sampleNumber = light->sampleNumber();
//                for (int i = 0; i < sampleNumber; ++i) {
//                    Color color;
//                    QVector3D toLight;
//                    QVector3D lightPos;
//                    float bright = light->illuminate(hit.position, color, toLight, lightPos) / sampleNumber;
//                    float cosTheta = QVector3D::dotProduct(toLight, hit.normal);
//                    if (cosTheta >= Config::Epsilon && bright >= Config::Epsilon) {
//                        if (!config->shadowEnabled() || !_isShaded(hit.position, toLight, lightPos, ray.time)) {
//                            Color materialColor = Color::WHITE;
//                            hit.material->computeReflectance(materialColor, toLight, ray, hit);
//                            materialColor.Scale(cosTheta);
//                            color.Multiply(materialColor);
//                            hit.shade.AddScaled(color, bright);
//                        }
//                    }
//                }
//            }
//            if (_causticPhotonMap && newRay.type == Ray::Diffused) {
//                Color causticColor;
//                _causticPhotonMap->computeColor(ray, hit, causticColor, config->numberNearestPhoton(), config->photonMaximumRadius());
//                hit.shade.Add(causticColor);
//            }
//        } else if (newRay.type == Ray::Diffused && _globalPhotonMap) {
//            Color globalColor;
//            _globalPhotonMap->computeColor(ray, hit, globalColor, config->numberNearestPhoton(), config->photonMaximumRadius());
//            hit.shade.Add(globalColor);
//        }

////        if (_globalPhotonMap) {
////            Color globalColor;
////            _globalPhotonMap->computeColor(ray, hit, globalColor, config->numberNearestPhoton(), config->photonMaximumRadius());
////            hit.shade.Add(globalColor);
////        }
////        if (_causticPhotonMap) {
////            Color causticColor;
////            _causticPhotonMap->computeColor(ray, hit, causticColor, config->numberNearestPhoton(), config->photonMaximumRadius());
////            hit.shade.Add(causticColor);
////        }
//    } else {
//        _scene->sky()->evaluate(ray.direction, hit.shade);
//    }
//}

//bool Renderer::_isShaded(QVector3D const& hitPosition, QVector3D const& toLight, QVector3D const& lightPos, float time) {
//    Intersection hit;
//    Ray shadowRay(hitPosition, toLight, Ray::Shadow);

//    shadowRay.time = time;
//    hit.hitDistance = hitPosition.distanceToPoint(lightPos);
//    return _scene->intersect(shadowRay, hit);
//}

void Renderer::_setPixel(int x, int y, const Color &color)
{
    int idx = x + y * _image.width();
    float f = 1.0f / (_sampleNumber + 1.0f);
    Color c = _imageColors[idx];
    c.Scale(1.0f - f);
    c.AddScaled(color, f);
    _imageColors[idx] = c;
    _image.setPixel(x, y, c.ToInt());
}
