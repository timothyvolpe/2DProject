#pragma once

#include <map>
#include <boost\filesystem.hpp>

class CTexture;
class CTexture2D;
struct TextureDescriptor;

class CTextureManager
{
private:
	typedef std::map<std::wstring, CTexture2D*> TextureMap2D;

	TextureMap2D m_texture2dCache;
public:
	CTextureManager();
	~CTextureManager();

	bool initialize();
	void destroy();

	CTexture2D* loadTexture2D( boost::filesystem::path relativePath, TextureDescriptor &desc );
};