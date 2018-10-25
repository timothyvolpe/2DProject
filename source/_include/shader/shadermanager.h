#pragma once

#include <map>

class CShaderProgram;

class CShaderManager
{
private:
	typedef std::map<std::wstring, CShaderProgram*> ProgramMap;

	ProgramMap m_shaderPrograms;
	CShaderProgram *m_pCurrentlyBound;

	bool loadPrograms();
public:
	CShaderManager();
	~CShaderManager();

	bool initialize();
	void destroy();

	void bind( CShaderProgram *pProgram );

	CShaderProgram* getShaderProgram( std::wstring name );
	CShaderProgram* getCurrentlyBound() const;
};
