//====================================================================================================
// Filename:	Texture.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "Texture.h"

#include "GraphicsSystem.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

using namespace X;

Texture::Texture()
	: mShaderResourceView(nullptr)
{
}

//----------------------------------------------------------------------------------------------------

Texture::~Texture()
{
	XASSERT(mShaderResourceView == nullptr, "[Texture] Texture not released!");
}

//----------------------------------------------------------------------------------------------------

void Texture::Initialize(const char* filename)
{
	ID3D11Device* device = GraphicsSystem::Get()->GetDevice();

	wchar_t wbuffer[1024];
	mbstowcs_s(nullptr, wbuffer, filename, 1024);

	if (strstr(filename, ".dds") != nullptr)
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device, wbuffer, nullptr, &mShaderResourceView);
		if (FAILED(hr))
		{
			XLOG("[Texture] Failed to load texture %s. (error = %d)", filename, hr);
		}
	}
	else
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, wbuffer, nullptr, &mShaderResourceView);
		if (FAILED(hr))
		{
			XLOG("[Texture] Failed to load texture %s. (error = %d)", filename, hr);
		}
	}
}

//----------------------------------------------------------------------------------------------------

void Texture::Terminate()
{
	SafeRelease(mShaderResourceView);
}

//----------------------------------------------------------------------------------------------------

void Texture::BindVS(uint32_t index)
{
	GraphicsSystem::Get()->GetContext()->VSSetShaderResources(index, 1, &mShaderResourceView);
}

//----------------------------------------------------------------------------------------------------

void Texture::BindPS(uint32_t index)
{
	GraphicsSystem::Get()->GetContext()->PSSetShaderResources(index, 1, &mShaderResourceView);
}