#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ImGui/imgui.h"
#include "C_Transform.h"
#include "C_Material.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	rootGameObject = nullptr;
	selectedGameObj = nullptr;
	draggedGameObject = nullptr;
	hoveredGameObj = nullptr;
	vehicle = nullptr;
	name = "Scene";
}

bool ModuleScene::Init()
{
	jsonFile.FileToValue("scene.json");
	return false;
}

bool ModuleScene::Start()
{

	rootGameObject = new GameObject(nullptr);
	rootGameObject->name = "Scene";
	AddGOList(rootGameObject);

	bakerHouse = App->geoLoader->LoadFile("Assets/Models/BakerHouse.fbx");
	bakerHouse->name = "BakerHouse";

	street = App->geoLoader->LoadFile("Assets/Models/scene.DAE");
	street->name = "Street";
	street->mTransform->mRotation.x = -90;
	street->mTransform->calculateMatrix();


	// Game camera at start
	currentGameCamera =App->scene->CreateGameObject(rootGameObject);
	currentGameCamera->name = "Game Camera";
	C_Camera* cameraComponent = new C_Camera(UUIDGenerator::Generate());
	currentGameCamera->AddComponent(cameraComponent);
	//CPhysics* physicsComponent = new CPhysics(UUIDGenerator::Generate());
	//currentGameCamera->AddComponent(physicsComponent);
	//currentGameCamera->GOphys->phys = App->physics;

	if (currentGameCamera->GetPhysicsComponent() == nullptr) {
		CPhysics* compPhys = new CPhysics(currentGameCamera, UUIDGenerator::Generate());
		currentGameCamera->GOphys = compPhys;
		currentGameCamera->AddComponent(compPhys);
		currentGameCamera->GOphys->phys = App->physics;
	}
	currentGameCamera->GOphys->shapeSelected = CPhysics::ColliderShape::SPHERE;
	currentGameCamera->GOphys->isStatic = true;
	currentGameCamera->GOphys->isShapeSelected[1] = true;
	currentGameCamera->mTransform->setPosition({ 0, 5, -10 });

	if (currentGameCamera->GOphys->shapeSelected != CPhysics::ColliderShape::NONE) 
	{
		currentGameCamera->GOphys->colPos = currentGameCamera->mTransform->GetPos();
		currentGameCamera->GOphys->sphereRadius = 2;
		currentGameCamera->GOphys->CreateCollider();
		currentGameCamera->GOphys->CallUpdateShape();
	}



	

	 CreateVehicle();

	f = 0;

	return true;
}

update_status ModuleScene::Update(float dt)
{
	bool ret = UPDATE_CONTINUE;

	for (uint i = 0; i < ListGO.size(); ++i)
	{
		ListGO[i]->Update();
	}
	//currentGameCamera->mTransform->mPosition += {.1f * App->dtGame, .1f * App->dtGame, .1f * App->dtGame};

	rotation = 1;
	f += App->dtGame;

	if (App->gameState == GameState::PLAY)
	{
		if (f > 0.03f) {
			currentGameCamera->mTransform->mRotation.y += rotation;
			currentGameCamera->mTransform->calculateMatrix();
			if (currentGameCamera->mTransform->mRotation.y == 360) {
				currentGameCamera->mTransform->mRotation.y = 0;
			}
			f = 0.0f;
		}
	}
	else
	{
		currentGameCamera->mTransform->mRotation.y = 0;
		currentGameCamera->mTransform->calculateMatrix();
	}

	// Gizmos controls
	if (App->input->GetKey(SDL_SCANCODE_W)) App->camera->operation = ImGuizmo::OPERATION::TRANSLATE;
	else if (App->input->GetKey(SDL_SCANCODE_E)) App->camera->operation = ImGuizmo::OPERATION::ROTATE;
	else if (App->input->GetKey(SDL_SCANCODE_R)) App->camera->operation = ImGuizmo::OPERATION::SCALE;

	UpdateGameObjects();

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	delete rootGameObject;

	return true;
}

void ModuleScene::CreateSphere(float force) {

	currentGameSphere = App->scene->CreateGameObject(rootGameObject);
	currentGameSphere->name = "Game Sphere";
	

	if (currentGameSphere->GetPhysicsComponent() == nullptr) {
		CPhysics* compPhys = new CPhysics(currentGameSphere, UUIDGenerator::Generate());
		currentGameSphere->GOphys = compPhys;
		currentGameSphere->AddComponent(compPhys);
		currentGameSphere->GOphys->phys = App->physics;
	}
	currentGameSphere->GOphys->shapeSelected = CPhysics::ColliderShape::SPHERE;
	currentGameSphere->GOphys->isStatic = false;
	currentGameSphere->GOphys->isShapeSelected[1] = true;
	currentGameSphere->mTransform->setPosition({ 0, 5, -10 });

	if (currentGameSphere->GOphys->shapeSelected != CPhysics::ColliderShape::NONE)
	{
		
		C_Camera* Cam = currentGameCamera->GetCameraComponent();
		currentGameSphere->GOphys->colPos = currentGameSphere->mTransform->GetPos();
		currentGameSphere->GOphys->sphereRadius = 2;
		currentGameSphere->GOphys->CreateCollider();
		currentGameSphere->GOphys->CallUpdateShape();
		currentGameSphere->GOphys->collider->Push(-(Cam->Z.x * force), -(Cam->Z.y * force), -(Cam->Z.z * force));
	}
}

