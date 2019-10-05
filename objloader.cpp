
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <vector>
#include <map>

#include "objloader.h"

#define NO_VERTEX   0x01
#define NO_TEXTURE  0x02
#define NO_NORMAL   0x04
#define PARSER_ERROR    0xff
#define PARSER_OK   0x0

static int lineno = 0;

#define PARSER_PARAM(value, start, del, no_flag) {  \
    char* end; char temp[1024];                      \
    end = strchr(start,del);                         \
    if(end!=NULL || end != start) {                   \
        memcpy(temp, start, end-start);             \
        temp[end-start] = 0;                      \
        *value = atoi(temp);                         \
        start = end+1;                              \
    } else { mask |= no_flag;                       \
    }}                                              \

int parserFacelet(char* line, 
    unsigned int * pv0, unsigned int * pu0, unsigned int * pn0, 
    unsigned int * pv1, unsigned int * pu1, unsigned int * pn1, 
    unsigned int * pv2, unsigned int * pu2, unsigned int * pn2)
{
    int mask = PARSER_OK; 
    char* p1;

    p1 = strtok(line, " ");
    if (!p1)
        return PARSER_ERROR;
    //p1 = v/u/n
    PARSER_PARAM(pv0, p1, '/', NO_VERTEX);
    /* {  \
    char* end; char temp[1024];                      \
    end = strchr(p1,'/');                         \
    if(end!=NULL || end != p1) {                   \
printf("p1=%s, end=%s end-p1=%d", p1, end, end-p1);
        memcpy(temp, p1, end-p1);             \
        temp[end-p1] = 0;                      \
printf("temp =%s", temp);
        *pv0 = atoi(temp);                         \
printf(" pv0=%d\n", *pv0);
        p1 = end+1;                              \
    } else { mask |= NO_VERTEX;                       \
    }}                                              \
    */

    PARSER_PARAM(pu0, p1, '/', NO_TEXTURE);
    PARSER_PARAM(pn0, p1, 0x00, NO_NORMAL);
 
    p1 = strtok(NULL, " ");
    if (!p1)
        return PARSER_ERROR;
    PARSER_PARAM(pv1, p1, '/', NO_VERTEX);
    PARSER_PARAM(pu1, p1, '/', NO_TEXTURE);
    PARSER_PARAM(pn1, p1, 0, NO_NORMAL);
    p1 = strtok(NULL, " ");
    if (!p1)
        return PARSER_ERROR;
    PARSER_PARAM(pv2, p1, '/', NO_VERTEX);
    PARSER_PARAM(pu2, p1, '/', NO_TEXTURE);
    PARSER_PARAM(pn2, p1, 0, NO_NORMAL);
    
 
    return mask;
} 
bool parserThreeElements(char* line, float* x, float* y, float* z )
{
    char* p1;
    p1 = strtok(line, " ");
    if (!p1)
        return false;
    *x = atof(p1);
    p1 = strtok(NULL, " ");
    if (!p1)
        return false;
    *y = atof(p1);
    p1 = strtok(NULL, " ");
    if (!p1)
        return false;
    *z = atof(p1);

    if(*x == 0.0 && *y==0.0 && *z == 0.0){
        printf("error line no = %d\n", lineno);
    }
    return true;
}
bool parserTwoElements(char* line, float* u, float* v)
{
    char* p1;
    p1 = strtok(line, " ");
    if (!p1)
        return false;
    *u = atof(p1);
    p1 = strtok(NULL, " ");
    if (!p1)
        return false;
    *v = atof(p1);
    return true;
}


bool loadOBJ(
    const char * path,
    std::vector<seFloat3D> & out_vertices,
    std::vector<seFloat2D> & out_uvs,
    std::vector<seFloat3D> & out_normals
)
{
    printf("Loading OBJ file %s...\n", path);

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<seFloat3D> temp_vertices;
    std::vector<seFloat2D> temp_uvs;
    std::vector<seFloat3D> temp_normals;


    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file ! Are you in the right path ? \n");
         getchar();
        return false;
    }

    seFloat3D temp3d;
    seFloat2D temp2d;

    char line[1024];
    int vv = 0;
    int nn = 0;
    int tt = 0;
    int ff = 0;
    while( fgets(line, sizeof(line), file)  ){
        lineno ++;
        if (line[0] == 'v') {
            switch(line[1]) {
                case ' ': 
                     if(parserThreeElements(line+2, &temp3d.x, &temp3d.y, &temp3d.z )) {
                        vv ++;
      
                        temp_vertices.push_back(temp3d);
                    }
                    break;
                case 't': 
                    if(parserTwoElements(line+2, &temp2d.x, &temp2d.y )) { 
                        tt ++; 
         
                        temp2d.y = -temp2d.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
                        temp_uvs.push_back(temp2d);
                    }
                    break;
                case 'n': 
                    if(parserThreeElements(line+2, &temp3d.x, &temp3d.y, &temp3d.z )){ 
                        nn ++; 
                        temp_normals.push_back(temp3d);
                    }
                    break;

            }
        } else if (line[0] == 'f' && line[1] == ' ') {
            ff ++;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
    
            int matches = parserFacelet(line+2, 
                    &vertexIndex[0], &uvIndex[0], &normalIndex[0], 
                    &vertexIndex[1], &uvIndex[1], &normalIndex[1], 
                    &vertexIndex[2], &uvIndex[2], &normalIndex[2] ); 
            if ((matches & NO_VERTEX) == 0) {
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[1]);
                vertexIndices.push_back(vertexIndex[2]);
            }
            if ((matches & NO_TEXTURE) ==0) {
                uvIndices    .push_back(uvIndex[0]);
                uvIndices    .push_back(uvIndex[1]);
                uvIndices    .push_back(uvIndex[2]);
            }
            if ((matches & NO_NORMAL) == 0) {
                normalIndices.push_back(normalIndex[0]);
                normalIndices.push_back(normalIndex[1]);
                normalIndices.push_back(normalIndex[2]);
            }
        }
        continue;
    }
    printf ("-- vrt = %d, text = %d, normal = %d, facelet=%d\n", vv,tt,nn,ff);
    // For each vertex of each triangle
    for( unsigned int i=0; i<vertexIndices.size(); i++ ){

        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        if (vertexIndex<= temp_vertices.size()) {
            seFloat3D vertex = temp_vertices[ vertexIndex-1 ];
            // Put the attributes in buffers
            out_vertices.push_back(vertex);
        } else {
            fprintf(stderr, "vertex index %d out of vertex tables size %d \n", vertexIndex, (int)temp_vertices.size());         
        }
    }

    for( unsigned int i=0; i<uvIndices.size(); i++ ){
        unsigned int uvIndex = uvIndices[i];
        if (uvIndex<= temp_uvs.size()) {

            seFloat2D uv = temp_uvs[ uvIndex-1 ];
            out_uvs     .push_back(uv);
        }else {
            fprintf(stderr, "texture index %d out of uv tables size %d \n", uvIndex, (int)temp_uvs.size());                             
        }
    }

    for( unsigned int i=0; i<normalIndices.size(); i++ ){
        unsigned int normalIndex = normalIndices[i];
            if (normalIndex<= temp_normals.size()) {

            seFloat3D normal = temp_normals[ normalIndex-1 ];
            out_normals .push_back(normal);
        }else {
            fprintf(stderr, "norm index %d out of norm table %d \n", normalIndex, (int)temp_normals.size());                             
        }
    }
    fclose(file);
    return true;
}

