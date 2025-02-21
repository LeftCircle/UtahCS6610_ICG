#include "model.h"


bool Model::load_model(const char* filename) {
    rc::rcTriMeshForGL mesh;
	bool success = mesh.LoadFromFileObj(filename);
	if (!success)
	{
		std::cerr << "Error loading file: " << filename << std::endl;
		return 1;
	}
    mesh.obj_to_gl_elements();

    
    // We need to add the mesh, materials, and transforms for this model
    _meshes.push_back(std::move(mesh));
    std::vector<rc::MaterialGroup> material_groups;
    _materials.push_back(std::move(material_groups));
    cyMatrix4f transform;
    _transforms.push_back(std::move(transform));

    _init_points_from_mesh(_meshes.back());
    
}

void Model::_init_points_from_mesh(rc::rcTriMeshForGL& mesh) {
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