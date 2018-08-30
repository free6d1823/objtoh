#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <time.h>
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
                  vector<QVector3D> n, vector<unsigned short> ind, FILE* fp, const char* sourceName);

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

    FILE* fp = fopen(gOutFile, "wt");
    if (fp == NULL)
        fp = stdout;
    printVectors(indexed_vertices, indexed_uvs, indexed_normals, indexed_indices, fp, gObjFile);
    if (fp != stdout)
        fclose(fp);
    return 0;
}

void printVectors(vector<QVector3D> v, vector<QVector2D> u,
                  vector<QVector3D> n, vector<unsigned short> ind, FILE* fp,  const char* sourceName)
{
    unsigned int i;
    time_t T= time(NULL);
    struct  tm tm = *localtime(&T);

    fprintf(fp, "/*********************************************************************\n");
    fprintf(fp, " 3D Obj vertex data converted from %s  \n\n", sourceName);
    fprintf(fp, "\t static float mVertexObj[%d][3]: Vertex object data\n", (int)v.size());
    fprintf(fp, "\t static float mTextureObj[%d][2]: Textture coordinates\n", (int)u.size());
    fprintf(fp, "\t static float mNormalObj[%d][3]: Normal of the vertec\n", (int)n.size());
    fprintf(fp, "\t static unsigned short mIndicesObj[%d]: facelet vertex indx\n", (int)ind.size());
    fprintf(fp, "\t static unsigned int mNumToDraw = %lu: elements to draw;\n\n", ind.size());
    fprintf(fp, " Date: %04d/%02d/%02d %02d:%02d:%02d  \n", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(fp, " **********************************************************************/\n\n");
    fprintf(fp, "static float mVertexObj[%d][3] = {\n", (int)v.size());
    for (i=0; i< v.size(); i++)
        fprintf(fp,"\t{%f, %f, %f},\n", v[i].x(), v[i].y(), v[i].z());
    fprintf(fp, "\t};\n\n");

    fprintf(fp, "/****************************************************************/\n");
    fprintf(fp, "static float mTextureObj[%d][2] = {\n", (int)u.size());
    for ( i=0; i< u.size(); i++)
        fprintf(fp,"\t{%f, %f},\n", u[i].x(), u[i].y());
    fprintf(fp, "\t};\n\n");
    fprintf(fp, "/****************************************************************/\n");
    fprintf(fp, "static float mNormalObj[%d][3] = {\n", (int)n.size());
    for ( i=0; i< n.size(); i++)
        fprintf(fp,"\t{%f, %f, %f},\n", n[i].x(), n[i].y(), n[i].z());
    fprintf(fp, "\t};\n\n");

    fprintf(fp, "/*<! vertex-texture-normal index on facelets *******************/\n");
    fprintf(fp, "static unsigned short mIndicesObj[%d] = {\n", (int)ind.size());
    for (i=0; i< ind.size()/5; i++)
        fprintf(fp,"\t%u, %u, %u, %u, %u,\n", ind[i*5], ind[i*5+1], ind[i*5+2], ind[i*5+3], ind[i*5+4]);
    if (i*5 < ind.size()) {
        fprintf(fp, "\t");
        for (i=i*5; i< ind.size(); i++) {
            fprintf(fp, "%u, ", ind[i]);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "\t};\n\n");

    fprintf(fp, "/*<! number of elements to draw *********************************/\n");
    fprintf(fp, "static unsigned int mNumToDraw = %lu;\n\n", ind.size());
}
