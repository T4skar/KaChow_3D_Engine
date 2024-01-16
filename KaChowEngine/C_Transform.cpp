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

	setIdentity(matrix);

	if (mParent != nullptr && mParent->mParent != nullptr)
	{
		ppos = mParent->mParent->mTransform->GetPos();
		prot = mParent->mParent->mTransform->GetRot();
		pscl = mParent->mParent->mTransform->GetScale();

	}

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
		if(ImGui::DragFloat3("Scale", mScale.ptr(), 0.25f)) {
			if (mParent->GOphys != nullptr) {
				if (mParent->GOphys->collider != nullptr) {
					SaveOffsetMatrix();
				}
			}
			setScale(mScale);
		}
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

void C_Transform::setIdentity(mat4x4 mat)
{
	for (int i = 0; i < 15; ++i)
	{
		if (i == 0 || i == 5 || i == 10 || i == 15) mat[i] = 1;
		else mat[i] = 0;
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

void C_Transform::TransformMatrix(float3 _pos, float3 _rot, float3 _scl)
{


	float x = _rot.x * DEGTORAD;
	float y = _rot.y * DEGTORAD;
	float z = _rot.z * DEGTORAD;



	Quat qrot = Quat::FromEulerXYZ(x, y, z);


	rmatrix = float4x4::FromTRS(_pos, qrot, _scl).Transposed();


	matrix.translate(_pos.x, _pos.y, _pos.z);
	matrix.scale(_scl.x, _scl.y, _scl.z);



	if (mParent->GetCameraComponent() != nullptr)
	{
		mParent->GetCameraComponent()->frustum.pos = _pos;
		mParent->GetCameraComponent()->reference = _pos;
		if (mParent->GOphys != nullptr)
		{
			if (mParent->GOphys->collider != nullptr)
			{
				mParent->GOphys->colPos = _pos;
				mParent->GOphys->CallUpdateShape();
			}
		}
		if (!App->physics->isWorldOn)
		{
			mParent->GetCameraComponent()->frustum.up = rmatrix.RotatePart().Col(1).Normalized();
			mParent->GetCameraComponent()->frustum.front = rmatrix.RotatePart().Col(2).Normalized();
		}


	}





}

void C_Transform::SetGlobalTrans()
{
	if (mParent->mParent->mParent == nullptr)
	{

		TransformMatrix(pos, rot, scl);
	}
	else
	{
		gpos = ppos + pos;
		grot = prot + rot;
		gscl.x = pscl.x * scl.x;
		gscl.y = pscl.y * scl.y;
		gscl.z = pscl.z * scl.z;
		TransformMatrix(gpos, grot, gscl);

	}

	if (mParent->GOphys != nullptr) {
		for (int i = 0; i < collidersAffecting.size(); i++) {

			mParent->GOphys->colPos.x = matrix[12] + collidersAffecting[i]->offsetMatrix[12];
			mParent->GOphys->colPos.y = matrix[13] + collidersAffecting[i]->offsetMatrix[13];
			mParent->GOphys->colPos.z = matrix[14] + collidersAffecting[i]->offsetMatrix[14];
			mParent->GOphys->CallUpdateShape();
		}
	}
}

void C_Transform::SetPos(float3 _pos)
{

	pos = _pos;



	UpdatePos();
}

void C_Transform::UpdatePos()
{

	float3 cpos = ppos + pos;
	if (mParent->mChildren.size() != 0)
	{

		for (uint i = 0; i < mParent->mChildren.size(); ++i)
		{
			mParent->mChildren[i]->mTransform->SetGPos(cpos);

		}
	}



	SetGlobalTrans();
}

void C_Transform::SetGPos(float3 _pos)
{

	ppos = _pos;

	UpdatePos();
}



void C_Transform::SetRot(float3 _rot) {
	rot = _rot;

	if (rot.x >= 361)
	{
		rot.x = 0;
	}
	if (rot.y >= 361)
	{
		rot.y = 0;
	}
	if (rot.z >= 361)
	{
		rot.z = 0;
	}

	if (rot.x < 0)
	{
		rot.x = 360;
	}
	if (rot.y < 0)
	{
		rot.y = 360;
	}
	if (rot.z < 0)
	{
		rot.z = 360;
	}

	UpdateRot();


}

void C_Transform::UpdateRot()
{

	float3 crot = prot + rot;
	if (mParent->mChildren.size() != 0)
	{

		for (uint i = 0; i < mParent->mChildren.size(); ++i)
		{
			mParent->mChildren[i]->mTransform->SetGRot(crot);

		}
	}



	SetGlobalTrans();

}

void C_Transform::SetGRot(float3 _rot)
{

	prot = _rot;

	UpdateRot();
}



void C_Transform::SetScale(float3 _scl) {
	scl = _scl;

	UpdateScl();

}

void C_Transform::UpdateScl()
{

	float3 cscl;

	cscl.x = mParent->GetParent()->mTransform->GetScale().x * scl.x;
	cscl.y = mParent->GetParent()->mTransform->GetScale().y * scl.y;
	cscl.z = mParent->GetParent()->mTransform->GetScale().z * scl.z;

	if (mParent->mChildren.size() != 0)
	{
		for (uint i = 0; i < mParent->mChildren.size(); ++i)
		{
			mParent->mChildren[i]->mTransform->SetGScl(cscl);
		}
	}

	SetGlobalTrans();
}

void C_Transform::SetGScl(float3 _scl)
{

	pscl = _scl;

	UpdateScl();
}