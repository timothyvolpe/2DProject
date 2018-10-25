#include "base.h"
#include "texture.h"

/////////////////
// ImagePacker //
/////////////////

// RGB8
ImagePacker_rgb8::ImagePacker_rgb8( std::vector<GLubyte>* storage, boost::gil::rgb8_image_t &image ) {
	pStorage = storage;
	pStorage->reserve( image.width() * image.height() * 3 );
}
void ImagePacker_rgb8::operator()( boost::gil::rgb8_pixel_t p ) const {
	pStorage->push_back( boost::gil::at_c<0>( p ) );
	pStorage->push_back( boost::gil::at_c<1>( p ) );
	pStorage->push_back( boost::gil::at_c<2>( p ) );
}
// RGBA8
ImagePacker_rgba8::ImagePacker_rgba8( std::vector<GLubyte>* storage, boost::gil::rgba8_image_t &image ) {
	pStorage = storage;
	pStorage->reserve( image.width() * image.height() * 4 );
}
void ImagePacker_rgba8::operator()( boost::gil::rgba8_pixel_t p ) const {
	pStorage->push_back( boost::gil::at_c<0>( p ) );
	pStorage->push_back( boost::gil::at_c<1>( p ) );
	pStorage->push_back( boost::gil::at_c<2>( p ) );
	pStorage->push_back( boost::gil::at_c<3>( p ) );
}

TextureDescriptor::TextureDescriptor() {
	minFilter = 0;
	magFilter = 0;
	wrapS = 0;
	wrapT = 0;
	useAlpha = false;
	monochrome = false;
}

//////////////
// CTexture //
//////////////

CTexture::CTexture() {
}
CTexture::~CTexture() {
}

////////////////
// CTexture2D //
////////////////

CTexture2D::CTexture2D() {
	m_textureId = 0;
}
CTexture2D::~CTexture2D() {
}

bool CTexture2D::initialize()
{
	// Create the texture
	glGenTextures( 1, &m_textureId );
	if( !m_textureId ) {
		PrintError( L"Could not create texture (code: %d)\n", glGetError() );
		return false;
	}

	return true;
}
void CTexture2D::destroy()
{
	// Destroy the texture
	if( m_textureId ) {
		glDeleteTextures( 1, &m_textureId ); 
		m_textureId = 0;
	}
}

bool CTexture2D::loadTextureFromFile( boost::filesystem::path relativePath, TextureDescriptor &desc )
{
	boost::filesystem::path fullPath;
	boost::gil::rgb8_image_t pngImage;
	boost::gil::rgba8_image_t pngAlpha;
	std::vector<GLubyte> pngData;

	// Construct the full path
	fullPath = GameFilesystem::ConstructPath( relativePath, FILESYSTEM_DIR_TEXTURES );
	// Make sure it exists
	if( !GameFilesystem::IsValidFile( fullPath ) ) {
		PrintError( L"Could not load texture \"%s\" because it could not be found\n", relativePath.wstring().c_str() );
		return false;
	}

	// Load the texture
	try {
		if( desc.useAlpha )
			boost::gil::png_read_and_convert_image( fullPath.string(), pngAlpha );
		else
			boost::gil::png_read_and_convert_image( fullPath.string(), pngImage );
	}
	catch( std::ios_base::failure &e ) {
		PrintError( L"Failed to load texture file \"%s\" (%hs)\n", relativePath.wstring().c_str(), e.what() );
		return false;
	}

	// Pack into a vector
	if( desc.useAlpha)
		boost::gil::for_each_pixel( const_view( pngAlpha ), ImagePacker_rgba8( &pngData, pngAlpha ) );
	else
		boost::gil::for_each_pixel( const_view( pngImage ), ImagePacker_rgb8( &pngData, pngImage ) );

	// Bind texture
	glBindTexture( GL_TEXTURE_2D, m_textureId );
	// Parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, desc.minFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, desc.magFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, desc.wrapS );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, desc.wrapT );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
	// Store the texture info
	if( desc.useAlpha )
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, pngAlpha.width(), pngAlpha.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, &pngData[0] );
	else if( desc.monochrome )
		glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, pngAlpha.width(), pngAlpha.height(), 0, GL_RED, GL_UNSIGNED_BYTE, &pngData[0] );
	else
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, pngImage.width(), pngImage.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, &pngData[0] );

	return true;
}
bool CTexture2D::loadTextureFromMemory( unsigned char *pBuffer, size_t length, int width, int height, TextureDescriptor &desc )
{
	// Bind texture
	glBindTexture( GL_TEXTURE_2D, m_textureId );
	// Parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, desc.minFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, desc.magFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, desc.wrapS );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, desc.wrapT );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
	// Store the texture info
	if( desc.useAlpha )
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pBuffer[0] );
	else if( desc.monochrome )
		glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, &pBuffer[0] );
	else
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &pBuffer[0] );

	return true;
}

void CTexture2D::bind( GLuint textureUnit ) {
	glActiveTexture( GL_TEXTURE0+textureUnit );
	glBindTexture( GL_TEXTURE_2D, m_textureId );
}

GLuint CTexture2D::getTextureId() const {
	return m_textureId;
}

/////////////////////
// CTextureTilemap //
/////////////////////

