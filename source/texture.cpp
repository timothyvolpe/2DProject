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

	StartGLDebug( "CreateTexture" );

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

	EndGLDebug();

	return true;
}
bool CTexture2D::loadTextureFromMemory( unsigned char *pBuffer, size_t length, int width, int height, TextureDescriptor &desc )
{
	StartGLDebug( "CreateTexture" );

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

	EndGLDebug();

	return true;
}

void CTexture2D::bind( GLuint textureUnit ) {
	if( textureUnit >= GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS ) {
		PrintError( L"Exceeded maximum number of texture units on the system!\n" );
	}
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
	m_batchId = -1;
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
	this->clear();
}

void CTextureTilemap::clear()
{
	m_tileQueue.clear();
	m_tileIndexTable.clear();
	m_tileCoordTable.clear();
	m_mapSize = 0;
	if( m_textureId ) {
		glDeleteTextures( 1, &m_textureId );
		m_textureId = 0;
	}
}

void CTextureTilemap::bind( GLuint textureUnit ) {
	if( textureUnit >= GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS ) {
		PrintError( L"Exceeded maximum number of texture units on the system!\n" );
	}
	glActiveTexture( GL_TEXTURE0+textureUnit );
	glBindTexture( GL_TEXTURE_2D, m_textureId );
}

void CTextureTilemap::addTile( std::wstring relativePath ) {
	m_tileQueue.push_back( relativePath );
}

