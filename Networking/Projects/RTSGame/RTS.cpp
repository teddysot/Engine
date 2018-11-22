#include "RTS.h"

using namespace RTS;

namespace
{
	X::Math::Vector3 camPosition(0.0f, 30.0f, 0.0f);
	X::Math::Vector3 camDirection(0.0f, -1.0f, 0.1f);

	bool drag = false;
	X::Math::Vector3 dragStart;
}

void RTS::UpdateCamera(float deltaTime)
{
	if (!drag)
	{
		const float kMoveSpeed = 20.0f;
		if (X::IsMouseRightEdge())
		{
			camPosition.x += kMoveSpeed * deltaTime;
		}
		else if (X::IsMouseLeftEdge())
		{
			camPosition.x -= kMoveSpeed * deltaTime;
		}
		if (X::IsMouseTopEdge())
		{
			camPosition.z += kMoveSpeed * deltaTime;
		}
		else if (X::IsMouseBottomEdge())
		{
			camPosition.z -= kMoveSpeed * deltaTime;
		}
	}

	X::SetCameraPosition(camPosition);
	X::SetCameraDirection(camDirection);
}

void RTS::DrawGround()
{
	for (int i = 0; i <= 100; ++i)
	{
		X::DrawLine(-50.0f + i, 0.0f, -50.0f, -50.0f + i, 0.0f, 50.0f, X::Math::Vector4::DarkGray());
		X::DrawLine(-50.0f, 0.0f, -50.0f + i, 50.0f, 0.0f, -50.0f + i, X::Math::Vector4::DarkGray());
	}
}

void RTS::DrawCursor()
{
	const float aX = (float)X::GetMouseScreenX();
	const float aY = (float)X::GetMouseScreenY();
	const float bX = aX + 20.0f;
	const float bY = aY + 10.0f;
	const float cX = aX + 10.0f;
	const float cY = aY + 20.0f;
	X::DrawScreenLine(aX, aY, bX, bY, X::Math::Vector4::Green());
	X::DrawScreenLine(aX, aY, cX, cY, X::Math::Vector4::Green());
	X::DrawScreenLine(bX, bY, cX, cY, X::Math::Vector4::Green());

	if (drag)
	{
		const float l = dragStart.x;
		const float t = dragStart.y;
		const float r = (float)X::GetMouseScreenX();
		const float b = (float)X::GetMouseScreenY();
		X::DrawScreenRect(l, t, r, b, X::Math::Vector4::Green());
	}
}

bool RTS::GetSelectionBox(float& minX, float& maxX, float& minZ, float& maxZ)
{
	if (!drag)
	{
		if (X::IsMousePressed(X::Mouse::LBUTTON))
		{
			drag = true;
			dragStart.x = (float)X::GetMouseScreenX();
			dragStart.y = (float)X::GetMouseScreenY();
		}
	}
	else
	{
		if (!X::IsMouseDown(X::Mouse::LBUTTON))
		{
			drag = false;

			X::Math::Ray startRay = X::GetScreenRay((int)dragStart.x, (int)dragStart.y);
			X::Math::Ray endRay = X::GetMouseRay();
			X::Math::Plane groundPlane;

			float startDist = 0.0f;
			float endDist = 0.0f;
			X::Math::Intersect(startRay, groundPlane, startDist);
			X::Math::Intersect(endRay, groundPlane, endDist);

			X::Math::Vector3 startPoint = X::Math::GetPoint(startRay, startDist);
			X::Math::Vector3 endPoint = X::Math::GetPoint(endRay, endDist);
			minX = X::Math::Min(startPoint.x, endPoint.x);
			maxX = X::Math::Max(startPoint.x, endPoint.x);
			minZ = X::Math::Min(startPoint.z, endPoint.z);
			maxZ = X::Math::Max(startPoint.z, endPoint.z);
			return true;
		}
	}
	return false;
}

X::Math::Vector3 RTS::GetGroundPosition(const X::Math::Ray& ray)
{
	X::Math::Plane groundPlane;

	// Check ground plane
	float distance = 0.0f;
	X::Math::Intersect(ray, groundPlane, distance);
	return X::Math::GetPoint(ray, distance);
}

X::Math::Vector3 RTS::GetSpiralPosition(const X::Math::Vector3& startPos, int n)
{
	X::Math::Vector3 pos = startPos;
	X::Math::Vector3 offset(0.0f, 0.0f, 1.0f);
	int nextDir = 1;
	int nextSign = 2;
	int nextStep = 1;
	for (int i = 1; i <= n; ++i)
	{
		pos += offset;

		if (i == nextSign)
		{
			offset *= -1.0f;
			nextStep += 1;
			nextSign += 2 * nextStep;
		}
		if (i == nextDir)
		{
			float temp = offset.x;
			offset.x = offset.z;
			offset.z = temp;
			nextDir += nextStep;
		}
	}

	return pos;
}