void ModuleScene::CreateVehicle()
{
	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(2, 2, 4);
	car.chassis_offset.Set(0, 1.5, 0);
	car.mass = 500.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x * 0.5f;
	float half_length = car.chassis_size.z * 0.5f;

	vec3 direction(0, -1, 0);
	vec3 axis(-1, 0, 0);

	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0, 12, 10);
}

void ModuleScene::ControlVehicle()
{
	if (vehicle == nullptr) {
		return;
	}
	turn = acceleration = brake = 0.0f;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		acceleration = MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (turn < TURN_DEGREES)
			turn += TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		brake = BRAKE_POWER;
	}

	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	vehicle->Render();

	

}


void ModuleScene::PrintHierarchy(GameObject* gameObject, int index)	
{
	ImGuiTreeNodeFlags flag_TNode = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow;

	bool isNodeOpen;

	if (gameObject->GetParent() == nullptr)
	{
		flag_TNode |= ImGuiTreeNodeFlags_DefaultOpen;
	}
	else
	{
		flag_TNode |= ImGuiTreeNodeFlags_OpenOnArrow;
	}

	if (gameObject == selectedGameObj) 
	{
		flag_TNode |= ImGuiTreeNodeFlags_Selected;
	}

	if (gameObject->mChildren.size() != 0)
		isNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)index, flag_TNode, gameObject->name.c_str(), index);

	else 
	{
		flag_TNode |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx((void*)(intptr_t)index, flag_TNode, gameObject->name.c_str(), index);
		isNodeOpen = false;
	}

	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("GameObject", gameObject, sizeof(GameObject*));

		draggedGameObject = gameObject;
		ImGui::Text("Drag to");
		ImGui::EndDragDropSource();
	}

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
	{
		hoveredGameObj = gameObject;
		if (ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			selectedGameObj = gameObject;
		}
	}

	if (ImGui::IsWindowHovered())
	{
		if (!ImGui::IsAnyItemHovered())
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
			{
				selectedGameObj = nullptr;
			}
		}
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject")) {

			draggedGameObject->SetNewParent(hoveredGameObj);
		}
		ImGui::EndDragDropTarget();
	}

	if (isNodeOpen)
	{
		if (!gameObject->mChildren.empty()) {
			for (unsigned int i = 0; i < gameObject->mChildren.size(); i++)
			{
				PrintHierarchy(gameObject->mChildren[i], i);
			}
		}
		ImGui::TreePop();
	}
				
}

void ModuleScene::SetGameObjectSelected(GameObject* gameObject)
{
	if (gameObject == nullptr) {
		selectedGameObj = nullptr;
		return;
	}

	selectedGameObj = gameObject;
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
	GameObject* newGameObject = new GameObject(parent);
	AddGOList(newGameObject);
	return newGameObject;
}

void ModuleScene::AddGOList(GameObject* objlist)
{
	ListGO.push_back(objlist);
}

bool ModuleScene::SaveScene()
{
	LOG("Saving scene");

	rootFile = jsonFile.GetRootValue();

	JsonParser scene = jsonFile.SetChild(rootFile, "GameObjects");

	SaveGameObjects(rootGameObject, scene.SetChild(scene.GetRootValue(), rootGameObject->name.c_str()));

	jsonFile.SerializeFile(rootFile, "scene.json");
	saveSceneRequest = false;
	return true;
}