std::vector<CTextureTilemap::TextureTile*> CTextureTilemap::GenerateTileData( std::vector<std::wstring>& tileQueue )
{
	std::vector<CTextureTilemap::TextureTile*> tileData;
	boost::filesystem::path tilePath;
	boost::gil::rgba8_image_t tileImage;

	for( auto it = tileQueue.begin(); it != tileQueue.end(); it++ )
	{
		tilePath = GameFilesystem::ConstructPath( (*it), FILESYSTEM_DIR_TEXTURES );
		// Make sure it exists
		if( !GameFilesystem::IsValidFile( tilePath ) ) {
			PrintWarn( L"Could not find tile \"%s\"\n", (*it).c_str() );
			continue;
		}
		// Load it
		try {
			boost::gil::png_read_and_convert_image( tilePath.string(), tileImage );
		}
		catch( std::ios_base::failure &e ) {
			PrintWarn( L"Could not load tile \"%s\" (%hs)\n", (*it).c_str(), e.what() );
			continue;
		}
		// Pack into tile array
		CTextureTilemap::TextureTile* pTextureTile = new CTextureTilemap::TextureTile();
		pTextureTile->name = (*it);
		pTextureTile->width = tileImage.width();
		pTextureTile->height = tileImage.height();
		boost::gil::for_each_pixel( const_view( tileImage ), ImagePacker_rgba8( &pTextureTile->tileData, tileImage ) );
		tileData.push_back( pTextureTile );
	}

	tileQueue.clear();
	return tileData;
}
CTextureTilemap::BinNode* CTextureTilemap::BinNode::binPackInsert( CTextureTilemap::TextureTile* pTile )
{
	// Check if it isn't a leaf
	if( this->pLeftChild || this->pRightChild ) {
		// Pick left node first
		if( this->pLeftChild ) {
			BinNode *pNewNode = this->pLeftChild->binPackInsert( pTile );
			// If we found a spot
			if( pNewNode )
				return pNewNode;
		}
		if( this->pRightChild ) {
			BinNode *pNewNode = this->pRightChild->binPackInsert( pTile );
			// If we found a spot
			if( pNewNode )
				return pNewNode;
		}
		// No space for it
		return NULL;
	}
	// Handle leaf nodes
	else
	{
		int deltaW, deltaH;

		// Check if this bin is full
		if( pTileStored )
			return NULL;
		// Check if it wont fit
		if( pTile->width > this->width || pTile->height > this->height )
			return NULL;
		// Check if it completely fills the bin
		if( pTile->width == this->width && pTile->height == this->height )
			return this;

		// It does fit and we can fit new nodes as well
		this->pLeftChild = new BinNode();
		this->pRightChild = new BinNode();
		// Calculate new size
		deltaW = this->width - pTile->width;
		deltaH = this->height -  pTile->height;
		if( deltaW > deltaH )
		{
			this->pLeftChild->width = pTile->width;
			this->pLeftChild->height = this->height;
			this->pLeftChild->top = this->top;
			this->pLeftChild->left = this->left;
			this->pRightChild->width = deltaW;
			this->pRightChild->height = this->height;
			this->pRightChild->top = this->top;
			this->pRightChild->left = this->left + pTile->width;
		}
		else
		{
			this->pLeftChild->width = this->width;
			this->pLeftChild->height = pTile->height;
			this->pLeftChild->top = this->top;
			this->pLeftChild->left = this->left;
			this->pRightChild->width = this->width;
			this->pRightChild->height = deltaH;
			this->pRightChild->top = this->top + pTile->height;
			this->pRightChild->left = this->left;
		}

		// Add to left node
		return this->pLeftChild->binPackInsert( pTile );
	}

	return NULL;
}
void CTextureTilemap::RenderAndDeleteBin( CTextureTilemap::BinNode **pNode, std::vector<GLubyte> &tileMapData, int size )
{
	// Perform postorder traversal
	BinNode *pNodeObj = (*pNode);

	// End of the line
	if( pNodeObj == NULL )
		return;
	// Perform on left
	CTextureTilemap::RenderAndDeleteBin( &pNodeObj->pLeftChild, tileMapData, size );
	// Perform on right
	CTextureTilemap::RenderAndDeleteBin( &pNodeObj->pRightChild, tileMapData, size );

	// Render this node
	if( pNodeObj->pTileStored ) {
		for( int rows = 0; rows < pNodeObj->pTileStored->height; rows++ ) {
			for( int width = 0; width < pNodeObj->pTileStored->width; width++ ) {
				// Copy pixel (4 bytes)
				memcpy( &tileMapData[(pNodeObj->left+width)*4 + (pNodeObj->top+rows)*(size*4)], &pNodeObj->pTileStored->tileData[(width*4)+rows*(pNodeObj->pTileStored->width*4)], 4 );
			}
		}
	}

	delete pNodeObj;
	*pNode = 0;
}
bool CTextureTilemap::binPackTilemap( int size )
{
	std::vector<CTextureTilemap::TextureTile*> tileData;
	BinNode *pRootNode, *pPickedBin;
	std::vector<GLubyte> tileMapData;
	glm::lowp_uvec4 normalizedCoords;

	assert( size );
	m_mapSize = size;

	// Generate tile data
	tileData = CTextureTilemap::GenerateTileData( m_tileQueue );

	// Sort nodes by area
	std::sort( tileData.begin(), tileData.end(), CTextureTilemap::CompareTileAreasPtr );
	// Create root node
	pRootNode = new BinNode();
	pRootNode->width = m_mapSize;
	pRootNode->height = m_mapSize;
	// Pack the nodes
	for( auto it = tileData.begin(); it != tileData.end(); it++ )
	{
		pPickedBin = pRootNode->binPackInsert( (*it) );
		if( !pPickedBin ) {
			PrintError( L"Tilemap ran out of space!\n" );
			continue;
		}
		// Put tile in bin
		pPickedBin->pTileStored = (*it);
		// Add coords
		normalizedCoords = glm::vec4( (float)pPickedBin->left / m_mapSize, (float)pPickedBin->top / m_mapSize, (float)(pPickedBin->left+pPickedBin->width) / m_mapSize, (float)(pPickedBin->top+pPickedBin->height) / m_mapSize );
		// Map to all possible vlaues of GLushort
		m_tileCoordTable.push_back( normalizedCoords * glm::lowp_uvec4( (~((GLushort)0)) ) );
		// Add to lookup table
		m_tileIndexTable.insert( std::pair<std::wstring, unsigned short>( (*it)->name, (unsigned short)m_tileIndexTable.size() ) );
	}

	// Traverse tree and construct tilemap
	tileMapData.resize( m_mapSize*m_mapSize*4 );
	RenderAndDeleteBin( &pRootNode, tileMapData, m_mapSize );

	// Clean up tiles
	for( auto it = tileData.begin(); it != tileData.end(); it++ ) {
		if( (*it) ) {
			delete (*it);
			(*it) = 0;
		}
	}
	tileData.clear();

	StartGLDebug( "CreateTexture" );

	// Create OpenGL texture
	// Bind texture
	glBindTexture( GL_TEXTURE_2D, m_textureId );
	// Parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
	// Store the texture info
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_mapSize, m_mapSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, &tileMapData[0] );

	EndGLDebug();

	return true;
}

glm::lowp_uvec4 CTextureTilemap::getTileCoords( unsigned short index ) {
	return this->m_tileCoordTable[index];
}
unsigned short CTextureTilemap::getTileIndex( std::wstring path ) {
	auto it = m_tileIndexTable.find( path );
	if( it != m_tileIndexTable.end() )
		return (*it).second;
	PrintWarn( L"Could not find texture \'%s\' in map", path.c_str() );
	return 0;
}

GLuint CTextureTilemap::getTextureId() const {
	return m_textureId;
}

int CTextureTilemap::getBatchId() {
	return m_batchId;
}
void CTextureTilemap::setBatchId( int id ) {
	m_batchId = id;
}