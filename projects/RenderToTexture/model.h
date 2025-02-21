#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>


#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyMatrix.h"
#include "cyCodeBase/cyGL.h"
#include "rcCodeBase/rcObjModifier.h"



// Plane vertices (position and texture coordinates)
const float planeVertices[] = {
    // positions        // texture coords
    -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
     1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,   1.0f, 1.0f
};

const unsigned int planeIndices[] = {
    0, 1, 2,
    0, 2, 3
};

class Model{
private:
	std::vector<rc::rcTriMeshForGL> _meshes;
    std::vector<std::vector<rc::MaterialGroup>> _materials;
    std::vector<cyMatrix4f> _transforms;


public:
	

private:
    void _init_points_from_mesh(rc::rcTriMeshForGL& mesh);

public:
    Model();
	
    bool load_model(const char* filename);


}

#endif