#include "base.h"
#include "blocks\block.h"

CBlock::CBlock( unsigned short blockId ) {
	m_blockId = blockId;
	m_bOpaque = false;
	m_textureIndex = 0;
}
CBlock::~CBlock() {
}

bool CBlock::isOpaque() const {
	return m_bOpaque;
}
void CBlock::setOpaque( bool opaque ) {
	m_bOpaque = opaque;
}
unsigned short CBlock::getTextureIndex() const {
	return m_textureIndex;
}
void CBlock::setTextureIndex( unsigned short index ) {
	m_textureIndex = index;
}

unsigned short CBlock::getBlockId() const {
	return m_blockId;
}
