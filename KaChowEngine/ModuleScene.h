#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include <vector>

class GameObject;
class ModuleScene : public Module
{
public:

	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene() {}

	bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void PrintHierarchy(GameObject* GO, int index);

	void SetGameObjectSelected(GameObject* gameObject);

	GameObject* CreateGameObject(GameObject* parent);

	void SaveSceneRequest() { saveSceneRequest = true; };
	void LoadSceneRequest() { loadSceneRequest = true; };

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

private:
	void UpdateGameObjects();
	bool SaveScene();
	void SaveGameObjects(GameObject* parent, JsonParser& rootFile);
	bool LoadScene();

	


	JsonParser jsonFile;
	JSON_Value* rootFile;

	GameObject* LoadGameObject(JsonParser parent, GameObject* father = nullptr);

	float4x4 strMatrixToF4x4(const char* convert);
	
	const char* FormatComponentType(GameObject* parentGO, const size_t& i);
	void LoadComponents(JsonParser& parent, std::string num, GameObject* gamObj);

	JsonParser rootGO;

	bool saveSceneRequest = false;
	bool loadSceneRequest = true;

public:
	GameObject* rootGameObject;
	GameObject* selectedGameObj;
	GameObject* hoveredGameObj;
	GameObject* draggedGameObject;

	GameObject* bakerHouse;

	GameObject* street;

	GameObject* currentGameCamera;

	GameObject* prova1;
	GameObject* prova2;
	GameObject* prova3;
	GameObject* prova4;
	GameObject* prova5;
	GameObject* prova6;

	float rotation;
	float f;
};