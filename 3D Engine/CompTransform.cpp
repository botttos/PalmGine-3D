#include "CompTransform.h"
#include "Component.h"
#include "GameObject.h"

CompTransform::CompTransform(GameObject * parent, COMP_TYPE type) : Component(parent, type)
{
}

CompTransform::~CompTransform()
{
}

void CompTransform::Update(float dt)
{
	// Update child's transformation
	if (!parent->GetParent()->IsRootGo() && parent->GetParent() != nullptr)
	{
		CompTransform* temp_transform = (CompTransform*)parent->GetParent()->FindComponent(COMP_TYPE::C_TRANSFORM);
		inhe_transform = transform_matrix.Transposed() * temp_transform->inhe_transform;
	}
}

void CompTransform::SetTransformation(math::float4x4 new_trans)
{
	transform_matrix = new_trans;

	transform_matrix.Decompose(position, quaternion_rotation, scale);
	euler_rotation = quaternion_rotation.ToEulerXYZ();
	// Inherate from parent if it have
	if (parent->GetParent()->IsRootGo() == false)
	{
		CompTransform* tmp_transform = ((CompTransform*)parent->GetParent()->FindComponent(COMP_TYPE::C_TRANSFORM));
		inhe_transform = transform_matrix.Transposed() * tmp_transform->inhe_transform;
	}
	else
	{
		inhe_transform = transform_matrix.Transposed();
	}
}

void CompTransform::SetPosition(math::float3 pos)
{
	position = pos;
	transform_matrix.Translate(position);
	global_transform = inhe_transform*transform_matrix;
}

void CompTransform::SetRotation(math::float3 rot)
{
	transform_matrix.RotateFromTo(rotation, rot);
	rotation = rot;
	global_transform = inhe_transform*transform_matrix;
}

void CompTransform::SetScale(math::float3 sca)
{
	scale = sca;
	transform_matrix.Scale(scale);
	global_transform = inhe_transform*transform_matrix;
}

math::float3 CompTransform::GetPosition()const
{
	return position;
}

math::float3 CompTransform::GetRotation() const
{
	return rotation;
}

math::float3 CompTransform::GetScale() const
{
	return scale;
}

math::float4x4 CompTransform::GetTransformationMatrix() const {
	return global_transform;
}