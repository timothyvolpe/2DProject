#pragma once

#include <map>

#define DEFAULT_CHARACTERS L" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789,<.>/?;:\'\"[{]}\\|!@#$%^&*()-_=+`~"
#define DEFAULT_CHARACTERS_COUNT 93
#define REPLACEMENT_CHAR 0xFFFD

class CTexture2D;

struct GlyphMetrics
{
	int horizAdvance;
	int vertAdvance;

	int descent;
	int ascent;
};
struct GlyphPadding
{
	int left, top, right, bottom;
};

struct CachedGlyph
{
	wchar_t character;
	unsigned char *pBuffer;
	unsigned int bufferSize;
	unsigned int width;
	unsigned int rows;
	unsigned int pitch;

	GlyphMetrics metrics;
	GlyphPadding padding;

	bool operator<( const CachedGlyph &rhs ) const;
};
struct GlyphBinNode
{
	GlyphBinNode *pLeft, *pRight;
	unsigned int x, y;
	unsigned int width, height;

	CachedGlyph glyph;
};
struct GlyphData
{
	unsigned int width, height;
	glm::vec2 uv_start, uv_end;

	GlyphMetrics metrics;
};

class CFont
{
private:
	std::wstring m_fontFileName;
	FT_Face m_cacheFace;

	CTexture2D *m_pFontMap;
	std::vector<CachedGlyph> m_cachedGlyphs;
	std::vector<GlyphBinNode*> m_glyphBinNodes;
	unsigned int m_cachedArea;

	std::map<wchar_t, GlyphData> m_glyphs;

	bool CacheCharacters( const wchar_t *pCharacters, size_t characterCount );
	bool GenerateMapFromCache();
	void ClearCache();

	bool binPackCache( unsigned int mapDimension );
	GlyphBinNode* insertIntoBin( GlyphBinNode *pNode, CachedGlyph cachedGlyph );
public:
	CFont();
	~CFont();

	bool initialize( bool systemFont, std::wstring const& fontFile );
	void destroy();

	GlyphData* getGlyph( wchar_t character );
	int getPixelKerning( wchar_t leftGlyph, wchar_t rightGlyph );

	CTexture2D* getFontMap();
};