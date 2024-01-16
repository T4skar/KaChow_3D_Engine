#pragma once
#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include <string>

struct Mesh;
class C_Camera : public Component
{
public:
	C_Camera(std::string uuid);
	C_Camera(GameObject owner, std::string uuid);
	~C_Camera();

	void Update();
	void OnEditor() override;

	void Look(const float3& Position, const float3& Reference);
	void LookAt(const float3& Spot);
	void Move(const float3& Movement);
	float* GetViewMatrix();
	float* GetProjetionMatrix();
	void CreateFrameBuffer();
	bool ObjectInsideFrustrum(Mesh* mesh);

	Frustum frustum;
	float3 reference;
	float3 Z = {0,0,1};
	int typeCameraSelected ;

	int FOV;
	float farDistance;
	float nearDistance;

	bool isMainCamera = false;

	unsigned int cameraBuffer;
	unsigned int frameBuffer;

private:
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	unsigned int renderObjBuffer;
};