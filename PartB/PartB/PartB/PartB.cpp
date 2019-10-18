#include <cstdlib>
#include <utility>
#include "Vec3f.h"
#include "Sphere.h"
#include "MeshTriangle.h"
#include "Ray.h"


// main render function
void render(
	const Camera& camera,
	const std::vector<std::unique_ptr<Object>>& objects,
	const std::vector<std::unique_ptr<Light>>& lights)
{
	// width/height of screen
	float width = camera.screen.first;
	float height = camera.screen.second;

	Vec3f* framebuffer = new Vec3f[width * height];
	Vec3f* pix = framebuffer;
	float scale = tan(deg2rad(camera.fov * 0.5));
	float imageAspectRatio = width / (float)height;
	Vec3f orig(0);

	// render each pixel
	for (uint32_t j = 0; j < height; ++j) {
		for (uint32_t i = 0; i < width; ++i) {
			// generate primary ray direction
			float x = (2 * (i + 0.5) / (float)width - 1) * imageAspectRatio * scale;
			float y = (1 - 2 * (j + 0.5) / (float)height) * scale;
			Vec3f dir = normalize(Vec3f(x, y, -1));
			*(pix++) = castRay(orig, dir, objects, lights, camera, 0); // cast ray
		}
	}

	// save framebuffer to file
	std::ofstream ofs;
	ofs.open("./outbig.ppm");
	ofs << "P6\n" << width << " " << height << "\n255\n\n";
	for (uint32_t i = 0; i < height * width; ++i) {
		char r = (char)(255 * clampf(0, 1, framebuffer[i].x));
		if (r == '\n') r += 1;
		char g = (char)(255 * clampf(0, 1, framebuffer[i].y));
		if (g == '\n') g += 1;
		char b = (char)(255 * clampf(0, 1, framebuffer[i].z));
		if (b == '\n') b += 1;
		ofs << r << g << b;
	}

	ofs.close();

	delete[] framebuffer;
}



