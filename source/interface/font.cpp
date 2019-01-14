#include "base.h"
#include "interface\font.h"
#include "interface\distance-field.h"
#include "game.h"
#include "texture.h"
#include <ftbitmap.h>

bool CachedGlyph::operator<( const CachedGlyph &rhs ) const { 
	return width*rows > rhs.width*rhs.rows; 
}

CFont::CFont() {
	m_cacheFace = 0;
	m_cachedArea = 0;
	m_fontFileName = L"";

	m_pFontMap = 0;
}
CFont::~CFont() { 
}

bool CFont::initialize( bool systemFont, std::wstring const& fontFile )
{
	this->ClearCache();

	FT_Library ftLib = CGame::getInstance().getFreeType();
	FT_Error ftError;
	boost::filesystem::wpath fontPath;

	m_fontFileName = fontFile;

	// Construct the full font path
	if( systemFont )
	{
#if defined( _WIN32 )
		wchar_t winDir[MAX_PATH];
		GetWindowsDirectory( winDir, MAX_PATH );
		fontPath = winDir;
		fontPath /= "fonts";
		fontPath /= fontFile;
#endif
	}
	else
		fontPath = GameFilesystem::ConstructPath( fontFile, FILESYSTEM_DIR_FONTS );

	// Load the font from freetype
	ftError = FT_New_Face( ftLib, fontPath.string().c_str(), 0, &m_cacheFace );
	if( ftError ) {
		PrintError( L"FreeType failed to load font \'%s\' (code: %d, '%hs')\n", m_fontFileName.c_str(), ftError, GetFreetypeError( ftError ) );
		return false;
	}
	// Select the unicode charmap
	ftError = FT_Select_Charmap( m_cacheFace, FT_ENCODING_UNICODE );
	if( ftError ) {
		PrintError( L"FreeType couldn't find unicode charmap in font \'%s\' (code: %d, '%hs')\n", m_fontFileName.c_str(), ftError, GetFreetypeError( ftError ) );
		return false;
	}
	// Check to see if the font is compatible
	if( (m_cacheFace->face_flags & FT_FACE_FLAG_HORIZONTAL) == 0 || m_cacheFace->face_flags & FT_FACE_FLAG_TRICKY ) {
		PrintError( L" Font \'%s\' is not supported\n", m_fontFileName.c_str() );
		return false;
	}
	// Set the font size to 24 for the distance field
	ftError = FT_Set_Char_Size( m_cacheFace, 0, 24*FT_HRES, 0, FT_DPI );
	if( ftError ) {
		PrintError( L"Font \'%s\' does not contain the correct size for distance field! (code: %d, '%hs')\n", m_fontFileName.c_str(), ftError, GetFreetypeError( ftError ) );
		return true;
	}

	// Load the default characters
	if( !this->CacheCharacters( DEFAULT_CHARACTERS, DEFAULT_CHARACTERS_COUNT ) )
		return false;
	// Generate the map
	if( !this->GenerateMapFromCache() ) {
		PrintError( L"Failed to generate font map for text \'%s\'\n", m_fontFileName.c_str() );
		return false;
	}

	return true;
}
void CFont::destroy()
{
	if( m_cacheFace ) {
		FT_Done_Face( m_cacheFace );
		m_cacheFace = 0;
	}
	this->ClearCache();
	m_fontFileName = L"";
	DestroyDelete( m_pFontMap );
}

