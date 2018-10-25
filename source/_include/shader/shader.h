#pragma once

#include <map>
#include <string>
#include <GL\glew.h>

#define COMPILE_LOG L"compile.log"
#define LINK_LOG L"link.log"

///////////////////
// CShaderObject //
///////////////////

class CShaderObject
{
private:
	std::wstring m_name;

	GLenum m_shaderType;
	GLuint m_shaderId;
public:
	CShaderObject();
	~CShaderObject();

	bool initializeShader( std::wstring file, GLenum type );
	void destroy();

	GLuint getShaderId() const;
};

////////////////////
// CShaderProgram //
////////////////////

class CShaderProgram
{
private:
	typedef std::map<std::string, GLint> UniformMap;

	std::wstring m_name;

	GLuint m_programId;
	UniformMap m_uniforms;

	void bind();
public:
	CShaderProgram();
	~CShaderProgram();

	bool initializeProgram( std::wstring name, CShaderObject *pVertexObject, CShaderObject *pFragmentObject, std::vector<std::string> uniformNames );
	void destroy();

	GLuint getUniform( std::string name );
	std::wstring getName() const;

	friend class CShaderManager;
};