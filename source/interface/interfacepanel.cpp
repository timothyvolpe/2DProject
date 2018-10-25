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
	CGame::getInstance().getInterfaceManager()->addQuad( this->getAbsolutePosition(), this->getSize() );
	return true;
}

void CInterfacePanel::onDraw()
{
	glm::ivec2 interfacePos, interfaceSize;

	interfaceSize = this->getSize();
	interfacePos = glm::ivec2( this->getAbsolutePosition().x, this->getAbsolutePosition().y );

	/*glBegin( GL_TRIANGLES );
	{
		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3i( interfacePos.x+interfaceSize.x, interfacePos.y, 200 );
		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3i( interfacePos.x, interfacePos.y, 200 );
		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3i( interfacePos.x, interfacePos.y+interfaceSize.y, 200 );
		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3i( interfacePos.x, interfacePos.y+interfaceSize.y, 200 );
		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3i( interfacePos.x+interfaceSize.x, interfacePos.y+interfaceSize.y, 200 );
		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3i( interfacePos.x+interfaceSize.x, interfacePos.y, 200 );
	}
	glEnd();*/
}