#pragma once

class CBlock
{
private:
	unsigned short m_blockId;

	bool m_bOpaque;
	unsigned short m_textureIndex;
public:
	CBlock( unsigned short blockId );
	virtual ~CBlock();

	bool isOpaque() const;
	void setOpaque( bool opaque );
	unsigned short getTextureIndex() const;
	void setTextureIndex( unsigned short index );
	unsigned short getBlockId() const;
};