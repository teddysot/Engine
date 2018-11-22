//====================================================================================================
// Filename:	XEngine.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "XEngine.h"

#include "Camera.h"
#include "Font.h"
#include "GraphicsSystem.h"
#include "Gui.h"
#include "InputSystem.h"
#include "ParticleSystem.h"
#include "SimpleDraw.h"
#include "SpriteRenderer.h"
#include "TextureManager.h"
#include "Timer.h"

#pragma comment(lib, "FW1FontWrapper.lib")

using namespace X;

namespace
{
	struct SpriteCommand
	{
		int textureId;
		Math::Vector2 position;
	};

	struct TextCommand
	{
		std::wstring str;
		float size, x, y;
		uint32_t color;
	};

	std::string myAppName;

	HINSTANCE myInstance;
	HWND myWindow;

	Camera myCamera;
	Font myFont;

	std::vector<SpriteCommand> mySpriteCommands;
	std::vector<TextCommand> myTextCommands;

	inline uint32_t ToColor(const Math::Vector4& color)
	{
		uint8_t r = (uint8_t)(color.x * 255);
		uint8_t g = (uint8_t)(color.y * 255);
		uint8_t b = (uint8_t)(color.z * 255);
		return 0xff000000 | (b << 16) | (g << 8) | r;
	}
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Process any input message
	InputSystem::ProcessMessage(hWnd, msg, wParam, lParam);

