#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D renderTex;
const vec3 planeColor = vec3(0.1, 0.1, 0.1); // Small constant to add

void main()
{
    vec4 texColor = texture(renderTex, TexCoord);
    FragColor = texColor + vec4(planeColor, 0.0);
}