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
#include "Vertex.h"

#include "Triangle.h"
#include "Box.h"
#include "Instance.h"
#include "Sphere.h"
#include "Plane.h"
#include "BoxTreeNode.h"

#include "PointLight.h"
#include "DirectionalLight.h"
#include "ParallelogramLight.h"
#include "SpotLight.h"

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
    _scenes.append(QPair<QString, Loader>("Cornell Box", &SceneGenerator::_loadCornellBox));
    _scenes.append(QPair<QString, Loader>("Dragon in Cornell Box", &SceneGenerator::_loadCornellDragon));
    _scenes.append(QPair<QString, Loader>("Many Dragons", &SceneGenerator::_loadManyDragons));
    _scenes.append(QPair<QString, Loader>("Many Teapots", &SceneGenerator::_loadManyTeapots));

    _scenes.append(QPair<QString, Loader>("4 Dragons / 1 Teapot", &SceneGenerator::_load4Dragons1Teapot));
    _scenes.append(QPair<QString, Loader>("Env Map", &SceneGenerator::_loadEnvMap));
    _scenes.append(QPair<QString, Loader>("Ring", &SceneGenerator::_loadRing));
    _scenes.append(QPair<QString, Loader>("Music", &SceneGenerator::_loadMusic));
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
    Config::Epsilon = 0.0001f;
    config->setPhotonMappingMaximumSearchRadius(50.0f);
    config->setPhotonMappingNumberNearestPhoton(100.0f);
    config->setPhotonMappingPhotonNumber(10000);
//    config->setPhotonMaximumRadius(25.0f);
//    config->setNumberNearestPhoton(5000.0f);
//    config->setPhotonNumber(5000000);

    Camera* camera = renderer->camera();
    camera->set(40.0f, 1.777f);
//    camera->setAperture(1.0f);
//    camera->setFocalPlane(87.15381);
//    renderer->setImageSize(1920, 1080);
    renderer->setImageSize(800, 450);

    Scene* scene = new Scene;
//    scene->setSky(new UniformSky(Color(0.1f, 0.1f, 0.1f)));
    scene->setSky(new SphereSky(config->sceneResourcesDir() + "/Textures/sphereMap_joshua.jpg"));

    AssimpLoader loader;
    QList<Instance*> meshes;
    QList<Light*> lights;
    loader.loadFile(config->sceneResourcesDir() + "Models/Bottles/Bottles.dae", meshes, lights, camera);
    for (int i = 0; i < lights.size(); ++i) {
        Light* lgt = lights[i];
        scene->addNode(lgt);
        if (i == 0) {
            lgt->setIntensity(200000.0f);
       } else if (i == 1) {
            lgt->setIntensity(250000.0f);
        }
    }
    QTime time;
    time.restart();
    QVector<Node*> tmp;
    for (int i = 0; i < meshes.size(); ++i) {
        Instance* mesh = meshes[i];
        Mesh* toSet = mesh->object<Mesh>();
        if (toSet->name().toLower().contains("light")) {
            AreaLight* light = new AreaLight;
            QVector<Triangle*> lightTris = toSet->triangles();
            light->setTriangles(lightTris);
            light->transform(mesh->matrix());
            light->setBaseColor(Color::WHITE);
            light->setIntensity(1.0f);
            light->setGeneratePhotons(false);
            scene->addNode(light);
        } else {
//            toSet->smooth();
            toSet->generateTextureTangents();
            BoxTreeNode* tree = new BoxTreeNode;
            tree->construct(mesh->object<Mesh>());
            Instance* treeInst = new Instance(tree);
            treeInst->setMatrix(mesh->matrix());
            tmp << treeInst;
        }
    }
    BoxTreeNode* tree = new BoxTreeNode;
    tree->construct(tmp);
    Instance* all = new Instance(tree);
    QMatrix4x4 mtx;
    all->setMatrix(mtx);
    scene->addNode(all);
    logger->writeInfo(QString("Scene Construction : %1 ms").arg(QString::number(time.elapsed())));

    renderer->setScene(scene);
    renderer->setCamera(camera);
    (void)renderer;
}