int main(int argc, char** argv)
{
	// creating the scene (adding objects and lights)

	// make a camera
	Camera camera;
	camera.fov = 45;
	camera.screen.first = 2048;
	camera.screen.second = 2048;
	camera.maxRecursions = 5;
	camera.bgColor = Vec3f(0.1, 0.4, 0.6);

	// vectors of objects and lights
	std::vector<std::unique_ptr<Object>> objects;
	std::vector<std::unique_ptr<Light>> lights;

	// spheres
	Sphere* sph1 = new Sphere(Vec3f(1, 1, -6), 1);
	sph1->materialType = DIFFUSE_AND_GLOSSY;
	sph1->diffuseColor = Vec3f(0.05, 0.4, 0.7);
	sph1->alpha = 1.0;

	Sphere* sph2 = new Sphere(Vec3f(-2, 2, -10), 2.5);
	sph2->ior = 10.0;
	sph2->materialType = REFLECTION_AND_REFRACTION;
	sph2->reflection = 0.9;
	sph2->alpha = 0.9;

	Sphere* sph3 = new Sphere(Vec3f(2, -2, -12), 1);
	sph3->materialType = DIFFUSE_AND_GLOSSY;
	sph3->diffuseColor = Vec3f(0.8, 1.0, 1.0);
	sph3->alpha = 0.5;

	Sphere* sph4 = new Sphere(Vec3f(2, 2, 12), 4);
	sph4->materialType = DIFFUSE_AND_GLOSSY;
	sph4->diffuseColor = Vec3f(1.0, 0.6, 0.2);
	sph4->alpha = 1.0;

	Sphere* sph5 = new Sphere(Vec3f(2, 16, 40), 16);
	sph5->materialType = DIFFUSE_AND_GLOSSY;
	sph5->diffuseColor = Vec3f(1.0, 0.4, 0.8);
	sph5->alpha = 1.0;

	// push spheres onto vector
	objects.push_back(std::unique_ptr<Sphere>(sph1));
	objects.push_back(std::unique_ptr<Sphere>(sph2));
	objects.push_back(std::unique_ptr<Sphere>(sph3));
	objects.push_back(std::unique_ptr<Sphere>(sph4));
	objects.push_back(std::unique_ptr<Sphere>(sph5));

	// planes

	// back wall
	Vec3f verts[4] = { {-20,-5,-25}, {20,-5,-25}, {20,15,-25}, {-20,15,-25} };
	uint32_t vertIndex[6] = { 0, 1, 3, 1, 2, 3 };
	Vec3f st[4] = { {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0} };
	MeshTriangle* mesh = new MeshTriangle(verts, vertIndex, 2, st);
	mesh->materialType = DIFFUSE_AND_GLOSSY;
	mesh->diffuseColor = Vec3f(0.8, 0.6, 0.2);

	objects.push_back(std::unique_ptr<MeshTriangle>(mesh));

	// floor
	Vec3f verts2[4] = { {-20,-3,25}, {20,-3,25}, {20,-3,-25}, {-20,-3,-25} };
	uint32_t vertIndex2[6] = { 0, 1, 3, 1, 2, 3 };
	Vec3f st2[4] = { {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0} };
	MeshTriangle* mesh2 = new MeshTriangle(verts2, vertIndex2, 2, st2);
	mesh2->materialType = DIFFUSE_AND_GLOSSY;
	mesh2->diffuseColor = Vec3f(0.3, 0.1, 0.4);
	

	objects.push_back(std::unique_ptr<MeshTriangle>(mesh2));

	// left wall
	Vec3f verts3[4] = { {-8,-5,0}, {-8,100,0}, {-8,100,-25}, {-8,-5,-25} };
	uint32_t vertIndex3[6] = { 3, 1, 0, 3, 2, 1 };
	Vec3f st3[4] = { {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0} };
	MeshTriangle* mesh3 = new MeshTriangle(verts3, vertIndex3, 2, st3);
	mesh3->materialType = REFLECTION_AND_REFRACTION;
	mesh3->diffuseColor = Vec3f(0.4, 0.8, 0.2);
	mesh3->reflection = 0.2;
	mesh3->ior = 10.0;

	objects.push_back(std::unique_ptr<MeshTriangle>(mesh3));

	// right wall
	Vec3f verts4[4] = { {8,-5,0}, {8,100,0}, {8,100,-25}, {8,-5,-25} };
	uint32_t vertIndex4[6] = { 0, 1, 2, 0, 2, 3 };
	Vec3f st4[4] = { {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0} };
	MeshTriangle* mesh4 = new MeshTriangle(verts4, vertIndex4, 2, st4);
	mesh4->materialType = REFLECTION_AND_REFRACTION;
	mesh4->diffuseColor = Vec3f(0.4, 0.8, 0.2);
	mesh4->reflection = 0.2;
	mesh4->ior = 10.0;

	objects.push_back(std::unique_ptr<MeshTriangle>(mesh4));

	// lights
	lights.push_back(std::unique_ptr<Light>(new Light(Vec3f(-0, 70, 0), 0.8)));
	lights.push_back(std::unique_ptr<Light>(new Light(Vec3f(4, 2, -2), 0.6)));
	lights.push_back(std::unique_ptr<Light>(new Light(Vec3f(-4, 8, -2), 0.4)));
	lights.push_back(std::unique_ptr<Light>(new Light(Vec3f(0, 16, 12), 0.5)));

	// set light colors
	lights[0]->color = Vec3f(1.0, 1.0, 1.0);
	lights[1]->color = Vec3f(0.8, 0.5, 0.9);
	lights[2]->color = Vec3f(0.5, 0.8, 0.6);
	lights[3]->color = Vec3f(0.6, 0.9, 0.2);

	// finally, render
	render(camera, objects, lights);

	return 0;
}