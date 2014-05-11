#include "SceneGenerator.h"

#include <QtMath>
#include <QTime>
#include <QDebug>

#include "Renderer.h"
#include "Config.h"
#include "Scene.h"
#include "Camera.h"
#include "AssimpLoader.h"
#include "Logger.h"
#include "UniformSky.h"
#include "SphereSky.h"

#include "Triangle.h"
#include "Box.h"
#include "Instance.h"
#include "Sphere.h"
#include "Plane.h"
#include "BoxTreeNode.h"

#include "PointLight.h"
#include "DirectionalLight.h"
#include "ParallelogramLight.h"

#include "CustomMaterial.h"
#include "MetalMaterial.h"
#include "DielectricMaterial.h"
#include "PlasticMaterial.h"
#include "LambertMaterial.h"
#include "AshikhminMaterial.h"

#include "UniformTexture.hpp"
#include "ImageTexture.h"

SceneGenerator* SceneGenerator::_instance = 0;

SceneGenerator::SceneGenerator()
{
    _scenes.append(QPair<QString, Loader>("Test", &SceneGenerator::_loadSceneTest));

    _scenes.append(QPair<QString, Loader>("Project 1 - Cubes", &SceneGenerator::_loadProject1_Cubes));
    _scenes.append(QPair<QString, Loader>("Project 1 - Spheres", &SceneGenerator::_loadProject1_Spheres));
    _scenes.append(QPair<QString, Loader>("Project 2 - 2 Dragons", &SceneGenerator::_loadProject2_2Dragons));
    _scenes.append(QPair<QString, Loader>("Project 3 - Standard", &SceneGenerator::_loadProject3_Standard));
    _scenes.append(QPair<QString, Loader>("Project 3 - Focus", &SceneGenerator::_loadProject3_Focus));
    _scenes.append(QPair<QString, Loader>("Project 3 - Animation", &SceneGenerator::_loadProject3_Anim));

    _scenes.append(QPair<QString, Loader>("1 Cube / 1 Plane", &SceneGenerator::_loadCube));
    _scenes.append(QPair<QString, Loader>("Teapot", &SceneGenerator::_loadTeapot));
    _scenes.append(QPair<QString, Loader>("Cornell Box", &SceneGenerator::_loadCornelBox));
    _scenes.append(QPair<QString, Loader>("Dragon in Cornell Box", &SceneGenerator::_loadCornelDragon));
    _scenes.append(QPair<QString, Loader>("Many Dragons", &SceneGenerator::_loadManyDragons));
    _scenes.append(QPair<QString, Loader>("Many Teapots", &SceneGenerator::_loadManyTeapots));
    _scenes.append(QPair<QString, Loader>("Big", &SceneGenerator::_loadBig));
    _scenes.append(QPair<QString, Loader>("Env Map", &SceneGenerator::_loadEnvMap));
}

SceneGenerator::~SceneGenerator()
{
}

SceneGenerator *SceneGenerator::instance()
{
    if (!_instance) {
        _instance = new SceneGenerator();
    }
    return _instance;
}

void SceneGenerator::loadScene(const QString &name, Renderer *renderer)
{
    if (renderer) {
        for (auto const& current : _scenes) {
            if (current.first == name) {
                logger->writeSeparator();
                logger->writeInfo(QString("Loading scene : '%1'").arg(name));
                (this->*current.second)(renderer);
                logger->clearMessage();
                return ;
            }
        }
    }
    logger->writeError(QString("No such scene : '%1'").arg(name));
}

