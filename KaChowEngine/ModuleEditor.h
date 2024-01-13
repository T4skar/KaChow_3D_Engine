#pragma once

#ifndef MODULE_EDITOR
#define MODULE_EDITOR

#include "Module.h"
#include "Globals.h"
#include <vector>
#include <string>
#include "ImGui/imgui.h"

#include "Glew/include/glew.h"

#include "ImGuizmo/ImGuizmo.h"


class ModuleEditor : public Module
{
public:

	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Init() override;
	update_status DrawEditor();
	bool CleanUp() override;
	void RefreshRenderSettings();

	void GameWindow();
	void SceneWindow();

	void AddHistogramData(const float aFPS, std::vector<float>& aMS);

	void ConsoleLog(const char* tmp_string);

	void GetHardwareInfo();

	ImVec2 NormMousePos(float x, float y, float w, float h, ImVec2 p);

	void MousePicking();

	void DrawGuizmos();

	std::vector<float> mFPSLog;
	std::vector<float> mMsLog;

private:

	ImVec2 guizmoSize;
	ImVec2 guizmoWindowPos;     
	int guizmoOffset;

	// Game & Scene window
	ImVec2 gameWindowSize;
	ImVec2 sceneWindowSize;


public:
	int cvCounter;


	bool playPressed;
	bool pausePressed;

	// Config window 
	bool fullscreen;
	float screenWidth;

	// Config render opengl
	bool lightning;
	bool isCullFace;
	bool depthTest;
	bool colorMat;
	bool texture2D;
	
	bool isWireframe;
	float bright;
	float bright_aux;

	bool isAABB;
private:
	bool isActivatedInspector;
	bool isActivatedHierarchy;
	bool isActivatedConsole;
	bool isActivatedDemo;
	bool isActivatedGameWindow;
	bool isActivatedSceneWindow;

	// Hardware info
	std::string caps;
	std::string SDLVersion;

	uint CPUCount;
	uint CPUCache;
	float systemRAM;

	uint GPUVendor = 0;
	uint GPUDevice = 0;
	char GPUBrand[250] = "";
	float VRAMBudget = 0.f;
	float VRAMUsage = 0.f;
	float VRAMAvailable = 0.f;
	float VRAMReserved = 0.f;

	GLuint playButton;
	GLuint stopButton;
	GLuint pauseButton;

public:
	std::vector<std::string>* logVector;
	bool vSync;
};

#endif