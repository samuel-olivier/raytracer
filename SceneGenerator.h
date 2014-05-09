#ifndef SCENEGENERATOR_H
#define SCENEGENERATOR_H

#include <QString>
#include <QList>
#include <QPair>

#define sceneGenerator SceneGenerator::instance()

class Renderer;

class SceneGenerator
{
public:

    static SceneGenerator*  instance();

    void    loadScene(QString const& name, Renderer* renderer);
    void    loadFile(QString const& filename, Renderer* renderer);

    QList<QString>  scenes() const;

private:
    SceneGenerator();
    ~SceneGenerator();

    static SceneGenerator*  _instance;

    typedef void    (SceneGenerator::*Loader)(Renderer*);

    void    _loadSceneTest(Renderer* renderer);

    void    _loadProject1_Cubes(Renderer* renderer);
    void    _loadProject1_Spheres(Renderer* renderer);
    void    _loadProject2_2Dragons(Renderer* renderer);
    void    _loadProject3_Standard(Renderer* renderer);
    void    _loadProject3_Focus(Renderer* renderer);
    void    _loadProject3_Anim(Renderer* renderer);

    void    _loadCube(Renderer* renderer);
    void    _loadTeapot(Renderer* renderer);
    void    _loadCornelBox(Renderer* renderer);
    void    _loadCornelDragon(Renderer* renderer);
    void    _loadManyDragons(Renderer* renderer);
    void    _loadManyTeapots(Renderer* renderer);
    void    _loadBig(Renderer* renderer);

    float   _rangeRand(float min,float max);

    QList<QPair<QString, Loader>>    _scenes;
};

#endif // SCENEGENERATOR_H