	switch (msg)
	{
	case WM_SIZE:
		// TODO: Resize logic
		//if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		//{
		//	ImGui_ImplDX11_InvalidateDeviceObjects();
		//	CleanupRenderTarget();
		//	g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
		//	CreateRenderTarget();
		//	ImGui_ImplDX11_CreateDeviceObjects();
		//}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProcA(hWnd, msg, wParam, lParam);
}

namespace X {

void Start(const char* appName, HINSTANCE instance, int width, int height)
{
	srand(static_cast<unsigned>(time(0)));

	CoInitialize(nullptr);

	myAppName = appName;
	myInstance = instance;

	// Every Windows Window requires at least oen window object. Three things are involved:
	// 1)	Register a window class.
	// 2)	Create a window object.
	// 3)	Retrieve and dispatch messages for this window.

	// Register class
	WNDCLASSEXA wcex;
	wcex.cbSize = sizeof(WNDCLASSEXA);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = appName;
	wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	XVERIFY(RegisterClassExA(&wcex), "[Window] Failed to register window class.");

	// Compute the correct window dimension
	RECT rc = { 0, 0, (LONG)width, (LONG)height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	
	const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	const int winWidth = rc.right - rc.left;
	const int winHeight = rc.bottom - rc.top;
	const int left = (screenWidth - winWidth) >> 1;
	const int top = (screenHeight - winHeight) >> 1;

	// Create window
	myWindow = CreateWindowA
	(
		appName,
		appName,
		WS_OVERLAPPEDWINDOW,
		left,
		top,
		winWidth,
		winHeight,
		nullptr,
		nullptr,
		instance,
		nullptr
	);

	XASSERT(myWindow != nullptr, "[Window] Failed to create window.");

	ShowWindow(myWindow, true);

	XVERIFY(SetCursorPos(screenWidth >> 1, screenHeight >> 1), "[Window] Failed to set cursor position.");
	
	// Initialize all engine systems
	GraphicsSystem::StaticInitialize(myWindow, false);
	InputSystem::StaticInitialize(myWindow);
	ParticleSystem::StaticInitialize();
	SimpleDraw::Initialize(1024 * 1024);
	SpriteRenderer::StaticInitialize();
	TextureManager::StaticInitialize();
	Gui::Initialize(myWindow);

	// Initialize camera
	myCamera.SetFOV(60.0f * Math::kDegToRad);
	myCamera.SetNearPlane(0.01f);
	myCamera.SetFarPlane(10000.0f);

	// Initialize font
	myFont.Initialize();
}

//----------------------------------------------------------------------------------------------------

void Run(bool (*GameLoop)(float))
{
	MSG msg;
	memset(&msg, 0, sizeof(MSG));

	Timer timer;
	timer.Initialize();

	// Start the message loop
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Update input and timer
			InputSystem::Get()->Update();
			timer.Update();

			const float kDeltaTime = timer.GetElapsedTime();

			// Begin Gui
			Gui::BeginRender(kDeltaTime);

			// Run game loop
			if (GameLoop(kDeltaTime))
			{
				PostQuitMessage(0);
			}

			// Particle system
			ParticleSystem::Get()->Update(kDeltaTime);
			ParticleSystem::Get()->Render();

			// Begin scene
			GraphicsSystem::Get()->BeginRender(Math::Vector4(0.1f, 0.1f, 0.1f, 1.0f));

			// Render
			SimpleDraw::Render(myCamera);

			// Sprites
			SpriteRenderer::Get()->BeginRender();
			for (const auto& command : mySpriteCommands)
			{
				Texture* texture = TextureManager::Get()->GetTexture(command.textureId);
				if (texture)
				{
					SpriteRenderer::Get()->Draw(*texture, command.position);
				}
			}
			mySpriteCommands.clear();
			SpriteRenderer::Get()->EndRender();

			// Text
			for (const auto& command : myTextCommands)
			{
				myFont.Draw(command.str.c_str(), command.size, command.x, command.y, command.color);
			}
			myTextCommands.clear();
			
			// End Gui
			Gui::EndRender();

			// End scene
			GraphicsSystem::Get()->EndRender();
		}
	}
}

//----------------------------------------------------------------------------------------------------

void Stop()
{
	// Destroy font
	myFont.Terminate();

	// Shutdown all engine systems
	Gui::Terminate();
	TextureManager::StaticTerminate();
	SpriteRenderer::StaticTerminate();
	SimpleDraw::Terminate();
	ParticleSystem::StaticTerminate();
	InputSystem::StaticTerminate();
	GraphicsSystem::StaticTerminate();

	// Destroy the window
	DestroyWindow(myWindow);

	// Unregister window class
	UnregisterClassA(myAppName.c_str(), myInstance);

	CoUninitialize();
}

//----------------------------------------------------------------------------------------------------
void SetCameraPosition(const Math::Vector3& position)
{
	myCamera.SetPosition(position);
}

//----------------------------------------------------------------------------------------------------
void SetCameraDirection(const Math::Vector3& direction)
{
	myCamera.SetDirection(direction);
}

//----------------------------------------------------------------------------------------------------

void SetCameraTarget(const Math::Vector3& target)
{
	myCamera.SetLookAt(target);
}

//----------------------------------------------------------------------------------------------------

void SetTexturePath(const char* path)
{
	TextureManager::SetPath(path);
}

//----------------------------------------------------------------------------------------------------

int LoadTexture(const char* filename)
{
	return TextureManager::Get()->LoadFromFile(filename);
}

//----------------------------------------------------------------------------------------------------

void ClearAllTextures()
{
	return TextureManager::Get()->Clear();
}

//----------------------------------------------------------------------------------------------------

void DrawLine(float x0, float y0, float z0, float x1, float y1, float z1, const Math::Vector4& color)
{
	SimpleDraw::AddLine(x0, y0, z0, x1, y1, z1, color);
}

//----------------------------------------------------------------------------------------------------

void DrawAABB(const Math::AABB& aabb, const Math::Vector4& color)
{
	SimpleDraw::AddAABB(aabb, color);
}

//----------------------------------------------------------------------------------------------------

void DrawAABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, const Math::Vector4& color)
{
	SimpleDraw::AddAABB(minX, minY, minZ, maxX, maxY, maxZ, color);
}

//----------------------------------------------------------------------------------------------------

