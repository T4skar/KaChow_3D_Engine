#include "CPhysics.h"
#include "ImGui/imgui.h"
#include "Primitive.h"
#include "ModulePhysics3D.h"
#include "MathGeoLib/include/Math/MathFunc.h"
#include "PhysBody3D.h"
#include "Module.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "glmath.h"
#include "C_Transform.h"
#include <vector>
#include "GameObject.h"
#include "Application.h"

CPhysics::CPhysics(std::string uuid) :Component(nullptr, uuid)
{
	type = ComponentType::PHYSICS;
	this->mParent = nullptr;

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

	glMat4x4 = nullptr;
	constraintGO = nullptr;
	p2pConstraint = nullptr;
	hingeConstraint = nullptr;

	constraitTypeSelected = ConstraintType::NONE;

	isConstraitSelected[0] = false;
	isConstraitSelected[1] = false;

	isConstraitCreated[0] = false;
	isConstraitCreated[1] = false;


}
CPhysics::CPhysics(GameObject* parent, std::string uuid) :Component(parent, uuid) 
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

	glMat4x4 = nullptr;
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
					beforePhysWithPos[12] = glMat4x4[12][i];
					beforePhysWithPos[13] = glMat4x4[13][i];
					beforePhysWithPos[14] = glMat4x4[14][i];
					beforePhysWithPos[12] -= mParent->mTransform->collidersAffecting[i]->offsetMatrix[12];
					beforePhysWithPos[13] -= mParent->mTransform->collidersAffecting[i]->offsetMatrix[13];
					beforePhysWithPos[14] -= mParent->mTransform->collidersAffecting[i]->offsetMatrix[14];
				}
			}
			mParent->mTransform->matrix = rotMatrix * beforePhysWithPos;

			for (int i = 0; i < mParent->mTransform->collidersAffecting.size(); i++) {
				if (mParent->mTransform->collidersAffecting[i]->colliderAffected == collider) {
					mParent->mTransform->matrix[12] = glMat4x4[12][i];
					mParent->mTransform->matrix[13] = glMat4x4[13][i];
					mParent->mTransform->matrix[14] = glMat4x4[14][i];

					mParent->mTransform->matrix[12] -= mParent->mTransform->collidersAffecting[i]->offsetMatrix[12];
					mParent->mTransform->matrix[13] -= mParent->mTransform->collidersAffecting[i]->offsetMatrix[13];
					mParent->mTransform->matrix[14] -= mParent->mTransform->collidersAffecting[i]->offsetMatrix[14];
				}
			}

			for (int i = 0; i < mParent->mChildren.size(); i++) {
				{
					for (int j = 0; j < mParent->mChildren[i]->mTransform->collidersAffecting.size(); j++) {

						if (mParent->mChildren[i]->mTransform->collidersAffecting[j]->colliderAffected == collider) {

							mParent->mChildren[i]->mTransform->matrix[12] = glMat4x4[12][i];
							mParent->mChildren[i]->mTransform->matrix[13] = glMat4x4[13][i];
							mParent->mChildren[i]->mTransform->matrix[14] = glMat4x4[14][i];

							mParent->mChildren[i]->mTransform->matrix[12] -= mParent->mChildren[i]->mTransform->collidersAffecting[j]->offsetMatrix[12];
							mParent->mChildren[i]->mTransform->matrix[13] -= mParent->mChildren[i]->mTransform->collidersAffecting[j]->offsetMatrix[13];
							mParent->mChildren[i]->mTransform->matrix[14] -= mParent->mChildren[i]->mTransform->collidersAffecting[j]->offsetMatrix[14];


						}



					}

				}







			}
		}
	}
}



