#ifndef CONFIG_H
#define CONFIG_H

#include <QVector3D>
#include <QString>

#include "Integrator.h"

#define config Config::instance()

class Config
{
public:
    enum AntialiasingType {
        Uniform,
        Random,
        Jittered,
        Shirley
    };

    static float    Epsilon;
    static Config*  instance();

    QVector3D const& xAxis() const;
    QVector3D const& yAxis() const;
    QVector3D const& zAxis() const;

    QString const&  rootDir() const;
    QString const&  sceneResourcesDir() const;
    QString const&  outputDir() const;

    bool    shadowEnabled() const;
    void    setShadowEnabled(bool shadowEnabled);

    int     threadNumber() const;
    void    setThreadNumber(int threadNumber);

    int     renderingTaskNumber() const;
    void    setRenderingTask(int renderingTaskNumber);

    int     lightSampleNumber() const;
    void    setLightSampleNumber(int lightSampleNumber);

    int     antialiasingResolution() const;
    void    setAntialiasingResolution(int antialiasingResolution);

    AntialiasingType     antialiasingType() const;
    void    setAntialiasingType(AntialiasingType antialiasingType);

    int     maximumNodesPerBox() const;
    void    setMaximumNodesPerBox(int maximumNodesPerBox);

    Integrator::Type    integrator() const;
    void    setIntegrator(Integrator::Type integrator);


    int     pathtracingPathDepth() const;
    void    setPathtracingPathDepth(int pathtracingPathDepth);


    int     photonMappingPhotonNumber() const;
    void    setPhotonMappingPhotonNumber(int photonMappingPhotonNumber);

    int     photonMappingNumberNearestPhoton() const;
    void    setPhotonMappingNumberNearestPhoton(int photonMappingNumberNearestPhoton);

    float   photonMappingMaximumSearchRadius() const;
    void    setPhotonMappingMaximumSearchRadius(float photonMappingMaximumSearchRadius);

    int     photonMappingMapDepth() const;
    void    setPhotonMappingMapDepth(int photonMappingMapDepth);

    int     photonMappingRayDepth() const;
    void    setPhotonMappingRayDepth(int photonMappingRayDepth);

    float   refractionIndex() const;
    void    setRefractionIndex(float refractionIndex);

    float   defaultCameraVerticalFOV() const;
    float   defaultCameraAspectRatio() const;
    float   defaultCameraAperture() const;
    float   defaultCameraFocusPlane() const;

    int     defaultImageWidth() const;
    int     defaultImageHeight() const;

private:
    Config();
    ~Config();

    static Config*  _instance;

    QVector3D   _xAxis;
    QVector3D   _yAxis;
    QVector3D   _zAxis;
    QString     _rootDir;
    QString     _sceneResourcesDir;
    QString     _outputDir;
    bool        _shadowEnabled;
    int         _threadNumber;
    int         _renderingTaskNumber;
    int         _lightSampleNumber;
    int         _antialiasingResolution;
    int         _maximumNodesPerBox;
    AntialiasingType    _antialiasingType;
    float       _refractionIndex;

    Integrator::Type    _integrator;

    // Pathtracing
    int         _pathtracingPathDepth;

    // Photon Mapping
    int         _photonMappingPhotonNumber;
    int         _photonMappingNumberNearestPhoton;
    float       _photonMappingMaximumSearchRadius;
    int         _photonMappingMapDepth;
    int         _photonMappingRayDepth;

    float       _defaultCameraVerticalFOV;
    float       _defaultCameraAspectRatio;
    float       _defaultCameraAperture;
    float       _defaultCameraFocusPlane;

    int         _defaultImageWidth;
    int         _defaultImageHeight;
};

#endif // CONFIG_H
