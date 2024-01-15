#include "CPhysics.h"
#include "ImGui/imgui.h"
#include "Primitive.h"
#include "ModulePhysics.h"
#include "MathGeoLib/include/Math/MathFunc.h"
#include "PhysBody3D.h"
#include "Module.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "glmath.h"
#include "C_Transform.h"
#include <vector>
#include "GameObject.h"
#include "Application.h"

CPhysics::CPhysics(GameObject* parent) :Component(parent, uuid)
{
	type = ComponentType::PHYSICS;
	this->mParent = parent;

	phys = nullptr;

	shapeSelected = ColliderShape::NONE;

	isShapeSelected[0] = false;
	isShapeSelected[1] = false;
	isShapeSelected[2] = false;

	isShapeCreated[0] = false;
	isShapeCreated[1] = false;
	isShapeCreated[2] = false;

	isStatic = false;
	collider = nullptr;
	hasInit = false;

	sphereRadius = 1.f;
	cylRadiusHeight = { 1.f, 1.f };

	constraintGO = nullptr;
	p2pConstraint = nullptr;
	hingeConstraint = nullptr;

	constraitTypeSelected = ConstraintType::NONE;

	isConstraitSelected[0] = false;
	isConstraitSelected[1] = false;

	isConstraitCreated[0] = false;
	isConstraitCreated[1] = false;

}

CPhysics::~CPhysics()
{

	RemoveCollider();
	RemoveConstraint();
	delete collider;

	phys = nullptr;
	delete phys;

}

