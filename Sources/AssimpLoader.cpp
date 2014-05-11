#include "AssimpLoader.h"

#include <QVector>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#include <assimp/postprocess.h>

#include "CustomMesh.h"
#include "Triangle.h"
#include "Vertex.h"
#include "LambertMaterial.h"
#include "Logger.h"
#include "ImageTexture.h"

AssimpLoader::AssimpLoader()
    : _importer(0)
{
}

void AssimpLoader::loadFile(const QString &filename, QList<Mesh *> &meshes)
{
    _importer = new Assimp::Importer;
    logger->showMessage("Opening File");
    const aiScene* scene = _importer->ReadFile(filename.toStdString(),
                                               aiProcess_Triangulate
                                               | aiProcess_GenSmoothNormals
                                               | aiProcess_FixInfacingNormals);
//    const aiScene* scene = _importer->ReadFile(filename.toStdString(),
//                                               aiProcess_Triangulate);

    QFileInfo fileInfo(filename);
    _baseDir = fileInfo.dir().path();
    if(!scene) {
        logger->writeError(QString("Could not load file : %1").arg(_importer->GetErrorString()));
        return ;
    }
    logger->showMessage(QString("%1 : Loading Materials...").arg(filename));
    _loadMaterials(scene);
    int numTriangles = 0;
    logger->showMessage(QString("%1 : Loading Meshes...").arg(filename));
    _loadAssimpNode(scene, scene->mRootNode, numTriangles, meshes);
    logger->writeSuccess(QString("File loaded : %1 meshes | %2 triangles").arg(QString::number(meshes.size())).arg(QString::number(numTriangles)));
    logger->showMessage(QString("%1 : Loaded").arg(filename));
//    for (Mesh* mesh : meshes) {
//        mesh->smooth();
//    }
}

void AssimpLoader::loadFile1(const QString &filename, QList<Mesh *> &meshes)
{
    // Open file
    logger->showMessage("Opening File");
    FILE *f=fopen(filename.toStdString().c_str(),"r");
    if(f==0) {
        logger->writeError(QString("Could not load file : Cannot open file"));
        return ;
    }

    logger->showMessage(QString("%1 : Loading Meshes...").arg(filename));
    // Read header
    char tmp[256];
    int numverts=0,numtris=0;
    int posprop=-99,normprop=-99;
    int props=0;
    while(1) {
        fgets(tmp,256,f);
        if(strncmp(tmp,"element vertex",14)==0)
            numverts=atoi(&tmp[14]);
        if(strncmp(tmp,"element face",12)==0)
            numtris=atoi(&tmp[12]);
        if(strncmp(tmp,"property",8)==0) {
            int len=strlen(tmp);
            if(strncmp(&tmp[len-3]," x",2)==0) posprop=props;
            if(strncmp(&tmp[len-3],"nx",2)==0) normprop=props;
            props++;
        }
        if(strcmp(tmp,"end_header\n")==0) break;
    }
    if(posprop==-1) {
        logger->writeError(QString("Could not load file : No vertex positions found"));
        fclose(f);
        return ;
    }

    // Read verts
    int i=0;
    int NumVertexes=numverts;
    Vertex** Vertexes=new Vertex*[NumVertexes];
    int NumTriangles;
    if(numverts>0) {

        for(i=0;i<NumVertexes;i++) {
            fgets(tmp,256,f);
            char *pch=strtok(tmp," ");
            int prop=0;
            Vertexes[i] = new Vertex();
            while(pch) {
                if(prop==posprop) Vertexes[i]->position.setX(float(atof(pch)));
                if(prop==posprop+1) Vertexes[i]->position.setY(float(atof(pch)));
                if(prop==posprop+2) Vertexes[i]->position.setZ(float(atof(pch)));
                if(prop==normprop) Vertexes[i]->normal.setX(float(atof(pch)));
                if(prop==normprop+1) Vertexes[i]->normal.setY(float(atof(pch)));
                if(prop==normprop+2) Vertexes[i]->normal.setZ(float(atof(pch)));
                pch=strtok(0," ");
                prop++;
            }
        }
    }

    // Read tris
    Triangle** Triangles;
    if(numtris>0) {
        Material* mtl=new LambertMaterial;
        NumTriangles=numtris;
        Triangles=new Triangle*[numtris];
        for(i=0;i<numtris;i++) {
            int count,i0,i1,i2;
            fscanf(f,"%d %d %d %d\n",&count,&i0,&i1,&i2);
            if(count!=3) {
                logger->writeError(QString("Could not load file : Only triangles are supported"));
                fclose(f);
                return ;
            }
            Triangles[i] = new Triangle(Vertexes[i0], Vertexes[i1], Vertexes[i2], mtl);
        }
    }
    CustomMesh* res = new CustomMesh();
    QVector<Triangle*> mesh(NumTriangles);
    for (int bla = 0; bla < NumTriangles; ++bla) {
        mesh[bla] = Triangles[bla];
    }
    res->setTriangles(mesh);
    res->smooth();
    res->generateTangents();
    meshes.append(res);
    // Close file
    fclose(f);
    logger->writeSuccess(QString("File loaded : 1 mesh | %1 triangles").arg(QString::number(numtris)));
    logger->showMessage(QString("%1 : Loaded").arg(filename));
}

