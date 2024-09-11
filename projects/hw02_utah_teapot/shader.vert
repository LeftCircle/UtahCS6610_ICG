# version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

// output values that will be interpolated per-fragment
out vec3 vNormal;
out vec3 vViewSpacePos; // FE in Angel's book

uniform mat4 mvp;
uniform mat4 mv_points;
uniform mat3 mv_normals;

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
	vNormal = normalize(mv_normals * normal);
	vViewSpacePos = vec3(mv_points * vec4(position, 1.0));
}