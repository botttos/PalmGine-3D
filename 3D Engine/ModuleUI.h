#ifndef MODULEUI_H
#define MODULEUI_H

#include "Module.h"
#include "Globals.h"

class Panel;
class PanelAbout;
class PanelConsole;
class PanelConfig;
class PanelInspector;
class PanelHierarchy;
class PanelTimeManagement;
class PanelSaveScene;
class PanelLoadScene;
class PanelAssets;

class ModuleUI : public Module
{
public:
	ModuleUI(bool start_enabled = true);
	~ModuleUI();

	bool Start();
	update_status Update(float dt);
	update_status PreUpdate(float dt);
	bool CleanUp();

private:
	bool configActive = true;
	std::vector<Panel *> panels;

public:
	PanelAbout* about = nullptr;
	PanelConsole* console = nullptr;
	PanelConfig* config = nullptr;
	PanelInspector* inspector = nullptr;
	PanelHierarchy* goHierarchy = nullptr;
	PanelTimeManagement* time_management = nullptr;
	PanelSaveScene* save_scene = nullptr;
	PanelLoadScene* load_scene = nullptr;
	PanelAssets* assets = nullptr;
};

#endif // !MODULEUI_H