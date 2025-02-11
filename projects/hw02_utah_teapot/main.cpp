/*
This is an OpenGL program that loads a teapot object and displays it on the screen.
This program demonstrates:
  - Converting an obj file to a format that can be used with OpenGL
  - Processing vertex, normal, and texture data to be used in an element buffer
  - Blinn Phong shading with ambient, diffuse, and specular lighting
  - A camera that can be rotated around the object
  - A light that can be rotated around the object

Keyboard functionality:
  - F6: Recompile shaders
  - ESC: Close the window
  - Arrow keys: Rotate the light

Mouse functionality:
  - Left click and drag: Rotate the camera
  - Scroll wheel: Zoom in and out

Usage:
  This project was created with Visual Studio.
  lodepng.cpp is required to load textures.

*/
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "cyCodeBase/cyMatrix.h"
#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyGL.h"

#include "rcCodeBase/rcCamera.hpp"
#include "rcCodeBase/rcOpenGLScene.hpp"
#include "rcCodeBase/rcCore.hpp"
#include "rcCodeBase/rcObjModifier.h"
#include "rcCodeBase/rcLights.hpp"
#include "rcCodeBase/rcTexture.h"

#include <iostream>
#include <vector>
#include <map>
#include <filesystem>

using namespace rc;

#define NULL_ID -1

const char* default_obj_path = "yoda.obj";
//const char* asset_path = "";
//const char* default_obj_path = "teapot.obj";
const char* asset_path = "";
const float scale = 0.01f;


const char* filename;

GLuint test_global_vao = NULL_ID;

std::filesystem::path dir = std::filesystem::current_path();
std::vector<rc::MaterialGroup> material_groups;
GLScene scene;
rc::SphericalDirectionalLight light;

// Create a bitflag for different lighting types
enum ShadingType
{
	AMBIENT = 1 << 0,
	DIFFUSE = 1 << 1,
	SPECULAR = 1 << 2
};
unsigned int shading_flag = ShadingType::AMBIENT | ShadingType::DIFFUSE | ShadingType::SPECULAR;


// OpenGL function that adjusts theta and phi based on mouse movement when the left mouse button is pressed
void rotate_light(int dx, int dy)
{
	light.add_rotation_and_get_direction(-dx, -dy);
	scene.program.SetUniform("light_direction", light.direction());
}

void mouse_motion(int x, int y)
{
	int button = glutGetModifiers();
	// set the rotation of the object only if the left button is down and ctrl is not pressed
	bool can_rotate_obj = (button == GLUT_LEFT_BUTTON);
	
	if (can_rotate_obj)
	{
		int dx = x - scene.mouse_position.x;
		int dy = y - scene.mouse_position.y;
	
		scene.camera.rotate_about_og_up(-dx, -dy);
	}
	scene.mouse_position.Set(x, y);
	glutPostRedisplay();
}

void mouse_buttons(int button, int state, int x, int y)
{
	// store the mouse position when the left button is pressed
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		scene.mouse_position.Set(x, y);
	}
	// adjust the radius of the camera based on the mouse wheel
	if (button == 3)
	{
		scene.camera.zoom(-1.0f);
	}
	if (button == 4)
	{
		scene.camera.zoom(1.0f);
	}
	glutPostRedisplay();
}

// function to close the window when escape is pressed
void keyboard(unsigned char key, int x, int y)
{
	if (key == 27)
	{
		glutLeaveMainLoop();
	}
}


