#include "base.h"
#include "shader\shader.h"

///////////////////
// CShaderObject //
///////////////////

CShaderObject::CShaderObject() {
	m_name = L"UNNAMED";
	m_shaderType = 0;
	m_shaderId = 0;
}
CShaderObject::~CShaderObject() {
}

bool CShaderObject::initializeShader( std::wstring file, GLenum type )
{
	boost::filesystem::path fullPath;
	std::string shaderSource;
	GLint compileStatus, logLength;

	m_name = file;
	m_shaderType = type;

	PrintInfo( L"Loading shader object \"%s\" ", file.c_str() );

	// Determine type
	switch( type )
	{
	case GL_VERTEX_SHADER:
		PrintInfo( L"(VERT)...\n" );
		file += L".vert";
		break;
	case GL_GEOMETRY_SHADER:
		PrintInfo( L"(GEOM)...\n" );
		file += L".geom";
		break;
	case GL_FRAGMENT_SHADER:
		PrintInfo( L"(FRAG)...\n" );
		file += L".frag";
		break;
	default:
		PrintError( L"Unknown shader type\n" );
		return false;
	}

	fullPath = GameFilesystem::ConstructPath( file, FILESYSTEM_DIR_SHADER );
	// Make sure it exists
	if( !GameFilesystem::IsValidFile( fullPath ) ) {
		PrintError( L"Could not find shader object (%s)\n", fullPath.wstring().c_str() );
		return false;
	}

	// Load the shader source
	try
	{
		// Read the contents of the file
		std::ifstream inStream( fullPath.string().c_str() );
		// Get length
		inStream.seekg( 0, std::ios::end );
		shaderSource.reserve( (unsigned int)inStream.tellg() );
		inStream.seekg( 0, std::ios::beg );
		// Read the file
		shaderSource.assign( std::istreambuf_iterator<char>( inStream ), std::istreambuf_iterator<char>() );
		inStream.close();
	}
	catch( const std::ifstream::failure& e ) {
		PrintError( L"Failed to load shader object because the file could not be read: %hs\n", e.what() );
		return false;
	}

	StartGLDebug( "CreateShader" );

	// Create the shader object
	m_shaderId = glCreateShader( m_shaderType );
	if( !m_shaderId ) {
		PrintError( L"OpenGL failed to create shader (code: %d)\n", glGetError() );
		EndGLDebug();
		return false;
	}
	// Compile the shader source
	const char *source_str = shaderSource.c_str();
	glShaderSource( m_shaderId, 1, &source_str, 0 );
	glCompileShader( m_shaderId );

	// Check compile status
	glGetShaderiv( m_shaderId, GL_COMPILE_STATUS, &compileStatus );
	if( compileStatus == GL_FALSE )
	{
		glGetShaderiv( m_shaderId, GL_INFO_LOG_LENGTH, &logLength );
		if( logLength > 0 )
		{
			std::vector<GLchar> infoLog( logLength );
			boost::filesystem::path logPath;
			std::string compileLogHeader;

			glGetShaderInfoLog( m_shaderId, logLength, 0, &infoLog[0] );
			// Write to the log file
			logPath = boost::filesystem::current_path();
			logPath /= FILESYSTEM_DIR_LOGS;
			// Check if it exists and if not, create
			if( !boost::filesystem::is_directory( logPath ) )
				boost::filesystem::create_directory( logPath );
			logPath /= COMPILE_LOG;
			try {
				std::ofstream outStream( logPath.string().c_str(), std::ios::app );
				compileLogHeader = ">" + fullPath.stem().string() + "\n";
				outStream.write( compileLogHeader.c_str(), compileLogHeader.length() );
				outStream.write( &infoLog[0], logLength );
				outStream.close();
			}
			catch( std::ofstream::failure &e ) {
				PrintError( L"Failed to write to compile.log (%hs)\n", e.what() );
				EndGLDebug();
				return false;
			}

			PrintError( L"Failed to compile shader \"%s\", see compile.log\n", m_name.c_str() );
		}
		else {
			PrintError( L"Failed to compile shader \"%s\", no log from OpenGL\n", m_name.c_str() );
			EndGLDebug();
			return false;
		}
		glDeleteShader( m_shaderId );
		m_shaderId = 0;
		EndGLDebug();
		return false;
	}

	EndGLDebug();

	return true;
}
void CShaderObject::destroy()
{
	// Delete the shader object
	if( m_shaderId ) {
		glDeleteShader( m_shaderId );
		m_shaderId = 0;
	}
	m_shaderType = 0;
	m_name = L"DELETED";
}

