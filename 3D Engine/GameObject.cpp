#include "GameObject.h"
#include "Globals.h"
#include "CompMesh.h"
#include "CompTransform.h"
#include "CompMaterial.h"
#include "CompCamera.h"
#include "imGUI/imgui.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Glew/include/glew.h"
#include "ModuleSceneIntro.h"
#include <time.h>
#include <random>
#include <limits>

GameObject::GameObject(GameObject* parent) : parent(parent)
{
	// TODO MAKE FUNCTION ON GLOBALS
	std::random_device rd;     
	std::mt19937_64 eng(rd()); 							   
	std::uniform_int_distribution<uint> distr;
	uuid = distr(eng);

	this->AddEmptyComponent(COMP_TYPE::C_TRANSFORM);
	if (parent != nullptr)
		parent->childs.push_back(this);
}

GameObject::~GameObject()
{
	parent = nullptr;
	for (uint i = 0; i < childs.size(); i++)
	{
		delete childs[i];
	}
	for (uint i = 0; i < components.size(); i++)
	{
		delete components[i];
	}

	childs.clear();
	components.clear();
}

void GameObject::Update(float dt)
{
	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->Update(dt);
	}
	for (uint i = 0; i < childs.size(); i++)
	{
		childs[i]->Update(dt);
	}

	if (App->scene_intro->selected_gameObject == this) {
		DebugDrawBox();
	}
}

void GameObject::SetName(const char* new_name)
{
	name = new_name;
}

std::string GameObject::GetName()
{
	return name;
}

bool GameObject::Enable()
{
	return active = true;
}

bool GameObject::Disable()
{
	return active = false;
}

bool GameObject::IsActive()
{
	return active;
}

bool GameObject::IsRootGo() const
{
	return App->scene_intro->IsRootGO(this);
}

bool GameObject::IsStatic() const {
	return static_obj;
}

void GameObject::SetStatic(bool isStatic) {
	static_obj = isStatic;
	if (GetNumChilds() > 0) {
		for (int i = 0; i < GetNumChilds(); ++i) {
			childs[i]->SetStatic(isStatic);
		}
	}
}

const GameObject * GameObject::GetParent() const
{
	return parent;
}

void GameObject::AddChild(GameObject * child)
{
	child->parent = this;
	childs.push_back(child);
}

uint GameObject::GetNumChilds() const
{
	return childs.size();
}

// Components -----------------------

CompTransform* GameObject::GetCompTransform() const
{
	return (CompTransform*)FindComponent(COMP_TYPE::C_TRANSFORM);
}

CompMesh* GameObject::GetCompMesh() const
{
	return (CompMesh*)FindComponent(COMP_TYPE::C_MESH);
}

CompMaterial* GameObject::GetCompMaterial() const
{
	return (CompMaterial*)FindComponent(COMP_TYPE::C_MATERIAL);
}

CompCamera* GameObject::GetCompCamera() const{
	return (CompCamera*)FindComponent(COMP_TYPE::C_CAMERA);
}

Component * GameObject::AddEmptyComponent(COMP_TYPE type)
{
	if (CompAlreadyExists(type))
	{
		LOG("COMPONENT ALREADY EXITS ON THE GAME OBJECT.");
		return nullptr;
	}
	if (type == COMP_TYPE::C_MESH)
	{
		LOG("Adding MESH COMPONENT to GameObject.");
		CompMesh* mesh = new CompMesh(this, C_MESH);
		components.push_back(mesh);
		return mesh;
	}
	else if (type == COMP_TYPE::C_TRANSFORM)
	{
		LOG("Adding TRANSFORM COMPONENT to GameObject.");
		CompTransform* transform = new CompTransform(this, C_TRANSFORM);
		components.push_back(transform);
		return transform;
	}
	else if (type == COMP_TYPE::C_MATERIAL)
	{
		LOG("Adding MATERIAL COMPONENT to GameObject.");
		CompMaterial* material = new CompMaterial(this, C_MATERIAL);
		components.push_back(material);
		return material;
	}
	
	return nullptr;
}

void GameObject::AddComponent(Component * comp)
{
	if (comp != nullptr)
	{
		if (CompAlreadyExists(comp->GetType()) == false)
		{
			components.push_back(comp);
		}
	}
}

void GameObject::BlitGameObjectHierarchy()
{
	// Check flags
	ImGuiTreeNodeFlags flag = 0;
	if (childs.empty())flag += ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;
	// Blit GO name
	char name_str[250];
	sprintf_s(name_str, 250, "%s##%i", name.c_str());
	bool op = ImGui::TreeNodeEx(name_str, flag);

	if (!IsRootGo())
	{
		if (ImGui::IsItemHovered())
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				App->scene_intro->SetSelectedGameObject(this);
			}
		}
	}

	// Blit childs
	if (op)
	{
		uint size = childs.size();
		for (uint k = 0; k < size; k++)
		{
			childs[k]->BlitGameObjectHierarchy();
		}

		ImGui::TreePop();
	}
}

Component * GameObject::FindComponent(COMP_TYPE type) const
{
	Component* temp_comp = nullptr;
	for (auto item = components.begin();item != components.end(); item++ )
	{
		if ((*item)->GetType() == type)
		{
			temp_comp = (*item);
			return temp_comp;
		}
	}
	return temp_comp;
}

bool GameObject::CompAlreadyExists(COMP_TYPE type) const
{
	// Checks if component already exists on the GameObject
	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
		{
			return true;
		}
	}
	return false;
}

void GameObject::DebugDrawBox() {

	glBegin(GL_LINES);
	glLineWidth(5.0f);
	glColor4f(1.f, 1.f, 0.f, 1.f);
	
	for (int i = 0; i < 12; ++i) {
		glVertex3f(GetAABB().Edge(i).a.x, GetAABB().Edge(i).a.y, GetAABB().Edge(i).a.z);
		glVertex3f(GetAABB().Edge(i).b.x, GetAABB().Edge(i).b.y, GetAABB().Edge(i).b.z);
	}

	glColor4f(1.f, 1.f, 1.f, 1.f);
	glEnd();

}

math::AABB GameObject::GetAABB()
{
	//CompMesh* tmp_mesh = GetCompMesh();
	//if (tmp_mesh != nullptr)
	//	return tmp_mesh->GetAABB();
	//LOG("ERROR: Game Object doesn't have mesh. Error getting the AABB on GameObject with name: %s", name.c_str());
	math::AABB newbox(float3(-0.25, 0, -0.25), float3(0.25, 0.25, 0.25));
	if (GetCompMesh() == nullptr) {
		CompTransform* transformation = GetCompTransform();
		//math::AABB newbox(float3(-0.5, 0, -1), float3(0.5, 0.5, 0.5));
		//newbox.Enclose((float3*)mesh->vertices, mesh->num_vertices);
		OBB boundingBox(newbox);
		boundingBox.Transform(transformation->GetTransformationMatrix());

		newbox = boundingBox.MinimalEnclosingAABB();
	}

	else {
		newbox = GetCompMesh()->GetAABB();
	}

	return newbox;
}

uint GameObject::GetUUID() const
{
	return uuid;
}
