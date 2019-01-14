#include "base.h"
#include "interface\interfacemanager.h"
#include "interface\interfacescreen.h"

CInterfaceScreen::CInterfaceScreen() {
}
CInterfaceScreen::~CInterfaceScreen() {
}

bool CInterfaceScreen::onCreate() {
	return true;
}
void CInterfaceScreen::onDestroy() {
}

void CInterfaceScreen::onDraw()
{
	InterfaceQuad quad;

	quad.absPos = this->getAbsolutePosition();
	quad.absSize = this->getSize();
	quad.textureId = -1;
	quad.interfaceLayer = 0;
	quad.tex_start = glm::vec2( 0.0f, 0.0f );
	quad.tex_stop = glm::vec2( 1.0f, 1.0f );
	quad.color = glm::ivec3( 255, 150, 40 );

	CGame::getInstance().getInterfaceManager()->drawQuad( quad );
}