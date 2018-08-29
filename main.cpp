#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <QVector>
#include "objloader.h"
using namespace std;
void usage(char* name)
{
    printf("Usage: %s <obj_file> <output_name>\n", name);
    printf("Convert a 3D OBJ file to header file\n");
}
char* gObjFile = NULL;
char* gOutFile = NULL;
void printVectors(vector<QVector3D> v, vector<QVector2D> u,
                  vector<QVector3D> n, vector<unsigned short> ind, FILE* fp);

int main(int argc, char *argv[])
{
    if (argc != 2)
        usage(argv[0]);
    gObjFile = argv[1];
    if (argc >=3)
        gOutFile = argv[2];

    std::vector<QVector3D> vertices;
    std::vector<QVector2D> uvs;
    std::vector<QVector3D> normals;
    bool res;
    res = loadOBJ(gObjFile, vertices, uvs, normals);
    if (!res) {
        perror("Fata error!! Check obj file.\n");
        return false;
    }

    vector<unsigned short> indexed_indices;
    vector<QVector3D> indexed_vertices;
    vector<QVector2D> indexed_uvs;
    vector<QVector3D> indexed_normals;
    indexVBO(vertices, uvs, normals, indexed_indices, indexed_vertices, indexed_uvs, indexed_normals);
    int m_nNumToDraw = indexed_indices.size();
    FILE* fp = fopen(gOutFile, "wt");
    if (fp == NULL)
        fp = stdout;
    printVectors(indexed_vertices, indexed_uvs, indexed_normals, indexed_indices, fp);
    if (fp != stdout)
        fclose(fp);
    return 0;
}

void printVectors(vector<QVector3D> v, vector<QVector2D> u,
                  vector<QVector3D> n, vector<unsigned short> ind, FILE* fp)
{

    fprintf(fp, "static float gVertex[%d][3] = {\n", (int)v.size());
    fprintf(fp, "\t};\n\n");
    fprintf(fp, "static float gTexture[%d][2] = {\n", (int)u.size());
    fprintf(fp, "\t};\n\n");
    fprintf(fp, "static float gNormal[%d][3] = {\n", (int)n.size());
    fprintf(fp, "\t};\n\n");
    fprintf(fp, "static float gIndices[%d] = {\n", (int)ind.size());
    fprintf(fp, "\t};\n\n");

}