void DrawOBB(const Math::OBB& obb, const Math::Vector4& color)
{
	SimpleDraw::AddOBB(obb, color);
}

//----------------------------------------------------------------------------------------------------

void DrawSphere(const Math::Vector3& center, float radius, const Math::Vector4& color, uint32_t slices, uint32_t rings)
{
	SimpleDraw::AddSphere(center, radius, color, slices, rings);
}

//----------------------------------------------------------------------------------------------------

void DrawSphere(float x, float y, float z, float radius, const Math::Vector4& color, uint32_t slices, uint32_t rings)
{
	SimpleDraw::AddSphere(Math::Vector3(x, y, z), radius, color, slices, rings);
}

//----------------------------------------------------------------------------------------------------

void DrawScreenLine(const Math::Vector2& v0, const Math::Vector2& v1, const Math::Vector4& color)
{
	SimpleDraw::AddScreenLine(v0, v1, color);
}

//----------------------------------------------------------------------------------------------------

void DrawScreenLine(float x0, float y0, float x1, float y1, const Math::Vector4& color)
{
	SimpleDraw::AddScreenLine(x0, y0, x1, y1, color);
}

//----------------------------------------------------------------------------------------------------

void DrawScreenRect(const Math::Rect& rect, const Math::Vector4& color)
{
	SimpleDraw::AddScreenRect(rect.left, rect.top, rect.right, rect.bottom, color);
}

//----------------------------------------------------------------------------------------------------

void DrawScreenRect(float l, float t, float r, float b, const Math::Vector4& color)
{
	SimpleDraw::AddScreenRect(l, t, r, b, color);
}

//----------------------------------------------------------------------------------------------------

void DrawScreenCircle(const Math::Vector2& center, float r, const Math::Vector4& color)
{
	SimpleDraw::AddScreenCircle(center, r, color);
}

//----------------------------------------------------------------------------------------------------

void DrawScreenCircle(float x, float y, float r, const Math::Vector4& color)
{
	SimpleDraw::AddScreenCircle(x, y, r, color);
}

//----------------------------------------------------------------------------------------------------

void DrawScreenText(const char* str, float x, float y, float size, const Math::Vector4& color)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	TextCommand command;
	command.str = converter.from_bytes(str);
	command.size = size;
	command.x = x;
	command.y = y;
	command.color = ToColor(color);
	myTextCommands.emplace_back(command);
}

//----------------------------------------------------------------------------------------------------

void DrawSprite(int textureId, const Math::Vector2& position)
{
	SpriteCommand command;
	command.textureId = textureId;
	command.position = position;
	mySpriteCommands.emplace_back(command);
}

//----------------------------------------------------------------------------------------------------

uint32_t GetScreenWidth()
{
	return GraphicsSystem::Get()->GetWidth();
}

//----------------------------------------------------------------------------------------------------

uint32_t GetScreenHeight()
{
	return GraphicsSystem::Get()->GetHeight();
}

//----------------------------------------------------------------------------------------------------

int Random()
{
	return rand();
}

//----------------------------------------------------------------------------------------------------

int Random(int min, int max)
{
	return min + Random() % (max - min + 1);
}

//----------------------------------------------------------------------------------------------------

float RandomFloat()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

//----------------------------------------------------------------------------------------------------

float RandomFloat(float min, float max)
{
	return min + (max - min) * RandomFloat();
}

//----------------------------------------------------------------------------------------------------

Math::Vector2 RandomVector2()
{
	return Math::Vector2
	(
		RandomFloat(),
		RandomFloat()
	);
}

//----------------------------------------------------------------------------------------------------

Math::Vector2 RandomVector2(const Math::Vector2& min, const Math::Vector2& max)
{
	return Math::Vector2
	(
		RandomFloat(min.x, max.x),
		RandomFloat(min.y, max.y)
	);
}

//----------------------------------------------------------------------------------------------------

Math::Vector3 RandomVector3()
{
	return Math::Vector3
	(
		RandomFloat(),
		RandomFloat(),
		RandomFloat()
	);
}