CTextureTilemap::CTextureTilemap() {
	m_mapSize = 0;
	m_mapTilesize = 0;
}
CTextureTilemap::~CTextureTilemap() {
}

bool CTextureTilemap::initialize()
{
	// Create the texture
	glGenTextures( 1, &m_textureId );
	if( !m_textureId ) {
		PrintError( L"Could not create texture (code: %d)\n", glGetError() );
		return false;
	}

	return true;
}
void CTextureTilemap::destroy()
{
	// Destroy the texture
	if( m_textureId ) {
		glDeleteTextures( 1, &m_textureId );
		m_textureId = 0;
	}
}

void CTextureTilemap::bind( GLuint textureUnit ) {
	glActiveTexture( GL_TEXTURE0+textureUnit );
	glBindTexture( GL_TEXTURE_2D, m_textureId );
}

void CTextureTilemap::addTile( std::wstring relativePath ) {
	m_tileQueue.push_back( relativePath );
}

bool CTextureTilemap::generateTilemap( int size, int tilesize )
{
	int tileCount, currentTile;
	std::vector<std::vector<GLubyte>> tilesData;
	boost::filesystem::path tilePath;
	boost::gil::rgba8_image_t tileImage;
	std::vector<GLubyte> fullTilemap;

	// Make sure they fit evenly
	if( size % tilesize != 0 ) {
		PrintError( L"Tilemap size must be evenly divisible by tile size\n" );
		return false;
	}
	// Make sure all the tiles will fit
	tileCount = m_tileQueue.size();
	if( tileCount > (size*size / (tilesize*tilesize)) ) {
		PrintWarn( L"Too many tiles in tilemap, some will be ignored\n" );
		tileCount = size*size / (tilesize*tilesize);
	}

	// Load each tile
	tilesData.resize( tileCount );
	currentTile = 0;
	for( auto it = m_tileQueue.begin(); it != m_tileQueue.end(); it++ )
	{
		tilePath = GameFilesystem::ConstructPath( (*it), FILESYSTEM_DIR_TEXTURES );
		// Make sure it exists
		if( !GameFilesystem::IsValidFile( tilePath ) ) {
			PrintWarn( L"Could not find tile \"%s\"\n", (*it).c_str() );
			currentTile++;
			continue;
		}
		// Load it
		try {
			boost::gil::png_read_and_convert_image( tilePath.string(), tileImage );
		}
		catch( std::ios_base::failure &e ) {
			PrintWarn( L"Could not load tile \"%s\" (%hs)\n", (*it).c_str(), e.what() );
			currentTile++;
			continue;
		}
		// make sure its the correct size
		if( tileImage.width() != tilesize && tileImage.height() != tilesize ) {
			PrintWarn( L"Discarding improperly sized tile \"%s\"\n", (*it).c_str() );
			currentTile++;
			continue;
		}
		// Pack into vector
		boost::gil::for_each_pixel( const_view( tileImage ), ImagePacker_rgba8( &tilesData[currentTile], tileImage ) );
		currentTile++;
	}
	// Construct full image
	int tilesPerRow = size / tilesize;
	currentTile = 0;
	fullTilemap.resize( size*size*4 );
	for( int y = 0; y < tilesPerRow; y++ )
	{
		for( int x = 0; x < tilesPerRow; x++ )
		{
			// for each pixel
			for( int u = 0; u < tilesize; u++ )
			{
				for( int v = 0; v < tilesize; v++ ){
					fullTilemap[((x*tilesize)+(y*tilesize*tilesize*tilesPerRow)+u+(v*tilesize*tilesPerRow))*4] = tilesData[currentTile][(u+v*tilesize)*4];
					fullTilemap[((x*tilesize)+(y*tilesize*tilesize*tilesPerRow)+u+(v*tilesize*tilesPerRow))*4+1] = tilesData[currentTile][(u+v*tilesize)*4+1];
					fullTilemap[((x*tilesize)+(y*tilesize*tilesize*tilesPerRow)+u+(v*tilesize*tilesPerRow))*4+2] = tilesData[currentTile][(u+v*tilesize)*4+2];
					fullTilemap[((x*tilesize)+(y*tilesize*tilesize*tilesPerRow)+u+(v*tilesize*tilesPerRow))*4+3] = tilesData[currentTile][(u+v*tilesize)*4+3];
				}
			}
			currentTile++;
			if( currentTile >= tileCount )
				break;
		}
		if( currentTile >= tileCount )
			break;
	}

	// Create the tilemap
	// Bind texture
	glBindTexture( GL_TEXTURE_2D, m_textureId );
	// Parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
	// Store the texture info
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, &fullTilemap[0] );

	m_mapSize = size;
	m_mapTilesize = tilesize;

	return true;
}

glm::vec4 CTextureTilemap::getTileBounds( int index )
{
	int y, x;
	y = (int)floor( index / (m_mapSize / m_mapTilesize) );
	x = index - y*(m_mapSize / m_mapTilesize);
	return glm::vec4( (float)(x*m_mapTilesize) / (float)m_mapSize, (float)(y*m_mapTilesize) / (float)m_mapSize, (float)((x+1)*m_mapTilesize) / (float)m_mapSize, (float)((y+1)*m_mapTilesize) / (float)m_mapSize );
}

GLuint CTextureTilemap::getTextureId() const {
	return m_textureId;
}