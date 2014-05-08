#ifndef CONFIG_H
#define CONFIG_H

#include <QVector3D>
#include <QString>

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

    int     maximumRecursionDepth() const;
    void    setMaximumRecursionDepth(int maximumRecursionDepth);

    float   refractionIndex() const;
    void    setRefractionIndex(float refractionIndex);

    int     reflectionSampleNumber() const;
    void    setReflectionSampleNumber(int reflectionSampleNumber);

private:
    Config();
    ~Config();

    static Config*  _instance;

    QVector3D   _xAxis;
    QVector3D   _yAxis;
    QVector3D   _zAxis;
    QString     _rootDir;
    bool        _shadowEnabled;
    int         _threadNumber;
    int         _renderingTaskNumber;
    int         _lightSampleNumber;
    int         _antialiasingResolution;
    int         _maximumNodesPerBox;
    AntialiasingType    _antialiasingType;
    int         _maximumRecursionDepth;
    float       _refractionIndex;
    int         _reflectionSampleNumber;
};

#endif // CONFIG_H
