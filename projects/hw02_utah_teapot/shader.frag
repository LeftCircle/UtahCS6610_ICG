#version 330 core

layout(location = 0) out vec4 fragColor;

in vec3 vColor;

void main()
{
	color = vec4(vColor, 1.0);
}
