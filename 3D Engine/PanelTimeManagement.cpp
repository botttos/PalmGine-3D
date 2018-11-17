#include "Application.h"
#include "PanelTimeManagement.h"

PanelTimeManagement::PanelTimeManagement() : Panel("TimeManagement")
{
	active = true;
}
PanelTimeManagement::~PanelTimeManagement()
{
}

//Function that draws all the console
void PanelTimeManagement::Draw()
{
	//Creates the console interface
	ImGui::Begin("Time Management", &active, 
		ImGuiWindowFlags_HorizontalScrollbar | 
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse);

	ImGui::SetWindowPos({(250.0f*App->window->width)/1280, 20.0f});
	ImGui::SetWindowSize(ImVec2((500*App->window->width)/1280, (60*App->window->height)/1024), ImGuiCond_Always);

	//Buttons
	if (ImGui::Button("Play")) {
		App->scene_intro->in_game_timer.Start();
		App->scene_intro->game_running = true;
	}
	ImGui::SameLine();
	if (App->scene_intro->in_game_timer.IsPaused() == false) {
		if (ImGui::Button("Pause")) {
			App->scene_intro->in_game_timer.Pause();
		}
	}
	else {
		if (ImGui::Button("Continue")) {
			App->scene_intro->in_game_timer.UnPause();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop")) {
		App->scene_intro->in_game_timer.Start();
		App->scene_intro->in_game_timer.Stop();
		App->scene_intro->game_running = false;
	}
	ImGui::SameLine();
	//if (ImGui::Button("NextFrame")) {
	//	
	//}
	//ImGui::SameLine();

	////Timer
	//ImGui::Text("%.3f", (float)App->scene_intro->in_game_timer.Read()/1000);
	//ImGui::SameLine();

	//TimeScale Slider
	ImGui::PushItemWidth(150);
	if (ImGui::SliderFloat("Time Scale", &App->scene_intro->in_game_time_scale, 0.0f, 2.0f, "%.1f")) {
		App->scene_intro->in_game_timer.SetTimerScale(App->scene_intro->in_game_time_scale);
	}

	ImGui::End();
}