#version 330 core

layout(location = 0) out vec4 fragColor;

in vec3 vNormal;
in vec3 vViewSpacePos;

// Light direction might have to be passed by the vertex shader to 
// be interpolated. But with directional light, it should be fine. 
uniform vec3 light_direction;
uniform vec3 k_diffuse;
uniform vec3 k_specular;
uniform vec3 k_ambient;

uniform float light_intensity;
uniform float ambient_intensity;
uniform float shininess;

void main()
{
	vec3 N = normalize(vNormal);
	vec3 omega = normalize(-light_direction);
	vec3 view_direction = normalize(-vViewSpacePos);
	vec3 h = normalize(omega + view_direction);
	vec3 diffuse = k_diffuse * max(dot(N, omega), 0.0);
	vec3 specular = k_specular * pow(max(dot(N, h), 0.0), shininess);
	if (dot(N, omega) < 0.0)
	{
		specular = vec3(0.0);
	}
	vec3 color = ambient_intensity * k_ambient + light_intensity * (diffuse + specular);
	fragColor = vec4(color, 1.0);
}
