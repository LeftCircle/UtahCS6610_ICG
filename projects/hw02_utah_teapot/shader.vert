# version 330 core

layout(location = 0) in vec3 position;
//layout(location = 0) in vec4 position;

layout(location = 1) in vec3 clr;

out vec3 vColor;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
	//gl_Position = mvp * position / position.w;
	vColor = clr;
}