#include "Component.h"

Component::Component(GameObject* parent, std::string uuid)
{
	type = ComponentType::NONE;
	active = true;
	this->mParent = parent;

	this->uuid = uuid;
}

Component::~Component()
{

}

void Component::OnEditor()
{

}

void Component::Update()
{

}

void Component::Enable()
{
	active = false;
}

void Component::Disable()
{
	active = false;
}
bool Component::IsEnable()
{ 
	return active; 
}
void Component::PrintOnInspector()
{

}