#pragma once

#include <GL\glew.h>
#include <boost\gil\gil_all.hpp>
#pragma warning( disable : 4996 )
#include <boost\gil\extension\io\png_dynamic_io.hpp>
#pragma warning( default : 4996 )

#include <map>

#define DEFAULT_TILEMAPSIZE 2048
#define DEFAULT_TILESIZE 32

/////////////////
// ImagePacker //
/////////////////

struct ImagePacker_rgb8 {
	std::vector<GLubyte> *pStorage;

	ImagePacker_rgb8( std::vector<GLubyte>* storage, boost::gil::rgb8_image_t &image );
	void operator()( boost::gil::rgb8_pixel_t p ) const;
};
struct ImagePacker_rgba8 {
	std::vector<GLubyte> *pStorage;

	ImagePacker_rgba8( std::vector<GLubyte>* storage, boost::gil::rgba8_image_t &image );
	void operator()( boost::gil::rgba8_pixel_t p ) const;
};

//////////////
// CTexture //
//////////////

struct TextureDescriptor
{
	TextureDescriptor();

	GLenum minFilter, magFilter, wrapS, wrapT;
	bool useAlpha;
	bool monochrome;
};

class CTexture
{
public:
	CTexture();
	virtual ~CTexture();

	virtual bool initialize() = 0;
	virtual void destroy() = 0;

	virtual void bind( GLuint textureUnit ) = 0;

	virtual GLuint getTextureId() const = 0;
};

////////////////
// CTexture2D //
////////////////

class CTexture2D : public CTexture
{
private:
	GLuint m_textureId;
public:
	CTexture2D();
	~CTexture2D();

	bool initialize();
	void destroy();

	bool loadTextureFromFile( boost::filesystem::path relativePath, TextureDescriptor &desc );
	bool loadTextureFromMemory( unsigned char *pBuffer, size_t length, int width, int height, TextureDescriptor &desc );

	void bind( GLuint textureUnit );

	GLuint getTextureId() const;
};

/////////////////////
// CTextureTilemap //
/////////////////////

class CTextureTilemap : public CTexture
{
private:
	struct TextureTile
	{
		std::wstring name;
		std::vector<GLubyte> tileData;
		int width, height;
	};
	struct BinNode
	{
		BinNode() : pLeftChild( 0 ), pRightChild( 0 ), pTileStored( 0 ), top( 0 ), left( 0 ) {}

		BinNode* pLeftChild;
		BinNode* pRightChild;
		int width, height;
		int top, left;

		TextureTile *pTileStored;

		BinNode* binPackInsert( TextureTile* pTile );
	};
	
	static bool CompareTileAreasPtr( TextureTile *pLHS, TextureTile *pRHS ) {
		return pLHS->width*pLHS->height > pRHS->width*pRHS->height;
	}

	GLuint m_textureId;

	std::vector<std::wstring> m_tileQueue;

	std::map<std::wstring, unsigned short> m_tileIndexTable;
	std::vector<glm::lowp_uvec4> m_tileCoordTable;

	int m_mapSize;
	int m_batchId;

	static std::vector<TextureTile*> GenerateTileData( std::vector<std::wstring>& tileQueue );
	static void RenderAndDeleteBin( BinNode **pNode, std::vector<GLubyte> &tileMapData, int size );
public:
	CTextureTilemap();
	~CTextureTilemap();

	bool initialize();
	void destroy();

	void clear();

	void bind( GLuint textureUnit );

	void addTile( std::wstring relativePath );
	bool binPackTilemap( int size );

	glm::lowp_uvec4 getTileCoords( unsigned short index );

	unsigned short getTileIndex( std::wstring path );

	GLuint getTextureId() const;

	int getBatchId();
	void setBatchId( int id );
};