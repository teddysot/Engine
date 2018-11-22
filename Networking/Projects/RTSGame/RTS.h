//====================================================================================================
// Filename:	RTS.h
// Created by:	Peter Chan
//====================================================================================================

#include <XEngine.h>

namespace RTS {

	void UpdateCamera(float deltaTime);

	void DrawGround();
	void DrawCursor();

	bool GetSelectionBox(float& minX, float& maxX, float& minZ, float& maxZ);

	X::Math::Vector3 GetGroundPosition(const X::Math::Ray& ray);
	X::Math::Vector3 GetSpiralPosition(const X::Math::Vector3& startPos, int n);

} // namespace RTS