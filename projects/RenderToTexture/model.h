#ifndef MODEL_H
#define MODEL_H

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

#endif