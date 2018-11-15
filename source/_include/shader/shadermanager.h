#pragma once

#include <map>

class CShaderProgram;

class CShaderManager
{
private:
	typedef std::map<std::wstring, CShaderProgram*> ProgramMap;

	ProgramMap m_shaderPrograms;
	CShaderProgram *m_pCurrentlyBound;

	bool createAndLoadProgram( const wchar_t *pName, bool vertexShader, bool geomtryShader, bool fragmentShader, std::vector<std::string>& uniforms, std::vector<std::pair<int, std::string>> &attribLocations );

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
