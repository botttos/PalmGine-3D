#include "Application.h"
#include "ModuleImporter.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Devil/include/il.h"
#include "Devil/include/ilut.h"
#include "MathGeoLib\Geometry\AABB.h"
#include "CompMesh.h"
#include "CompTransform.h"
#include "CompMaterial.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")
#pragma comment (lib, "Devil/libx86/DevIL.lib")
#pragma comment ( lib, "Devil/libx86/ILU.lib" )
#pragma comment ( lib, "Devil/libx86/ILUT.lib" )

using namespace std;

ModuleImporter::ModuleImporter(bool start_enabled) : Module(start_enabled)
{}

ModuleImporter::~ModuleImporter()
{}

bool ModuleImporter::Start()
{
	bool ret = true;

	ilInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	struct aiLogStream stream = aiLogStream();
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

void ModuleImporter::ClearMeshes()
{
	for (int i = meshes.size() - 1; meshes.size() != 0; i--)
	{
		delete[] meshes[i].indices;
		delete[] meshes[i].vertices;
		delete[] meshes[i].uvs;
		delete[] meshes[i].normals;
		meshes.pop_back();
	}

	//Geometry
	delete data.indices;
	delete data.vertices;

	//Texture
	delete data.uvs;
	delete data.normals;
}

void ModuleImporter::DrawMeshes()
{
	for (std::vector<ModelConfig>::iterator item = App->fbx->meshes.begin(); item != App->fbx->meshes.end(); ++item)
		App->renderer3D->DrawMeshes(*item);
}

bool ModuleImporter::CleanUp()
{
	aiDetachAllLogStreams();
	ClearMeshes();
	return true;
}

bool ModuleImporter::LoadFBX(const char* path)
{
	LOG("Loading FBX...");
	file_name.clear();
	this->path = path;
	//Substract the name of the file
	std::string name(path);
	temp_go->SetName((char*)name.substr(name.find_last_of('\\') + 1).c_str()); 
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		aiNode* rootNode = scene->mRootNode;
		for (int i = 0; i < rootNode->mNumChildren; ++i)
		{
			LoadModel(scene, rootNode->mChildren[i], path);
		}

		// ---- Release resources ----
		aiReleaseImport(scene);
		LOG("---- FBX LOADED WITH SUCCESS ----");
		return true;
	}
	else
		LOG("---- ERROR, COULDN'T LOAD FBX ----");
	return false;
}