struct PackedVertex{
    seFloat3D position;
    seFloat2D uv;
    seFloat3D normal;
    bool operator<(const PackedVertex that) const{
        return memcmp((void*)(this), (void*)&that, sizeof(PackedVertex))>0;
    }
};
//no UV data
struct PackedVertex2{
    seFloat3D position;
    seFloat3D normal;
    bool operator<(const PackedVertex2 that) const{
        return memcmp((void*)(this), (void*)&that, sizeof(PackedVertex2))>0;
    }
};

bool getSimilarVertexIndex_fast(
    PackedVertex & packed,
    std::map<PackedVertex,unsigned short> & VertexToOutIndex,
    unsigned short & result
){
    std::map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
    if ( it == VertexToOutIndex.end() ){
        return false;
    }else{
        result = it->second;
        return true;
    }
}
bool getSimilarVertexIndex2(
    PackedVertex2 & packed,
    std::map<PackedVertex2,unsigned short> & VertexToOutIndex,
    unsigned short & result
){
    std::map<PackedVertex2,unsigned short>::iterator it = VertexToOutIndex.find(packed);
    if ( it == VertexToOutIndex.end() ){
        return false;
    }else{
        result = it->second;
        return true;
    }
}
//only vertex and normal are available
void indexVBO_noUv(
    std::vector<seFloat3D> & in_vertices,
    std::vector<seFloat3D> & in_normals,

    std::vector<unsigned short> & out_indices,
    std::vector<seFloat3D> & out_vertices,
    std::vector<seFloat3D> & out_normals
){
    std::map<PackedVertex2,unsigned short> VertexToOutIndex;

    PackedVertex2 packed2;
    // For each input vertex
    for ( unsigned int i=0; i<in_vertices.size(); i++ ){
        //    packed2 = {in_vertices[i], in_normals[i]} ;
        packed2.position = in_vertices[i];
        packed2.normal = in_normals[i];

        // Try to find a similar vertex in out_XXXX
        unsigned short index;
        bool found = getSimilarVertexIndex2( packed2, VertexToOutIndex, index);

        if ( found ){ // A similar vertex is already in the VBO, use it instead !
            out_indices.push_back( index );
        }else{ // If not, it needs to be added in the output data.
            out_vertices.push_back( in_vertices[i]);
            out_normals .push_back( in_normals[i]);
            unsigned short newindex = static_cast<unsigned short>(out_vertices.size() - 1);
            out_indices .push_back( newindex );
            VertexToOutIndex[ packed2 ] = newindex;
        }
    }
}

void indexVBO(
    std::vector<seFloat3D> & in_vertices,
    std::vector<seFloat2D> & in_uvs,
    std::vector<seFloat3D> & in_normals,

    std::vector<unsigned short> & out_indices,
    std::vector<seFloat3D> & out_vertices,
    std::vector<seFloat2D> & out_uvs,
    std::vector<seFloat3D> & out_normals
){
    std::map<PackedVertex,unsigned short> VertexToOutIndex;

    PackedVertex packed;
    // For each input vertex
    for ( unsigned int i=0; i<in_vertices.size(); i++ ){

        packed.position = in_vertices[i];
        packed.uv = in_uvs[i];
        packed.normal = in_normals[i];


        // Try to find a similar vertex in out_XXXX
        unsigned short index;
        bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);
        
        if ( found ){ // A similar vertex is already in the VBO, use it instead !
            out_indices.push_back( index );
        }else{ // If not, it needs to be added in the output data.
            out_vertices.push_back( in_vertices[i]);
            out_uvs.push_back( in_uvs[i]);
            out_normals .push_back( in_normals[i]);
            unsigned short newindex = static_cast<unsigned short>(out_vertices.size() - 1);
            out_indices .push_back( newindex );
            VertexToOutIndex[ packed ] = newindex;
        }
    }
}
