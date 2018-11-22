//====================================================================================================
// Filename:	TextureManager.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_TEXTUREMANAGER_H
#define INCLUDED_XENGINE_TEXTUREMANAGER_H

#include "XCore.h"
#include "Texture.h"

namespace X {


//====================================================================================================
// Class Declarations
//====================================================================================================

class TextureManager
{
public:
	static void StaticInitialize();
	static void StaticTerminate();
	static TextureManager* Get();

	static void SetPath(const char* path);

public:
	TextureManager();
	~TextureManager();

	int LoadFromFile(const char* filename);
	void Clear();

	Texture* GetTexture(int textureId);

private:
	struct Entry
	{
		uint32_t hash;
		Texture* texture;
	};

	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	std::vector<Entry> mInventory;
};

} // namespace Graphics

#endif // #ifndef INCLUDED_XENGINE_TEXTUREMANAGER_H