//----------------------------------------------------------------------------------------------------

Math::Vector3 RandomVector3(const Math::Vector3& min, const Math::Vector3& max)
{
	return Math::Vector3
	(
		RandomFloat(min.x, max.x),
		RandomFloat(min.y, max.y),
		RandomFloat(min.z, max.z)
	);
}

//----------------------------------------------------------------------------------------------------

Math::Vector3 RandomUnitSphere()
{
	return Math::Normalize(Math::Vector3(
		RandomFloat(-1.0f, 1.0f) + 0.001f,
		RandomFloat(-1.0f, 1.0f),
		RandomFloat(-1.0f, 1.0f))
	);
}

//----------------------------------------------------------------------------------------------------

bool IsKeyDown(int key)
{
	return InputSystem::Get()->IsKeyDown(key);
}

//----------------------------------------------------------------------------------------------------

bool IsKeyPressed(int key)
{
	return InputSystem::Get()->IsKeyPressed(key);
}

//----------------------------------------------------------------------------------------------------

bool IsMouseDown(int button)
{
	return InputSystem::Get()->IsMouseDown(button);
}

//----------------------------------------------------------------------------------------------------

bool IsMousePressed(int button)
{
	return InputSystem::Get()->IsMousePressed(button);
}

//----------------------------------------------------------------------------------------------------

int GetMouseScreenX()
{
	return InputSystem::Get()->GetMouseScreenX();
}

//----------------------------------------------------------------------------------------------------

bool IsMouseLeftEdge()
{
	return InputSystem::Get()->IsMouseLeftEdge();
}

//----------------------------------------------------------------------------------------------------

bool IsMouseRightEdge()
{
	return InputSystem::Get()->IsMouseRightEdge();
}

//----------------------------------------------------------------------------------------------------

bool IsMouseTopEdge()
{
	return InputSystem::Get()->IsMouseTopEdge();
}

//----------------------------------------------------------------------------------------------------

bool IsMouseBottomEdge()
{
	return InputSystem::Get()->IsMouseBottomEdge();
}

//----------------------------------------------------------------------------------------------------

int GetMouseScreenY()
{
	return InputSystem::Get()->GetMouseScreenY();
}

//----------------------------------------------------------------------------------------------------

int GetMouseMoveX()
{
	return InputSystem::Get()->GetMouseMoveX();
}

//----------------------------------------------------------------------------------------------------

int GetMouseMoveY()
{
	return InputSystem::Get()->GetMouseMoveY();
}

//----------------------------------------------------------------------------------------------------

Math::Ray GetMouseRay()
{
	const int screenX = InputSystem::Get()->GetMouseScreenX();
	const int screenY = InputSystem::Get()->GetMouseScreenY();
	return GetScreenRay(screenX, screenY);
}

//----------------------------------------------------------------------------------------------------

Math::Ray GetScreenRay(int screenX, int screenY)
{
	const uint32_t screenWidth = GraphicsSystem::Get()->GetWidth();
	const uint32_t screenHeight = GraphicsSystem::Get()->GetHeight();
	return myCamera.ScreenPointToRay(screenX, screenY, screenWidth, screenHeight);
}

//----------------------------------------------------------------------------------------------------

void SpawnParticles(const Math::Vector3& pos, const Math::Vector3& vel, float degreeSpan, float length, float minLifetime, float maxLifetime, int count, const Math::Vector4& color)
{
	for (int i = 0; i < count; ++i)
	{
		Math::Quaternion q = Math::QuaternionRotationAxis(RandomUnitSphere(), degreeSpan * Math::kDegToRad);
		Math::Vector3 v = Math::TransformNormal(vel, Math::MatrixRotationQuaternion(q));
		ParticleSystem::Get()->AddParticle(pos, v, length, RandomFloat(minLifetime, maxLifetime), color);
	}
}

} // namespace X