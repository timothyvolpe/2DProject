#include "base.h"
#include "blocks\block.h"
#include "texture.h"
#include "world\world.h"
#include "world\spritemanager.h"

CBlock::CBlock( unsigned short blockId ) {
	m_blockId = blockId;
	m_bOpaque = false;
	m_spriteBatchId = -1;
	m_textureTileIndex = 0;
}
CBlock::~CBlock() {
}

bool CBlock::isOpaque() const {
	return m_bOpaque;
}
void CBlock::setOpaque( bool opaque ) {
	m_bOpaque = opaque;
}
void CBlock::setTexture( std::wstring texturePath, unsigned char batchCode )
{
	CWorld *pWorld =  CGame::getInstance().getWorld();
	CTextureTilemap *pTilemap = pWorld->getSpriteBatchTilemap( batchCode );
	unsigned short tileIndex;

	// Get texture and batch info
	assert( pTilemap );
	tileIndex = pTilemap->getTileIndex( texturePath );
	m_spriteBatchId = pTilemap->getBatchId();
	m_textureTileIndex = tileIndex;
	m_textureTileCoords = pTilemap->getTileCoords( tileIndex );
}

int CBlock::getBatchId() {
	return m_spriteBatchId;
}
unsigned short CBlock::getTileIndex() {
	return m_textureTileIndex;
}
glm::lowp_uvec4 CBlock::getTextureTileCoords() {
	return m_textureTileCoords;
}

unsigned short CBlock::getBlockId() const {
	return m_blockId;
}
