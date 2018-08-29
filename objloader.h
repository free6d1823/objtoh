#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <vector>
#include <QVector2D>
#include <QVector3D>

bool loadOBJ(
    const char * path,
    std::vector<QVector3D> & out_vertices,
    std::vector<QVector2D> & out_uvs,
    std::vector<QVector3D> & out_normals
);
void indexVBO(
    std::vector<QVector3D> & in_vertices,
    std::vector<QVector2D> & in_uvs,
    std::vector<QVector3D> & in_normals,

    std::vector<unsigned short> & out_indices,
    std::vector<QVector3D> & out_vertices,
    std::vector<QVector2D> & out_uvs,
    std::vector<QVector3D> & out_normals
);
#endif // OBJLOADER_H
