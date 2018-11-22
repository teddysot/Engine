//====================================================================================================
// Filename:	XEngine.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_H
#define INCLUDED_XENGINE_H

#include "XCore.h"
#include "XMath.h"
#include "XInputTypes.h"

namespace X {

void Start(const char* appName, HINSTANCE instance, int width, int height);
void Run(bool (*GameLoop)(float));
void Stop();

// Camera Functions
void SetCameraPosition(const Math::Vector3& position);
void SetCameraDirection(const Math::Vector3& direction);
void SetCameraTarget(const Math::Vector3& target);

// Resource Functions
void SetTexturePath(const char* path);
int LoadTexture(const char* filename);
void ClearAllTextures();

// Graphics Functions
void DrawLine(float x0, float y0, float z0, float x1, float y1, float z1, const Math::Vector4& color);
void DrawAABB(const Math::AABB& aabb, const Math::Vector4& color);
void DrawAABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, const Math::Vector4& color);
void DrawOBB(const Math::OBB& obb, const Math::Vector4& color);
void DrawSphere(const Math::Vector3& center, float radius, const Math::Vector4& color, uint32_t slices = 8, uint32_t rings = 4);
void DrawSphere(float x, float y, float z, float radius, const Math::Vector4& color, uint32_t slices = 8, uint32_t rings = 4);
void DrawScreenLine(const Math::Vector2& v0, const Math::Vector2& v1, const Math::Vector4& color);
void DrawScreenLine(float x0, float y0, float x1, float y1, const Math::Vector4& color);
void DrawScreenRect(const Math::Rect& rect, const Math::Vector4& color);
void DrawScreenRect(float l, float t, float r, float b, const Math::Vector4& color);
void DrawScreenCircle(const Math::Vector2& center, float r, const Math::Vector4& color);
void DrawScreenCircle(float x, float y, float r, const Math::Vector4& color);
void DrawScreenText(const char* str, float x, float y, float size, const Math::Vector4& color);
void DrawSprite(int textureId, const Math::Vector2& position);
uint32_t GetScreenWidth();
uint32_t GetScreenHeight();

// Random Functions
int Random();
int Random(int min, int max);
float RandomFloat();
float RandomFloat(float min, float max);
Math::Vector2 RandomVector2();
Math::Vector2 RandomVector2(const Math::Vector2& min, const Math::Vector2& max);
Math::Vector3 RandomVector3();
Math::Vector3 RandomVector3(const Math::Vector3& min, const Math::Vector3& max);
Math::Vector3 RandomUnitSphere();

// Keyboard Functions
bool IsKeyDown(int key);
bool IsKeyPressed(int key);

// Mouse Functions
bool IsMouseDown(int button);
bool IsMousePressed(int button);
int GetMouseScreenX();
int GetMouseScreenY();
bool IsMouseLeftEdge();
bool IsMouseRightEdge();
bool IsMouseTopEdge();
bool IsMouseBottomEdge();
int GetMouseMoveX();
int GetMouseMoveY();
Math::Ray GetMouseRay();
Math::Ray GetScreenRay(int screenX, int screenY);

// Special Effects Functions
void SpawnParticles(const Math::Vector3& pos, const Math::Vector3& vel, float degreeSpan, float length, float minLifetime, float maxLifetime, int count, const Math::Vector4& color);

} // namespace X

#endif // #ifndef INCLUDED_XENGINE_H