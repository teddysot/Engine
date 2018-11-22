#include <XEngine.h>

enum class State
{
	Init,
	Update
};

State state = State::Init;
int cursorTextureId = -1;
int scvTextureIds[16];

X::Math::Vector2 position;
X::Math::Vector2 destination;
float angle = 0.0f;

bool GameLoop(float deltaTime)
{
	switch (state)
	{
		case State::Init:
		{
			cursorTextureId = X::LoadTexture("sword.png");

			scvTextureIds[0] = X::LoadTexture("scv_01.png");
			scvTextureIds[1] = X::LoadTexture("scv_02.png");
			scvTextureIds[2] = X::LoadTexture("scv_03.png");
			scvTextureIds[3] = X::LoadTexture("scv_04.png");
			scvTextureIds[4] = X::LoadTexture("scv_05.png");
			scvTextureIds[5] = X::LoadTexture("scv_06.png");
			scvTextureIds[6] = X::LoadTexture("scv_07.png");
			scvTextureIds[7] = X::LoadTexture("scv_08.png");
			scvTextureIds[8] = X::LoadTexture("scv_09.png");
			scvTextureIds[9] = X::LoadTexture("scv_10.png");
			scvTextureIds[10] = X::LoadTexture("scv_11.png");
			scvTextureIds[11] = X::LoadTexture("scv_12.png");
			scvTextureIds[12] = X::LoadTexture("scv_13.png");
			scvTextureIds[13] = X::LoadTexture("scv_14.png");
			scvTextureIds[14] = X::LoadTexture("scv_15.png");
			scvTextureIds[15] = X::LoadTexture("scv_16.png");

			state = State::Update;
		}
		break;

		case State::Update:
		{
			float mouseX = (float)X::GetMouseScreenX();
			float mouseY = (float)X::GetMouseScreenY();
			X::DrawSprite(cursorTextureId, X::Math::Vector2(mouseX, mouseY));

			if (X::IsMousePressed(X::Mouse::LBUTTON))
			{
				destination.x = mouseX;
				destination.y = mouseY;
			}
 
			if (X::Math::Distance(position, destination) > 5.0f)
			{
				X::Math::Vector2 direction = X::Math::Normalize(destination - position);
				position += direction * 100.0f * deltaTime;
				angle = atan2(-direction.x, direction.y) + X::Math::kPi;
			}

			int frame = (int)(angle / X::Math::kTwoPi * 16.0f) % 16;
			X::DrawSprite(scvTextureIds[frame], position - X::Math::Vector2(32.0f, 32.0));

			X::DrawScreenLine(position, destination, X::Math::Vector4::Green());
			X::DrawScreenCircle(destination, 20.0f, X::Math::Vector4::Red());
		}
		break;
	}

	return X::IsKeyPressed(X::Keys::ESCAPE);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	X::SetTexturePath("../Assets/Images");

	X::Start("Hello Sprites", hInstance, 1280, 720);
	X::Run(GameLoop);
	X::Stop();
	return 0;
}