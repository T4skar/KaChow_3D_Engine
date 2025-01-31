#pragma once
#include "Module.h"
#include "Globals.h"
#include "Primitive.h"


#include "Bullet/include/btBulletDynamicsCommon.h"
#include "CPhysics.h"
#include "ModuleEditor.h"
#include "glmath.h"


// Recommended scale is 1.0f == 1 meter, no less than 0.2 objects
#define GRAVITY btVector3(0.0f, -10.0f, 0.0f) 

class DebugDrawer;
class ModuleScene;

struct PhysBody3D;
struct PhysVehicle3D;
struct VehicleInfo;

class ModulePhysics3D : public Module
{
public:
	ModulePhysics3D(Application* app, bool start_enabled = true);
	~ModulePhysics3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	void DrawWorld();
	void DrawWorldGame();
	update_status PostUpdate(float dt);
	bool CleanUp();

	PhysBody3D* AddBody(const CSphere& sphere, float mass = 1.0f);
	PhysBody3D* AddBody(const Cube& cube, float mass = 1.0f);
	PhysBody3D* AddBody(const CCylinder& cylinder, float mass = 1.0f);
	PhysVehicle3D* AddVehicle(const VehicleInfo& info);

	btTypedConstraint* AddConstraintP2P(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB);
	void DeleteConstraintP2P(btTypedConstraint* constraint);

	btHingeConstraint* AddConstraintHinge(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisS, const vec3& axisB, bool disable_collision = false);
	void DeleteConstraintHinge(btHingeConstraint* constraint);

	void DeleteVehicle();

	void RemoveBody(PhysBody3D* body);

	void UpdateAABBs();

	PhysBody3D* UpdateBoxColliderSize(PhysBody3D*& collider, float3 colPos, float3 colRot, float3 colScl, float mass = 1.0f);
	PhysBody3D* UpdateSphereColliderSize(PhysBody3D*& collider, float3 colPos, float3 colRot, float radius, float mass = 1.0f);
	PhysBody3D* UpdateCylinderColliderSize(PhysBody3D*& collider, float3 colPos, float3 colRot, float2 radiusHeight, float mass = 1.0f);

	void SaveWorldTransforms();

	ModuleScene* GetScene();

	//PhysBody3D* UpdateSphereColliderSize(PhysBody3D* collider, float mass = 1.0f);
	//PhysBody3D* UpdateCylinderColliderSize(PhysBody3D* collider, float mass = 1.0f);

	bool isWorldOn;

private:
	bool debugGame;

	bool debug;

	btDefaultCollisionConfiguration* collision_conf;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* broad_phase;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* world;
	btDefaultVehicleRaycaster* vehicle_raycaster;
	DebugDrawer* debug_draw;

	std::vector <btCollisionShape*> shapes;
	std::vector <PhysBody3D*> bodies;
	std::vector <btDefaultMotionState*> motions;
	std::vector <btTypedConstraint*> constraints;
	std::vector <PhysVehicle3D*> vehicles;

};

class DebugDrawer : public btIDebugDraw
{
public:
	DebugDrawer() : line(0, 0, 0)
	{}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	int	 getDebugMode() const;

	DebugDrawModes mode;
	CLine line;
	Primitive point;
};
