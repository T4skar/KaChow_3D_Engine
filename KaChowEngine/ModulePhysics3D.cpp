#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "ModuleScene.h"
#include <vector>
#include "glmath.h"

#ifdef _DEBUG
#pragma comment (lib, "Bullet/libx86/BulletDynamics_debug.lib")
#pragma comment (lib, "Bullet/libx86/BulletCollision_debug.lib")
#pragma comment (lib, "Bullet/libx86/LinearMath_debug.lib")
#else
#pragma comment (lib, "Bullet/libx86/BulletDynamics.lib")
#pragma comment (lib, "Bullet/libx86/BulletCollision.lib")
#pragma comment (lib, "Bullet/libx86/LinearMath.lib")
#endif



ModulePhysics3D::ModulePhysics3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;

	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	debug_draw = new DebugDrawer();

	vehicle_raycaster = nullptr;
	world = nullptr;

	isWorldOn = false;

}

// Destructor
ModulePhysics3D::~ModulePhysics3D()
{
	delete debug_draw;
	delete solver;
	delete broad_phase;
	delete dispatcher;
	delete collision_conf;
}

// Render not available yet----------------------------------
bool ModulePhysics3D::Init()
{
	LOG("Creating 3D Physics simulation");
	bool ret = true;

	debug_draw->setDebugMode(1);

	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics3D::Start()
{
	LOG("Creating Physics environment");

	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	world->setDebugDrawer(debug_draw);
	world->setGravity(GRAVITY);
	vehicle_raycaster = new btDefaultVehicleRaycaster(world);

	// Big plane as ground

	debugGame = true;

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PreUpdate(float dt)
{
	//world->stepSimulation(dt, 15);

	if (App->editor->playPressed == false) {
		world->stepSimulation(0);
	}
	else {
		if (App->editor->pausePressed == true) {
			world->stepSimulation(0);
		}
		else {
			world->stepSimulation(dt, 15);
		}

	}


	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		int numContacts = contactManifold->getNumContacts();
		if (numContacts > 0)
		{
			PhysBody3D* pbodyA = (PhysBody3D*)obA->getUserPointer();
			PhysBody3D* pbodyB = (PhysBody3D*)obB->getUserPointer();

			App->scene->OnCollision(pbodyA, pbodyB);


		}
	}

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;



	DrawWorld();

	if ((App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) && App->physics->isWorldOn)
	{
		App->scene->CreateSphere(20);
	}

	return UPDATE_CONTINUE;
}

void ModulePhysics3D::DrawWorld()
{
	if (debug == true)
	{
		world->debugDrawWorld();

		// Render vehicles
		for each (PhysVehicle3D * item in vehicles)
		{
			
		}

	}
}
void ModulePhysics3D::DrawWorldGame()
{
	if (debug == true)
	{
		if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
			debugGame ? debugGame = false : debugGame = true;
		}

		if (debugGame)
			world->debugDrawWorld();

		// Render vehicles
		for each (PhysVehicle3D * item in vehicles)
		{
			
		}

	}
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics3D::CleanUp()
{
	LOG("Destroying 3D Physics elements");

	// Remove from the world all collision bodies
	for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	for each (btTypedConstraint * item in constraints)
	{
		world->removeConstraint(item);
		delete item;
	}

	constraints.clear();

	for each (btDefaultMotionState * item in motions)
	{
		delete item;
	}

	motions.clear();

	for each (btCollisionShape * item in shapes)
	{
		delete item;
	}

	shapes.clear();

	for each (PhysBody3D * item in bodies)
	{
		delete item;
	}

	bodies.clear();

	for each (PhysVehicle3D * item in vehicles)
	{
		delete item;
	}

	vehicles.clear();


	delete vehicle_raycaster;
	delete world;

	return true;
}

void ConvertFloat4x4ToBulletTransform(const math::float4x4& source, btTransform& target)
{
	const float* matrixData = reinterpret_cast<const float*>(source.Transposed().ptr());
	target.setFromOpenGLMatrix(matrixData);
}

// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const CSphere& sphere, float mass)
{
	btCollisionShape* colShape = new btSphereShape(sphere.radius);
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&sphere.transform);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.push_back(pbody);

	return pbody;
}


// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const Cube& cube, float mass)
{
	btCollisionShape* colShape = new btBoxShape(btVector3(cube.size.x * 0.5f, cube.size.y * 0.5f, cube.size.z * 0.5f));
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cube.transform);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);

	bodies.push_back(pbody);

	return pbody;
}

// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const CCylinder& cylinder, float mass)
{
	btCollisionShape* colShape = new btCylinderShapeX(btVector3(cylinder.height * 0.5f, cylinder.radius, 0.0f));
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cylinder.transform);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.push_back(pbody);

	return pbody;
}

PhysVehicle3D* ModulePhysics3D::AddVehicle(const VehicleInfo& info)
{
	btCompoundShape* comShape = new btCompoundShape();
	shapes.push_back(comShape);

	btCollisionShape* colShape = new btBoxShape(btVector3(info.chassis_size.x * 0.5f, info.chassis_size.y * 0.5f, info.chassis_size.z * 0.5f));
	shapes.push_back(colShape);

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z));

	comShape->addChildShape(trans, colShape);

	btTransform startTransform;
	startTransform.setIdentity();

	btVector3 localInertia(0, 0, 0);
	comShape->calculateLocalInertia(info.mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(info.mass, myMotionState, comShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	body->setContactProcessingThreshold(BT_LARGE_FLOAT);
	body->setActivationState(DISABLE_DEACTIVATION);

	world->addRigidBody(body);

	btRaycastVehicle::btVehicleTuning tuning;
	tuning.m_frictionSlip = info.frictionSlip;
	tuning.m_maxSuspensionForce = info.maxSuspensionForce;
	tuning.m_maxSuspensionTravelCm = info.maxSuspensionTravelCm;
	tuning.m_suspensionCompression = info.suspensionCompression;
	tuning.m_suspensionDamping = info.suspensionDamping;
	tuning.m_suspensionStiffness = info.suspensionStiffness;

	btRaycastVehicle* vehicle = new btRaycastVehicle(tuning, body, vehicle_raycaster);

	vehicle->setCoordinateSystem(0, 1, 2);

	for (int i = 0; i < info.num_wheels; ++i)
	{
		btVector3 conn(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
		btVector3 dir(info.wheels[i].direction.x, info.wheels[i].direction.y, info.wheels[i].direction.z);
		btVector3 axis(info.wheels[i].axis.x, info.wheels[i].axis.y, info.wheels[i].axis.z);

		vehicle->addWheel(conn, dir, axis, info.wheels[i].suspensionRestLength, info.wheels[i].radius, tuning, info.wheels[i].front);
	}
	// ---------------------

	PhysVehicle3D* pvehicle = new PhysVehicle3D(body, vehicle, info);
	world->addVehicle(vehicle);

	vehicles.push_back(pvehicle);
	return pvehicle;
}

void ModulePhysics3D::RemoveBody(PhysBody3D* body) {

	world->removeRigidBody(body->body);

}

void ModulePhysics3D::UpdateAABBs()
{
	world->updateAabbs();
}

PhysBody3D* ModulePhysics3D::UpdateBoxColliderSize(PhysBody3D*& collider, float3 colPos, float3 colRot, float3 colScl, float mass)
{
	RemoveBody(collider);
	collider->~PhysBody3D();
	collider = nullptr;

	Cube cube;

	float rx;
	float ry;
	float rz;

	float3 _rot = colRot;

	//	//TransformMatrix(pos, rot, scl);
	rx = _rot.x * DEGTORAD;
	ry = _rot.y * DEGTORAD;
	rz = _rot.z * DEGTORAD;




	cube.transform[0] = cos(ry) * cos(rz);
	cube.transform[1] = sin(ry) * sin(rx) * cos(rz) - (cos(rx) * sin(rz));
	cube.transform[2] = sin(rx) * sin(rz) + sin(ry) * cos(rz) * cos(rx);


	cube.transform[4] = cos(ry) * sin(rz);
	cube.transform[5] = cos(rx) * cos(rz) + sin(ry) * sin(rz) * sin(rz);
	cube.transform[6] = sin(ry) * sin(rz) * cos(rx) - sin(rx) * cos(rz);


	cube.transform[8] = -sin(ry);
	cube.transform[9] = cos(ry) * sin(rx);
	cube.transform[10] = cos(rx) * cos(ry);

	cube.SetPos(colPos.x, colPos.y, colPos.z);
	//cube.SetRotation(ra,rvec);
	//cube.SetRotation(ra, (rx, ry, rz));
	cube.size.x = colScl.x;
	cube.size.y = colScl.y;
	cube.size.z = colScl.z;


	//cube.transform = transpose(cube.transform);


	cube.color = Green;

	collider = AddBody(cube, mass);

	return nullptr;
}

PhysBody3D* ModulePhysics3D::UpdateSphereColliderSize(PhysBody3D*& collider, float3 colPos, float3 colRot, float radius, float mass)
{

	RemoveBody(collider);
	collider->~PhysBody3D();
	collider = nullptr;



	CSphere sphere;


	float rx;
	float ry;
	float rz;

	float3 _rot = colRot;

	//	//TransformMatrix(pos, rot, scl);
	rx = _rot.x * DEGTORAD;
	ry = _rot.y * DEGTORAD;
	rz = _rot.z * DEGTORAD;




	sphere.transform[0] = cos(ry) * cos(rz);
	sphere.transform[1] = sin(ry) * sin(rx) * cos(rz) - (cos(rx) * sin(rz));
	sphere.transform[2] = sin(rx) * sin(rz) + sin(ry) * cos(rz) * cos(rx);


	sphere.transform[4] = cos(ry) * sin(rz);
	sphere.transform[5] = cos(rx) * cos(rz) + sin(ry) * sin(rz) * sin(rz);
	sphere.transform[6] = sin(ry) * sin(rz) * cos(rx) - sin(rx) * cos(rz);


	sphere.transform[8] = -sin(ry);
	sphere.transform[9] = cos(ry) * sin(rx);
	sphere.transform[10] = cos(rx) * cos(ry);

	sphere.SetPos(colPos.x, colPos.y, colPos.z);
	//sphere.SetRotation();
	sphere.radius = radius;
	sphere.color = Green;

	collider = AddBody(sphere, mass);

	return nullptr;
}

PhysBody3D* ModulePhysics3D::UpdateCylinderColliderSize(PhysBody3D*& collider, float3 colPos, float3 colRot, float2 radiusHeight, float mass)
{
	RemoveBody(collider);
	collider->~PhysBody3D();
	collider = nullptr;

	CCylinder cylinder;

	float rx;
	float ry;
	float rz;

	float3 _rot = colRot;

	//	//TransformMatrix(pos, rot, scl);
	rx = _rot.x * DEGTORAD;
	ry = _rot.y * DEGTORAD;
	rz = _rot.z * DEGTORAD;




	cylinder.transform[0] = cos(ry) * cos(rz);
	cylinder.transform[1] = sin(ry) * sin(rx) * cos(rz) - (cos(rx) * sin(rz));
	cylinder.transform[2] = sin(rx) * sin(rz) + sin(ry) * cos(rz) * cos(rx);


	cylinder.transform[4] = cos(ry) * sin(rz);
	cylinder.transform[5] = cos(rx) * cos(rz) + sin(ry) * sin(rz) * sin(rz);
	cylinder.transform[6] = sin(ry) * sin(rz) * cos(rx) - sin(rx) * cos(rz);


	cylinder.transform[8] = -sin(ry);
	cylinder.transform[9] = cos(ry) * sin(rx);
	cylinder.transform[10] = cos(rx) * cos(ry);

	cylinder.SetPos(colPos.x, colPos.y, colPos.z);
	//cylinder.SetRotation();
	cylinder.radius = radiusHeight.x;
	cylinder.height = radiusHeight.y;
	cylinder.color = Green;

	collider = AddBody(cylinder, mass);

	return nullptr;
}

void ModulePhysics3D::SaveWorldTransforms()
{
	
	if (App->scene != nullptr) {
		
		JsonParser& rootGO = App->scene->rootGO;

		for (int i = 0; i < App->scene->ListGO.size(); i++) {
			if (App->scene->ListGO[i]->mTransform != nullptr) {
				App->scene->ListGO[i]->mTransform->SaveMatrixBeforePhys();
				App->scene->ListGO[i]->mTransform->SaveOffsetMatrix();
			}
		}
	}
}


ModuleScene* ModulePhysics3D::GetScene()
{
	return App->scene;
}

// ---------------------------------------------------------
btTypedConstraint* ModulePhysics3D::AddConstraintP2P(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB)
{
	btTypedConstraint* p2p = new btPoint2PointConstraint(
		*(bodyA.body),
		*(bodyB.body),
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z));
	world->addConstraint(p2p);
	constraints.push_back(p2p);
	p2p->setDbgDrawSize(2.0f);

	return p2p;
}

void ModulePhysics3D::DeleteConstraintP2P(btTypedConstraint* constraint)
{

	for (int i = 0; i < constraints.size(); i++) {
		if (constraints[i] == constraint) {
			constraints.erase(constraints.begin() + i);
		}
	}

	world->removeConstraint(constraint);

}


btHingeConstraint* ModulePhysics3D::AddConstraintHinge(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisA, const vec3& axisB, bool disable_collision)
{
	btHingeConstraint* hinge = new btHingeConstraint(
		*(bodyA.body),
		*(bodyB.body),
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z),
		btVector3(axisA.x, axisA.y, axisA.z),
		btVector3(axisB.x, axisB.y, axisB.z));

	world->addConstraint(hinge, disable_collision);
	constraints.push_back(hinge);
	hinge->setDbgDrawSize(2.0f);

	return hinge;
}

