#include <XEngine.h>

X::Math::Vector3 dudePosition(0.0f, 0.0f, 0.0f);
X::Math::Vector3 dudeDirection(0.0f, 0.0f, 1.0f);

void MoveDude(float deltaTime)
{
	const float kMoveSpeed = X::IsKeyDown(X::Keys::LSHIFT) ? 5.0f : 2.0f;
	const float kTurnSpeed = 0.05f;
	if (X::IsKeyDown(X::Keys::W))
	{
		dudePosition += dudeDirection * kMoveSpeed * deltaTime;
	}
	else if (X::IsKeyDown(X::Keys::S))
	{
		dudePosition -= dudeDirection * kMoveSpeed * deltaTime;
	}
	if (X::IsKeyDown(X::Keys::D))
	{
		X::Math::Vector3 right = X::Math::Cross(X::Math::Vector3::YAxis(), dudeDirection);
		dudePosition += right * kMoveSpeed * deltaTime;
	}
	else if (X::IsKeyDown(X::Keys::A))
	{
		X::Math::Vector3 left = X::Math::Cross(dudeDirection, X::Math::Vector3::YAxis());
		dudePosition += left * kMoveSpeed * deltaTime;
	}
	X::Math::Matrix matRot = X::Math::Matrix::RotationY(X::GetMouseMoveX() * kTurnSpeed * deltaTime);
	dudeDirection = X::Math::TransformNormal(dudeDirection, matRot);
}

void DrawDude(const X::Math::Vector3& position, const X::Math::Vector3& direction)
{
	//X::DrawSphere(position.x, position.y + 1.0f, position.z, 1.0f, X::Math::Vector4::Green());
	//X::DrawSphere(position.x, position.y + 2.5f, position.z, 0.75f, X::Math::Vector4::Green());
	X::Math::Vector3 center(position.x, position.y + 1.0f, position.z);
	X::Math::Vector3 extend(0.5f, 1.0f, 0.5f);
	const float angle = atan2(direction.x, direction.z);
	X::Math::Quaternion orientation = X::Math::QuaternionRotationAxis(X::Math::Vector3::YAxis(), angle);
	X::Math::OBB obb(center, extend, orientation);
	X::DrawOBB(obb, X::Math::Vector4::Green());
}

void DrawGround()
{
	for (int i = 0; i <= 100; ++i)
	{
		X::DrawLine(-50.0f + i, 0.0f, -50.0f, -50.0f + i, 0.0f, 50.0f, X::Math::Vector4::Gray());
		X::DrawLine(-50.0f, 0.0f, -50.0f + i, 50.0f, 0.0f, -50.0f + i, X::Math::Vector4::Gray());
	}
}

bool GameLoop(float deltaTime)
{
	MoveDude(deltaTime);
	DrawDude(dudePosition, dudeDirection);
	DrawGround();

	X::Math::Vector3 camPosition = dudePosition - (dudeDirection * 7.0f) + X::Math::Vector3(0.0f, 3.0f, 0.0);
	X::SetCameraPosition(camPosition);
	X::SetCameraDirection(dudeDirection);

	return X::IsKeyPressed(X::Keys::ESCAPE);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	X::Start("Basic 3D", hInstance, 1280, 720);
	X::Run(GameLoop);
	X::Stop();
	return 0;
}