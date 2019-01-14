#include "base.h"
#include "interface\interfacemanager.h"
#include "interface\interfacelabel.h"
#include "interface\font.h"

CInterfaceLabel::CInterfaceLabel() {
	m_pFontHandle = 0;
	m_text = L"FPS: 243";
}
CInterfaceLabel::~CInterfaceLabel() {
}

bool CInterfaceLabel::onCreate()
{
	m_pFontHandle = CGame::getInstance().getInterfaceManager()->getFont( L"DEFAULT_FONT" );
	return true;
}
void CInterfaceLabel::onDestroy()
{
	m_pFontHandle = 0;
}

bool CInterfaceLabel::onActivate() {
	return true;
}

void CInterfaceLabel::onDraw()
{
	CInterfaceManager *pManager = CGame::getInstance().getInterfaceManager();

	// Draw the letters
	if( m_pFontHandle )
	{
		InterfaceQuad quad;
		GlyphData *pGlyph;
		int horiCur = 0;
		int vertCur = 0;
		for( size_t i = 0; i < m_text.size(); i++ )
		{
			pGlyph = m_pFontHandle->getGlyph( m_text[i] );
			if( !pGlyph )
				continue;

			quad.absPos = this->getAbsolutePosition() + glm::ivec2( horiCur, vertCur );
			horiCur += pGlyph->width;
			// Kerning?
			//if( i != m_text.size()-1 )
				//horiCur -= m_pFontHandle->getPixelKerning( m_text[i], m_text[i+1] );
			quad.absSize = glm::vec2( pGlyph->width, pGlyph->height );
			quad.textureId = 0;
			quad.interfaceLayer = 0;
			quad.tex_start = pGlyph->uv_start;
			quad.tex_stop = pGlyph->uv_end;
			quad.color = glm::ivec3( 255, 255, 0 );

			pManager->drawQuad( quad );
		}
	}
}