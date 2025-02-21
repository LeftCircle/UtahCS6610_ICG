#ifndef MODEL_H
#define MODEL_H

#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyMatrix.h"
#include "rcCodeBase/rcObjModifier.h"

#include <vector>

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
	std::vector<rc::rcTriMeshForGL> meshes;

public:
	Model();
	bool load_model(std::string filename);

}

#endif