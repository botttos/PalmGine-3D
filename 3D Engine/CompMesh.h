#pragma once
#include "Component.h"
#include "Globals.h"
#include <vector>
#include "glmath.h"
#include "MathGeoLib\MathGeoLib.h"

struct Mesh
{
	uint id_vertices = 0;
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_indices = 0;
	uint num_vertices = 0;
	uint* vertices = nullptr;

	uint id_normals = 0;
	uint num_normals = 0;
	float* normals = nullptr;

	uint id_uvs = 0;
	uint num_uvs = 0;
	float* uvs = nullptr;
};

class GameObject;

class CompMesh : public Component
{
public:
	CompMesh(GameObject* parent, COMP_TYPE type);
	~CompMesh();

private:
	std::vector<Mesh> meshes;

	Mesh data;
	Mesh mesh;
};