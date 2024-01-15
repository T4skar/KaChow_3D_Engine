#include "C_Transform.h"


C_Transform::C_Transform(std::string uuid) : Component(nullptr, uuid)
{
	type = ComponentType::TRANSFORM;

	resetMatrix();
}

C_Transform::C_Transform(GameObject* parent, std::string uuid) : Component(parent, uuid)
{
	type = ComponentType::TRANSFORM;
	this->mParent = parent;

	resetMatrix();
}

C_Transform::~C_Transform()
{
}

void C_Transform::OnEditor()
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::Text("X\t\t Y\t\t Z");
		ImGui::DragFloat3("Position", mPosition.ptr(), 0.25f);

		ImGui::Text("X\t\t Y\t\t Z");
		ImGui::DragFloat3("Rotation", mRotation.ptr(), 0.25f);

		ImGui::Text("X\t\t Y\t\t Z");
		ImGui::DragFloat3("Scale", mScale.ptr(), 0.25f);
	}


	calculateMatrix();
}

void C_Transform::Enable()
{
	active = false;
}

void C_Transform::Disable()
{
	active = false;
}

float4x4 C_Transform::getGlobalMatrix()
{
	// fer getter de parent
	if (mParent == nullptr) return getLocalMatrix();
	return mLocalMatrix * mParent->mTransform->getGlobalMatrix();
}

float4x4 C_Transform::getLocalMatrix()
{
	return mLocalMatrix;
}

void C_Transform::resetMatrix()
{
	mLocalMatrix = { 1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1 };
	mPosition = { 0,0,0 };
	mRotation = { 0,0,0 };
	mScale = { 1,1,1 };
}

void C_Transform::SaveMatrixBeforePhys() {

	for (int j = 0; j < 16; j++) {
		matrixBeforePhys[j] = mParent->mTransform->matrix[j];
	}

}
void C_Transform::SaveOffsetMatrix() {


	for (int i = 0; i < collidersAffecting.size(); i++) {
		float glMat[16];
		collidersAffecting[i]->colliderAffected->body->getWorldTransform().getOpenGLMatrix(glMat);

		for (int j = 0; j < 16; j++) {
			collidersAffecting[i]->offsetMatrix[j] = glMat[j] - mParent->mTransform->matrix[j];
		}
	}


}

float3 C_Transform::getPosition(bool globalPosition)
{
	if (!globalPosition) return float3(mPosition);

	float4x4 m = getGlobalMatrix();
	return m.Row3(3);
}

void C_Transform::setPosition(float3 pos)
{
	mPosition = pos;
	calculateMatrix();
}

float3 C_Transform::getRotation()
{
	return mRotation;
}

void C_Transform::setRotation(float3 rotation)
{
	mRotation = rotation;
	calculateMatrix();
}

float3 C_Transform::getScale()
{
	return mScale;
}

void C_Transform::setScale(float3 scale)
{
	mScale = scale;
	calculateMatrix();
}

void C_Transform::SetTransformMatrixW(float4x4 matrix)
{
	math::Quat q;
	matrix.Decompose(mPosition, q, mScale);

	mRotation = q.ToEulerXYZ();
	mRotation.x = mRotation.x * RADTODEG;
	mRotation.y = mRotation.y * RADTODEG;
	mRotation.z = mRotation.z * RADTODEG;
}


//void C_Transform::SetTransfoMatrix(float3 position, Quat rotation, float3 scale)
//{
//	// Pilota
//	mPosition = position;
//	mRotation = rotation;
//	mScale = scale;
//}

void C_Transform::calculateMatrix()
{
	float rx = mRotation.x * DEGTORAD;
	float ry = mRotation.y * DEGTORAD;
	float rz = mRotation.z * DEGTORAD;

	Quat q;
	q = Quat::FromEulerXYZ(rx, ry, rz);
	mLocalMatrix = float4x4::FromTRS(mPosition, q, mScale).Transposed();
}

void C_Transform::SetLocalMatrix(float4x4 localMatrix)
{
	mLocalMatrix = localMatrix;
}