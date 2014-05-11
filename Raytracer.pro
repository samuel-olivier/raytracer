#-------------------------------------------------
#
# Project created by QtCreator 2014-04-03T16:27:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Raytracer
TEMPLATE = app


SOURCES += Sources/main.cpp\
    Sources/MainWindow.cpp \
    Sources/Renderer.cpp \
    Sources/Mesh.cpp \
    Sources/Node.cpp \
    Sources/Scene.cpp \
    Sources/Material.cpp \
    Sources/Triangle.cpp \
    Sources/Ray.cpp \
    Sources/Intersection.cpp \
    Sources/Box.cpp \
    Sources/Vertex.cpp \
    Sources/Config.cpp \
    Sources/Camera.cpp \
    Sources/Instance.cpp \
    Sources/Light.cpp \
    Sources/PointLight.cpp \
    Sources/LambertMaterial.cpp \
    Sources/Utils.cpp \
    Sources/DirectionalLight.cpp \
    Sources/Color.cpp \
    Sources/Sphere.cpp \
    Sources/Plane.cpp \
    Sources/AssimpLoader.cpp \
    Sources/CustomMesh.cpp \
    Sources/DielectricMaterial.cpp \
    Sources/AreaLight.cpp \
    Sources/ParallelogramLight.cpp \
    Sources/BoundingBox.cpp \
    Sources/BoxTreeNode.cpp \
    Sources/SceneGenerator.cpp \
    Sources/Console.cpp \
    Sources/Logger.cpp \
    Sources/MetalMaterial.cpp \
    Sources/PlasticMaterial.cpp \
    Sources/AshikhminMaterial.cpp \
    Sources/Texture.cpp \
    Sources/ImageTexture.cpp \
    Sources/Sky.cpp \
    Sources/UniformSky.cpp \
    Sources/SphereSky.cpp

HEADERS  += Sources/MainWindow.h \
    Sources/Renderer.h \
    Sources/Ray.h \
    Sources/Scene.h \
    Sources/Triangle.h \
    Sources/Mesh.h \
    Sources/Node.h \
    Sources/Material.h \
    Sources/Intersection.h \
    Sources/Box.h \
    Sources/Vertex.h \
    Sources/Config.h \
    Sources/Camera.h \
    Sources/Instance.h \
    Sources/Light.h \
    Sources/PointLight.h \
    Sources/LambertMaterial.h \
    Sources/Utils.h \
    Sources/DirectionalLight.h \
    Sources/Color.h \
    Sources/Sphere.h \
    Sources/Plane.h \
    Sources/AssimpLoader.h \
    Sources/CustomMesh.h \
    Sources/DielectricMaterial.h \
    Sources/AreaLight.h \
    Sources/ParallelogramLight.h \
    Sources/BoundingBox.h \
    Sources/BoxTreeNode.h \
    Sources/SceneGenerator.h \
    Sources/Console.h \
    Sources/Logger.h \
    Sources/MetalMaterial.h \
    Sources/PlasticMaterial.h \
    Sources/AshikhminMaterial.h \
    Sources/Texture.h \
    Sources/UniformTexture.hpp \
    Sources/ImageTexture.h \
    Sources/Sky.h \
    Sources/UniformSky.h \
    Sources/SphereSky.h

FORMS    += Forms/MainWindow.ui

LIBS += -L"c:/Program Files/Assimp/lib/x64/" -lassimp

INCLUDEPATH += "c:/Program Files/Assimp/include"
DEPENDPATH += "c:/Program Files/Assimp/include"

RESOURCES += \
    ui.qrc