void SceneGenerator::_loadProject1_Cubes(Renderer *renderer)
{
    Config::Epsilon = 0.00001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(2.0f, 2.0f, 5.0f), QVector3D(0.0f, 0.0f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);
    renderer->setImageSize(800, 600);

    Scene* scene = new Scene();
    scene->setSky(new UniformSky(Color(0.8f, 0.9f, 1.0f)));

    LambertMaterial* cubeMat = new LambertMaterial;
    cubeMat->setDiffuseColor(Color(0.6f, 0.6f, 0.6f));
    cubeMat->setNormalMap(new ImageTexture(config->sceneResourcesDir() + "Textures/halflife.jpg"));

    LambertMaterial* groundMat = new LambertMaterial;
    groundMat->setDiffuseColor(Color(0.3f, 0.3f, 0.3f));
    groundMat->setNormalMap(new ImageTexture(config->sceneResourcesDir() + "Textures/halflife.jpg"));
    Box* box1 = new Box();
    box1->set(5.0f, 0.1f, 5.0f);
    box1->setMaterial(groundMat);
    scene->addNode(box1);

//    Box* box2 = new Box();
//    box2->set(1.0f, 1.0f, 1.0f);
//    box2->generateTextureTangents();
//    box2->setMaterial(cubeMat);

//    Instance* inst1 = new Instance(box2);
//    QMatrix4x4 mtx;
//    mtx.rotate(qRadiansToDegrees(0.5f), 1.0f, 0.0f, 0.0f);
//    mtx.setColumn(3, QVector3D(0.0f, 1.0f, 0.0f));
//    inst1->setMatrix(mtx);
//    scene->addNode(inst1);

//    Instance* inst2 = new Instance(box2);
//    mtx.setToIdentity();
//    mtx.rotate(qRadiansToDegrees(1.0f), 0.0f, 1.0f, 0.0f);
//    mtx.setColumn(3, QVector3D(-1.0f, 0.0f, 1.0f));
//    inst2->setMatrix(mtx);
//    scene->addNode(inst2);

    DirectionalLight* sunlgt = new DirectionalLight;
    sunlgt->setBaseColor(Color(1.0f, 1.0f, 0.9f));
    sunlgt->setIntensity(0.5f);
    sunlgt->setDirection(QVector3D(-0.5f, -1.0f, -0.5f));
    scene->addNode(sunlgt);

    PointLight* redlgt = new PointLight();
    redlgt->setBaseColor(Color(1.0f, 0.2f, 0.2f));
    redlgt->setIntensity(2.0f);
    redlgt->setPosition(QVector3D(2.0f, 2.0f, 0.0f));
//    scene->addNode(redlgt);

    SpotLight* spotlgt = new SpotLight();
    spotlgt->setBaseColor(Color(1.0f, 0.2f, 0.2f));
    spotlgt->setIntensity(10.0f);
    spotlgt->setPosition(QVector3D(0.0f, 3.0f, 0.0f));
    spotlgt->setDirection(QVector3D(0.0f, -1.0f, 0.0f));
    scene->addNode(spotlgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadProject1_Spheres(Renderer *renderer)
{
    Config::Epsilon = 0.001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(-0.75f, 0.25f, 5.0f), QVector3D(0.0f, 0.5f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);
    renderer->setImageSize(800, 600);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.8f, 0.8f, 1.0f)));

    LambertMaterial* sMat = new LambertMaterial;
