#include "CompAudioListener.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "CompTransform.h"
#include "MathGeoLib\MathGeoLib.h"

CompAudioListener::CompAudioListener(GameObject * parent, COMP_TYPE type) : Component(parent, type)
{
	listener = App->audio->CreateSoundEmitter("listener");
	App->audio->SetListener(listener);
}

CompAudioListener::~CompAudioListener()
{
	RELEASE(listener);
}

void CompAudioListener::Update(float dt)
{
	UpdateListenerPos();
	if (parent != App->scene_intro->camera) {
		DebugDraw();
	}
	else if (App->scene_intro->game_running == false) {
		DebugDraw();
	}
}

void CompAudioListener::UpdateListenerPos()
{
	CompTransform* transformation = parent->GetCompTransform();

	if (transformation != nullptr) 
	{
		math::Quat rot = transformation->GetRotationQuat();

		math::float3 vector_pos = transformation->GetPosition();
		math::float3 vector_front = rot.Transform(math::float3(0, 0, 1));
		math::float3 vector_up = rot.Transform(math::float3(0, 1, 0));

		listener->SetPos(vector_pos.x, vector_pos.y, vector_pos.z, vector_front.x, vector_front.y, vector_front.z, vector_up.x, vector_up.y, vector_up.z);
	}
}

uint CompAudioListener::GetSize() const
{
	// GET SIZE FOR SERIALIZATION
	uint size = 0;

	// COMPONENT TYPE
	size += sizeof(int);
	// IS ACTIVE
	size += sizeof(int);
	// LISTENER ID
	size += sizeof(uint);
	// NAME
	size += sizeof(char)*128;

	return size;
}

void CompAudioListener::WriteComponentData(char ** cursor)
{
	uint bytes = 0;

	// COMPONENT TYPE
	bytes = sizeof(int);
	memcpy(cursor[0], &type, bytes);
	cursor[0] += bytes;
	// IS ACTIVE
	bytes = sizeof(int);
	int tmp_active = (int)active;
	memcpy(cursor[0], &tmp_active, bytes);
	cursor[0] += bytes;
	// LISTENER ID
	bytes = sizeof(uint);
	uint tmp_id = (uint)listener->GetID();
	memcpy(cursor[0], &tmp_id, bytes);
	cursor[0] += bytes;
	// NAME
	bytes = sizeof(char) * 128;
	char* name = new char[128];
	strcpy(name, listener->GetName());
	memcpy(cursor[0], name, bytes);
	cursor[0] += bytes;
}

void CompAudioListener::DebugDraw() 
{
	math::Sphere sphere;

	sphere.pos = parent->GetCompTransform()->GetPosition();
	sphere.r = 0.25;

	glLineWidth(3.0f);
	glColor3f(2.0f, 0.0f, 0.0f);

	float radius = sphere.r;
	float3 pos = sphere.pos;
	float degInRad = 360.0f / 12;
	degInRad *= DEGTORAD;
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i < 12; i++)
		glVertex3f(cos(degInRad * i) * radius + pos.x, pos.y, sin(degInRad * i) * radius + pos.z);
	glEnd();
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i < 12; i++)
		glVertex3f(cos(degInRad * i) * radius + pos.x, sin(degInRad * i) * radius + pos.y, pos.z);
	glEnd();
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i < 12; i++)
		glVertex3f(pos.x, sin(degInRad * i) * radius + pos.y, cos(degInRad * i) * radius + pos.z);
	glEnd();

	glLineWidth(1.0f);
}