void ModuleScene::SaveGameObjects(GameObject* parentGO, JsonParser& node) 
{
	std::string num;
	JsonParser& child = node;
	C_Transform* transform;
	float4x4 localTransform;

	node.SetNewJsonString(node.ValueToObject(node.GetRootValue()), "name", parentGO->name.c_str());
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "Deleting", parentGO->deleteGameObject);


	JsonParser components = node.SetChild(node.GetRootValue(), "components");
	JsonParser tmp = node;
	for (size_t i = 0; i < parentGO->mComponents.size(); i++)
	{
		// Create Child of component
		num = "Component " + std::to_string(i);
		//num += FormatComponentType(parentGO, i);



		tmp = components.SetChild(components.GetRootValue(), num.c_str());

		tmp.SetNewJsonNumber(tmp.ValueToObject(tmp.GetRootValue()), "Type", (int)parentGO->mComponents.at(i)->type);

		switch ((ComponentType)parentGO->mComponents.at(i)->type)
		{
		case ComponentType::TRANSFORM:
			num = "";
			transform = (C_Transform*)(parentGO->mComponents.at(i));
			localTransform = transform->getLocalMatrix();
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					/* if (i == 0 && j == 0)num += std::to_string(localTransform.At(i, j));
					 else num += "," + std::to_string(localTransform.At(i, j));*/

					num += std::to_string(localTransform.At(i, j));
					num += ",";
				}
			}

			tmp.SetNewJsonString(tmp.ValueToObject(tmp.GetRootValue()), "LocalTransform", num.c_str());

			break;

		case ComponentType::MESH:

			tmp.SetNewJsonString(tmp.ValueToObject(tmp.GetRootValue()), "Mesh", parentGO->name.c_str());

			break;

		case ComponentType::MATERIAL:

			C_Material* componentMaterial;
			componentMaterial = (C_Material*)(parentGO->mComponents.at(i));

			tmp.SetNewJsonString(tmp.ValueToObject(tmp.GetRootValue()), "Material", componentMaterial->texture_path.c_str());


			break;

		default:
			break;

		}
		parentGO->mComponents.at(i)->type;
	}

	for (size_t i = 0; i <= parentGO->mChildren.size(); i++)
	{

		num = "Child " + std::to_string(i);

		if (parentGO->mChildren.size() > i) {
			SaveGameObjects(parentGO->mChildren[i], child.SetChild(child.GetRootValue(), num.c_str()));
		}
	}
}

bool ModuleScene::LoadScene()
{
	LOG("Loading configurations");


	rootFile = jsonFile.GetRootValue();

	rootGO = jsonFile.GetChild(rootFile, "GameObjects");

	LoadGameObject(rootGO.GetChild(rootGO.GetRootValue(), "Scene"));

	loadSceneRequest = false;

	return false;
}

void ModuleScene::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

}

void ModuleScene::LoadComponents(JsonParser& parent, std::string num, GameObject* gamObj)
{

	C_Transform* transform;
	C_Mesh* meshRender;
	Mesh* mesh;
	C_Material* material;
	float3 size = float3::one;
	LOG("Loading Components \n");
	std::string debugPath;
	std::string debugUID;

	JsonParser components = parent.GetChild(parent.GetRootValue(), "components");
	JsonParser tmp = components;

	std::string pos;

	for (int i = 0; i < 4; i++)
	{
		num = "Component " + std::to_string(i);
		LOG((std::string("Loading ") + num).c_str());

		if (components.ExistChild(components.GetRootValue(), num.c_str()))
		{
			tmp = components.GetChild(components.GetRootValue(), num.c_str());
			switch ((ComponentType)(int)tmp.JsonValToNumber("Type"))
			{
			case ComponentType::TRANSFORM:
				gamObj->mTransform->mLocalMatrix = strMatrixToF4x4(tmp.ValueToString("LocalTransform"));

				LOG(gamObj->mTransform->getLocalMatrix().ToString().c_str());
				break;
			case ComponentType::MESH:

				break;
			case ComponentType::MATERIAL:
				break;
			}
		}
		else
		{
			break;
		}
	}
}

GameObject* ModuleScene::LoadGameObject(JsonParser parent, GameObject* father)
{
	std::string num;
	std::string convert;

	std::string name = parent.ValueToString("name");
	GameObject* gamObj = new GameObject();
	gamObj->name = name;
	gamObj->SetDeletion(parent.JsonValToBool("isTimeToDelete"));
	
	LoadComponents(parent, num, gamObj);
	int count = 0;
	num = "Child " + std::to_string(count);
	while (parent.ExistChild(parent.GetRootValue(), num.c_str()))
	{
		gamObj->SetNewParent(LoadGameObject(parent.GetChild(parent.GetRootValue(), num.c_str()), gamObj));
		++count;
		num = "Child " + std::to_string(count);
	}


	return gamObj;
}

void ModuleScene::UpdateGameObjects()
{
	if (saveSceneRequest)SaveScene();
	if (loadSceneRequest)LoadScene();
}

const char* ModuleScene::FormatComponentType(GameObject* parentGO, const size_t& i)
{
	switch ((ComponentType)parentGO->mComponents.at(i)->type)
	{
	case ComponentType::TRANSFORM:

		return " Transform";
		break;

	case ComponentType::MESH:

		return " Mesh";
		break;

	case ComponentType::MATERIAL:

		return " Material";
		break;

	default:
		break;

	}
}

float4x4 ModuleScene::strMatrixToF4x4(const char* convert)
{
	std::string text = convert;
	std::string delimiter = ",";
	std::vector<float> floatArray{};

	size_t pos = 0;
	while ((pos = text.find(delimiter)) != std::string::npos) {
		floatArray.push_back(stof(text.substr(0, pos)));
		text.erase(0, pos + delimiter.length());
	}


	float4x4 matrix;
	int count = 0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{

			matrix.At(i, j) = floatArray.at(count);
			++count;
		}

	return matrix;
}