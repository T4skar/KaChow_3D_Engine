#pragma once

#include <string>
enum class ComponentType
{
	NONE,
	TRANSFORM,
	MESH,
	MATERIAL,
	CAMERA,
	PHISICS
};

class GameObject;
class Component
{
public:
	Component(GameObject* parent, std::string uuid);
	virtual ~Component();

	virtual void OnEditor();
	virtual void Update();

	virtual void Enable();
	virtual void Disable();

	virtual bool IsEnable();

	virtual void PrintOnInspector();

	bool active;

	ComponentType type;
	GameObject* mParent;

	std::string uuid;
};


