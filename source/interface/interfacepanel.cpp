#include "base.h"
#include "interface\interfacemanager.h"
#include "interface\interfacepanel.h"

CInterfacePanel::CInterfacePanel() {
}
CInterfacePanel::~CInterfacePanel() {
}

bool CInterfacePanel::onCreate()
{
	return true;
}
void CInterfacePanel::onDestroy() {
}

bool CInterfacePanel::onActivate()
{
	
	return true;
}

void CInterfacePanel::onDraw()
{
	InterfaceQuad quad;

	quad.absPos = this->getAbsolutePosition();
	quad.absSize = this->getSize();
	quad.textureId = 0;
	quad.interfaceLayer = 0;
	quad.tex_start = glm::vec2( 0.0f, 0.0f );
	quad.tex_stop = glm::vec2( 1.0f, 1.0f );

	CGame::getInstance().getInterfaceManager()->drawQuad( quad );
}