GLuint CShaderObject::getShaderId() const {
	return m_shaderId;
}

////////////////////
// CShaderProgram //
////////////////////

CShaderProgram::CShaderProgram() {
	m_name = L"UNNAMED";
	m_programId = 0;
}
CShaderProgram::~CShaderProgram() {
}

bool CShaderProgram::initializeProgram( std::wstring name, CShaderObject *pVertexObject, CShaderObject* pGeomtryObject, CShaderObject *pFragmentObject, std::vector<std::string> uniformNames )
{
	GLint linkStatus, logLength;

	m_name = name;
	PrintInfo( L"Linking shader program \"%s\"...\n", m_name.c_str() );

	StartGLDebug( "CreateProgram" );

	// Create the program
	m_programId = glCreateProgram();
	if( !m_programId ) {
		PrintError( L"OpenGL failed to create a shader program\n" );
		EndGLDebug();
		return false;
	}
	// Attach the shaders
	if( pVertexObject )
		glAttachShader( m_programId, pVertexObject->getShaderId() );
	if( pGeomtryObject )
		glAttachShader( m_programId, pGeomtryObject->getShaderId() );
	if( pFragmentObject )
		glAttachShader( m_programId, pFragmentObject->getShaderId() );

	// Link the program
	glLinkProgram( m_programId );
	// Make sure it was successful
	glGetProgramiv( m_programId, GL_LINK_STATUS, &linkStatus );
	if( linkStatus == GL_FALSE )
	{
		glGetProgramiv( m_programId, GL_INFO_LOG_LENGTH, &logLength );
		if( logLength > 0 )
		{
			std::vector<GLchar> infoLog( logLength );
			boost::filesystem::path logPath;

			glGetProgramInfoLog( m_programId, logLength, 0, &infoLog[0] );
			// Write to the log file
			logPath = boost::filesystem::current_path();
			logPath /= FILESYSTEM_DIR_LOGS;
			// Check if it exists and if not, create
			if( !boost::filesystem::is_directory( logPath ) )
				boost::filesystem::create_directory( logPath );
			logPath /= LINK_LOG;
			try {
				std::ofstream outStream( logPath.string().c_str(), std::ios::app );
				outStream.write( &infoLog[0], logLength );
				outStream.close();
			}
			catch( std::ofstream::failure &e ) {
				PrintError( L"Failed to write to link.log (%hs)\n", e.what() );
				EndGLDebug();
				return false;
			}

			PrintError( L"Failed to link shader program \"%s\", see link.log\n", m_name.c_str() );
			EndGLDebug();
			return false;
		}
		else {
			PrintError( L"Failed to link shader program \"%s\", no log from OpenGL\n", m_name.c_str() );
			EndGLDebug();
			return false;
		}
		glDeleteProgram( m_programId );
		m_programId = 0;
	}

	EndGLDebug();
	StartGLDebug( "FindShaders" );

	// Find uniforms
	for( auto it = uniformNames.begin(); it != uniformNames.end(); it++ )
	{
		GLint location;

		location = glGetUniformLocation( m_programId, (*it).c_str() );
		if( location == -1 ) {
			PrintWarn( L"Could not find uniform \"%hs\", ignoring!\n", (*it).c_str() );
			continue;
		}
		m_uniforms.insert( std::pair<std::string, GLint>( (*it), location ) );
	}

	EndGLDebug();
	StartGLDebug( "DetachShaders" );

	// Detach the shaders
	if( pVertexObject )
		glDetachShader( m_programId, pVertexObject->getShaderId() );
	if( pGeomtryObject )
		glDetachShader( m_programId, pGeomtryObject->getShaderId() );
	if( pFragmentObject )
		glDetachShader( m_programId, pFragmentObject->getShaderId() );

	EndGLDebug();

	return true;
}
void CShaderProgram::destroy()
{
	m_name = L"DELETED";
	if( m_programId ) {
		glDeleteProgram( m_programId );
		m_programId = 0;
	}
	m_uniforms.clear();
}

void CShaderProgram::bind() {
	StartGLDebug( "BindShader" );
	glUseProgram( m_programId );
	EndGLDebug();
}

std::wstring CShaderProgram::getName() const {
	return m_name;
}

GLuint CShaderProgram::getUniform( std::string name ) {
	if( m_uniforms.find( name ) != m_uniforms.end() )
		return m_uniforms[name];
	else {
		PrintWarn( L"Could not find uniform \"%hs\"\n", name.c_str() );
		return 0;
	}
}