//    sMat->setDiffuseColor(Color(0.8f, 0.8f, 0.8f));
    sMat->setDiffuseColor(new ImageTexture(config->sceneResourcesDir() + "/Textures/earth_color.jpg"));

    LambertMaterial* gMat = new LambertMaterial;
    gMat->setDiffuseColor(new ImageTexture(config->sceneResourcesDir() + "/Textures/checkerboard.png"));

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
    renderer->setImageSize(800, 600);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.8f, 0.8f, 1.0f)));

    AssimpLoader loader;
    QList<Instance*> meshes;
    loader.loadFile1(config->sceneResourcesDir() + "Models/Dragon/dragon.ply", meshes);
    QTime time;
    time.restart();
    Mesh* dragon = meshes.first()->object<Mesh>();
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
    renderer->setImageSize(800, 600);

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
    QList<Instance*> meshes;
    loader.loadFile1(config->sceneResourcesDir() + "Models/Dragon/dragon.ply", meshes);
    QTime time;
    time.restart();
    Mesh* dragon = meshes.first()->object<Mesh>();
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
    camera->setAperture(0.003f);
    camera->setFocalPlane(0.285f);
    renderer->setImageSize(800, 600);

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
    QList<Instance*> meshes;
    loader.loadFile1(config->sceneResourcesDir() + "Models/Dragon/dragon.ply", meshes);
    QTime time;
    time.restart();
    Mesh* dragon = meshes.first()->object<Mesh>();
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
    renderer->setImageSize(800, 600);

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
    QList<Instance*> meshes;
    loader.loadFile1(config->sceneResourcesDir() + "Models/Dragon/dragon.ply", meshes);
    QTime time;
    time.restart();
    Mesh* dragon = meshes.first()->object<Mesh>();
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
    renderer->setImageSize(800, 600);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.8f, 0.8f, 1.0f)));

    AssimpLoader loader;
    QList<Instance*> meshes;
    loader.loadFile(config->sceneResourcesDir() + "Models/Cube/cube.obj", meshes);
    for (Instance* mesh : meshes) {
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
    camera->set(40.0f, 1.33f);
    renderer->setImageSize(800, 600);

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
    QList<Instance*> meshes;
    loader.loadFile(config->sceneResourcesDir() + "Models/Teapot/teapot.obj", meshes);
    QTime time;
    time.restart();
    QVector<Node*> tmp;
    for (Instance* mesh : meshes) {
        BoxTreeNode* tree = new BoxTreeNode;
        tree->construct(mesh->object<Mesh>());
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

void SceneGenerator::_loadCornellBox(Renderer *renderer)
{
    Config::Epsilon = 0.0001f;
    config->setPhotonMappingMaximumSearchRadius(0.0001f);

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(0.0f, 0.75f, 3.0f), QVector3D(0.0f, 0.75f, 0.0f), config->yAxis());
    camera->set(40.0f, 800.0f / 600.0f);
    renderer->setImageSize(800, 600);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.0f, 0.0f, 0.0f)));

    LambertMaterial* lWall = new LambertMaterial;
    lWall->setDiffuseColor(Color(0.63f, 0.065f, 0.05f));

    LambertMaterial* rWall = new LambertMaterial;
    rWall->setDiffuseColor(Color(0.161f, 0.133f, 0.427f));

    LambertMaterial* oWall = new LambertMaterial;
    oWall->setDiffuseColor(Color(0.725f, 0.71f, 0.68f));

    MetalMaterial* metal = new MetalMaterial();
    metal->setN(0.177f);
    metal->setK(3.638f);
    metal->setTransmittedColor(Color(1.0f, 1.0f, 1.0f));
    metal->setRoughness(0.0f);

    DielectricMaterial* trans = new DielectricMaterial();
    trans->setN(1.45f);
    trans->setAbsorptionColor(Color::GREEN);
    trans->setAbsorptionCoef(0.0f);
    trans->setRoughness(0.0f);

    AssimpLoader loader;
    QList<Instance*> meshes;
    loader.loadFile(config->sceneResourcesDir() + "Models/CornellBox/CornellBox-Sphere.obj", meshes);
    QTime time;
    time.restart();

    int idx = 0;
    for (Instance* mesh : meshes) {
//        if (idx != 5 && idx != 6 && idx != 3) {
            BoxTreeNode* tree = new BoxTreeNode;
            tree->construct(mesh->object<Mesh>());
            mesh->object<Mesh>()->setMaterial(oWall);
            scene->addNode(tree);
//        }
        ++idx;
    }
    meshes.at(0)->object<Mesh>()->setMaterial(metal);
    meshes.at(1)->object<Mesh>()->setMaterial(trans);
    meshes.at(5)->object<Mesh>()->setMaterial(rWall);
    meshes.at(6)->object<Mesh>()->setMaterial(lWall);
    logger->writeInfo(QString("Cornell Box Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    //    PointLight* lgt = new PointLight;
    //    lgt->setBaseColor(Color(1.0f, 1.0f, 1.0f));
    //    lgt->setIntensity(0.7f);
    //    lgt->setPosition(QVector3D(0.0f, 1.0f, 0.0f));
    //    scene->addNode(lgt);

//    SpotLight* lgt = new SpotLight;
//    lgt->setBaseColor(Color(1.0f, 1.0f, 1.0f));
//    lgt->setIntensity(2.0f);
//    lgt->setPosition(QVector3D(0.0f, 1.0f, 0.0f));
//    lgt->setOuterAngle(90);
//    lgt->setInnerAngle(75);
//    lgt->setDirection(QVector3D(0.0f, -1.0f, 0.0f));
//    scene->addNode(lgt);

    ParallelogramLight* lgt = new ParallelogramLight();
    lgt->setBaseColor(Color(1.0f, 1.0f, 1.0f));
    lgt->setIntensity(10.0f);
    lgt->set(QVector3D(-0.24f, 1.58f, -0.22f), QVector3D(0.47f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.38f));
    scene->addNode(lgt);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadCornellDragon(Renderer *renderer)
{
    Config::Epsilon = 0.0001f;
    config->setPhotonMappingMaximumSearchRadius(0.0001f);

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(0.0f, 0.75, 3.0f), QVector3D(0.0f, 0.75f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.0f, 0.0f, 0.0f)));
    renderer->setImageSize(800, 600);

    MetalMaterial* metal = new MetalMaterial();
    metal->setN(0.37f);
    metal->setK(2.82f);
    metal->setTransmittedColor(Color(1.0f, 0.843f, 0.0f));

    DielectricMaterial* trans = new DielectricMaterial();
    trans->setN(2.0f);
    trans->setRoughness(0.0f);
    trans->setAbsorptionCoef(10.0f);
    trans->setAbsorptionColor(Color(0.0f, 0.8f, 0.0f));

    PlasticMaterial* plastic = new PlasticMaterial();
    plastic->setDiffuseColor(Color(194.f / 255, 0.0f, 0.0f));
    plastic->setN(1.5f);
    plastic->setRoughness(0.2f);

    AssimpLoader loader;
    QList<Instance*> meshes;
    loader.loadFile(config->sceneResourcesDir() + "Models/CornellBox/CornellBox-Empty.obj", meshes);
    QTime time;
    time.restart();
    for (Instance* mesh : meshes) {
        BoxTreeNode* tree = new BoxTreeNode;
        tree->construct(mesh->object<Mesh>());
        scene->addNode(tree);
    }
    logger->writeInfo(QString("Cornell Box Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    meshes.clear();
    loader.loadFile1(config->sceneResourcesDir() + "Models/Dragon/dragon.ply", meshes);
    time.restart();
    Mesh* dragon = meshes.first()->object<Mesh>();
    BoxTreeNode* tree = new BoxTreeNode;
    tree->construct(dragon);
    logger->writeInfo(QString("Dragon Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    Instance* inst = new Instance(tree);
    inst->setMaterial(trans);
    QMatrix4x4 mtx;
    mtx.scale(7);
    mtx.setColumn(3, QVector4D(0.0f, -0.25f, 0.0f, 1.0f));
    inst->setMatrix(mtx);
    scene->addNode(inst);

    ParallelogramLight* lgt = new ParallelogramLight();
    lgt->setBaseColor(Color(1.0f, 1.0f, 1.0f));
    lgt->setIntensity(15.0f);
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
    renderer->setImageSize(800, 600);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.8f, 0.8f, 1.0f)));

    AssimpLoader loader;
    QList<Instance*> meshes;
    loader.loadFile(config->sceneResourcesDir() + "Models/Dragon/dragon.ply", meshes);
    QTime time;
    time.restart();
    Mesh* dragon = meshes.first()->object<Mesh>();
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
    QList<Instance*> meshes;
    loader.loadFile(config->sceneResourcesDir() + "Models/Teapot/teapot.obj", meshes);

    QTime time;
    time.restart();

    QVector<Node*> teapot;
    for (Instance* mesh : meshes) {
        BoxTreeNode* tree = new BoxTreeNode;
        tree->construct(mesh->object<Mesh>());
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
    renderer->setImageSize(800, 600);

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_load4Dragons1Teapot(Renderer *renderer)
{
    Config::Epsilon = 0.01f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(0.0f, 2.0f, -5.7f), QVector3D(0.0f, 0.0f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.33f);
    renderer->setImageSize(800, 600);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.0f, 0.0f, 0.0f)));

    QVector<Node*> allSceneObjects;

    AssimpLoader loader;
    QList<Instance*> meshes;
    loader.loadFile(config->sceneResourcesDir() + "Models/Teapot/teapot.obj", meshes);
    QTime time;
    time.restart();
    BoxTreeNode* teapot = new BoxTreeNode;
    QVector<Node*> teapotList;
    for (Instance* mesh : meshes) {
        BoxTreeNode* tree = new BoxTreeNode;
        tree->construct(mesh->object<Mesh>());
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
    loader.loadFile(config->sceneResourcesDir() + "Models/Dragon/dragon.ply", meshes);
    time.restart();
    Mesh* dragon = meshes.first()->object<Mesh>();
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
    camera->lookAt(QVector3D(-0.7f, 1.2f, -4.5f), QVector3D(0.0f, 0.3f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.777f);
    renderer->setImageSize(960, 540);

    Scene* scene = new Scene;
    scene->setSky(new SphereSky(config->sceneResourcesDir() + "/Textures/sphereMap_joshua.jpg"));

    QVector<Node*> allSceneObjects;

    AssimpLoader loader;
    QList<Instance*> meshes;
    QTime time;
    QMatrix4x4 mtx;
    loader.loadFile(config->sceneResourcesDir() + "Models/Teapot/teapot.obj", meshes);
    time.restart();
    BoxTreeNode* teapot = new BoxTreeNode;
    QVector<Node*> teapotList;
    for (Instance* mesh : meshes) {
        BoxTreeNode* tree = new BoxTreeNode;
        tree->construct(mesh->object<Mesh>());
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
    loader.loadFile(config->sceneResourcesDir() + "Models/Dragon/dragon.ply", meshes);
    time.restart();
    Mesh* dragon = meshes.first()->object<Mesh>();
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
    sphereMat->setSpecularColor(Color(1.0f, 1.0f, 1.0f));
//    sphereMat->setSpecularColor(Color(0.44f, 0.92f, 0.46f));
    ImageTexture* roughness = new ImageTexture(config->sceneResourcesDir() + "/Textures/earth_white_black.jpg", 10000.0f);
    sphereMat->setRoughness(roughness, roughness);
    Sphere* sphere = new Sphere;
    sphere->setRadius(0.35f);
    sphere->setMaterial(sphereMat);
    Instance* sphereInst = new Instance(sphere);
    mtx.setToIdentity();
    mtx.translate(1.4f, -0.35f, -0.5f);
    mtx.rotate(290.0f, 0.0f, 1.0f, 0.0f);
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

void SceneGenerator::_loadRing(Renderer *renderer)
{
    Config::Epsilon = 0.0001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(-1.986f, 2.972f, 1.2f), QVector3D(-2.709f, 1.850f, 0.256f), config->yAxis());
    camera->set(40.0f, 1.33f);
    renderer->setImageSize(800, 600);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.0f, 0.0f, 0.0f)));

    AshikhminMaterial* ringMat = new AshikhminMaterial();
    ringMat->setDiffuseLevel(0.0f);
    ringMat->setSpecularLevel(1.0f);
    ringMat->setSpecularColor(Color(0.95f,0.7f,0.3f));
    ringMat->setRoughness(10000.0f,10000.0f);

    AssimpLoader loader;
    QList<Instance*> meshes;
    loader.loadFile(config->sceneResourcesDir() + "Models/Ring/scene.dae", meshes);
    QTime time;
    time.restart();

    AreaLight* light = new AreaLight;
    QVector<Triangle*> lightTris = meshes.at(0)->object<Mesh>()->triangles();
    light->setTriangles(lightTris);
    light->transform(meshes.at(0)->matrix());
    light->setBaseColor(Color::WHITE);
    light->setIntensity(20.0f);
    scene->addNode(light);

    BoxTreeNode* tree = new BoxTreeNode;
    tree->construct(meshes.at(1)->object<Mesh>());
    Instance* ground = new Instance(tree);
    ground->setMatrix(meshes.at(1)->matrix());
    scene->addNode(ground);

    tree = new BoxTreeNode;
    tree->construct(meshes.at(2)->object<Mesh>());
    Instance* ring = new Instance(tree);
    ring->setMaterial(ringMat);
    ring->setMatrix(meshes.at(2)->matrix());
    scene->addNode(ring);

    logger->writeInfo(QString("Scene Tree Construction : %1 ms").arg(QString::number(time.elapsed())));

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadMusic(Renderer *renderer)
{
    Config::Epsilon = 0.0001f;

    Camera* camera = renderer->camera();
    camera->lookAt(QVector3D(-100.0f, 40.0f, 100.0f), QVector3D(-40.0f, 40.0f, 0.0f), config->yAxis());
    camera->set(40.0f, 1.777f);
    renderer->setImageSize(800, 450);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.2f, 0.2f, 0.2f)));

    AssimpLoader loader;
    QList<Instance*> meshes;
    QList<Light*> lights;
    loader.loadFile(config->sceneResourcesDir() + "Models/Music/Music.dae", meshes, lights, camera);
    for (Light* lgt : lights) {
        lgt->setIntensity(2.5f);
        scene->addNode(lgt);
    }
    QTime time;
    time.restart();
    QVector<Node*> tmp;
    for (int i = 0; i < meshes.size(); ++i) {
        Instance* mesh = meshes[i];
        Mesh* toSet = mesh->object<Mesh>();
        toSet->smooth();
        toSet->generateTextureTangents();

        if ((i <= 35 || i >= 43)) {
            BoxTreeNode* tree = new BoxTreeNode;
            tree->construct(mesh->object<Mesh>());
            Instance* treeInst = new Instance(tree);
            treeInst->setMatrix(mesh->matrix());
            tmp << treeInst;
        } else {
//            AreaLight* light = new AreaLight;
//            QVector<Triangle*> lightTris = toSet->triangles();
//            light->setTriangles(lightTris);
//            light->transform(mesh->matrix());
//            light->setBaseColor(Color::WHITE);
//            light->setIntensity(50.0f);
//            scene->addNode(light);
        }
    }
    BoxTreeNode* tree = new BoxTreeNode;
    tree->construct(tmp);
    Instance* guitar = new Instance(tree);
    QMatrix4x4 mtx;
    guitar->setMatrix(mtx);
    scene->addNode(guitar);
    logger->writeInfo(QString("Scene Construction : %1 ms").arg(QString::number(time.elapsed())));

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

