#ifndef ASSIMPLOADER_H
#define ASSIMPLOADER_H

#include <QString>
#include <QVector>

#include <cstring>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

class Mesh;
class Material;

class AssimpLoader
{
public:
    AssimpLoader();
    void loadFile(QString const& filename, QList<Mesh*>& meshes);
    void loadFile1(QString const& filename, QList<Mesh*>& meshes);

private:
    void    _loadMaterials(const aiScene* scene);
    void    _loadAssimpNode(const aiScene* scene, aiNode* assimpNode, int& numTriangles, QList<Mesh*>& meshes);

    Assimp::Importer*   _importer;
    QVector<Material*>  _materials;
    QString             _baseDir;
};

#endif // ASSIMPLOADER_H
