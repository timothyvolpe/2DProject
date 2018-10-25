#pragma once

#include <glm\glm.hpp>
#include <vector>

class CInterfaceParent;

class CInterfaceBase
{
private:
	CInterfaceParent *m_pParent;

	std::wstring m_name;
	unsigned char m_layer;
protected:
	glm::ivec2 m_position;
	glm::ivec2 m_size;
public:
	CInterfaceBase();
	virtual ~CInterfaceBase();

	virtual bool initialize( std::wstring name );
	virtual void destroy();

	virtual bool activate();

	virtual bool onCreate() = 0;
	virtual void onDestroy() = 0;
	virtual bool onActivate() { return true; }

	virtual void onDraw() {}

	virtual void onPositionChange( glm::ivec2 oldPos ) {};
	virtual void onSizeChange( glm::ivec2 oldSize ) {};
	virtual void onParentChange( CInterfaceParent *pOldParent ) {};

	glm::ivec2 getRelativePosition() const;
	glm::ivec2 getAbsolutePosition() const;
	void setPosition( glm::ivec2 pos );
	glm::ivec2 getSize() const;
	void setSize( glm::ivec2 size );
	std::wstring getName() const;
	unsigned char getLayer() const;
	void setLayer( unsigned char layer );

	CInterfaceParent* getParent();
	void setParent( CInterfaceParent *pParent, bool notifyParent = true );

	virtual bool isParent() { return false; }
};

class CInterfaceParent : public CInterfaceBase
{
private:
	std::vector<CInterfaceBase*> m_children;
public:
	CInterfaceParent();
	virtual ~CInterfaceParent();

	bool initialize( std::wstring name );
	void destroy();

	bool addChild( CInterfaceBase *pChild, bool notifyChild = true );
	bool removeChild( CInterfaceBase *pChild,  bool notifyChild = true );
	void clearChildren();

	std::vector<CInterfaceBase*> getChildren() const;

	bool isParent() { return true; }
};