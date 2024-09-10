# version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 vColor;

uniform mat4 mvp;
uniform mat4 mv_points;
uniform mat3 mv_normals;

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
	vColor = normalize((mv_normals * normal));
}