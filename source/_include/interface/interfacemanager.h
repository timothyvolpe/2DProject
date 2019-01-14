#pragma once

#include <map>

#define GAME_FONT_FILE L"fontdef.info"

#define MAX_QUADS_PER_BUFFER 8192

class CFont;
class CInterfaceBase;
class CInterfaceScreen;

// POS DENOTES THE TOP LEFT OF THE QUAD
#pragma pack(push, 4)
typedef struct {
	glm::lowp_uvec2 pos;
	glm::lowp_uvec2 size;
	glm::lowp_uvec4 texcoords;
	char textureId;
	unsigned char color[3];
} InterfaceVertex;
#pragma pack(pop)

struct InterfaceQuad {
	glm::ivec2 absPos, absSize;
	glm::vec2 tex_start, tex_stop;
	glm::ivec3 color;
	char textureId;
	unsigned char interfaceLayer;
};

class CInterfaceManager
{
private:
	std::map<std::wstring, CFont*> m_fontMap;
	std::vector<CInterfaceScreen*> m_activeScreens;

	std::vector<InterfaceQuad> m_interfaceQuads;

	GLuint m_vaoId;
	GLuint m_vboId;

	unsigned int m_vertexCount, m_indexCount;

	bool loadFonts();
	bool loadFontFromFile( bool systemFont, std::wstring const& fontId, std::wstring const& fontFile);

	void drawChildren( glm::mat4 interfaceOrthoMat, std::vector<CInterfaceBase*> children );
public:
	CInterfaceManager();
	~CInterfaceManager();

	bool initialize();
	void destroy();

	void draw( glm::mat4 interfaceOrthoMat );

	bool addScreen( CInterfaceScreen *pScreen );
	bool removeScreen( CInterfaceScreen *pScreen );

	void drawQuad( InterfaceQuad interfaceQuad );

	CFont* getFont( const wchar_t* fontName );
};