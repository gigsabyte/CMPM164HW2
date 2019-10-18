#pragma once
#include "Sphere.h"
#include "Light.h"
#include "Vec3f.h"
#include<iostream>

struct Camera {
	std::pair<float, float> screen;
	float fov;
	int maxRecursions;
	Vec3f bgColor;
	float bias;
	Camera() : screen(500, 500), fov(90), maxRecursions(4), bgColor(0, 0, 0), bias(0.001) {}
	Camera(float x, float y, float ffov) : screen(x, y), fov(ffov), maxRecursions(4), bgColor(0, 0, 0), bias(0.001) {}
};

bool trace(
	const Vec3f& orig, const Vec3f& dir,
	const std::vector<std::unique_ptr<Object>>& objects,
	float& tNear, uint32_t& index, Object** hitObject);


Vec3f castRay(
	const Vec3f& orig, const Vec3f& dir,
	const std::vector<std::unique_ptr<Object>>& objects,
	const std::vector<std::unique_ptr<Light>>& lights,
	const Camera& camera,
	uint32_t depth,
	bool test = false,
	Object* ignoreObject = nullptr);

