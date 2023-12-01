#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "Glew/include/glew.h"

#include "MathGeoLib/include/Math/float3x3.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "Primitive.h"

#define MAX_LIGHTS 8

class GameObject;
class C_Camera;
class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	bool Start() override;
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void DrawBox(float3* corners, float3 color);

	void OnResize(int width, int height);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

	CPlane Grid;

	float3x3 NormalMatrix;
	float4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	unsigned int cameraBuffer;
	unsigned int inGameBuffer;
	unsigned int frameBuffer;
	unsigned int renderObjBuffer;

	GLuint VBO;
	GLuint EBO;
	GLuint VAO;

	GameObject* GameCamera;
	C_Camera* mainCam;
};	
