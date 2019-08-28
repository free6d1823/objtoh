#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <vector>

#ifndef _SE_DATA_TYPE___
#define _SE_DATA_TYPE___
typedef struct _seFloat2D{
    float x;
    float y;
}seFloat2D;

typedef struct _seFloat3D{
    float x;
    float y;
    float z;
}seFloat3D;
#endif //_SE_DATA_TYPE___

bool loadOBJ(
    const char * path,
    std::vector<seFloat3D> & out_vertices,
    std::vector<seFloat2D> & out_uvs,
    std::vector<seFloat3D> & out_normals
);
//only vertex and normal are available
void indexVBO_noUv(
    std::vector<seFloat3D> & in_vertices,
    std::vector<seFloat3D> & in_normals,

    std::vector<unsigned short> & out_indices,
    std::vector<seFloat3D> & out_vertices,
    std::vector<seFloat3D> & out_normals
);
void indexVBO(
    std::vector<seFloat3D> & in_vertices,
    std::vector<seFloat2D> & in_uvs,
    std::vector<seFloat3D> & in_normals,

    std::vector<unsigned short> & out_indices,
    std::vector<seFloat3D> & out_vertices,
    std::vector<seFloat2D> & out_uvs,
    std::vector<seFloat3D> & out_normals
);
#endif // OBJLOADER_H