bool CFont::CacheCharacters( const wchar_t *pCharacters, size_t characterCount )
{
	if( !m_cacheFace ) {
		PrintError( L"Cannot cache characters when font face map has already been created\n" );
		return false;
	}
	for( size_t i = 0; i < characterCount; i++ )
	{
		FT_UInt glyphIndex;
		FT_Error ftError;
		CachedGlyph cachedGlyph;
		unsigned char *pTempBuffer;

		// Get the glyph index
		glyphIndex = FT_Get_Char_Index( m_cacheFace, pCharacters[i] );
		if( !glyphIndex ) {
			PrintWarn( L"Missing character \'%c\' in font \'%s\'\n", pCharacters[i], m_fontFileName.c_str() );
			glyphIndex = FT_Get_Char_Index( m_cacheFace, REPLACEMENT_CHAR );
		}

		// Attempt to load the glyph
		ftError = FT_Load_Glyph( m_cacheFace, glyphIndex, FT_LOAD_RENDER );
		if( ftError != 0 ) {
			PrintWarn( L"Failed to load glyph (%c) in font \'%s\' (code: %d, '%hs')\n", pCharacters[i], m_fontFileName.c_str(), ftError, GetFreetypeError( ftError ) );
			continue;
		}
		if( m_cacheFace->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY ) {
			PrintWarn( L"Glyph loaded as invalid format\n" );
			continue;
		}

		// Store the glyphs data
		cachedGlyph.character = pCharacters[i];
		cachedGlyph.width = m_cacheFace->glyph->bitmap.width;
		cachedGlyph.rows = m_cacheFace->glyph->bitmap.rows;
		cachedGlyph.pitch = m_cacheFace->glyph->bitmap.pitch;
		cachedGlyph.metrics.horizAdvance = m_cacheFace->glyph->metrics.horiAdvance / FT_HRES;
		cachedGlyph.metrics.vertAdvance = m_cacheFace->glyph->metrics.vertAdvance / FT_HRES;
		cachedGlyph.metrics.descent = (m_cacheFace->glyph->metrics.height - m_cacheFace->glyph->metrics.horiBearingY) / FT_HRES;
		cachedGlyph.metrics.ascent = m_cacheFace->glyph->metrics.horiBearingY / FT_HRES;
		cachedGlyph.padding = { 1, (m_cacheFace->ascender / FT_HRES) - cachedGlyph.metrics.ascent, 1, (-m_cacheFace->descender / FT_HRES) - cachedGlyph.metrics.descent };

		// Check if glyph is special char
		if( pCharacters[i] == L' ' ) {
			cachedGlyph.width = cachedGlyph.metrics.horizAdvance;
			cachedGlyph.rows = 1;
			cachedGlyph.bufferSize = cachedGlyph.rows*cachedGlyph.width;
			cachedGlyph.pBuffer = new unsigned char[cachedGlyph.bufferSize];
			cachedGlyph.padding.bottom--;
			memset( cachedGlyph.pBuffer, 0, cachedGlyph.bufferSize );
		}
		else
		{
			// Copy bitmap
			cachedGlyph.bufferSize = cachedGlyph.rows*cachedGlyph.pitch;
			cachedGlyph.pBuffer = new unsigned char[cachedGlyph.bufferSize];
			memcpy( cachedGlyph.pBuffer, m_cacheFace->glyph->bitmap.buffer, cachedGlyph.bufferSize );
		}

		// Convert bitmap to distance field
		pTempBuffer = make_distance_mapb( cachedGlyph.pBuffer, cachedGlyph.width, cachedGlyph.rows );
		SafeDeleteArray( cachedGlyph.pBuffer );
		cachedGlyph.pBuffer = pTempBuffer;

		m_cachedArea += cachedGlyph.bufferSize;

		m_cachedGlyphs.push_back( cachedGlyph );
	}

	return true;
}
bool CFont::GenerateMapFromCache()
{
	TextureDescriptor textureDesc;
	unsigned char *pMapBuffer;
	unsigned int mapDimension;

	DestroyDelete( m_pFontMap );

	m_pFontMap = new CTexture2D();
	if( !m_pFontMap->initialize() )
		return false;

	mapDimension = 256;
	if( m_cachedArea > mapDimension*mapDimension ) {
		PrintError( L"Font map is too small! Time to fix this...\n" );
		return false;
	}

	// Create a test buffer
	pMapBuffer = new unsigned char[mapDimension*mapDimension];
	memset( pMapBuffer, 0, mapDimension*mapDimension );

	// Now we have to bin sort each glyph
	this->binPackCache( mapDimension );

	// Render the map and populate glyph map
	unsigned int width, height, pitch;
	unsigned int offsetX, offsetY;
	for( auto it = m_glyphBinNodes.begin(); it != m_glyphBinNodes.end(); it++ )
	{
		if( (*it)->pLeft || (*it)->pRight )
		{
			width = (*it)->width - (*it)->glyph.padding.left - (*it)->glyph.padding.right;
			height = (*it)->height - (*it)->glyph.padding.top - (*it)->glyph.padding.bottom;
			pitch = (*it)->glyph.pitch;
			offsetX = (*it)->x + (*it)->glyph.padding.left;
			offsetY = (*it)->y + (*it)->glyph.padding.top;
			// Insert a glyph into the buffer
			for( unsigned int x = offsetX; x < offsetX+width; x++ ) {
				for( unsigned int y = offsetY; y < offsetY+height; y++ ) {
					pMapBuffer[y*mapDimension+x] = (*it)->glyph.pBuffer[(y-offsetY)*width+(x-offsetX)];
				}
			}

			// Store the relevant glyph data
			GlyphData glyphData;
			glyphData.width = (*it)->width;
			glyphData.height = (*it)->height;
			glyphData.uv_start = glm::vec2( (float)(*it)->x / (float)mapDimension, (float)(*it)->y / (float)mapDimension );
			glyphData.uv_end = glm::vec2(  (float)(*it)->width / (float)mapDimension, (float)(*it)->height / (float)mapDimension );
			glyphData.metrics = (*it)->glyph.metrics;
			m_glyphs.insert( std::pair<wchar_t, GlyphData>( (*it)->glyph.character, glyphData ) );
		}
	}

	textureDesc.magFilter = GL_LINEAR;
	textureDesc.minFilter = GL_LINEAR;
	textureDesc.wrapS = GL_CLAMP_TO_EDGE;
	textureDesc.wrapT = GL_CLAMP_TO_EDGE;
	textureDesc.useAlpha = false;
	textureDesc.monochrome = true;
	if( !m_pFontMap->loadTextureFromMemory( pMapBuffer, mapDimension*mapDimension, mapDimension, mapDimension, textureDesc ) )
		return false;

	// Clear cache data
	this->ClearCache();

	return true;
}
void CFont::ClearCache()
{
	// Clean up bin nodes
	for( auto it = m_glyphBinNodes.begin(); it != m_glyphBinNodes.end(); it++ ) {
		delete (*it);
	}
	m_glyphBinNodes.clear();
	// Clean up cached glyph
	for( auto it = m_cachedGlyphs.begin(); it != m_cachedGlyphs.end(); it++ ) {
		delete[] ( *it ).pBuffer;
	}
	m_cachedGlyphs.clear();
	m_cachedArea = 0;
}
bool CFont::binPackCache( unsigned int mapDimension )
{
	GlyphBinNode *pRootNode;

	// Sort by area
	std::sort( m_cachedGlyphs.begin(), m_cachedGlyphs.end() );

	// Root node
	pRootNode = new GlyphBinNode;
	pRootNode->pLeft = NULL;
	pRootNode->pRight = NULL;
	pRootNode->width = mapDimension;
	pRootNode->height = mapDimension;
	pRootNode->x = pRootNode->y = 0;
	m_glyphBinNodes.push_back( pRootNode );
	// Pack the glyphs
	for( std::vector<CachedGlyph>::iterator it = m_cachedGlyphs.begin(); it != m_cachedGlyphs.end(); it++ ) {
		// Insert into a node
		if( !this->insertIntoBin( pRootNode, (*it) ) ) {
			PrintError( L"Font tree ran out of space\n" );
			break;
		}
	}

	return true;
}
GlyphBinNode* CFont::insertIntoBin( GlyphBinNode *pNode, CachedGlyph cachedGlyph )
{
	int newWidth, newHeight;
	unsigned int boxWidth, boxHeight;

	// Check if this node is internal (has children)
	if( pNode->pLeft || pNode->pRight )
	{
		// Check its neighbors for space
		if( pNode->pLeft ) {
			// Try to put it in the neighbor
			GlyphBinNode *pNewNode = insertIntoBin( pNode->pLeft, cachedGlyph );
			if( pNewNode )
				return pNewNode;
		}
		if( pNode->pRight ) {
			// Try to put it in the neighbor
			GlyphBinNode *pNewNode = insertIntoBin( pNode->pRight, cachedGlyph );
			if( pNewNode )
				return pNewNode;
		}
		// No space in its neighbors
		return NULL;
	}

	boxWidth = cachedGlyph.width + cachedGlyph.padding.left + cachedGlyph.padding.right;
	boxHeight = cachedGlyph.rows + cachedGlyph.padding.top + cachedGlyph.padding.bottom;

	// If it has no children, check if the glyph fits here
	if( boxWidth > pNode->width || boxHeight > pNode->height )
		return NULL;

	// Split the node along the short axis and 
	// insert the glyph
	newWidth = pNode->width - boxWidth;
	newHeight = pNode->height - boxHeight;
	// Create new children
	pNode->pLeft = new GlyphBinNode();
	pNode->pRight = new GlyphBinNode();
	m_glyphBinNodes.push_back( pNode->pLeft );
	m_glyphBinNodes.push_back( pNode->pRight );
	if( newWidth <= newHeight )
	{
		pNode->pLeft->x = pNode->x + boxWidth;
		pNode->pLeft->y = pNode->y;
		pNode->pLeft->width = newWidth;
		pNode->pLeft->height = boxHeight;

		pNode->pRight->x = pNode->x;
		pNode->pRight->y = pNode->y + boxHeight;
		pNode->pRight->width = pNode->width;
		pNode->pRight->height = newHeight;
	}
	else
	{
		pNode->pLeft->x = pNode->x;
		pNode->pLeft->y = pNode->y + boxHeight;
		pNode->pLeft->width = boxWidth;
		pNode->pLeft->height = newHeight;

		pNode->pRight->x = pNode->x + boxWidth;
		pNode->pRight->y = pNode->y;
		pNode->pRight->width = newWidth;
		pNode->pRight->height = pNode->height;
	}

	// Shrink the node
	pNode->width = boxWidth;
	pNode->height = boxHeight;
	pNode->glyph = cachedGlyph;

	return pNode;
}

