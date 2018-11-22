#include "Unit.h"

Unit::Unit()
	: mPosition(X::Math::Vector3::Zero())
	, mDirection(X::Math::Vector3::ZAxis())
	, mSelected(false)
{
}

void Unit::Update(float deltaTime)
{
	if (HasDestination())
	{
		const float kMoveSpeed = 2.0f;
		mDirection = X::Math::Normalize(mDestination - mPosition);
		mPosition += mDirection * kMoveSpeed * deltaTime;
	}
}

void Unit::Draw(bool master)
{
	X::Math::Vector3 center(mPosition.x, mPosition.y + 0.25f, mPosition.z);
	X::Math::Vector3 extend(0.5f, 0.25f, 1.0f);
	const float angle = atan2(mDirection.x, mDirection.z);
	X::Math::Quaternion orientation = X::Math::QuaternionRotationAxis(X::Math::Vector3::YAxis(), angle);

	X::Math::OBB obb(center, extend, orientation);

	if(master)
		X::DrawOBB(obb, X::Math::Vector4::Cyan());
	else
		X::DrawOBB(obb, X::Math::Vector4::LightRed());

	if (mSelected)
	{
		const float minX = mPosition.x - 1.0f;
		const float minY = mPosition.y;
		const float minZ = mPosition.z - 1.0f;
		const float maxX = mPosition.x + 1.0f;
		const float maxY = mPosition.y;
		const float maxZ = mPosition.z + 1.0f;
		X::DrawAABB(minX, minY, minZ, maxX, maxY, maxZ, X::Math::Vector4::Green());

		if (HasDestination())
		{
			X::DrawSphere(mDestination, 0.5f, X::Math::Vector4::Magenta(), 6, 2);
		}
	}
}

void Unit::Serialize(Network::StreamWriter& writer)
{
	writer.Write(mPosition.x);
	writer.Write(mPosition.z);
	writer.Write(mDirection.x);
	writer.Write(mDirection.z);
}

void Unit::Deserialize(Network::StreamReader& reader)
{
	reader.Read(mPosition.x);
	reader.Read(mPosition.z);
	reader.Read(mDirection.x);
	reader.Read(mDirection.z);

	// Make sure destination matches position for now
	mDestination = mPosition;
}

bool Unit::HasDestination() const
{
	const float checkDist = 0.5f;
	
	return X::Math::DistanceXZSqr(mPosition, mDestination) > X::Math::Sqr(checkDist);
}