#version 330 core

layout(location = 0) out vec4 fragColor;

in vec3 vNormal;
in vec3 vViewSpacePos;
in vec2 vTexCoord;

uniform sampler2D tex;

// Flags to see if the different textures exist
uniform bool has_map_Ka;
uniform bool has_map_Kd;
uniform bool has_map_Ks;

// textures
uniform sampler2D map_Ka;
uniform sampler2D map_Kd;
uniform sampler2D map_Ks;

// colors
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;


// Light direction might have to be passed by the vertex shader to 
// be interpolated. But with directional light, it should be fine. 
uniform vec3 light_direction;
uniform float shininess;

void main()
{
	// start by loading the textures
	vec4 ambient = vec4(1.0);
	vec4 diffuse = vec4(1.0); 
	vec4 specular = vec4(1.0);
	if (has_map_Ka) {
		ambient = texture(map_Ka, vTexCoord);
	}
	if (has_map_Kd) {
		diffuse = texture(map_Kd, vTexCoord);
	}
	if (has_map_Ks) {
		specular = texture(map_Ks, vTexCoord);
	}

	// Now adjust for ka, kd, ks
	ambient.rgb *= Ka;
	diffuse.rgb *= Kd;
	specular.rgb *= Ks;

	// Shading
	vec3 N = normalize(vNormal);
	vec3 omega = normalize(-light_direction);
	vec3 view_direction = normalize(-vViewSpacePos);
	vec3 h = normalize(omega + view_direction);

	vec3 diffuse_shade = diffuse.rgb * max(dot(N, omega), 0.0);
	vec3 specular_shade = specular.rgb * pow(max(dot(N, h), 0.0), shininess);
	if (dot(N, omega) < 0.0)
	{
		specular_shade = vec3(0.0);
	}
	vec3 color = ambient.rgb + diffuse_shade + specular_shade;
	fragColor = vec4(color, 1.0);
}
