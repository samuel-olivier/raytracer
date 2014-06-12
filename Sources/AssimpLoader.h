#ifndef ASSIMPLOADER_H
#define ASSIMPLOADER_H

#include <QString>
#include <QVector>
#include <QMatrix4x4>

#include <cstring>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

class Instance;
class Material;
class Camera;
class Light;
class Texture;

class AssimpLoader
{
public:
    AssimpLoader();
    void loadFile(QString const& filename, QList<Instance *> &meshes, Camera* camera = 0);
    void loadFile(QString const& filename, QList<Instance *> &meshes, QList<Light *> &lights, Camera* camera = 0);
    void loadFile1(QString const& filename, QList<Instance *> &meshes);

private:
    void    _loadMaterials(const aiScene* scene);
    void    _loadAssimpNode(const aiScene* scene, aiNode* assimpNode, int& numTriangles, QList<Instance *> &meshes, QList<Light *> &lights, Camera* camera, QMatrix4x4 const& parent = QMatrix4x4());

    QMatrix4x4  _toQMatrix(aiMatrix4x4 const& aiMtx) const;
    QVector3D   _toQVector(aiVector3D const& v) const;
    Assimp::Importer*   _importer;
    QVector<QPair<Texture*, Material*>>  _materials;
    QString             _baseDir;
};

#endif // ASSIMPLOADER_H
