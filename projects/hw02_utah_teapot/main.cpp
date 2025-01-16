/*
This is an OpenGL program that loads a teapot object and displays it on the screen.
This program demonstrates:
  - Converting an obj file to a format that can be used with OpenGL
  - Processing vertex, normal, and texture data to be used in an element buffer
  - Blinn Phong shading with ambient, diffuse, and specular lighting
  - A camera that can be rotated around the object
  - A light that can be rotated around the object

Keyboard functionality:
  - a: Toggle ambient lighting
  - s: Toggle specular lighting
  - d: Toggle diffuse lighting
  - F6: Recompile shaders
  - ESC: Close the window
  - Arrow keys: Rotate the light

Mouse functionality:
  - Left click and drag: Rotate the camera
  - Scroll wheel: Zoom in and out

Usage:
  This project was created with Visual Studio.
  lodepng.cpp is required to load textures.

Notes: 
	I created this project trying to follow the structure of cyCodeBase. 
	All of the files in rcCodeBase are .h files, and I am not entirely 
	pleased with this. 
	After starting the projects and examples from Donald H. House's Physics
	Based Animation course,	I got an example of what a clean C++ project 
	looks like. If I were to redo this project, I would	more strictly adhere
	to the single responsibility principle and separate the classes into .h
	and .cpp files.	Having the files split more strictly into a Model-View-Controller
	structure would have greatly benifited this project.
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

using namespace rc;

const char* default_obj_path = "teapot.obj";
const char* filename;
const char* material_filename = "assets\\texture_teapot\\teapot.mtl";
const char* brick_png_path = "assets\\texture_teapot\\brick.png";
const char* brick_specular_png_path = "assets\\texture_teapot\\brick-specular.png";

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
	// if A key is pressed, either disable or enable ambient lighting
	if (key == 'a')
	{
		if (shading_flag & ShadingType::AMBIENT)
		{
			shading_flag &= ~ShadingType::AMBIENT;
			scene.program.SetUniform("intensity_k_ambient", cy::Vec3f(0.0f, 0.0f, 0.0f));
		}
		else
		{
			shading_flag |= ShadingType::AMBIENT;
			rcTriMeshForGL* mesh = scene.get_mesh();
			scene.program.SetUniform("intensity_k_ambient", cy::Vec3f(mesh->M(0).Ka));
		}
	}
	if (key == 's')
	{
		if (shading_flag & ShadingType::SPECULAR)
		{
			shading_flag &= ~ShadingType::SPECULAR;
			scene.program.SetUniform("intensity_k_specular", cy::Vec3f(0.0f, 0.0f, 0.0f));
		}
		else
		{
			shading_flag |= ShadingType::SPECULAR;
			rcTriMeshForGL* mesh = scene.get_mesh();
			scene.program.SetUniform("intensity_k_specular",cy::Vec3f(mesh->M(0).Ks));
		}
	}
	if (key == 'd')
	{
		if (shading_flag & ShadingType::DIFFUSE)
		{
			shading_flag &= ~ShadingType::DIFFUSE;
			scene.program.SetUniform("intensity_k_diffuse", cy::Vec3f(0.0f, 0.0f, 0.0f));
		}
		else
		{
			shading_flag |= ShadingType::DIFFUSE;
			rcTriMeshForGL* mesh = scene.get_mesh();
			scene.program.SetUniform("intensity_k_diffuse", cy::Vec3f(mesh->M(0).Kd));
		}
	}

	if (key == 'a' || key == 's' || key == 'd')
	{
		glutPostRedisplay();
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

// Renders the scene by using glDrawElements to reduce the amount of data that has
// to be sent to the GPU
void render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, scene.n_elements, GL_UNSIGNED_INT, 0);
	scene.set_mvp_and_update_uniforms();
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

void _bind_buffers(rc::rcTriMeshForGL& mesh)
{
	// Create vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint v_vbo, vn_vbo, vt_vbo, ebuffer;

	glGenBuffers(1, &v_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, v_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.get_vbo_size(), &mesh.V_vbo(0), GL_STATIC_DRAW);

	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.get_vbo_size(), &mesh.VN_vbo(0), GL_STATIC_DRAW);

	glGenBuffers(1, &vt_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.get_vbo_size(), &mesh.VT_vbo(0), GL_STATIC_DRAW);

	glGenBuffers(1, &ebuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * mesh.get_n_elements(), &mesh.E(0), GL_STATIC_DRAW);
	
	// Load the shaders with cy calls
	bool shader_comp_success = scene.program.BuildFiles("shader.vert", "shader.frag");
	scene.program.Bind();

	scene.program.SetAttribBuffer("position", v_vbo, 3, GL_FLOAT, GL_FALSE, 0, 0);
	scene.program.SetAttribBuffer("normal", vn_vbo, 3, GL_FLOAT, GL_FALSE, 0, 0);
	scene.program.SetAttribBuffer("textCoord", vt_vbo, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void _bind_texture(rc::rcTriMeshForGL& mesh)
{
	// Load the texture
	Texture texture(brick_png_path);

	cyGLTexture2D tex;
	tex.Initialize();
	tex.SetImage(texture.data_const_ptr(), 4, texture.width(), texture.height());
	tex.BuildMipmaps();
	tex.Bind(0);
	scene.program["tex"] = 0;

	// Now for the material parts
	cy::TriMesh::Mtl const& mtl = mesh.M(0);
	scene.program["intensity_k_diffuse"] = cy::Vec3f(mtl.Kd[0], mtl.Kd[1], mtl.Kd[2]);
	scene.program["intensity_k_ambient"] = cy::Vec3f(mtl.Ka[0], mtl.Ka[1], mtl.Ka[2]);
	scene.program["intensity_k_specular"] = cy::Vec3f(mtl.Ks[0], mtl.Ks[1], mtl.Ks[2]);
	scene.program["shininess"] = mtl.Ns;

	// Load the specular map
	Texture specular_texture(brick_specular_png_path);
	cyGLTexture2D spec_tex;
	spec_tex.Initialize();
	spec_tex.SetImage(specular_texture.data_const_ptr(), 4, specular_texture.width(), specular_texture.height());
	spec_tex.BuildMipmaps();
	spec_tex.Bind(1);
	scene.program["specular_map"] = 1;
}

void init_points_from_mesh(rc::rcTriMeshForGL& mesh)
{
	scene.set_mesh(&mesh);
	scene.n_points = mesh.NF() * 3;
	scene.n_elements = mesh.NE();

	_bind_buffers(mesh);
	_bind_texture(mesh);
		
	// Some final point transformations 
	// Rotate the points to sit on the +y axis
	scene.point_transform = cy::Matrix4f::RotationX(-PI_OVER_2);

	// Center the object using the bounding box
	mesh.ComputeBoundingBox();
	cy::Vec3f bounding_box_center = (mesh.GetBoundMin() + mesh.GetBoundMax()) / 2.0f;
	bounding_box_center = cy::Vec3f(cy::Matrix4f::RotationX(-PI_OVER_2) * bounding_box_center);
	scene.point_transform.AddTranslation(-bounding_box_center);

	// Add some lights!
	mesh.set_k(1.0f, 0.0, 0.0);
	light.set_spherical_position(0.0f, 90.0f);
	light.SetRotation(cy::Matrix4f::RotationX(PI_OVER_2));
	light.set_ambient_intensity(cy::Vec3f(0.1f, 0.1f, 0.1f));
	light.set_specular_intensity(cy::Vec3f(0.5f, 0.5f, 0.5f));
	light.set_diffuse_intensity(cy::Vec3f(0.85f));
	
	scene.program.SetUniform("light_direction", light.direction());
	scene.program.SetUniform("intensity_k_diffuse", light.diffuse_intensity() * mesh.get_k_vec3f());
	scene.program.SetUniform("intensity_k_ambient", light.ambient_intensity() * mesh.get_k_vec3f());
	scene.program.SetUniform("intensity_k_specular", light.specular_intensity() * mesh.get_k_vec3f());
	scene.program.SetUniform("shininess", 200.0f);
}

void init_camera()
{
	// getting the projection matrix
	float aspect_ratio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	scene.camera.set_perspective_projection(90, aspect_ratio, 1.0f, 50.0f);

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
	init_camera();
	bind_glut_functions();

	glutMainLoop();
	return 0;
}