void ModuleImporter::LoadModel(const aiScene* scene, aiNode* node, const char* path)
{
	if (node->mNumMeshes <= 0)
	{
		LOG("Unable to load the mesh with path: %s. The number of meshes is below or equal to 0.", path);
	}
	else
	{
		LOG("Loading mesh from path %s", path);
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* new_mesh = scene->mMeshes[node->mMeshes[i]];
			mesh = ModelConfig();
			
			mesh.num_vertices = new_mesh->mNumVertices;
			mesh.vertices = new uint[mesh.num_vertices * 3];
			memcpy(mesh.vertices, new_mesh->mVertices, sizeof(float)*mesh.num_vertices * 3);

			// ---- Generate buffers ----
			glGenBuffers(1, (GLuint*)&(mesh.id_vertices));
			glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh.num_vertices, mesh.vertices, GL_STATIC_DRAW);
			
			// ---- Geometry ----
			if (new_mesh->HasFaces())
			{
				mesh.num_indices = new_mesh->mNumFaces * 3;
				mesh.indices = new uint[mesh.num_indices];
				for (uint j = 0; j < new_mesh->mNumFaces; j++)
				{
					if (new_mesh->mFaces[j].mNumIndices == 3)
					{
						memcpy(&mesh.indices[j * 3], new_mesh->mFaces[j].mIndices, 3 * sizeof(uint));
					}
				}

				glGenBuffers(1, (GLuint*)&(mesh.id_indices));
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_indices);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * mesh.num_indices, mesh.indices, GL_STATIC_DRAW);
			}
			else
			{
				LOG("Mesh with %i faces can not be loaded.", new_mesh->mNumFaces);
			}

			// ---- Texture ----
			if (new_mesh->HasTextureCoords(mesh.id_uvs))
			{
				// ---- UVs ----
				mesh.num_uvs = new_mesh->mNumVertices;
				mesh.uvs = new float[mesh.num_uvs * 2];

				for (int i = 0; i < new_mesh->mNumVertices; ++i)
				{
					memcpy(&mesh.uvs[i * 2], &new_mesh->mTextureCoords[0][i].x, sizeof(float));
					memcpy(&mesh.uvs[(i * 2) + 1], &new_mesh->mTextureCoords[0][i].y, sizeof(float));
				}

				glGenBuffers(1, (GLuint*)&(mesh.id_uvs));
				glBindBuffer(GL_ARRAY_BUFFER, mesh.id_uvs);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh.num_uvs, mesh.uvs, GL_STATIC_DRAW);
			}
			else
			{
				LOG("Texture coords couldn�t be found for the specified mesh.");
			}

			// ---- Texture ID ----
			aiMaterial* material = scene->mMaterials[new_mesh->mMaterialIndex];
			if (material)
			{
				aiString path;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

				if (path.length > 0)
				{
					// ---- Find the texture on textures folder ----
					std::string texture_folder = "Textures/";
					std::string final_path = path.data;
					final_path.erase(0, final_path.find_last_of("\\") + 1);
					texture_folder += final_path;

					mesh.texture_id = CreateTextureID(texture_folder.c_str());
					LOG("Texture with path %s has been loaded.", texture_folder.c_str());
					final_path.clear();
					texture_folder.clear();
				}
			}

			// ---- Normals ----
			if (new_mesh->HasNormals())
			{
				mesh.num_normals = new_mesh->mNumVertices;
				mesh.normals = new float[mesh.num_normals * 3];
				memcpy(mesh.normals, new_mesh->mNormals, sizeof(float) * mesh.num_normals * 3);

				glGenBuffers(1, (GLuint*)&(mesh.id_normals));
				glBindBuffer(GL_ARRAY_BUFFER, mesh.id_normals);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh.num_normals, mesh.normals, GL_STATIC_DRAW);
			}
			else
			{
				LOG("Mesh has no normals.");
			}
			 
			// ---- Push the mesh ----
			meshes.push_back(mesh);
			LOG("Loaded mesh with %i vertices.", mesh.num_vertices);
			LOG("Loaded mesh with %i indices.", mesh.num_indices);
			LOG("Loaded mesh with %i triangles.", mesh.num_vertices / 3);
			LOG("Loaded mesh with %i normals.", mesh.num_normals);
			LOG("Loaded mesh with %i uvs.", mesh.num_uvs);
		}
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		LoadModel(scene, node->mChildren[i], path);
	}

	// ---- Default transformation values ----
	mesh.position = (0.f, 0.f, 0.f);
	mesh.rotation = (0.f, 0.f, 0.f);
	mesh.scale = (1.f, 1.f, 1.f);

	LOG("Mesh position: (%f, %f, %f)", mesh.position.x, mesh.position.y, mesh.position.z);
	LOG("Mesh rotation: (%f, %f, %f)", mesh.rotation.x, mesh.rotation.y, mesh.rotation.z);
	LOG("Mesh scale: (%f, %f, %f)", mesh.scale.x, mesh.scale.y, mesh.scale.z);
}

uint ModuleImporter::CreateTextureID(const char* texture_path)
{
	ILuint id;
	uint texture_id;
	ilGenImages(1, &id);
	ilBindImage(id);
	ilLoadImage(texture_path);

	texture_id = ilutGLBindTexImage();

	return texture_id;
}

void ModuleImporter::ApplyTexture(const char* path)
{
	ILuint id;
	ilGenImages(1, &id);
	ilBindImage(id);
	ilLoadImage(path);

	last_texture_id = ilutGLBindTexImage();
	LOG("Loaded and applied new texture correctly from path %s.", path);
}

void const ModuleImporter::CentrateObjectView()
{
	math::AABB box(float3(0, 0, 0), float3(0, 0, 0));
	box.Enclose((float3*)App->fbx->mesh.vertices, App->fbx->mesh.num_vertices);

	App->camera->Reference.x = box.CenterPoint().x;
	App->camera->Reference.y = box.CenterPoint().y;
	App->camera->Reference.z = box.CenterPoint().z;

	App->camera->Position.x = box.maxPoint.x * 2; // Increase the distance view
	App->camera->Position.y = box.maxPoint.y * 2;
	App->camera->Position.z = box.maxPoint.z * 2;

	App->camera->LookAt(App->camera->Reference);
}

math::AABB const ModuleImporter::GetAABB()
{
	math::AABB box(float3(0, 0, 0), float3(0, 0, 0));
	box.Enclose((float3*)App->fbx->mesh.vertices, App->fbx->mesh.num_vertices);

	return box;
}

uint ModuleImporter::MeshesSize()
{
	return meshes.size();
}

uint const ModuleImporter::GetIndices()
{
	return(mesh.num_indices);
}

uint const ModuleImporter::GetVertices()
{
	return(mesh.num_vertices);
}

vec3 const ModuleImporter::GetPosition()
{
	return(mesh.position);
}

vec3 const ModuleImporter::GetRotation()
{
	return(mesh.rotation);
}

vec3 const ModuleImporter::GetScale()
{
	return(mesh.scale);
}

float const ModuleImporter::GetNormals()
{
	return(mesh.num_normals);
}

float const ModuleImporter::GetUvs()
{
	return(mesh.num_uvs);
}

uint const ModuleImporter::GetTextureId()
{
	return(mesh.texture_id);
}