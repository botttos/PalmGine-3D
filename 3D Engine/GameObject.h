#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Globals.h"
#include "Component.h"
#include "CompMaterial.h"
#include <vector>

class Component;
class CompMesh;
class CompMaterial;
class CompTransform;
class CompCamera;
class CompAudioListener;
class CompAudioSource;
class CompMovement;
class CompReverbZone;

class GameObject
{
public:
	GameObject(GameObject* parent);
	~GameObject();

	void Update(float dt);
	void PopGameObject();
	//Name
	void SetName(const char* new_name);
	std::string GetName();

	//State
	bool Enable();
	bool Disable();
	bool IsActive();
	bool IsRootGo() const;
	bool IsStatic() const;
	void SetStatic(bool isStatic);
	void SetUUID(uint uuid);
	void SetParentUUID(uint uuid);
	void SetActive(bool set);
	void SetUniqueName(const char* name);

	//Parent
	const GameObject* GetParent() const;

	//Childs
	void AddChild(GameObject* child);
	uint GetNumChilds() const;

	//Components
	Component* FindComponent(COMP_TYPE type) const;
	bool CompAlreadyExists(COMP_TYPE type) const;
	CompMesh* GetCompMesh() const;
	CompTransform* GetCompTransform() const;
	CompMaterial* GetCompMaterial() const;
	CompCamera* GetCompCamera() const;
	CompAudioListener* GetCompAudioListener() const;
	CompAudioSource* GetCompAudioSource() const;
	CompMovement* GetCompMovement() const;
	CompReverbZone* GetReverbZone() const;
	Component* AddEmptyComponent(COMP_TYPE type);
	void AddComponent(Component* comp);

	//UI
	void BlitGameObjectHierarchy();

	//DebugDrawBox
	void DebugDrawBox();
	math::AABB GetAABB();

	//UUID
	uint GetUUID()const;
	uint GetParentUUID()const;

public:
	std::vector<GameObject*> childs;
	std::vector<Component*> components;

private:
	uint uuid = 0;
	uint parent_uuid = 0;
	std::string name = "Unnamed";
	bool active = true;
	bool static_obj = false;
	GameObject* parent = nullptr;
};

#endif // !GAMEOBJECT_H