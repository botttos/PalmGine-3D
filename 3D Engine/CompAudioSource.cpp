#include "CompAudioSource.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "CompTransform.h"
#include "MathGeoLib\MathGeoLib.h"

CompAudioSource::CompAudioSource(GameObject * parent, COMP_TYPE type, const char* name) : Component(parent, type)
{
	source = WwiseT::CreateAudSource(name);
	audio_source_id = source->GetID();
}

CompAudioSource::~CompAudioSource()
{
	delete source;
}

void CompAudioSource::Update(float dt)
{
	UpdateSourcePos();

	if (audio_to_play != "") {
		source->PlayEventByName(audio_to_play);
	}

}

void CompAudioSource::UpdateSourcePos()
{
	AkSoundPosition audio_pos;

	CompTransform* transformation = parent->GetCompTransform();

	if (transformation != nullptr) 
	{
		math::Quat rot = transformation->GetRotationQuat();

		math::float3 vector_pos = transformation->GetPosition();
		math::float3 vector_front = rot.Transform(math::float3(0, 0, 1));
		math::float3 vector_up = rot.Transform(math::float3(0, 1, 0));

		audio_pos.Set(vector_pos.x, vector_pos.z, vector_pos.y, vector_front.x, vector_front.z, vector_front.y, vector_up.x, vector_up.z, vector_up.y);
	}

}

void CompAudioSource::SetAudio(const char* audio) {
	audio_to_play = audio;
}