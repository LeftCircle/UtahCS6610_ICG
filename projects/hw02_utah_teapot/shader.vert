# version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 vColor;

uniform mat4 mvp;
uniform mat4 mv_points;
uniform mat3 mv_normals;

uniform vec3 light_direction;
uniform vec3 k_diffuse;
uniform vec3 k_specular;
uniform vec3 k_ambient;

uniform float light_intensity;
uniform float shininess;

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
	vec3 vNormal = normalize(mv_normals * normal);
	vec3 view_position = vec3(mv_points * vec4(position, 1.0));
	vec3 view_direction = normalize(-view_position);
	// Vector from the vertex to the light. Since we have directional light 
	// we can assume that the light is at infinity.
	vec3 omega = -light_direction;
	// halfway vector between light direction and the view direction
	vec3 h = normalize(omega + view_direction);
	
	// Not sure if this should occur here or in the fragment shader. Probably fragment?
	vec3 specular = k_specular * pow(max(dot(vNormal, h), 0.0), shininess);
	vec3 diffuse = k_diffuse * max(dot(vNormal, omega), 0.0);
	
	vColor = light_intensity * (diffuse + specular) + k_ambient;
}