void SceneGenerator::_loadRoom(Renderer *renderer)
{
    Config::Epsilon = 0.1f;
    config->setPhotonMappingMaximumSearchRadius(100.0f);

    Camera* camera = renderer->camera();
    camera->set(40.0f, 1.777f);
    renderer->setImageSize(800, 450);

    Scene* scene = new Scene;
    scene->setSky(new UniformSky(Color(0.0f, 0.0f, 0.0f)));
//    scene->setSky(new SphereSky(config->sceneResourcesDir() + "/Textures/sphereMap_joshua.jpg"));

    AssimpLoader loader;
    QList<Instance*> meshes;
    QList<Light*> lights;
    loader.loadFile(config->sceneResourcesDir() + "Models/Appartment/Appartment.dae", meshes, lights, camera);
    for (Light* lgt : lights) {
        lgt->setIntensity(2.0f);
        scene->addNode(lgt);
    }
    QTime time;
    time.restart();
    QVector<Node*> tmp;
    for (int i = 0; i < meshes.size(); ++i) {
        Instance* mesh = meshes[i];
        Mesh* toSet = mesh->object<Mesh>();
        toSet->smooth();
        toSet->generateTextureTangents();

        if ((i <= 35 || i >= 43)) {
            BoxTreeNode* tree = new BoxTreeNode;
            tree->construct(mesh->object<Mesh>());
            Instance* treeInst = new Instance(tree);
            treeInst->setMatrix(mesh->matrix());
            tmp << treeInst;
        } else {
//            AreaLight* light = new AreaLight;
//            QVector<Triangle*> lightTris = toSet->triangles();
//            light->setTriangles(lightTris);
//            light->transform(mesh->matrix());
//            light->setBaseColor(Color::WHITE);
//            light->setIntensity(50.0f);
//            scene->addNode(light);
        }
    }
    BoxTreeNode* tree = new BoxTreeNode;
    tree->construct(tmp);
    Instance* all = new Instance(tree);
    QMatrix4x4 mtx;
    all->setMatrix(mtx);
    scene->addNode(all);
    logger->writeInfo(QString("Scene Construction : %1 ms").arg(QString::number(time.elapsed())));

    renderer->setScene(scene);
    renderer->setCamera(camera);
}

float SceneGenerator::_rangeRand(float min,float max) {
    return min+(max-min)*float(rand()) / float(RAND_MAX);
}
