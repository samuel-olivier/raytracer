#include "Config.h"

Config*     Config::_instance = 0;
float       Config::Epsilon = 0.001f;

Config *Config::instance()
{
    if (!_instance) {
        _instance = new Config();
    }
    return _instance;
}

const QVector3D &Config::xAxis() const
{
    return _xAxis;
}

const QVector3D &Config::yAxis() const
{
    return _yAxis;
}

const QVector3D &Config::zAxis() const
{
    return _zAxis;
}

const QString &Config::rootDir() const
{
    return _rootDir;
}

bool Config::shadowEnabled() const
{
    return _shadowEnabled;
}

void Config::setShadowEnabled(bool shadowEnabled)
{
    _shadowEnabled = shadowEnabled;
}

int Config::threadNumber() const
{
    return _threadNumber;
}

void Config::setThreadNumber(int threadNumber)
{
    _threadNumber = threadNumber;
}

int Config::renderingTaskNumber() const
{
    return _renderingTaskNumber;
}

void Config::setRenderingTask(int renderingTaskNumber)
{
    _renderingTaskNumber = renderingTaskNumber;
}

int Config::lightSampleNumber() const
{
    return _lightSampleNumber;
}

void Config::setLightSampleNumber(int lightSampleNumber)
{
    _lightSampleNumber = lightSampleNumber;
}

int Config::antialiasingResolution() const
{
    return _antialiasingResolution;
}

void Config::setAntialiasingResolution(int antialiasingResolution)
{
    _antialiasingResolution = antialiasingResolution;
}

Config::AntialiasingType Config::antialiasingType() const
{
    return _antialiasingType;
}

void Config::setAntialiasingType(Config::AntialiasingType antialiasingType)
{
    _antialiasingType = antialiasingType;
}

int Config::maximumNodesPerBox() const
{
    return _maximumNodesPerBox;
}

void Config::setMaximumNodesPerBox(int maximumNodesPerBox)
{
    _maximumNodesPerBox = maximumNodesPerBox;
}

int Config::maximumRecursionDepth() const
{
    return _maximumRecursionDepth;
}

void Config::setMaximumRecursionDepth(int maximumRecursionDepth)
{
    _maximumRecursionDepth = maximumRecursionDepth;
}

float Config::refractionIndex() const
{
    return _refractionIndex;
}

void Config::setRefractionIndex(float refractionIndex)
{
    _refractionIndex = refractionIndex;
}

int Config::reflectionSampleNumber() const
{
    return _reflectionSampleNumber;
}

void Config::setReflectionSampleNumber(int reflectionSampleNumber)
{
    _reflectionSampleNumber = reflectionSampleNumber;
}

Config::Config()
{
    _xAxis = QVector3D(1.0f, 0.0f, 0.0f);
    _yAxis = QVector3D(0.0f, 1.0f, 0.0f);
    _zAxis = QVector3D(0.0f, 0.0f, 1.0f);
    _rootDir = "C:/Users/samuel/Documents/Dev/Raytracer/Files";
    _shadowEnabled = true;
    _threadNumber = 8;
    _renderingTaskNumber = 2000;
    _lightSampleNumber = 2;
    _antialiasingResolution = 1;
    _antialiasingType = AntialiasingType::Random;
    _maximumNodesPerBox = 10;
    _maximumRecursionDepth = 5;
    _refractionIndex = 1.0003f;
    _reflectionSampleNumber = 1.0f;
}

Config::~Config()
{
}