// Provides the ability to rotate the light with the arrow keys
// and recompile the shaders with F6
void fn_keyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_F6)
	{
		std::cout << "Recompiling shaders" << std::endl;
		scene.program.BuildFiles("shader.vert", "shader.frag");
		glutPostRedisplay();
	}
	if (key == GLUT_KEY_RIGHT || key == GLUT_KEY_LEFT || key == GLUT_KEY_UP || key == GLUT_KEY_DOWN) {
		float side_direction = 0.0f;
		float up_direction =  0.0f;
		if (key == GLUT_KEY_RIGHT || key == GLUT_KEY_LEFT)
		{
			side_direction = (key == GLUT_KEY_RIGHT) ? 1.0f : -1.0f;
			rotate_light(5.0 * side_direction, 0.0f);
		}
		else if (key == GLUT_KEY_UP || key == GLUT_KEY_DOWN)
		{
			up_direction = (key == GLUT_KEY_UP) ? 1.0f : -1.0f;
			rotate_light(0.0f, 5.0 * up_direction);
		}
		rotate_light(5.0 * side_direction, 5.0 * up_direction);
		glutPostRedisplay();
	}
}

void _set_material_flags(rc::MaterialGroup& material_group)
{
	bool has_map_Ka = material_group.map_Ka_id != NULL_ID;
	bool has_map_Kd = material_group.map_Kd_id != NULL_ID;
	bool has_map_Ks = material_group.map_Ks_id != NULL_ID;
	scene.program.SetUniform("has_map_Ka", has_map_Ka);
	scene.program.SetUniform("has_map_Kd", has_map_Kd);
	scene.program.SetUniform("has_map_Ks", has_map_Ks);
}

void _set_material_textures(rc::MaterialGroup& material_group)
{
	if (material_group.map_Ka_id != NULL_ID)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material_group.map_Ka_id);
		// 0 is the texture unit
		scene.program.SetUniform("map_Ka", 0);
	}
	if (material_group.map_Kd_id != NULL_ID)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material_group.map_Kd_id);
		scene.program.SetUniform("map_Kd", 1);
	}
	if (material_group.map_Ks_id != NULL_ID)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material_group.map_Ks_id);
		scene.program.SetUniform("map_Ks", 2);
	}
}

void _set_material_colors(const cy::TriMesh::Mtl& mtl)
{
	scene.program.SetUniform("Ka", cy::Vec3f(mtl.Ka));
	scene.program.SetUniform("Kd", cy::Vec3f(mtl.Kd));
	scene.program.SetUniform("Ks", cy::Vec3f(mtl.Ks));
}

void _render_material_group(rc::MaterialGroup& material_group, rc::rcTriMeshForGL& mesh)
{
	_set_material_flags(material_group);
	_set_material_textures(material_group);
	const cy::TriMesh::Mtl& mtl = mesh.M(material_group.material_index);
	_set_material_colors(mtl);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, material_group.ebuffer_id);
	
	// Apparently we should be setting the buffer data only once in the code. 
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * material_group.n_indices, material_group.indices, GL_STATIC_DRAW);
	//glDrawElements(GL_TRIANGLES, material_group.n_indices, GL_UNSIGNED_INT, material_group.indices);

	glDrawElements(GL_TRIANGLES, material_group.n_indices, GL_UNSIGNED_INT, (void*)(material_group.start_index * sizeof(int)));
}

// Renders the scene by using glDrawElements to reduce the amount of data that has
// to be sent to the GPU
void render(void)
{
	glBindVertexArray(test_global_vao);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	scene.set_mvp_and_update_uniforms();
	
	rc::rcTriMeshForGL& mesh = *scene.get_mesh();
	for (MaterialGroup& material_group : material_groups)
	{
		_render_material_group(material_group, mesh);
	}
	glutSwapBuffers();
}

bool _are_command_arguments_valid_or_debug(int argc, char** argv)
{
	if (argc != 2)
	{
		#ifdef _DEBUG
		filename = default_obj_path;
		return true;
		#else
		std::cerr << "NO FILENAME GIVEN!! Usage: " << argv[0] << " <filename.obj>" << std::endl;
		return false;
		#endif
	}
	return true;
}

