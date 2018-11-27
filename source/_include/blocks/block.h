#pragma once

class CBlock
{
private:
	unsigned short m_blockId;

	bool m_bOpaque;
	
	int m_spriteBatchId;
	unsigned short m_textureTileIndex;
	glm::lowp_uvec4 m_textureTileCoords;
public:
	CBlock( unsigned short blockId );
	virtual ~CBlock();

	void setTexture( std::wstring texturePath, unsigned char batchCode );

	bool isOpaque() const;
	void setOpaque( bool opaque );

	int getBatchId();
	unsigned short getTileIndex();
	glm::lowp_uvec4 getTextureTileCoords();

	unsigned short getBlockId() const;
};