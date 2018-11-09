#ifndef MODULEIMPORTER_H
#define MODULEIMPORTER_H

#include "Module.h"
#include "Globals.h"
#include <vector>
#include "Glew/include/glew.h"
#include "MathGeoLib/Geometry/AABB.h"

class aiScene;
class aiNode;
class aiMaterial;

class ModuleImporter : public Module
{
public:
	ModuleImporter(bool start_enabled = true);
	~ModuleImporter();

	// 
	bool Start();
	bool CleanUp();
	
	// Utility
	bool LoadFBX(const char* path);

	// Binary
	std::string ImportFBX(const aiScene* scene, const char* path, const char* name);
	std::string CreateBinary(const aiScene* scene, const char* path, const char* name);
	void WriteBinaryRecursive(aiNode* node, char** cursor, const char* name, const aiScene* scene, const char*path);
	std::string WriteFileInMemory(char* data, const char* name, const char* path, uint size, char* extension_name);
	void CreateBinaryMesh(const aiScene* scene, const char* path);
	// Binary Size
	uint BinarySize(const aiScene* scene);
	uint GetRecursiveSize(const aiNode* root_node, const aiScene* scene);
	// READ BINARY
	GameObject* ReadBinaryHierarchy(char** cursor, uint* num_childs, GameObject* parent);
	void SetBinaryMesh(const char* path, GameObject* go);
	char* LoadData(const char* path);
	void LoadRecursiveHierarchy(char** cursor, GameObject* parent);
	
	void GenBuffers(CompMesh* mesh);
	void ImportImage(const char* path /*PNG Path*/);
};

#endif // !MODULEIMPORTER_H