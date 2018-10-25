#pragma once

#include <GL\glew.h>
#include <boost\gil\gil_all.hpp>
#pragma warning( disable : 4996 )
#include <boost\gil\extension\io\png_dynamic_io.hpp>
#pragma warning( default : 4996 )

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
	GLuint m_textureId;

	std::vector<std::wstring> m_tileQueue;

	int m_mapSize;
	int m_mapTilesize;
public:
	CTextureTilemap();
	~CTextureTilemap();

	bool initialize();
	void destroy();

	void bind( GLuint textureUnit );

	void addTile( std::wstring relativePath );
	bool generateTilemap( int size, int tilesize );

	glm::vec4 getTileBounds( int index );

	GLuint getTextureId() const;
};