void _init_glut_settings()
{
	// check to see if the window has been created with glutCreateWindow
	if (glutGetWindow() == 0)
	{
		std::cerr << "Window not created with glutCreateWindow" << std::endl;
		return;
	}
	glEnable(GL_DEPTH_TEST);
	glClearColor(scene.bg[0], scene.bg[1], scene.bg[2], scene.bg[3]);
}

void init_window(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Utah Teapot");
	glewInit();
	CY_GL_REGISTER_DEBUG_CALLBACK;
	// These must occur after the window is created
	_init_glut_settings();
}

void bind_glut_functions()
{
	// Display
	glutDisplayFunc(render);
	// Keyboard
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(fn_keyboard);
	// Mouse
	glutMouseFunc(mouse_buttons);
	glutMotionFunc(mouse_motion);
}

void _bind_VNT_buffers(rc::rcTriMeshForGL& mesh, std::vector<GLuint> &v_n_t) {
	GLuint v_vbo, vn_vbo, vt_vbo;

	glGenBuffers(1, &v_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, v_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f)* mesh.get_vbo_size(), &mesh.V_vbo(0), GL_STATIC_DRAW);

	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f)* mesh.get_vbo_size(), &mesh.VN_vbo(0), GL_STATIC_DRAW);

	glGenBuffers(1, &vt_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f)* mesh.get_vbo_size(), &mesh.VT_vbo(0), GL_STATIC_DRAW);
	
	v_n_t.push_back(v_vbo);
	v_n_t.push_back(vn_vbo);
	v_n_t.push_back(vt_vbo);
}

