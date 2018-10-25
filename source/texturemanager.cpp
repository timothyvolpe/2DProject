#include <algorithm>
#include "base.h"
#include "texturemanager.h"
#include "texture.h"

CTextureManager::CTextureManager() {
}
CTextureManager::~CTextureManager() {
}

bool CTextureManager::initialize()
{
	return true;
}
void CTextureManager::destroy()
{
	// Delete all the textures
	for( TextureMap2D::iterator it = m_texture2dCache.begin(); it != m_texture2dCache.end(); it++ ) {
		DestroyDelete( (*it).second );
	}
	m_texture2dCache.clear();
}

CTexture2D* CTextureManager::loadTexture2D( boost::filesystem::path relativePath, TextureDescriptor &desc )
{
	CTexture2D *pTexture;
	std::wstring texKey;

	// Check if its in the map
	texKey = relativePath.wstring();
	std::transform( texKey.begin(), texKey.end(), texKey.begin(), ::tolower );
	if( m_texture2dCache.find( texKey ) != m_texture2dCache.end() )
		return m_texture2dCache[texKey];
	// If its not in the map, load it
	pTexture = new CTexture2D();
	if( !pTexture->initialize() )
		return false;
	if( !pTexture->loadTextureFromFile( relativePath.wstring(), desc ) )
		return false;
	m_texture2dCache.insert( std::pair<std::wstring, CTexture2D*>( texKey, pTexture ) );

	return pTexture;
}