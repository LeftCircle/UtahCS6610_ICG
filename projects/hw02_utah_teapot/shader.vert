# version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 mvp;
uniform mat4 mv_points;
uniform mat3 mv_normals;

out vec3 vNormal;
out vec3 vViewDir;

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
	vNormal = normalize(mv_normals * normal);
	vec3 view_position = vec3(mv_points * vec4(position, 1.0));
	vViewDir = normalize(-view_position);
}