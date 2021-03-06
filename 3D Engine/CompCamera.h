#ifndef COMCAMERA_H
#define COMCAMERA_H

#include "Component.h"
#include "Globals.h"
#include <vector>
#include "MathGeoLib/MathGeoLib.h"

class GameObject;

class CompCamera : public Component
{
public:
	CompCamera(GameObject* parent, COMP_TYPE type);
	~CompCamera();
	void Update(float dt);
	void DebugDraw();

	//Get methods
	float GetNearPlaneDistance() const;
	float GetFarPlaneDistance() const;
	float GetFOV() const;
	float GetApectRatio() const;
	uint GetSize() const;
	float4x4 GetViewMatrix() const;
	float4x4 GetProjectionMatrix() const;

	//Set methods
	void SetNearPlaneDistance(float distance);
	void SetFarPlaneDistance(float distance);
	void SetFOV(float fov);
	void SetAspectRatio(float aspect_ratio);

	//Utility
	void Look(const float3& position);
	void WriteComponentData(char** cursor);

public:
	Frustum frustum;
	bool frustum_culling = false;
	bool active = true;
};

#endif // !COMCAMERA_H