void ModulePhysics3D::DeleteConstraintHinge(btHingeConstraint* constraint)
{
	for (int i = 0; i < constraints.size(); i++) {
		if (constraints[i] == constraint) {
			constraints.erase(constraints.begin() + i);
		}
	}

	world->removeConstraint(constraint);
}

void ModulePhysics3D::DeleteVehicle()
{
	std::vector<PhysVehicle3D*> newList;
	//int vSize = vehicles.size();
	
	

	

	world->removeRigidBody(App->scene->vehicle->body);

	world->removeAction(App->scene->vehicle->vehicle);

	for (int i = 0; i < vehicles.size(); i++) {
		//newList.push_back(vehicles.at(i));
		delete vehicles[i];
	}
	for (int i = 0; i < newList.size(); i++) {
		//delete newList.at(i);
	}

	vehicles.clear();
	
}

// =============================================
void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	line.origin.Set(from.getX(), from.getY(), from.getZ());
	line.destination.Set(to.getX(), to.getY(), to.getZ());
	line.color.Set(color.getX(), color.getY(), color.getZ());
	line.Render();
}

void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	point.transform.translate(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
	point.color.Set(color.getX(), color.getY(), color.getZ());
	point.Render();
}

void DebugDrawer::reportErrorWarning(const char* warningString)
{
	LOG("Bullet warning: %s", warningString);
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	LOG("Bullet draw text: %s", textString);
}

void DebugDrawer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes)debugMode;
}

int	 DebugDrawer::getDebugMode() const
{
	return mode;
}