void AssimpLoader::_loadMaterials(const aiScene *scene)
{
    if (!scene->HasMaterials()) {
        return ;
    }
    _materials.resize(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial* mtl = scene->mMaterials[i];
        LambertMaterial* mat = new LambertMaterial;
        aiString path;
        aiColor4D diffuse;
        //        aiColor4D specular;
        //        aiColor4D ambient;
        //        aiColor4D emission;
//        float shininess, strength, opacity;
//        int max;

        if (aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse) == AI_SUCCESS) {
            mat->setDiffuseColor(Color(diffuse.r, diffuse.g, diffuse.b));
        }
        //        if(aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular) == AI_SUCCESS) {
        //            mat->setSpecularColor(Color(specular.r, specular.g, specular.b));
        //        }
        //        if(aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient) == AI_SUCCESS) {
        //            mat->setAmbientColor(Color(ambient.r, ambient.g, ambient.b));
        //        }
        //        if(aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission) == AI_SUCCESS) {
        //            mat->setEmissiveColor(Color(emission.r, emission.g, emission.b));
        //        }
        //        max = 1;
        //        if (aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max) == AI_SUCCESS) {
        //            mat->setShininess(shininess);
        //        }
        //        max = 1;
        //        if (aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max) == AI_SUCCESS) {
        //            mat->setShininessStrength(strength);
        //        }
        if (mtl->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
            ImageTexture* t = new ImageTexture(_baseDir + "/" + path.C_Str());
            if (t->hasImage()) {
                mat->setDiffuseColor(t);
            } else {
                delete t;
            }
        }
        _materials[i] = mat;
    }
}

void AssimpLoader::_loadAssimpNode(const aiScene *scene, aiNode *assimpNode, int &numTriangles, QList<Mesh *> &meshes)
{
    for (uint i = 0; i < assimpNode->mNumMeshes; ++i) {
        aiMesh* assimpMesh = scene->mMeshes[assimpNode->mMeshes[i]];

        if (!(assimpMesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) ||
                assimpMesh->mNumVertices <= 0)
            continue ;

        CustomMesh* mesh = new CustomMesh;
        meshes << mesh;
        mesh->setName(assimpMesh->mName.data);

        QVector<Vertex*> vertices(assimpMesh->mNumVertices, 0);
        for (uint j = 0; j < assimpMesh->mNumVertices; ++j) {
            Vertex* v = new Vertex;
            v->position = QVector3D(assimpMesh->mVertices[j].x,
                                    assimpMesh->mVertices[j].y,
                                    assimpMesh->mVertices[j].z);
            if (assimpMesh->mNormals) {
                v->normal = QVector3D(assimpMesh->mNormals[j].x,
                                      assimpMesh->mNormals[j].y,
                                      assimpMesh->mNormals[j].z);
            }
            if (assimpMesh->mTextureCoords[0]) {
                v->texCoord = QVector2D(assimpMesh->mTextureCoords[0][j].x,
                        assimpMesh->mTextureCoords[0][j].y);
            }
            vertices[j] = v;
        }
        numTriangles += assimpMesh->mNumFaces;
        QVector<Triangle*> triangles(assimpMesh->mNumFaces, 0);
        for (uint j = 0; j < assimpMesh->mNumFaces; ++j) {
            if (assimpMesh->mFaces[j].mNumIndices == 3) {
                Triangle* triangle = new Triangle(vertices[assimpMesh->mFaces[j].mIndices[0]],
                        vertices[assimpMesh->mFaces[j].mIndices[1]],
                        vertices[assimpMesh->mFaces[j].mIndices[2]]);
                if (assimpMesh->mMaterialIndex >= 0 && assimpMesh->mMaterialIndex < (unsigned int)_materials.size()) {
                    triangle->setMaterial(_materials[assimpMesh->mMaterialIndex]);
                }
                triangles[j] = triangle;
            }
        }
        mesh->setTriangles(triangles);
    }
    for (uint i = 0; i < assimpNode->mNumChildren; ++i) {
        _loadAssimpNode(scene, assimpNode->mChildren[i], numTriangles, meshes);
    }
}
