//====================================================================================================
// Filename:	ParticleSystem.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "ParticleSystem.h"

#include "SimpleDraw.h"

using namespace X;

namespace
{
	ParticleSystem* sParticleSystem = nullptr;
}

void ParticleSystem::StaticInitialize()
{
	XASSERT(sParticleSystem == nullptr, "[ParticleSystem] System already initialized!");
	sParticleSystem = new ParticleSystem();
}

//----------------------------------------------------------------------------------------------------

void ParticleSystem::StaticTerminate()
{
	if (sParticleSystem != nullptr)
	{
		SafeDelete(sParticleSystem);
	}
}

//----------------------------------------------------------------------------------------------------

ParticleSystem* ParticleSystem::Get()
{
	XASSERT(sParticleSystem != nullptr, "[ParticleSystem] No system registered.");
	return sParticleSystem;
}

//----------------------------------------------------------------------------------------------------

ParticleSystem::ParticleSystem()
{
}

//----------------------------------------------------------------------------------------------------

ParticleSystem::~ParticleSystem()
{
}

//----------------------------------------------------------------------------------------------------

void ParticleSystem::AddParticle(const Math::Vector3& pos, const Math::Vector3& vel, float length, float lifetime, const Math::Vector4& color)
{
	mParticle.emplace_back(Particle{pos, vel, color, length, lifetime});
}

//----------------------------------------------------------------------------------------------------

void ParticleSystem::Update(float deltaTime)
{
	for (size_t i = 0; i < mParticle.size();)
	{
		auto& p = mParticle[i];
		p.pos += p.vel * deltaTime;
		p.lifetime -= deltaTime;

		if (p.lifetime <= 0.0f)
		{
			mParticle[i] = mParticle.back();
			mParticle.pop_back();
		}
		else
		{
			i++;
		}
	}
}

//----------------------------------------------------------------------------------------------------

void ParticleSystem::Render()
{
	for (const auto& p : mParticle)
	{
		SimpleDraw::AddLine(p.pos, p.pos - (p.vel * p.length), p.color);
	}
}