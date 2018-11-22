//====================================================================================================
// Filename:	ParticleSystem.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_PARTICLESYSTEM_H
#define INCLUDED_XENGINE_PARTICLESYSTEM_H

#include "XCore.h"
#include "XMath.h"

namespace X {

class ParticleSystem
{
public:
	static void StaticInitialize();
	static void StaticTerminate();
	static ParticleSystem* Get();

public:
	ParticleSystem();
	~ParticleSystem();

	void AddParticle(const Math::Vector3& pos, const Math::Vector3& vel, float length, float lifetime, const Math::Vector4& color);

	void Update(float deltaTime);
	void Render();

private:
	struct Particle
	{
		Math::Vector3 pos;
		Math::Vector3 vel;
		Math::Vector4 color;
		float length;
		float lifetime;
	};

	ParticleSystem(const ParticleSystem&) = delete;
	ParticleSystem& operator=(const ParticleSystem&) = delete;

	std::vector<Particle> mParticle;
};

} // namespace X

#endif // #ifndef INCLUDED_XENGINE_PARTICLESYSTEM_H