CTexture2D* CFont::getFontMap() {
	return m_pFontMap;
}

GlyphData* CFont::getGlyph( wchar_t character )
{
	auto it = m_glyphs.find( character );
	if( it != m_glyphs.end() )
		return &(*it).second;
	else {
		PrintError( L"Character %c not found in font name %s\n", character, m_fontFileName.c_str() );
		return 0;
	}
}
int CFont::getPixelKerning( wchar_t leftGlyph, wchar_t rightGlyph )
{
	FT_UInt leftGlyphIndex, rightGlyphIndex;
	FT_Error ftError;
	FT_Vector kerningVector;

	if( !m_cacheFace )
		return 0;
	// Get the glyph index
	leftGlyphIndex = FT_Get_Char_Index( m_cacheFace, leftGlyph );
	if( !leftGlyphIndex ) {
		PrintWarn( L"Missing character \'%c\' in font \'%s\'\n", leftGlyph, m_fontFileName.c_str() );
		return 0;
	}
	rightGlyphIndex = FT_Get_Char_Index( m_cacheFace, rightGlyph );
	if( !rightGlyphIndex ) {
		PrintWarn( L"Missing character \'%c\' in font \'%s\'\n", rightGlyph, m_fontFileName.c_str() );
		return 0;
	}
	ftError = FT_Get_Kerning( m_cacheFace, leftGlyphIndex, rightGlyphIndex, FT_KERNING_DEFAULT, &kerningVector );
	if( ftError != 0 ) {
		PrintWarn( L"Failed to extract kerning in font \'%s\' (code: %d, '%hs')\n", m_fontFileName.c_str(), ftError, GetFreetypeError( ftError ) );
		return 0;
	}
	return (kerningVector.x / FT_HRES);
}