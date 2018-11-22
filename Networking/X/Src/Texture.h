//====================================================================================================
// Filename:	Texture.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_TEXTURE_H
#define INCLUDED_XENGINE_TEXTURE_H

namespace X {

class Texture
{
public:
	Texture();
	~Texture();
	
	void Initialize(const char* filename);
	void Terminate();
	
	void BindVS(uint32_t index);
	void BindPS(uint32_t index);

private:
	friend class SpriteRenderer;

	ID3D11ShaderResourceView* mShaderResourceView;
};

} // namespace X

#endif // #ifndef INCLUDED_XENGINE_TEXTURE_H