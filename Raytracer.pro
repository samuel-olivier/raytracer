#-------------------------------------------------
#
# Project created by QtCreator 2014-04-03T16:27:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Raytracer
TEMPLATE = app


SOURCES += main.cpp\
    MainWindow.cpp \
    Renderer.cpp \
    Mesh.cpp \
    Node.cpp \
    Scene.cpp \
    Material.cpp \
    Triangle.cpp \
    Ray.cpp \
    Intersection.cpp \
    Box.cpp \
    Vertex.cpp \
    Config.cpp \
    Camera.cpp \
    Instance.cpp \
    Light.cpp \
    PointLight.cpp \
    LambertMaterial.cpp \
    Utils.cpp \
    DirectionalLight.cpp \
    Color.cpp \
    Sphere.cpp \
    Plane.cpp \
    CustomMaterial.cpp \
    AssimpLoader.cpp \
    CustomMesh.cpp \
    DielectricMaterial.cpp \
    AreaLight.cpp \
    ParallelogramLight.cpp \
    BoundingBox.cpp \
    BoxTreeNode.cpp \
    SceneGenerator.cpp \
    Console.cpp \
    Logger.cpp \
    MetalMaterial.cpp \
    PlasticMaterial.cpp \
    AshikhminMaterial.cpp

HEADERS  += MainWindow.h \
    Renderer.h \
    Ray.h \
    Scene.h \
    Triangle.h \
    Mesh.h \
    Node.h \
    Material.h \
    Intersection.h \
    Box.h \
    Vertex.h \
    Config.h \
    Camera.h \
    Instance.h \
    Light.h \
    PointLight.h \
    LambertMaterial.h \
    Utils.h \
    DirectionalLight.h \
    Color.h \
    Sphere.h \
    Plane.h \
    CustomMaterial.h \
    AssimpLoader.h \
    CustomMesh.h \
    DielectricMaterial.h \
    AreaLight.h \
    ParallelogramLight.h \
    BoundingBox.h \
    BoxTreeNode.h \
    SceneGenerator.h \
    Console.h \
    Logger.h \
    MetalMaterial.h \
    PlasticMaterial.h \
    AshikhminMaterial.h

FORMS    += MainWindow.ui

LIBS += -L"c:/Program Files/Assimp/lib/x64/" -lassimp

INCLUDEPATH += "c:/Program Files/Assimp/include"
DEPENDPATH += "c:/Program Files/Assimp/include"