void SceneGenerator::loadFile(const QString &filename, Renderer *renderer)
{
    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.8f, 0.8f, 1.0f)));

    // Create camera
    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(0.0f, 50.0f, 200.0f), QVector3D(0.0f,50.0f,0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);

    AssimpLoader loader;
    QList<Mesh*> meshes;
    loader.loadFile(filename, meshes);
    for (Mesh* mesh : meshes) {
        scene->addNode(mesh);
    }

    Plane* ground = new Plane;
    ground->setPosition(QVector3D(0.0f, -1.5f, 0.0f));
    scene->addNode(ground);

    PointLight* lgt = new PointLight();
    lgt->setBaseColor(Color(0.8f, 0.8f, 0.8f));
    lgt->setIntensity(2.0f);
    lgt->setPosition(QVector3D(0.0f, 1.0f, -1.0f));
    scene->addNode(lgt);

    lgt = new PointLight();
    lgt->setBaseColor(Color(0.8f, 0.8f, 0.8f));
    lgt->setIntensity(2.0f);
    lgt->setPosition(QVector3D(-2.0f, 2.0f, 2.0f));
    scene->addNode(lgt);

    lgt = new PointLight();
    lgt->setBaseColor(Color(1.0f, 0.2f, 0.2f));
    lgt->setIntensity(4.0f);
    lgt->setPosition(QVector3D(0.0f, 3.0f, -1.0f));
    scene->addNode(lgt);

    DirectionalLight* sunlgt = new DirectionalLight;
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    sunlgt->setIntensity(0.5f);
    sunlgt->setDirection(QVector3D(-1.0f, -1.0f, -0.5f));
    scene->addNode(sunlgt);

    sunlgt = new DirectionalLight;
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    sunlgt->setIntensity(0.5f);
    sunlgt->setDirection(QVector3D(1.0f, -1.0f, -0.5f));
    scene->addNode(sunlgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

QList<QString> SceneGenerator::scenes() const
{
    QList<QString> res;
    for (auto const& current : _scenes) {
        res << current.first;
    }
    return res;
}

void SceneGenerator::_loadSceneTest(Renderer *renderer)
{
    (void)renderer;
}

void SceneGenerator::_loadProject1_Cubes(Renderer *renderer)
{
    Config::Epsilon = 0.00001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(2.0f, 2.0f, 5.0f), QVector3D(0.0f, 0.0f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);

    Scene* scene = new Scene();
    scene->setSky(new UniformSky(Color(0.8f, 0.9f, 1.0f)));
//    scene->setSky(new SphereSky(config->rootDir() + "/sphereMap_joshua.jpg"));

    LambertMaterial* cubeMat = new LambertMaterial;
    cubeMat->setDiffuseColor(Color(0.6f, 0.6f, 0.6f));

    LambertMaterial* groundMat = new LambertMaterial;
    groundMat->setDiffuseColor(Color(0.3f, 0.3f, 0.3f));

    Box* box1 = new Box();
    box1->set(5.0f, 0.1f, 5.0f);
    box1->setMaterial(groundMat);
    scene->addNode(box1);

    Box* box2 = new Box();
    box2->set(1.0f, 1.0f, 1.0f);
    box2->setMaterial(cubeMat);

    Instance* inst1 = new Instance(box2);
    QMatrix4x4 mtx;
    mtx.rotate(qRadiansToDegrees(0.5f), 1.0f, 0.0f, 0.0f);
    mtx.setColumn(3, QVector3D(0.0f, 1.0f, 0.0f));
    inst1->setMatrix(mtx);
    scene->addNode(inst1);

    Instance* inst2 = new Instance(box2);
    mtx.setToIdentity();
    mtx.rotate(qRadiansToDegrees(1.0f), 0.0f, 1.0f, 0.0f);
    mtx.setColumn(3, QVector3D(-1.0f, 0.0f, 1.0f));
    inst2->setMatrix(mtx);
    scene->addNode(inst2);

    DirectionalLight* sunlgt = new DirectionalLight;
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    sunlgt->setIntensity(0.5f);
    sunlgt->setDirection(QVector3D(-0.5f, -1.0f, -0.5f));
    scene->addNode(sunlgt);

    PointLight* redlgt = new PointLight();
    redlgt->setBaseColor(Color(1.0f, 0.2f, 0.2f));
    redlgt->setIntensity(2.0f);
    redlgt->setPosition(QVector3D(2.0f, 2.0f, 0.0f));
    scene->addNode(redlgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadProject1_Spheres(Renderer *renderer)
{
    Config::Epsilon = 0.001f;

    Texture* t = new UniformTexture<float>(5.0f);
    Color tmp;
    t->evaluateColor(0.0f, 0.0f, tmp);
    float f;
    t->evaluateFloat(0.0f, 0.0f, f);
    qDebug() << tmp.Red << tmp.Green << tmp.Blue << f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(-0.75f, 0.25f, 5.0f), QVector3D(0.0f, 0.5f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.8f, 0.8f, 1.0f)));

    LambertMaterial* sMat = new LambertMaterial;
//    sMat->setDiffuseColor(Color(0.8f, 0.8f, 0.8f));
    sMat->setDiffuseColor(new ImageTexture(config->rootDir() + "/earth.jpg"));

    LambertMaterial* gMat = new LambertMaterial;
    gMat->setDiffuseColor(new ImageTexture(config->rootDir() + "/checkerboard.png"));
    //    gMat->setDiffuseColor(Color(0.3f, 0.3f, 0.3f));
    //    mat->loadTexture(config->rootDir() + "/earth.jpg");

    Plane* ground = new Plane;
    ground->setMaterial(gMat);
    scene->addNode(ground);

    for(int i = 0; i < 20; i++) {
        Sphere*     sphere = new Sphere;
        float       rad = _rangeRand(0.25f, 0.5f);
        QVector3D   pos(_rangeRand(-5.0f, 5.0f), rad, _rangeRand(-5.0f, 5.0f));

        sphere->setRadius(rad);
        sphere->setOrigin(pos);
        sphere->setMaterial(sMat);
        scene->addNode(sphere);
    }

    DirectionalLight* sunlgt = new DirectionalLight;
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    sunlgt->setIntensity(1.0f);
    sunlgt->setDirection(QVector3D(2.0f, -3.0f, -2.0f));
    scene->addNode(sunlgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadProject2_2Dragons(Renderer *renderer)
{
    Config::Epsilon = 0.0001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(-0.1f, 0.1f, 0.2f), QVector3D(-0.05f, 0.12f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.8f, 0.8f, 1.0f)));

    AssimpLoader loader;
    QList<Mesh*> meshes;
    loader.loadFile1(config->rootDir() + "/Dragon/dragon.ply", meshes);
    QTime time;
    time.restart();
    Mesh* dragon = meshes.first();
    LambertMaterial* mat  = dynamic_cast<LambertMaterial*>(dragon->triangles().first()->material());
    mat->setDiffuseColor(Color(0.6f, 0.6f, 0.6f));
    BoxTreeNode* tree = new BoxTreeNode;
    tree->construct(dragon);
    scene->addNode(tree);
    logger->writeInfo(QString("Dragon Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    Instance* inst = new Instance(tree);
    inst->setMaterial(mat);
    QMatrix4x4 mtx;
    mtx.rotate(180.0f, 0.0f, 1.0f, 0.0f);
    mtx.setColumn(3, QVector4D(-0.05f, 0.0f, -0.1f, 1.0f));
    inst->setMatrix(mtx);
    scene->addNode(inst);

    Box* ground = new Box;
    LambertMaterial* mat1  = new LambertMaterial;
    mat1->setDiffuseColor(Color(0.3f, 0.3f, 0.3f));
    ground->set(5.0f, 0.1f, 5.0f);
    ground->setMaterial(mat1);
    scene->addNode(ground);

    DirectionalLight* sunlgt = new DirectionalLight;
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    sunlgt->setIntensity(1.0f);
    sunlgt->setDirection(QVector3D(2.0f, -3.0f, -2.0f));
    scene->addNode(sunlgt);

    PointLight* redlgt = new PointLight;
    redlgt->setBaseColor(Color(1.0f, 0.2f, 0.2f));
    redlgt->setIntensity(0.02f);
    redlgt->setPosition(QVector3D(-0.2f, 0.2f, 0.2f));
    scene->addNode(redlgt);

    PointLight* bluelgt = new PointLight;
    bluelgt->setBaseColor(Color(0.2f, 0.2f, 1.0f));
    bluelgt->setIntensity(0.02f);
    bluelgt->setPosition(QVector3D(0.1f, 0.1f, 0.3f));
    scene->addNode(bluelgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadProject3_Standard(Renderer *renderer)
{
    Config::Epsilon = 0.0001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(-0.5f, 0.25f, -0.2f), QVector3D(0.0f, 0.15f, -0.15f), config->yAxis());
    camera->set(40.0f, 1.33f);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.8f, 0.9f, 1.0f)));

    const int numDragon = 4;
    AshikhminMaterial* mtls[numDragon];

    // Diffuse
    mtls[0] = new AshikhminMaterial;
    mtls[0]->setSpecularLevel(0.0f);
    mtls[0]->setDiffuseLevel(1.0f);
    mtls[0]->setDiffuseColor(Color(0.7f,0.7f,0.7f));

    // Roughened copper
    mtls[1] = new AshikhminMaterial;
    mtls[1]->setDiffuseLevel(0.0f);
    mtls[1]->setSpecularLevel(1.0f);
    mtls[1]->setSpecularColor(Color(0.9f,0.6f,0.5f));
    mtls[1]->setRoughness(100.0f,100.0f);

    // Anisotropic gold
    mtls[2] = new AshikhminMaterial;
    mtls[2]->setDiffuseLevel(0.0f);
    mtls[2]->setSpecularLevel(1.0f);
    mtls[2]->setSpecularColor(Color(0.95f,0.7f,0.3f));
    mtls[2]->setRoughness(1.0f,1000.0f);

    // Red plastic
    mtls[3] = new AshikhminMaterial;
    mtls[3]->setDiffuseColor(Color(1.0f,0.1f,0.1f));
    mtls[3]->setDiffuseLevel(0.8f);
    mtls[3]->setSpecularLevel(0.2f);
    mtls[3]->setSpecularColor(Color(1.0f,1.0f,1.0f));
    mtls[3]->setRoughness(1000.0f,1000.0f);

    AssimpLoader loader;
    QList<Mesh*> meshes;
    loader.loadFile1(config->rootDir() + "/Dragon/dragon.ply", meshes);
    QTime time;
    time.restart();
    Mesh* dragon = meshes.first();
    BoxTreeNode* tree = new BoxTreeNode;
    tree->construct(dragon);
    logger->writeInfo(QString("Dragon Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    QMatrix4x4 mtx;
    for (int i = 0; i < numDragon; ++i) {
        Instance* inst=new Instance(tree);
        mtx.setToIdentity();
        mtx.translate(0.0f, 0.0f, -0.1f * float(i));
        inst->setMatrix(mtx);
        inst->setMaterial(mtls[i]);
        scene->addNode(inst);
    }

    Box* ground = new Box;
    LambertMaterial* groundMat  = new LambertMaterial;
    groundMat->setDiffuseColor(Color(0.3f, 0.3f, 0.35f));
    ground->set(2.0f, 0.11f, 2.0f);
    ground->setMaterial(groundMat);
    scene->addNode(ground);

    DirectionalLight* sunlgt = new DirectionalLight;
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    sunlgt->setIntensity(1.0f);
    sunlgt->setDirection(QVector3D(2.0f, -3.0f, -2.0f));
    scene->addNode(sunlgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadProject3_Focus(Renderer *renderer)
{
    Config::Epsilon = 0.0001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(-0.2f, 0.12f, 0.25f), QVector3D(0.0f, 0.15f, -0.15f), config->yAxis());
    camera->set(40.0f, 1.33f);
    camera->setAperture(0.01f);
    camera->setFocalPlane(0.29f);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.8f, 0.9f, 1.0f)));

    const int numDragon = 4;
    AshikhminMaterial* mtls[numDragon];

    // Diffuse
    mtls[0] = new AshikhminMaterial;
    mtls[0]->setSpecularLevel(0.0f);
    mtls[0]->setDiffuseLevel(1.0f);
    mtls[0]->setDiffuseColor(Color(0.7f,0.7f,0.7f));

    // Roughened copper
    mtls[1] = new AshikhminMaterial;
    mtls[1]->setDiffuseLevel(0.0f);
    mtls[1]->setSpecularLevel(1.0f);
    mtls[1]->setSpecularColor(Color(0.9f,0.6f,0.5f));
    mtls[1]->setRoughness(100.0f,100.0f);

    // Anisotropic gold
    mtls[2] = new AshikhminMaterial;
    mtls[2]->setDiffuseLevel(0.0f);
    mtls[2]->setSpecularLevel(1.0f);
    mtls[2]->setSpecularColor(Color(0.95f,0.7f,0.3f));
    mtls[2]->setRoughness(1.0f,1000.0f);

    // Red plastic
    mtls[3] = new AshikhminMaterial;
    mtls[3]->setDiffuseColor(Color(1.0f,0.1f,0.1f));
    mtls[3]->setDiffuseLevel(0.8f);
    mtls[3]->setSpecularLevel(0.2f);
    mtls[3]->setSpecularColor(Color(1.0f,1.0f,1.0f));
    mtls[3]->setRoughness(1000.0f,1000.0f);

    AssimpLoader loader;
    QList<Mesh*> meshes;
    loader.loadFile1(config->rootDir() + "/Dragon/dragon.ply", meshes);
    QTime time;
    time.restart();
    Mesh* dragon = meshes.first();
    BoxTreeNode* tree = new BoxTreeNode;
    tree->construct(dragon);
    logger->writeInfo(QString("Dragon Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    QMatrix4x4 mtx;
    for (int i = 0; i < numDragon; ++i) {
        Instance* inst=new Instance(tree);
        mtx.setToIdentity();
        mtx.translate(0.0f, 0.0f, -0.1f * float(i));
        inst->setMatrix(mtx);
        inst->setMaterial(mtls[i]);
        scene->addNode(inst);
    }

    Box* ground = new Box;
    LambertMaterial* groundMat  = new LambertMaterial;
    groundMat->setDiffuseColor(Color(0.3f, 0.3f, 0.35f));
    ground->set(2.0f, 0.11f, 2.0f);
    ground->setMaterial(groundMat);
    scene->addNode(ground);

    DirectionalLight* sunlgt = new DirectionalLight;
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    sunlgt->setIntensity(1.0f);
    sunlgt->setDirection(QVector3D(2.0f, -3.0f, -2.0f));
    scene->addNode(sunlgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadProject3_Anim(Renderer *renderer)
{
    Config::Epsilon = 0.0001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(-0.5f, 0.25f, -0.2f), QVector3D(0.0f, 0.15f, -0.15f), config->yAxis());
    camera->set(40.0f, 1.33f);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.8f, 0.9f, 1.0f)));

    const int numDragon = 4;
    AshikhminMaterial* mtls[numDragon];

    // Diffuse
    mtls[0] = new AshikhminMaterial;
    mtls[0]->setSpecularLevel(0.0f);
    mtls[0]->setDiffuseLevel(1.0f);
    mtls[0]->setDiffuseColor(Color(0.7f,0.7f,0.7f));

    // Roughened copper
    mtls[1] = new AshikhminMaterial;
    mtls[1]->setDiffuseLevel(0.0f);
    mtls[1]->setSpecularLevel(1.0f);
    mtls[1]->setSpecularColor(Color(0.9f,0.6f,0.5f));
    mtls[1]->setRoughness(100.0f,100.0f);

    // Anisotropic gold
    mtls[2] = new AshikhminMaterial;
    mtls[2]->setDiffuseLevel(0.0f);
    mtls[2]->setSpecularLevel(1.0f);
    mtls[2]->setSpecularColor(Color(0.95f,0.7f,0.3f));
    mtls[2]->setRoughness(1.0f,1000.0f);

    // Red plastic
    mtls[3] = new AshikhminMaterial;
    mtls[3]->setDiffuseColor(Color(1.0f,0.1f,0.1f));
    mtls[3]->setDiffuseLevel(0.8f);
    mtls[3]->setSpecularLevel(0.2f);
    mtls[3]->setSpecularColor(Color(1.0f,1.0f,1.0f));
    mtls[3]->setRoughness(1000.0f,1000.0f);

    AssimpLoader loader;
    QList<Mesh*> meshes;
    loader.loadFile1(config->rootDir() + "/Dragon/dragon.ply", meshes);
    QTime time;
    time.restart();
    Mesh* dragon = meshes.first();
    BoxTreeNode* tree = new BoxTreeNode;
    tree->construct(dragon);
    logger->writeInfo(QString("Dragon Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    QMatrix4x4 mtx;
    for (int i = 0; i < numDragon; ++i) {
        Instance* inst=new Instance(tree);
        mtx.setToIdentity();
        mtx.translate(0.0f, 0.0f, -0.1f * float(i));
        if (i == 0) {
            QMatrix4x4 mtx1;
            mtx1.setToIdentity();
            mtx1.translate(-0.05f, 0.0f, -0.1f * float(i));
            inst->setAnimation(mtx, mtx1);
        } else {
            inst->setMatrix(mtx);
        }
        inst->setMaterial(mtls[i]);
        scene->addNode(inst);
    }

    Box* ground = new Box;
    LambertMaterial* groundMat  = new LambertMaterial;
    groundMat->setDiffuseColor(Color(0.3f, 0.3f, 0.35f));
    ground->set(2.0f, 0.11f, 2.0f);
    ground->setMaterial(groundMat);
    scene->addNode(ground);

    DirectionalLight* sunlgt = new DirectionalLight;
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    sunlgt->setIntensity(1.0f);
    sunlgt->setDirection(QVector3D(2.0f, -3.0f, -2.0f));
    scene->addNode(sunlgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadCube(Renderer *renderer)
{
    Config::Epsilon = 0.00001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(1.0f, 2.0f, 3.0f), QVector3D(0.0f, 0.0f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.8f, 0.8f, 1.0f)));

    AssimpLoader loader;
    QList<Mesh*> meshes;
    loader.loadFile(config->rootDir() + "/Cube/cube.obj", meshes);
    for (Mesh* mesh : meshes) {
        scene->addNode(mesh);
    }

    Plane* ground = new Plane;
    ground->setPosition(QVector3D(0.0f, -0.5f, 0.0f));
    scene->addNode(ground);

    PointLight* lgt = new PointLight();
    lgt->setBaseColor(Color(0.8f, 0.8f, 0.8f));
    lgt->setIntensity(2.0f);
    lgt->setPosition(QVector3D(2.0f, 2.0f, 2.0f));
    scene->addNode(lgt);

    lgt = new PointLight();
    lgt->setBaseColor(Color(0.8f, 0.8f, 0.8f));
    lgt->setIntensity(2.0f);
    lgt->setPosition(QVector3D(-2.0f, 2.0f, 2.0f));
    scene->addNode(lgt);

    lgt = new PointLight();
    lgt->setBaseColor(Color(1.0f, 0.2f, 0.2f));
    lgt->setIntensity(2.0f);
    lgt->setPosition(QVector3D(0.0f, 2.0f, 2.0f));
    scene->addNode(lgt);

    DirectionalLight* sunlgt = new DirectionalLight;
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 1.0f));
    sunlgt->setIntensity(0.2f);
    sunlgt->setDirection(QVector3D(1.0f, -1.0f, -0.5f));
    scene->addNode(sunlgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadTeapot(Renderer *renderer)
{
    Config::Epsilon = 0.0001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(0.0f, 0.5f, 2.0f), QVector3D(0.0f, 0.5f, 0.0f), config->yAxis());
//    camera->lookAt(QVector3D(0.0f, 50.0f, 200.0f), QVector3D(0.0f, 50.0f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.8f, 0.8f, 1.0f)));

//    LambertMaterial* teapotMat  = new LambertMaterial;
//    teapotMat->setDiffuseColor(Color(0.6f, 0.6f, 0.6f));

    AshikhminMaterial* teapotMat = new AshikhminMaterial();
    teapotMat ->setDiffuseLevel(0.0f);
    teapotMat ->setSpecularLevel(1.0f);
    teapotMat ->setSpecularColor(Color(0.95f,0.7f,0.3f));
    teapotMat ->setRoughness(1.0f,1000.0f);

    AssimpLoader loader;
    QList<Mesh*> meshes;
    loader.loadFile(config->rootDir() + "/Teapot/teapot.obj", meshes);
    QTime time;
    time.restart();
    QVector<Node*> tmp;
    for (Mesh* mesh : meshes) {
        BoxTreeNode* tree = new BoxTreeNode;
        tree->construct(mesh);
//        mesh->setMaterial(teapotMat);
        tmp << tree;
//        scene->addNode(tree);
    }
    BoxTreeNode* tree = new BoxTreeNode;
    tree->construct(tmp);
    Instance* teapot = new Instance(tree);
    teapot->setMaterial(teapotMat);
    QMatrix4x4 mtx;
    mtx.scale(0.01f);
    teapot->setMatrix(mtx);
    scene->addNode(teapot);
    logger->writeInfo(QString("Teapot Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    Plane* ground = new Plane;
    LambertMaterial* simpleMat  = new LambertMaterial;
    simpleMat->setDiffuseColor(Color(0.3f, 0.3f, 0.3f));
    ground->setMaterial(simpleMat);
    scene->addNode(ground);

    DirectionalLight* sunlgt = new DirectionalLight;
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    sunlgt->setIntensity(0.5f);
    sunlgt->setDirection(QVector3D(-1.0f, -1.0f, -0.5f));
    scene->addNode(sunlgt);

    sunlgt = new DirectionalLight;
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    sunlgt->setIntensity(0.5f);
    sunlgt->setDirection(QVector3D(1.0f, -1.0f, -0.5f));
    scene->addNode(sunlgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadCornelBox(Renderer *renderer)
{
    Config::Epsilon = 0.00001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(0.0f, 0.75f, 3.0f), QVector3D(0.0f, 0.75f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.2f, 0.2f, 0.2f)));

    MetalMaterial* metal = new MetalMaterial();
    metal->setN(0.37f);
    metal->setK(2.82f);
    metal->setTransmittedColor(Color(1.0f, 0.843f, 0.0f));
    metal->setRoughness(0.0f);

    DielectricMaterial* trans = new DielectricMaterial();
    trans->setN(1.333f);
    trans->setRoughness(0.0f);

    LambertMaterial* lamb = new LambertMaterial();
    lamb->setDiffuseColor(Color::WHITE);

    LambertMaterial* sp1 = new LambertMaterial();
    sp1->setDiffuseColor(Color(0.0f, 0.0f, 1.0f));

    LambertMaterial* sp2 = new LambertMaterial();
    sp2->setDiffuseColor(Color(1.0f, 1.0f, 1.0f));

    PlasticMaterial* plastic = new PlasticMaterial();
    plastic->setDiffuseColor(Color(200.f / 255, 0.02f, 0.0f));
    plastic->setN(1.7f);
    plastic->setRoughness(0.2f);

    AssimpLoader loader;
    QList<Mesh*> meshes;
    loader.loadFile(config->rootDir() + "/CornelBox/CornellBox-Sphere.obj", meshes);
    QTime time;
    time.restart();
    meshes.at(0)->setMaterial(sp1);
    meshes.at(1)->setMaterial(sp2);

    for (Mesh* mesh : meshes) {
        BoxTreeNode* tree = new BoxTreeNode;
        tree->construct(mesh);
        scene->addNode(tree);
    }
    logger->writeInfo(QString("Cornell Box Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    ParallelogramLight* lgt = new ParallelogramLight();
    lgt->setBaseColor(Color(1.0f, 1.0f, 1.0f));
    lgt->setIntensity(0.4f);
    lgt->set(QVector3D(-0.24f, 1.58f, -0.22f), QVector3D(0.47f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.38f));
    scene->addNode(lgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadCornelDragon(Renderer *renderer)
{
    Config::Epsilon = 0.001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(0.0f, 0.75, 3.0f), QVector3D(0.0f, 0.75f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.0f, 0.0f, 0.0f)));

    MetalMaterial* metal = new MetalMaterial();
    metal->setN(0.37f);
    metal->setK(2.82f);
    metal->setTransmittedColor(Color(1.0f, 0.843f, 0.0f));

    DielectricMaterial* trans = new DielectricMaterial();
    trans->setN(1.333f);
    trans->setRoughness(0.0f);
    trans->setAbsorptionCoef(10.0f);
    trans->setAbsorptionColor(Color(0.0f, 0.8f, 0.0f));

    PlasticMaterial* plastic = new PlasticMaterial();
    plastic->setDiffuseColor(Color(194.f / 255, 0.0f, 0.0f));
    plastic->setN(1.5f);
    plastic->setRoughness(0.2f);

    AssimpLoader loader;
    QList<Mesh*> meshes;
    loader.loadFile(config->rootDir() + "/CornelBox/CornellBox-Empty.obj", meshes);
    QTime time;
    time.restart();
    for (Mesh* mesh : meshes) {
        BoxTreeNode* tree = new BoxTreeNode;
        tree->construct(mesh);
        scene->addNode(tree);
    }
    logger->writeInfo(QString("Cornell Box Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    meshes.clear();
    loader.loadFile1(config->rootDir() + "/Dragon/dragon.ply", meshes);
    time.restart();
    Mesh* dragon = meshes.first();
    BoxTreeNode* tree = new BoxTreeNode;
    tree->construct(dragon);
    logger->writeInfo(QString("Dragon Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    Instance* inst = new Instance(tree);
    inst->setMaterial(trans);
    QMatrix4x4 mtx;
    mtx.scale(7);
    mtx.setColumn(3, QVector4D(0.0f, -0.37f, 0.0f, 1.0f));
    inst->setMatrix(mtx);
    scene->addNode(inst);

    ParallelogramLight* lgt = new ParallelogramLight();
    lgt->setBaseColor(Color(1.0f, 1.0f, 1.0f));
    lgt->setIntensity(0.35f);
    lgt->set(QVector3D(-0.24f, 1.58f, -0.22f), QVector3D(0.47f, 0, 0.0f), QVector3D(0.0f, 0, 0.38f));
    scene->addNode(lgt);

//    PointLight* lgt = new PointLight();
//    lgt->setBaseColor(Color(1.0f, 1.0f, 1.0f));
//    lgt->setIntensity(1.0f);
//    lgt->setPosition(QVector3D(-0.01f, 1.50f, -0.02f));
//    scene->addNode(lgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadManyDragons(Renderer *renderer)
{
    Config::Epsilon = 0.00005f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(0.0f, 0.3f, 0.5f), QVector3D(0.0f, 0.0f, -1.0f), config->yAxis());
    camera->set(40.0f, 1.33f);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.8f, 0.8f, 1.0f)));

    AssimpLoader loader;
    QList<Mesh*> meshes;
    loader.loadFile(config->rootDir() + "/Dragon/dragon.ply", meshes);
    QTime time;
    time.restart();
    Mesh* dragon = meshes.first();
    BoxTreeNode* tree = new BoxTreeNode;
    tree->construct(dragon);
    logger->writeInfo(QString("Dragon Tree Construction : %1 ms").arg(QString::number(time.elapsed())));
    LambertMaterial* simpleMat  = new LambertMaterial;
    simpleMat->setDiffuseColor(Color::WHITE);

    QList<Node*> dragons;
    for (float x = -3.0f; x < 3.0f; x += 0.3f) {
        for (float z = 0.0f; z > -10.0f; z -= 0.3f) {
            Instance* inst = new Instance(tree);
            inst->setMaterial(simpleMat);
            QMatrix4x4 mtx;
            mtx.setColumn(3, QVector4D(x, 0.0f, z, 1.0f));
            inst->setMatrix(mtx);
            dragons.append(inst);
        }
    }
    tree = new BoxTreeNode;
    tree->construct(dragons.toVector());
    scene->addNode(tree);


    Plane* ground = new Plane;
    ground->setPosition(QVector3D(0.0f, 0.05f, 0.0f));
    ground->setMaterial(simpleMat);
    scene->addNode(ground);

    DirectionalLight* sunlgt = new DirectionalLight;
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    sunlgt->setIntensity(1.0f);
    sunlgt->setDirection(QVector3D(2.0f, -3.0f, -2.0f));
    scene->addNode(sunlgt);

    PointLight* redlgt = new PointLight;
    redlgt->setBaseColor(Color(1.0f, 0.2f, 0.2f));
    redlgt->setIntensity(0.02f);
    redlgt->setPosition(QVector3D(0.0f, 0.2f, 0.2f));
    scene->addNode(redlgt);

    PointLight* bluelgt = new PointLight;
    bluelgt->setBaseColor(Color(0.2f, 0.2f, 1.0f));
    bluelgt->setIntensity(0.02f);
    bluelgt->setPosition(QVector3D(0.1f, 0.1f, 0.3f));
    scene->addNode(bluelgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadManyTeapots(Renderer *renderer)
{
    Config::Epsilon = 0.01f;
    Scene* scene = new Scene();
    scene->setSky(new UniformSky(Color(0.8f, 0.9f, 1.0f)));

    AssimpLoader loader;
    QList<Mesh*> meshes;
    loader.loadFile(config->rootDir() + "/Teapot/teapot.obj", meshes);

    QTime time;
    time.restart();

    QVector<Node*> teapot;
    for (Mesh* mesh : meshes) {
        BoxTreeNode* tree = new BoxTreeNode;
        tree->construct(mesh);
        teapot << tree;
    }
    logger->writeInfo(QString("Teapot Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    BoxTreeNode* teapotTree = new BoxTreeNode();
    teapotTree->construct(teapot);

    LambertMaterial* simpleMat  = new LambertMaterial;
    simpleMat->setDiffuseColor(Color::WHITE);
    QVector<Node*> teapots;
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 100; ++j) {
            QMatrix4x4 mtx;
            mtx.setColumn(3, QVector4D(-100.0f + i * 10.0f, 0.0f, -j * 10.0f, 1.0f));
            mtx.scale(0.05f);
            Instance* oo = new Instance(teapotTree);
            oo->setMaterial(simpleMat);
            oo->setMatrix(mtx);
            teapots << oo;
        }
    }

    BoxTreeNode* teapotsTree = new BoxTreeNode();
    teapotsTree->construct(teapots);
    scene->addNode(teapotsTree);

    Plane* groundShape = new Plane;
    groundShape->setMaterial(simpleMat);
    scene->addNode(groundShape);

    // Create lights
//    PointLight* light;

//    light = new PointLight(glm::vec3(0.4, 0.75, 1));
//    light->setBaseColor(Color(0.8f, 0.9f, 1.0f));
//    light->setIntensity(1.0);

    DirectionalLight* sunlgt = new DirectionalLight;
    sunlgt->setDirection(QVector3D(2.0f, -3.0f, -2.0f));
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    sunlgt->setIntensity(1.0f);
    scene->addNode(sunlgt);

    // Create camera
    Camera* camera = renderer->camera();

    camera->lookAt(QVector3D(0.0f, 10.0f, 20.0f), QVector3D(0.0f, 5.0f, 0.0f), config->yAxis());
    camera->set(40.0f, 800.0f / 600.0f);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadBig(Renderer *renderer)
{
    Config::Epsilon = 0.01f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(0.0f, 2.0f, -5.7f), QVector3D(0.0f, 0.0f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.0f, 0.0f, 0.0f)));

    QVector<Node*> allSceneObjects;

    AssimpLoader loader;
    QList<Mesh*> meshes;
    loader.loadFile(config->rootDir() + "/Teapot/teapot.obj", meshes);
    QTime time;
    time.restart();
    BoxTreeNode* teapot = new BoxTreeNode;
    QVector<Node*> teapotList;
    for (Mesh* mesh : meshes) {
        BoxTreeNode* tree = new BoxTreeNode;
        tree->construct(mesh);
        teapotList << tree;
    }
    teapot->construct(teapotList);
    logger->writeInfo(QString("Teapot Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    Instance* centerTeapot = new Instance(teapot);
    QMatrix4x4 mtx;
    mtx.translate(0.0f, 0.65f, 0.0f);
    mtx.scale(0.005f);
    centerTeapot->setMatrix(mtx);
    MetalMaterial* teapotMaterial = new MetalMaterial();
    teapotMaterial->setN(0.37f);
    teapotMaterial->setK(2.82f);
    teapotMaterial->setTransmittedColor(Color(1.0f, 0.843f, 0.0f));
    teapotMaterial->setRoughness(0.2f);
    centerTeapot->setMaterial(teapotMaterial);
    allSceneObjects << centerTeapot;

    meshes.clear();
    loader.loadFile(config->rootDir() + "/Dragon/dragon.ply", meshes);
    time.restart();
    Mesh* dragon = meshes.first();
    BoxTreeNode* dragonTree = new BoxTreeNode;
    dragonTree->construct(dragon);
    logger->writeInfo(QString("Dragon Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    Instance* dragonBottomLeft = new Instance(dragonTree);
    mtx.setToIdentity();
    mtx.translate(1.5f, 0.41f, -1.5f);
    mtx.rotate(45.0f, 0.0f, 1.0f, 0.0f);
    mtx.scale(4.5f);
    dragonBottomLeft->setMatrix(mtx);
    DielectricMaterial* dragonBottomLeftMat = new DielectricMaterial;
    dragonBottomLeftMat->setN(1.8f);
    dragonBottomLeftMat->setAbsorptionCoef(0.0f);
    dragonBottomLeft->setMaterial(dragonBottomLeftMat);
    allSceneObjects << dragonBottomLeft;

    Instance* dragonBottomRight = new Instance(dragonTree);
    mtx.setToIdentity();
    mtx.translate(-1.5f, 0.41f, -1.5f);
    mtx.rotate(135.0f, 0.0f, 1.0f, 0.0f);
    mtx.scale(4.5f);
    dragonBottomRight->setMatrix(mtx);
    DielectricMaterial* dragonBottomRightMat = new DielectricMaterial;
    dragonBottomRightMat->setN(1.333f);
    dragonBottomRightMat->setAbsorptionCoef(15.0f);
    dragonBottomRightMat->setAbsorptionColor(Color(0.1f, 0.2f, 0.9f));
    dragonBottomRight->setMaterial(dragonBottomRightMat);
    allSceneObjects << dragonBottomRight;

    Instance* dragonTopLeft = new Instance(dragonTree);
    mtx.setToIdentity();
    mtx.translate(1.5f, 0.41f, 1.5f);
    mtx.rotate(-45.0f, 0.0f, 1.0f, 0.0f);
    mtx.scale(4.5f);
    dragonTopLeft->setMatrix(mtx);
    PlasticMaterial* dragonTopLeftMat = new PlasticMaterial;
    dragonTopLeftMat->setDiffuseColor(Color(0.1f, 0.9f, 0.2f));
    dragonTopLeftMat->setN(2.0f);
    dragonTopLeftMat->setRoughness(0.2f);
    dragonTopLeft->setMaterial(dragonTopLeftMat);
    allSceneObjects << dragonTopLeft;

    Instance* dragonTopRight = new Instance(dragonTree);
    mtx.setToIdentity();
    mtx.translate(-1.5f, 0.41f, 1.5f);
    mtx.rotate(225.0f, 0.0f, 1.0f, 0.0f);
    mtx.scale(4.5f);
    dragonTopRight->setMatrix(mtx);
    MetalMaterial* dragonTopRightMat = new MetalMaterial;
    dragonTopRightMat->setTransmittedColor(Color(0.753f, 0.753f, 0.753f));
    dragonTopRightMat->setN(0.177f);
    dragonTopRightMat->setK(3.638f);
    dragonTopRightMat->setRoughness(0.2f);
    dragonTopRight->setMaterial(dragonTopRightMat);
    allSceneObjects << dragonTopRight;

    Box* ground = new Box;
    LambertMaterial* groundMat  = new LambertMaterial;
    groundMat->setDiffuseColor(Color::WHITE);
    ground->set(50.0f, 0.1f, 50.0f);
    ground->setMaterial(groundMat);
    scene->addNode(ground);

    Box* stand = new Box;
    stand->set(0.8f, 0.6f, 0.8f);

    PlasticMaterial* standMaterial = new PlasticMaterial;
    standMaterial->setDiffuseColor(Color(0.5f, 0.5f, 0.5f));
    standMaterial->setN(1.5f);
    standMaterial->setRoughness(0.2f);

    Instance* bCenter = new Instance(stand);
    mtx.setToIdentity();
    mtx.translate(0.0f, 0.35f, 0.0f);
    bCenter->setMatrix(mtx);
    bCenter->setMaterial(standMaterial);
    scene->addNode(bCenter);

    Instance* bBottomLeft = new Instance(stand);
    mtx.setToIdentity();
    mtx.translate(1.5f, 0.35f, -1.5f);
    bBottomLeft->setMatrix(mtx);
    bBottomLeft->setMaterial(standMaterial);
    scene->addNode(bBottomLeft);

    Instance* bBottomRight = new Instance(stand);
    mtx.setToIdentity();
    mtx.translate(-1.5f, 0.35f, -1.5f);
    bBottomRight->setMatrix(mtx);
    bBottomRight->setMaterial(standMaterial);
    scene->addNode(bBottomRight);

    Instance* bTopLeft = new Instance(stand);
    mtx.setToIdentity();
    mtx.translate(1.5f, 0.35f, 1.5f);
    bTopLeft->setMatrix(mtx);
    bTopLeft->setMaterial(standMaterial);
    scene->addNode(bTopLeft);

    Instance* bTopRight = new Instance(stand);
    mtx.setToIdentity();
    mtx.translate(-1.5f, 0.35f, 1.5f);
    bTopRight->setMatrix(mtx);
    bTopRight->setMaterial(standMaterial);
    scene->addNode(bTopRight);

    float lightValue = 4.0f;
    ParallelogramLight* lgtCenter = new ParallelogramLight;
    lgtCenter->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    lgtCenter->setIntensity(lightValue * 1.3f);
    lgtCenter->set(QVector3D(-0.5f, 4.0f, -0.5f), QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f));
    scene->addNode(lgtCenter);

    ParallelogramLight* lgtBottomLeft = new ParallelogramLight;
    lgtBottomLeft->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    lgtBottomLeft->setIntensity(lightValue);
    lgtBottomLeft->set(QVector3D(2.0f, 4.0f, -2.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(-1.0f, 0.0f, 0.0f));
    scene->addNode(lgtBottomLeft);

    ParallelogramLight* lgtBottomRight = new ParallelogramLight;
    lgtBottomRight->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    lgtBottomRight->setIntensity(lightValue);
    lgtBottomRight->set(QVector3D(-2.0f, 4.0f, -2.0f), QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f));
    scene->addNode(lgtBottomRight);

    ParallelogramLight* lgtTopLeft = new ParallelogramLight;
    lgtTopLeft->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    lgtTopLeft->setIntensity(lightValue);
    lgtTopLeft->set(QVector3D(2.0f, 4.0f, 2.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(-1.0f, 0.0f, 0.0f));
    scene->addNode(lgtTopLeft);

    ParallelogramLight* lgtTopRight = new ParallelogramLight;
    lgtTopRight->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    lgtTopRight->setIntensity(lightValue);
    lgtTopRight->set(QVector3D(-2.0f, 4.0f, 2.0f), QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f));
    scene->addNode(lgtTopRight);

    BoxTreeNode* sceneTree = new BoxTreeNode;
    sceneTree->construct(allSceneObjects);

    scene->addNode(sceneTree);
    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadEnvMap(Renderer *renderer)
{
    Config::Epsilon = 0.0001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(1.0f, 1.0f, -5.7f), QVector3D(0.0f, 0.3f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);

    Scene* scene = new Scene;
    scene->setSky(new SphereSky(config->rootDir() + "/sphereMap_joshua.jpg"));

    QVector<Node*> allSceneObjects;

    AssimpLoader loader;
    QList<Mesh*> meshes;
    QTime time;
    QMatrix4x4 mtx;
    loader.loadFile(config->rootDir() + "/Teapot/teapot.obj", meshes);
    time.restart();
    BoxTreeNode* teapot = new BoxTreeNode;
    QVector<Node*> teapotList;
    for (Mesh* mesh : meshes) {
        BoxTreeNode* tree = new BoxTreeNode;
        tree->construct(mesh);
        teapotList << tree;
    }
    teapot->construct(teapotList);
    logger->writeInfo(QString("Teapot Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    Instance* centerTeapot = new Instance(teapot);
    mtx.translate(-1.4f, -0.7f, -0.4f);
    mtx.scale(0.008f);
    centerTeapot->setMatrix(mtx);
    AshikhminMaterial* teapotMaterial = new AshikhminMaterial();
    teapotMaterial->setDiffuseLevel(0.0f);
    teapotMaterial->setSpecularLevel(1.0f);
    teapotMaterial->setSpecularColor(Color(0.9f,0.6f,0.5f));
    teapotMaterial->setRoughness(100.0f,100.0f);
    centerTeapot->setMaterial(teapotMaterial);
    allSceneObjects << centerTeapot;

    meshes.clear();
    loader.loadFile(config->rootDir() + "/Dragon/dragon.ply", meshes);
    time.restart();
    Mesh* dragon = meshes.first();
    BoxTreeNode* dragonTree = new BoxTreeNode;
    dragonTree->construct(dragon);
    logger->writeInfo(QString("Dragon Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    Instance* dragonInst = new Instance(dragonTree);
    mtx.setToIdentity();
    mtx.translate(0.00f, -1.25f, 0.3f);
    mtx.rotate(180.0f, 0.0f, 1.0f, 0.0f);
    mtx.scale(10.0f);
    dragonInst->setMatrix(mtx);
    AshikhminMaterial* dragonMat = new AshikhminMaterial();
    dragonMat->setDiffuseColor(Color(0.7f,0.7f,0.7f));
    dragonMat->setDiffuseLevel(0.8f);
    dragonMat->setSpecularLevel(0.2f);
    dragonMat->setSpecularColor(Color(1.0f,1.0f,1.0f));
    dragonMat->setRoughness(1000.0f,1000.0f);
    dragonInst->setMaterial(dragonMat);
    allSceneObjects << dragonInst;

    AshikhminMaterial* sphereMat = new AshikhminMaterial;
    sphereMat->setDiffuseLevel(0.0f);
    sphereMat->setSpecularLevel(1.0f);
    sphereMat->setSpecularColor(Color::WHITE);
    ImageTexture* roughness = new ImageTexture(config->rootDir() + "earth1.bmp");
    sphereMat->setRoughness(roughness, roughness);
    Sphere* sphere = new Sphere;
    sphere->setRadius(0.35f);
    sphere->setMaterial(sphereMat);
    Instance* sphereInst = new Instance(sphere);
    mtx.setToIdentity();
    mtx.translate(1.4f, -0.35f, -0.5f);
    mtx.rotate(270.0f, 0.0f, 1.0f, 0.0f);
    sphereInst->setMatrix(mtx);
    scene->addNode(sphereInst);

    Box* stand = new Box;
    stand->set(4.0f, 1.6f, 1.6f);

    AshikhminMaterial* standMaterial = new AshikhminMaterial();
    standMaterial->setDiffuseColor(Color(0.3f, 0.3f, 0.3f));
    standMaterial->setDiffuseLevel(0.8f);
    standMaterial->setSpecularLevel(0.2f);
    standMaterial->setSpecularColor(Color(1.0f,1.0f,1.0f));
    standMaterial->setRoughness(100.0f,100.0f);

    Instance* bCenter = new Instance(stand);
    mtx.setToIdentity();
    mtx.translate(0.0f, -1.5f, 0.0f);
    bCenter->setMatrix(mtx);
    bCenter->setMaterial(standMaterial);
    scene->addNode(bCenter);

    DirectionalLight* sun = new DirectionalLight;
    sun->setIntensity(1.0f);
    sun->setDirection(QVector3D(0.5f, -1.0f, 0.5f));
    scene->addNode(sun);

    BoxTreeNode* sceneTree = new BoxTreeNode;
    sceneTree->construct(allSceneObjects);

    scene->addNode(sceneTree);
    renderer->setScene(scene);
    renderer->setCamera(camera);
}

float SceneGenerator::_rangeRand(float min,float max) {
    return min+(max-min)*float(rand()) / float(RAND_MAX);
}