void CPhysics::CheckShapes() {

	for (int h = 0; h < static_cast<int>(ColliderShape::Count); h++) {


		if (isShapeSelected[h] == true) {
			if (isShapeCreated[h] == false)
			{
				if (shapeSelected != ColliderShape::NONE && shapeSelected != static_cast<ColliderShape>(h)) {

				}
				shapeSelected = static_cast<ColliderShape>(h);
				isShapeCreated[h] = true;
			}
		}
		else {
			if (isShapeCreated[h] == true) {

			}
		}

	}

	int a = 0;
	for (int i = 0; i < static_cast<int>(ColliderShape::Count); i++) {
		if (isShapeSelected[i] == true)
		{
			a++;
		}
	}

	if (a == 0) {
		shapeSelected = ColliderShape::NONE;
		for (int i = 0; i < static_cast<int>(ColliderShape::Count); i++) {
			if (isShapeCreated[i] == true)
			{
				isShapeCreated[i] = false;
			}
		}
	}

	if (shapeSelected == ColliderShape::NONE)
	{
		for (int i = 0; i < static_cast<int>(ColliderShape::Count); i++) {
			if (isShapeSelected[i] == true) {
				isShapeSelected[i] = false;
				if (isShapeCreated[i] == true) {
					isShapeCreated[i] = false;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < static_cast<int>(ColliderShape::Count); i++) {

			if (i != static_cast<int>(shapeSelected)) {
				if (isShapeSelected[i] == true) {
					isShapeSelected[i] = false;
					if (isShapeCreated[i] == true) {
						isShapeCreated[i] = false;
					}
				}
			}

		}
	}
}

void CPhysics::CheckConstraints()
{
	for (int h = 0; h < static_cast<int>(ConstraintType::Count); h++) {

		if (isConstraitSelected[h] == true) {
			if (isConstraitCreated[h] == false)
			{
				if (constraitTypeSelected != ConstraintType::NONE && constraitTypeSelected != static_cast<ConstraintType>(h)) {

				}
				constraitTypeSelected = static_cast<ConstraintType>(h);
				isConstraitCreated[h] = true;
			}
		}
		else {
			if (isConstraitCreated[h] == true) {

			}
		}

	}

	int a = 0;
	for (int i = 0; i < static_cast<int>(ConstraintType::Count); i++) {
		if (isConstraitSelected[i] == true)
		{
			a++;
		}
	}

	if (a == 0) {
		constraitTypeSelected = ConstraintType::NONE;
		for (int i = 0; i < static_cast<int>(ConstraintType::Count); i++) {
			if (isConstraitCreated[i] == true)
			{
				isConstraitCreated[i] = false;
			}
		}
	}

	if (constraitTypeSelected == ConstraintType::NONE)
	{
		for (int i = 0; i < static_cast<int>(ConstraintType::Count); i++) {
			if (isConstraitSelected[i] == true) {
				isConstraitSelected[i] = false;
				if (isConstraitCreated[i] == true) {
					isConstraitCreated[i] = false;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < static_cast<int>(ConstraintType::Count); i++) {

			if (i != static_cast<int>(constraitTypeSelected)) {
				if (isConstraitSelected[i] == true) {
					isConstraitSelected[i] = false;
					if (isConstraitCreated[i] == true) {
						isConstraitCreated[i] = false;
					}
				}
			}

		}
	}
}

void CPhysics::CallUpdateShape()
{
	float mass;

	if (isStatic == true) {
		mass = 0.f;
	}
	else {
		mass = 1.f;
	}


	for (int i = 0; i < mParent->mTransform->collidersAffecting.size(); i++) {

		if (mParent->mTransform->collidersAffecting[i]->colliderAffected == collider) {

			C_Transform::CollidersRelation* colPtr = mParent->mTransform->collidersAffecting[i];
			mParent->mTransform->collidersAffecting.erase(mParent->mTransform->collidersAffecting.begin() + i);
			delete*& colPtr;
			colPtr = nullptr;

		}
	}

	for (int i = 0; i < mParent->mChildren.size(); i++) {

		for (int j = 0; j < mParent->mChildren[i]->mTransform->collidersAffecting.size(); j++) {

			if (mParent->mChildren[i]->mTransform->collidersAffecting[j]->colliderAffected == collider) {

				C_Transform::CollidersRelation* colPtr = mParent->mChildren[i]->mTransform->collidersAffecting[j];
				mParent->mChildren[i]->mTransform->collidersAffecting.erase(mParent->mChildren[i]->mTransform->collidersAffecting.begin() + j);
				delete*& colPtr;
				colPtr = nullptr;
			}
		}
	}

	switch (shapeSelected)
	{
	case CPhysics::ColliderShape::BOX:
		phys->UpdateBoxColliderSize(collider, colPos, colRot, colScl, mass);
		break;
	case CPhysics::ColliderShape::SPHERE:
		phys->UpdateSphereColliderSize(collider, colPos, colRot, sphereRadius, mass);
		break;
	case CPhysics::ColliderShape::CYLINDER:
		phys->UpdateCylinderColliderSize(collider, colPos, colRot, cylRadiusHeight, mass);
		break;
	default:
		break;
	}

	AddColliderRelations();

	mParent->mTransform->SaveOffsetMatrix();

}

void CPhysics::CreateCollider()
{

	float rx;
	float ry;
	float rz;

	float3 _rot = mParent->mTransform->getRotation();
	float3 _grot = mParent->mTransform->GetGRot();

	if (mParent->mParent->mParent == nullptr)
	{

		//TransformMatrix(pos, rot, scl);
		rx = _rot.x * DEGTORAD;
		ry = _rot.y * DEGTORAD;
		rz = _rot.z * DEGTORAD;
	}
	else
	{
		rx = _grot.x * DEGTORAD;
		ry = _grot.y * DEGTORAD;
		rz = _grot.z * DEGTORAD;


	}
	Quat qrot = Quat::FromEulerXYZ(rx, ry, rz);

	//rotation vector
	vec3 rvec(qrot.Axis().x, qrot.Axis().y, qrot.Axis().z);

	//rotation angle
	float ra = qrot.Angle();

	float mass;
	if (isStatic == true) {
		mass = 0.f;
	}
	else {
		mass = 1.f;
	}

	switch (shapeSelected)
	{
	case CPhysics::ColliderShape::BOX:
	{
		Cube cube;
		mParent->mTransform->setIdentity(cube.transform);

		cube.SetPos(colPos.x, colPos.y, colPos.z);
		//cube.SetRotation();
		cube.size.x = mParent->mTransform->getScale().x;
		cube.size.y = mParent->mTransform->getScale().y;
		cube.size.z = mParent->mTransform->getScale().z;


		cube.color = Green;


		collider = phys->AddBody(cube, mass);
		AddColliderRelations();

	}
	break;
	case CPhysics::ColliderShape::SPHERE:
	{
		CSphere sphere;

		sphere.SetPos(colPos.x, colPos.y, colPos.z);
		sphere.SetRotation(ra, rvec);
		sphere.radius = 1.0f;

		sphere.color = Green;

		collider = phys->AddBody(sphere, mass);
		AddColliderRelations();
	}
	break;
	case CPhysics::ColliderShape::CYLINDER:
	{
		CCylinder cylinder;

		cylinder.SetPos(colPos.x, colPos.y, colPos.z);
		cylinder.SetRotation(ra, rvec);
		cylinder.radius = 1.0f;
		cylinder.height = 1.0f;
		cylinder.color = Green;

		collider = phys->AddBody(cylinder, mass);
		//collider->SetTransform(&GO->GOtrans->matrix);
		AddColliderRelations();
	}
	break;
	default:
		break;
	}
}

void CPhysics::AddColliderRelations()
{
	C_Transform::CollidersRelation* newRelation = new C_Transform::CollidersRelation();
	newRelation->colliderAffected = collider;
	mat4x4 newMat;
	newRelation->offsetMatrix = newMat;
	mParent->mTransform->collidersAffecting.push_back(newRelation);
	mParent->mTransform->SaveOffsetMatrix();


	for (int i = 0; i < mParent->mChildren.size(); i++) {
		C_Transform::CollidersRelation* newRel = new C_Transform::CollidersRelation();
		newRel->colliderAffected = collider;
		mat4x4 newMt;
		newRelation->offsetMatrix = newMt;
		mParent->mChildren[i]->mTransform->collidersAffecting.push_back(newRel);
		mParent->mChildren[i]->mTransform->SaveOffsetMatrix();

	}


}

void CPhysics::RemoveCollider()
{
	if (collider == nullptr) {
		return;
	}
	for (int i = 0; i < mParent->mTransform->collidersAffecting.size(); i++) {

		if (mParent->mTransform->collidersAffecting[i]->colliderAffected == collider) {

			C_Transform::CollidersRelation* colPtr = mParent->mTransform->collidersAffecting[i];
			mParent->mTransform->collidersAffecting.erase(mParent->mTransform->collidersAffecting.begin() + i);
			delete*& colPtr;
			colPtr = nullptr;

		}
	}

	for (int i = 0; i < mParent->mChildren.size(); i++) {

		for (int j = 0; j < mParent->mChildren[i]->mTransform->collidersAffecting.size(); j++) {

			if (mParent->mChildren[i]->mTransform->collidersAffecting[j]->colliderAffected == collider) {

				C_Transform::CollidersRelation* colPtr = mParent->mChildren[i]->mTransform->collidersAffecting[j];
				mParent->mChildren[i]->mTransform->collidersAffecting.erase(mParent->mChildren[i]->mTransform->collidersAffecting.begin() + j);
				delete*& colPtr;
				colPtr = nullptr;
			}
		}
	}

	phys->RemoveBody(collider);
	collider->~PhysBody3D();
	collider = nullptr;


}

void CPhysics::RemoveConstraint()
{
	if (p2pConstraint != nullptr) {
		phys->DeleteConstraintP2P(p2pConstraint);

		p2pConstraint = nullptr;

	}

	if (hingeConstraint != nullptr) {
		phys->DeleteConstraintP2P(hingeConstraint);

		hingeConstraint = nullptr;

	}

	if (constraintGO != nullptr) {
		if (constraintGO->GOphys->p2pConstraint != nullptr) {
			constraintGO->GOphys->p2pConstraint = nullptr;
		}

		if (constraintGO->GOphys->hingeConstraint != nullptr) {
			constraintGO->GOphys->hingeConstraint = nullptr;
		}

		if (constraintGO->GOphys->constraintGO != nullptr) {
			constraintGO->GOphys->constraintGO = nullptr;
		}

	}

	if (constraintGO != nullptr) {
		constraintGO = nullptr;
	}



}


void CPhysics::OnEditor()
{
	if (ImGui::CollapsingHeader("Physics"))
	{
		//if (ImGui::CollapsingHeader("Physic Body"))
		{
			if (ImGui::Checkbox("Static", &isStatic)) {
				if (collider != nullptr) {
					CallUpdateShape();
				}

			}
		}

		//if (ImGui::CollapsingHeader("Collider"))
		{
			if (collider == nullptr)
			{
				ImGui::Text("This object has no collider attached to it.");
				ImGui::Text("Select one shape and create one.");

				ImGui::Checkbox("Box", &isShapeSelected[0]);

				ImGui::Checkbox("Sphere", &isShapeSelected[1]);

				ImGui::Checkbox("Cylinder", &isShapeSelected[2]);

				CheckShapes();

				if (shapeSelected != ColliderShape::NONE) {
					if (ImGui::Button("Create Collider"))
					{
						colPos = mParent->mTransform->getPosition();
						CreateCollider();
					}
				}

			}
			else {
				const char* colliderType = "";
				switch (shapeSelected)
				{
				case CPhysics::ColliderShape::BOX:
				{
					colliderType = "Box";
				}
				break;
				case CPhysics::ColliderShape::SPHERE:
				{
					colliderType = "Sphere";
				}
				break;
				case CPhysics::ColliderShape::CYLINDER:
				{
					colliderType = "Cylinder";
				}
				break;
				default:
					break;
				}
				std::string colName = "Collider Attached: ";
				colName += colliderType;
				ImGui::Text(colName.c_str());

				switch (shapeSelected)
				{
				case CPhysics::ColliderShape::BOX:
				{
					ImGui::Text("X	");
					ImGui::SameLine();
					ImGui::Text("Y	");
					ImGui::SameLine();
					ImGui::Text("Z	");
					if (ImGui::DragFloat3("Position: ", colPos.ptr(), 0.1)) {
						CallUpdateShape();
					}
					ImGui::Text("X	");
					ImGui::SameLine();
					ImGui::Text("Y	");
					ImGui::SameLine();
					ImGui::Text("Z	");
					if (ImGui::DragFloat3("Rotation: ", colRot.ptr(), 0.1)) {
						CallUpdateShape();
					}
					ImGui::Text("X	");
					ImGui::SameLine();
					ImGui::Text("Y	");
					ImGui::SameLine();
					ImGui::Text("Z	");
					if (ImGui::DragFloat3("Scale: ", colScl.ptr(), 0.1)) {
						CallUpdateShape();
					}
				}
				break;
				case CPhysics::ColliderShape::SPHERE:
				{
					ImGui::Text("X	");
					ImGui::SameLine();
					ImGui::Text("Y	");
					ImGui::SameLine();
					ImGui::Text("Z	");
					if (ImGui::DragFloat3("Position: ", colPos.ptr(), 0.1)) {
						CallUpdateShape();
					}
					ImGui::Text("X	");
					ImGui::SameLine();
					ImGui::Text("Y	");
					ImGui::SameLine();
					ImGui::Text("Z	");
					if (ImGui::DragFloat3("Rotation: ", colRot.ptr(), 0.1)) {
						CallUpdateShape();
					}
					if (ImGui::DragFloat("Radius: ", &sphereRadius, 0.1)) {
						CallUpdateShape();
					}
				}
				break;
				case CPhysics::ColliderShape::CYLINDER:
				{
					ImGui::Text("X	");
					ImGui::SameLine();
					ImGui::Text("Y	");
					ImGui::SameLine();
					ImGui::Text("Z	");
					if (ImGui::DragFloat3("Position: ", colPos.ptr(), 0.1)) {
						CallUpdateShape();
					}
					ImGui::Text("X	");
					ImGui::SameLine();
					ImGui::Text("Y	");
					ImGui::SameLine();
					ImGui::Text("Z	");
					if (ImGui::DragFloat3("Rotation: ", colRot.ptr(), 0.1)) {
						CallUpdateShape();
					}
					if (ImGui::DragFloat2("Radius & Height: ", cylRadiusHeight.ptr(), 0.1)) {
						CallUpdateShape();
					}
				}
				break;
				default:
					break;
				}




				if (ImGui::Button("Remove Collider"))
				{

					RemoveCollider();

				}

				if (constraintGO == nullptr && p2pConstraint == nullptr && hingeConstraint == nullptr) {

					ImGui::Checkbox("P2P", &isConstraitSelected[0]);

					ImGui::Checkbox("Hinge", &isConstraitSelected[1]);

					CheckConstraints();


					if (constraitTypeSelected != ConstraintType::NONE && constraitTypeSelected != ConstraintType::Count) {


						std::vector<GameObject*> constraintTargs;
						std::vector<const char*> constraintTargsStr;
						std::vector<std::string> constraintTargsString;

						for (int k = 0; k < phys->GetScene()->ListGO.size(); k++) {
							if (phys->GetScene()->ListGO[k] != mParent) {
								if (phys->GetScene()->ListGO[k]->GOphys != nullptr) {

									if (phys->GetScene()->ListGO[k]->GOphys->collider != nullptr) {

										int count = 0;
										for (int l = 0; l < k; l++) {
											if (phys->GetScene()->ListGO[l]->GOphys != nullptr) {
												if (phys->GetScene()->ListGO[l]->GOphys->collider != nullptr) {
													if (phys->GetScene()->ListGO[l]->name == phys->GetScene()->ListGO[k]->name) {
														count++;
													}
												}
											}

										}

										if (count > 0) {
											//const char* new_count = "(" + to_string(count);
											std::string new_name = phys->GetScene()->ListGO[k]->name.c_str();
											new_name += " (";
											new_name += std::to_string(count);
											new_name += ")";
											const char* new_ch = new_name.c_str();
											constraintTargsString.push_back(new_name);
										}
										else {
											constraintTargsString.push_back(phys->GetScene()->ListGO[k]->name.c_str());
										}
										constraintTargs.push_back(phys->GetScene()->ListGO[k]);
									}



								}
							}
						}



						if (ImGui::Button("Add Constraint"))
						{

							ImGui::OpenPopup("ConstraintTargets");

						}


						if (ImGui::BeginPopup("ConstraintTargets"))
						{

							for (int j = 0; j < constraintTargsString.size(); j++) {
								if (ImGui::Selectable(constraintTargsString[j].c_str())) {

									switch (constraitTypeSelected)
									{
									case CPhysics::ConstraintType::P2P:
									{
										constraintGO = constraintTargs[j];

										vec3 anchor1 = (colPos.x, colPos.y, colPos.z);
										vec3 anchor2 = (constraintGO->GOphys->colPos.x, constraintGO->GOphys->colPos.y, constraintGO->GOphys->colPos.z);
										p2pConstraint = phys->AddConstraintP2P(*collider, *constraintGO->GOphys->collider, anchor1, anchor2);

										constraintGO->GOphys->p2pConstraint = p2pConstraint;
										constraintGO->GOphys->constraintGO = mParent;
									}
									break;
									case CPhysics::ConstraintType::HINGE:
									{
										constraintGO = constraintTargs[j];

										vec3 anchor1 = (colPos.x, colPos.y, colPos.z);
										vec3 anchor2 = (constraintGO->GOphys->colPos.x, constraintGO->GOphys->colPos.y, constraintGO->GOphys->colPos.z);

										p2pConstraint = phys->AddConstraintP2P(*collider, *constraintGO->GOphys->collider, anchor1, anchor2);

										constraintGO->GOphys->hingeConstraint = hingeConstraint;
										constraintGO->GOphys->constraintGO = mParent;
									}
									break;
									default:
										break;
									}




								}


							}

							ImGui::EndPopup();
						}
					}

				}
				else {

					std::string constraintText;// = "Constraint attached: ";

					switch (constraitTypeSelected)
					{
					case CPhysics::ConstraintType::P2P:
					{
						constraintText = "P2P Constraint attached: ";
					}
					break;
					case CPhysics::ConstraintType::HINGE:
					{
						constraintText = "Hinge Constraint attached: ";
					}
					break;
					default:
						break;
					}


					for (int k = 0; k < phys->GetScene()->ListGO.size(); k++) {
						if (phys->GetScene()->ListGO[k] == constraintGO) {
							int count = 0;
							for (int l = 0; l < k; l++) {
								if (phys->GetScene()->ListGO[l]->GOphys != nullptr) {
									if (phys->GetScene()->ListGO[l]->GOphys->collider != nullptr) {
										if (phys->GetScene()->ListGO[l]->name == phys->GetScene()->ListGO[k]->name) {
											count++;
										}
									}
								}
							}

							if (count > 0) {
								//const char* new_count = "(" + to_string(count);
								std::string new_name = phys->GetScene()->ListGO[k]->name.c_str();
								new_name += " (";
								new_name += std::to_string(count);
								new_name += ")";

								constraintText += new_name;
							}
							else {
								constraintText += phys->GetScene()->ListGO[k]->name.c_str();
							}
						}

					}
					ImGui::Text(constraintText.c_str());
					ImGui::Text("");
					if (ImGui::Button("Remove Constraint"))
					{

						RemoveConstraint();

					}
				}





			}

		}

		ImGui::Text("");

		if (ImGui::Button("Remove Component", ImVec2(140, 20)))
		{
			RemoveCollider();

			for (int i = 0; i < mParent->mComponents.size(); i++) {
				if (mParent->mComponents[i]->type == ComponentType::PHYSICS) {
					mParent->mComponents.erase(mParent->mComponents.begin() + i);
				}
			}

			mParent->GOphys = nullptr;
			delete this;

		}

	}
}
