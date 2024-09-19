#version 330 core

layout(location = 0) out vec4 fragColor;

in vec3 vNormal;
in vec3 vViewSpacePos;
in vec2 vTexCoord;

uniform sampler2D tex;
uniform sampler2D specular_map;

// Light direction might have to be passed by the vertex shader to 
// be interpolated. But with directional light, it should be fine. 
uniform vec3 light_direction;

uniform vec3 intensity_k_diffuse;
uniform vec3 intensity_k_specular;
uniform vec3 intensity_k_ambient;

uniform float shininess;

void main()
{
	// Texture
	vec4 texColor = texture(tex, vTexCoord);
	vec4 specularColor = texture(specular_map, vTexCoord);

	vec3 diffuse = intensity_k_diffuse * texColor.rgb;
	vec3 specular = intensity_k_specular * specularColor.rgb;
	vec3 ambient = intensity_k_ambient * texColor.rgb;

	// Shading
	vec3 N = normalize(vNormal);
	vec3 omega = normalize(-light_direction);
	vec3 view_direction = normalize(-vViewSpacePos);
	vec3 h = normalize(omega + view_direction);
	
	vec3 diffuse_shade = diffuse * max(dot(N, omega), 0.0);
	vec3 specular_shade = specular * pow(max(dot(N, h), 0.0), shininess);
	if (dot(N, omega) < 0.0)
	{
		specular_shade = vec3(0.0);
	}
	vec3 color = ambient + diffuse_shade + specular_shade;
	fragColor = vec4(color, 1.0);
}
