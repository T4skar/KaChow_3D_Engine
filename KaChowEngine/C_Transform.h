#pragma once
#include "Globals.h"
#include "GameObject.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"
#include "ImGui/imgui.h"
#include <string>
#include "PhysBody3D.h"
#include "glmath.h"

class GameObject;
class Component;

class C_Transform : public Component
{
public:

	class CollidersRelation {
	public:
		CollidersRelation() {};
		~CollidersRelation() {};
		PhysBody3D* colliderAffected;
		mat4x4 offsetMatrix;
	};

	C_Transform(std::string uuid);
	C_Transform(GameObject* parent, std::string uuid);
	~C_Transform();

	void OnEditor() override;

	virtual void Enable();
	virtual void Disable();
	/*virtual Update();*/

	float4x4 getGlobalMatrix();
	float4x4 getLocalMatrix();

	void resetMatrix();

	float3 getPosition(bool globalPosition = false);
	void setPosition(float3 pos);
	float3 getRotation();
	void setRotation(float3 rotation);
	float3 getScale();
	void setScale(float3 scale);
	void SetTransformMatrixW(float4x4 matrix);

	void TransformMatrix(float3 _pos, float3 _rot, float3 _scl);
	void SetGlobalTrans();

	void SetLocalMatrix(float4x4 localMatrix);
	void setIdentity(mat4x4 mat);

	void SetPos(float3 _pos);
	void UpdatePos();

	void SetRot(float3 _rot);
	void UpdateRot();

	void SetScale(float3 _scl);
	void UpdateScl();

	void SetGPos(float3 _pos);
	void SetGRot(float3 _rot);
	void SetGScl(float3 _scl);

	float3 GetPos() { return mPosition; }
	float3 GetRot() { return mRotation; }
	float3 GetScale() { return mScale; }

	float3 GetGPos() { return gpos; }
	float3 GetGRot() { return grot; }
	float3 GetGScale() { return gscl; }

	void SaveMatrixBeforePhys();
	void SaveOffsetMatrix();

	//// Pilota
	//void SetTransfoMatrix(float3 position, Quat rotation, float3 scale);

	void calculateMatrix();

	float3 mPosition;
	float3 mScale;
	float3 mRotation;

	// Posarli matriux local del parent
	float4x4 mGlobalMatrix;
	float4x4 mLocalMatrix;

	mat4x4 matrix;
	float4x4 rmatrix;

	mat4x4 matrixBeforePhys;
	std::vector<CollidersRelation*> collidersAffecting;


	float3 pos = { 0,0,0 };
	float3 rot = { 0,0,0 };
	float3 scl = { 1,1,1 };

private:
	float3 ppos = { 0,0,0 };
	float3 prot = { 0,0,0 };
	float3 pscl = { 1,1,1 };

	float3 gpos = { 0,0,0 };
	float3 grot = { 0,0,0 };
	float3 gscl = { 1,1,1 };
};