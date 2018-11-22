//====================================================================================================
// Filename:	TextureManager.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "TextureManager.h"

#include "Texture.h"

using namespace X;

namespace
{
	uint32_t ComputeHash(const char* str)
	{
		// Based on djb2 implementation:
		// http://www.cse.yorku.ca/~oz/hash.html

		uint32_t hash = 5381;
		int c;

		while (c = *str++)
		{
			hash = ((hash << 5) + hash) + c;
		}
		return hash;
	}

	TextureManager* sTextureManager = nullptr;
	std::string sPath;
}

void TextureManager::StaticInitialize()
{
	XASSERT(sTextureManager == nullptr, "[TextureManager] System already initialized!");
	sTextureManager = new TextureManager();
}

//----------------------------------------------------------------------------------------------------

void TextureManager::StaticTerminate()
{
	if (sTextureManager != nullptr)
	{
		sTextureManager->Clear();
		SafeDelete(sTextureManager);
	}
}

//----------------------------------------------------------------------------------------------------

TextureManager* TextureManager::Get()
{
	XASSERT(sTextureManager != nullptr, "[TextureManager] No system registered.");
	return sTextureManager;
}

//----------------------------------------------------------------------------------------------------

void TextureManager::SetPath(const char* path)
{
	sPath = path;
}

//----------------------------------------------------------------------------------------------------

TextureManager::TextureManager()
{}

//----------------------------------------------------------------------------------------------------

TextureManager::~TextureManager()
{}

//----------------------------------------------------------------------------------------------------

int TextureManager::LoadFromFile(const char* filename)
{
	uint32_t hash = ComputeHash(filename);
	for (int i = 0; i < (int)mInventory.size(); ++i)
	{
		if (hash == mInventory[i].hash)
		{
			return i;
		}
	}

	std::string fullname = sPath + "/" + std::string(filename);
	mInventory.push_back(Entry());
	mInventory.back().hash = hash;
	mInventory.back().texture = new Texture();
	mInventory.back().texture->Initialize(fullname.c_str());
	return mInventory.size() - 1;
}

//----------------------------------------------------------------------------------------------------

void TextureManager::Clear()
{
	for (auto& item : mInventory)
	{
		item.texture->Terminate();
		SafeDelete(item.texture);
	}
	mInventory.clear();
}

//----------------------------------------------------------------------------------------------------

Texture* TextureManager::GetTexture(int textureId)
{
	if (textureId < (int)mInventory.size())
	{
		return mInventory[textureId].texture;
	}

	return nullptr;
}