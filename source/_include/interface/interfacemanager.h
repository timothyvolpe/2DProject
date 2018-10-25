#pragma once

#include <map>

#define GAME_FONT_FILE L"fontdef.info"

class CFont;
class CInterfaceBase;
class CInterfaceScreen;

struct InterfaceQuad {
	glm::ivec2 absPos, absSize;
};
#pragma pack(push, 4)
typedef struct {
	GLushort posx, posy;
	GLushort layer;
} InterfaceVertex;
#pragma pack(pop)

class CInterfaceManager
{
private:
	std::map<std::wstring, CFont*> m_fontMap;
	std::vector<CInterfaceScreen*> m_activeScreens;
	std::vector<InterfaceQuad> m_interfaceQuads;

	GLuint m_vaoId;
	GLuint m_vboId;
	GLuint m_iboId;

	unsigned int m_quadBufferSize;
	unsigned int m_vertexCount, m_indexCount;

	bool loadFonts();
	bool loadFontFromFile( bool systemFont, std::wstring const& fontId, std::wstring const& fontFile);

	void drawChildren( glm::mat4 interfaceOrthoMat, std::vector<CInterfaceBase*> children );

	void allocateQuadBuffer();
	void flushQuads();
public:
	CInterfaceManager();
	~CInterfaceManager();

	bool initialize();
	void destroy();

	void draw( glm::mat4 interfaceOrthoMat );

	bool addScreen( CInterfaceScreen *pScreen );
	bool removeScreen( CInterfaceScreen *pScreen );

	void addQuad( glm::ivec2 absolutePosition, glm::ivec2 absoluteSize );
	void clearQuads();

	CFont* getFont( const wchar_t* fontName );
};