/*
void CPhysics::Update()
{
	if (phys->isWorldOn == true) {
		if (collider != nullptr) {
			float glMat[16];
			//
			//



			collider->body->getWorldTransform().getOpenGLMatrix(glMat);

			glMat4x4[0][0] = glMat[0];
			glMat4x4[0][1] = glMat[1];
			glMat4x4[0][2] = glMat[2];
			glMat4x4[0][3] = glMat[3];

			glMat4x4[1][0] = glMat[4];
			glMat4x4[1][1] = glMat[5];
			glMat4x4[1][2] = glMat[6];
			glMat4x4[1][3] = glMat[7];

			glMat4x4[2][0] = glMat[8];
			glMat4x4[2][1] = glMat[9];
			glMat4x4[2][2] = glMat[10];
			glMat4x4[2][3] = glMat[11];

			glMat4x4[3][0] = glMat[12];
			glMat4x4[3][1] = glMat[13];
			glMat4x4[3][2] = glMat[14];
			glMat4x4[3][3] = glMat[15];

			btQuaternion SCLrot = collider->body->getWorldTransform().getRotation();


			Quat SCLquatRot = (Quat)SCLrot;
			float rad = SCLquatRot.Angle();

			btScalar SCLrotAngle = RadToDeg(SCLquatRot.Angle());


			float3 SCLrotAxis;

			SCLrotAxis.x = SCLquatRot.Axis().x;
			SCLrotAxis.y = SCLquatRot.Axis().y;
			SCLrotAxis.z = SCLquatRot.Axis().z;

			SCLrotAxis.Normalized();
			float a = pow(SCLrotAxis.x, 2) + pow(SCLrotAxis.y, 2) + pow(SCLrotAxis.z, 2);
			Quat quat(SCLquatRot);

			mat4x4 rotMatrix;

			rotMatrix[0] = cos(SCLrotAngle) + pow(SCLrotAxis.x, 2) * (1 - cos(SCLrotAngle));
			rotMatrix[4] = SCLrotAxis.x * SCLrotAxis.y * (1 - cos(SCLrotAngle)) - SCLrotAxis.z * sin(SCLrotAngle);
			rotMatrix[8] = SCLrotAxis.x * SCLrotAxis.z * (1 - cos(SCLrotAngle)) + SCLrotAxis.y * sin(SCLrotAngle);
			rotMatrix[12] = 0;

			rotMatrix[1] = SCLrotAxis.y * SCLrotAxis.x * (1 - cos(SCLrotAngle)) + SCLrotAxis.z * sin(SCLrotAngle);
			rotMatrix[5] = cos(SCLrotAngle) + pow(SCLrotAxis.y, 2) * (1 - cos(SCLrotAngle));
			rotMatrix[9] = SCLrotAxis.y * SCLrotAxis.z * (1 - cos(SCLrotAngle)) - SCLrotAxis.x * sin(SCLrotAngle);
			rotMatrix[13] = 0;

			rotMatrix[2] = SCLrotAxis.z * SCLrotAxis.x * (1 - cos(SCLrotAngle)) - SCLrotAxis.y * sin(SCLrotAngle);
			rotMatrix[6] = SCLrotAxis.z * SCLrotAxis.y * (1 - cos(SCLrotAngle)) + SCLrotAxis.x * sin(SCLrotAngle);
			rotMatrix[10] = cos(SCLrotAngle) + pow(SCLrotAxis.z, 2) * (1 - cos(SCLrotAngle));
			rotMatrix[14] = 0;

			rotMatrix[3] = 0;
			rotMatrix[7] = 0;
			rotMatrix[11] = 0;
			rotMatrix[15] = 1;

			mat4x4 beforePhysWithPos = mParent->mTransform->matrixBeforePhys;
			for (int i = 0; i < mParent->mTransform->collidersAffecting.size(); i++) {
				if (mParent->mTransform->collidersAffecting[i]->colliderAffected == collider) {
					beforePhysWithPos[12] = glMat4x4[12];
					beforePhysWithPos[13] = glMat4x4[13];
					beforePhysWithPos[14] = glMat4x4[14];
					beforePhysWithPos[12] -= GO->GOtrans->collidersAffecting[i]->offsetMatrix[12];
					beforePhysWithPos[13] -= GO->GOtrans->collidersAffecting[i]->offsetMatrix[13];
					beforePhysWithPos[14] -= GO->GOtrans->collidersAffecting[i]->offsetMatrix[14];
				}
			}
			GO->GOtrans->matrix = rotMatrix * beforePhysWithPos;

			for (int i = 0; i < GO->GOtrans->collidersAffecting.size(); i++) {
				if (GO->GOtrans->collidersAffecting[i]->colliderAffected == collider) {
					GO->GOtrans->matrix[12] = glMat4x4[12];
					GO->GOtrans->matrix[13] = glMat4x4[13];
					GO->GOtrans->matrix[14] = glMat4x4[14];

					GO->GOtrans->matrix[12] -= GO->GOtrans->collidersAffecting[i]->offsetMatrix[12];
					GO->GOtrans->matrix[13] -= GO->GOtrans->collidersAffecting[i]->offsetMatrix[13];
					GO->GOtrans->matrix[14] -= GO->GOtrans->collidersAffecting[i]->offsetMatrix[14];
				}
			}

			for (int i = 0; i < GO->children.size(); i++) {
				{
					for (int j = 0; j < GO->children[i]->GOtrans->collidersAffecting.size(); j++) {

						if (GO->children[i]->GOtrans->collidersAffecting[j]->colliderAffected == collider) {

							GO->children[i]->GOtrans->matrix[12] = glMat4x4[12];
							GO->children[i]->GOtrans->matrix[13] = glMat4x4[13];
							GO->children[i]->GOtrans->matrix[14] = glMat4x4[14];

							GO->children[i]->GOtrans->matrix[12] -= GO->children[i]->GOtrans->collidersAffecting[j]->offsetMatrix[12];
							GO->children[i]->GOtrans->matrix[13] -= GO->children[i]->GOtrans->collidersAffecting[j]->offsetMatrix[13];
							GO->children[i]->GOtrans->matrix[14] -= GO->children[i]->GOtrans->collidersAffecting[j]->offsetMatrix[14];


						}



					}

				}







			}
		}
	}
}

*/