GLuint _bind_buffers(rc::rcTriMeshForGL& mesh)
{
	// Create vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	test_global_vao = vao;

	std::vector<GLuint> v_n_t;
	_bind_VNT_buffers(mesh, v_n_t);

	GLuint ebuffer;
	glGenBuffers(1, &ebuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * mesh.get_n_elements(), &mesh.E(0), GL_STATIC_DRAW);
	
	// Load the shaders with cy calls
	bool shader_comp_success = scene.program.BuildFiles("shader.vert", "shader.frag");
	scene.program.Bind();

	scene.program.SetAttribBuffer("position", v_n_t[0], 3, GL_FLOAT, GL_FALSE, 0, 0);
	scene.program.SetAttribBuffer("normal", v_n_t[1], 3, GL_FLOAT, GL_FALSE, 0, 0);
	scene.program.SetAttribBuffer("textCoord", v_n_t[2], 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	return ebuffer;
}

GLuint _cy_initialize_gl_texture(const char* path) {
	Texture new_texture(path);
	cyGLTexture2D tex;
	tex.Initialize();
	tex.SetImage(new_texture.data_const_ptr(), 4, new_texture.width(), new_texture.height());
	tex.BuildMipmaps();
	// tex.Bind() -> No need to bind. This comes later with the draw
	// No need to bind to the shader either. Comes with draw.
	return tex.GetID();
}

GLuint _init_gl_texture(const std::string& path) {
	Texture new_texture(path);
	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	// Set the texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Load the texture
	unsigned int w = new_texture.width();
	unsigned int h = new_texture.height();
	const unsigned char* data = new_texture.data_const_ptr();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	return tex_id;
}

void _init_material_groups_for_ebo(rc::rcTriMeshForGL& mesh, GLuint ebuffer) {
	int n_materials = mesh.NM();
	for (int i = 0; i < n_materials; i++) {
		rc::MaterialGroup mg;
		mg.ebuffer_id = ebuffer;
		mg.material_index = i;
		
		// Set the data for the indices
		int n_indices = mesh.GetMaterialFaceCount(i) * 3;
		mg.n_indices = n_indices;
		int first_face = mesh.GetMaterialFirstFace(i);
		int start_index = first_face * 3;
		mg.start_index = start_index;
		mg.indices = &mesh.E(start_index);
		
		// Initialize all of the textures for this material
		cy::TriMesh::Mtl const& mtl = mesh.M(i);
		if (mtl.map_Ka.data != nullptr) {
			const char* path = mtl.map_Ka.data;
			std::string full_path = std::string(asset_path) + std::string(path);
			GLuint tex_id = _init_gl_texture(full_path);
			mg.map_Ka_id = tex_id;
		}
		if (mtl.map_Kd.data != nullptr) {
			const char* path = mtl.map_Kd.data;
			std::string full_path = std::string(asset_path) + std::string(path);
			GLuint tex_id = _init_gl_texture(full_path);
			mg.map_Kd_id = tex_id;
		}
		if (mtl.map_Ks.data != nullptr) {
			const char* path = mtl.map_Ks.data;
			std::string full_path = std::string(asset_path) + std::string(path);
			GLuint tex_id = _init_gl_texture(full_path);
			mg.map_Ks_id = tex_id;
		}
		material_groups.push_back(mg);
	} 
}

void init_points_from_mesh(rc::rcTriMeshForGL& mesh)
{
	scene.set_mesh(&mesh);
	scene.n_points = mesh.NF() * 3;
	scene.n_elements = mesh.NE();

	
	GLuint ebuffer = _bind_buffers(mesh);
	_init_material_groups_for_ebo(mesh, ebuffer);

	// Some final point transformations 
	// Rotate the points to sit on the +y axis
	scene.point_transform = cy::Matrix4f::RotationX(-PI_OVER_2);
	// Scale the points
	

	// Center the object using the bounding box
	mesh.ComputeBoundingBox();
	cy::Vec3f bounding_box_center = (mesh.GetBoundMin() + mesh.GetBoundMax()) / 2.0f;
	bounding_box_center = cy::Vec3f(cy::Matrix4f::RotationX(-PI_OVER_2) * bounding_box_center);
	scene.point_transform.AddTranslation(-bounding_box_center);
	const cy::Vec3f scale_v3f = cy::Vec3f(1.0f, 1.0f, 1.0f) * scale;
	scene.point_transform.SetScale(scale_v3f);

	// Add some lights!
	mesh.set_k(1.0f, 0.0, 0.0);
	light.set_spherical_position(0.0f, 90.0f);
	light.SetRotation(cy::Matrix4f::RotationX(PI_OVER_2));
	light.set_ambient_intensity(cy::Vec3f(0.1f, 0.1f, 0.1f));
	light.set_specular_intensity(cy::Vec3f(0.5f, 0.5f, 0.5f));
	light.set_diffuse_intensity(cy::Vec3f(0.85f));
	
	scene.program.SetUniform("light_direction", light.direction());
	scene.program.SetUniform("shininess", 200.0f);
}

void init_camera()
{
	// getting the projection matrix
	float aspect_ratio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	scene.camera.set_perspective_projection(90, aspect_ratio, 1.0f, 500.0f);

	cy::Vec3f camera_pos = cy::Vec3f(0.0f, 0.0f, 30.0f);
	cy::Vec3f up = cy::Vec3f(0.0, 1.0, 0.0);

	cy::Vec3f target = cy::Vec3f(0.0f, 0.0f, 0.0f);
	scene.camera.lookat(camera_pos, target, up);

	scene.set_mvp_and_update_uniforms();
}

int main(int argc, char** argv)
{
	if (!_are_command_arguments_valid_or_debug(argc, argv))
	{
		return 1;
	}
	rc::rcTriMeshForGL mesh;
	bool success = mesh.LoadFromFileObj(filename);
	if (!success)
	{
		std::cerr << "Error loading file: " << filename << std::endl;
		return 1;
	}
	
	// Convert the obj file to a format that can be used with OpenGL
	mesh.obj_to_gl_elements();
	
	init_window(argc, argv);
	init_points_from_mesh(mesh);
	std::cout << "Loaded mesh with " << mesh.NF() << " faces" << std::endl;
	init_camera();
	bind_glut_functions();

	glutMainLoop();
	return 0;
}
