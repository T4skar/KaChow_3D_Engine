#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	geoLoader = new ModuleGeometry(this);
	renderer3D = new ModuleRenderer3D(this);
	texture2D = new ModuleTextures(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);
	scene = new ModuleScene(this);
	physics = new ModulePhysics3D(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update() in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(texture2D);

	AddModule(geoLoader);
	AddModule(physics);
	AddModule(scene);
	
	// Renderer last!
	AddModule(renderer3D);
	AddModule(editor);


	loadRequested = true;
	saveRequested = false;
}

Application::~Application()
{
	for (std::vector<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); ++it)
	{
		delete (*it);
		(*it) = nullptr;
	}

	JSON_Value* root = jsonFile.FileToValue("config.json");

	if (jsonFile.GetRootValue() == NULL)
	{
		LOG("Couldn't load config.json");
	}

	JsonParser application = jsonFile.GetChild(root, "App");
	
	maxFPS = application.JsonValToNumber("FPS");
}

bool Application::Init()
{
	bool ret = true;

	JSON_Value* root = jsonFile.FileToValue("config.json");

	if (jsonFile.GetRootValue() == NULL)
	{
		LOG("Couldn't load config.json");
		ret = false;
	}

	JsonParser application = jsonFile.GetChild(root, "App");

	maxFPS = application.JsonValToNumber("FPS");

	maxFPS = 400;

	// Call Init() in all modules
	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret; ++it)
	{
		(*it)->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("-------------- Application Start --------------");
	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret; ++it)
	{
		(*it)->Start();
	}

	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (editor->vSync)
	{
		lastMsFrame = ms_timer.Read();

		float timeToWait = 1000.0f / (float)maxFPS;

		float result = timeToWait - lastMsFrame;

		if (lastMsFrame < timeToWait)
			SDL_Delay(static_cast<Uint32>(fabs(result)));

		lastMsFrame = ms_timer.Read();

	}

	if (loadRequested) LoadConfig();
	if (saveRequested) SaveConfig();

}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	if (gameState == GameState::PLAY) dtGame = dt;
	else dtGame = 0;

	PrepareUpdate();
	
	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->PreUpdate(dt);
	}

	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->Update(dt);
	}

	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	for (std::vector<Module*>::reverse_iterator it = list_modules.rbegin(); it != list_modules.rend() && ret; ++it)
	{
		ret = (*it)->CleanUp();
	}
	return ret;
}

float Application::GetDT()
{
	return dt;
}

void Application::SetDT(float dt)
{
	this->dt = dt;
}

float Application::DTGame()
{
	return dtGame;
}

bool Application::IsRunning()
{
	return gameState == GameState::PLAY;
}

bool Application::IsPaused()
{
	return gameState == GameState::PAUSE;
}

GameState Application::GetState()
{
	return gameState;
}

void Application::SetState(GameState gameState)
{
	this->gameState = gameState;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::LOGToEditor(const char* tmp_string)
{
	editor->ConsoleLog(tmp_string);
}

float Application::GetGameDT()
{
	return gamedt;
}

void Application::SetGameDT()
{
	game_timer.Start();
	gamedt = ((float)game_timer.Read() / 1000.0f);
}

void Application::StopGameDT()
{
	game_timer.Stop();
	gamedt = 0;
}

void Application::PauseGameDT()
{
	if (gamedt == 0)
		gamedt = ((float)game_timer.Read() / 1000.0f);
	else
		gamedt = 0;
}

void Application::SaveConfig()
{
	LOG("Saving configuration");

	JSON_Value* root = jsonFile.GetRootValue();

	JsonParser application = jsonFile.SetChild(root, "App");

	application.SetNewJsonNumber(application.ValueToObject(application.GetRootValue()), "FPS", maxFPS);


	// Call SaveConfig() in all modules
	std::vector<Module*>::iterator item;

	for (item = list_modules.begin(); item != list_modules.end(); ++item)
	{
		(*item)->SaveConfig(jsonFile.SetChild(root, (*item)->name));
	}

	jsonFile.SerializeFile(root, "config.json");
	saveRequested = false;
}

void Application::LoadConfig()
{
	LOG("Loading configurations");

	JSON_Value* root = jsonFile.GetRootValue();

	JsonParser application = jsonFile.GetChild(root, "App");

	maxFPS = application.JsonValToNumber("FPS");


	std::vector<Module*>::iterator item;

	for (item = list_modules.begin(); item != list_modules.end(); ++item)
	{
		(*item)->LoadConfig(jsonFile.GetChild(root, (*item)->name));
	}

	loadRequested = false;
}