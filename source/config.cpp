#include "base.h"
#include "config.h"

CGameConfig::CGameConfig() {
	m_windowWidth = 0;
	m_windowWidth = 0;
	m_resolutionX = 0;
	m_resolutionY = 0;
	m_bFullscreen = false;
	m_bBorderless = false;
	m_bVsync = false;
	m_bAntialiasing = true;
	m_multisampleSamples = 0;
}
CGameConfig::~CGameConfig() {
}

bool CGameConfig::load()
{
	boost::filesystem::path configPath;
	boost::property_tree::wptree xmlTree;

	// Get the config path
	configPath = GameFilesystem::ConstructPath( GAME_CONFIG_FILE, FILESYSTEM_DIR_CONFIG );

	// Load the xml
	try {
		PrintInfo( L"Loading config file...\n" );
		boost::property_tree::read_xml( configPath.string(), xmlTree );
	}
	catch( const boost::property_tree::xml_parser_error& e ) {
		PrintError( L"Failed to load config file (%hs)\n", e.what() );
	}

	// Read all the properties
	m_windowWidth = xmlTree.get( L"GameConfig.Video.WindowSize.X", 800 );
	m_windowHeight = xmlTree.get( L"GameConfig.Video.WindowSize.Y", 600 );
	m_resolutionX = xmlTree.get( L"GameConfig.Video.Resolution.X", 800 );
	m_resolutionY = xmlTree.get( L"GameConfig.Video.Resolution.Y", 600 );
	m_bFullscreen = xmlTree.get( L"GameConfig.Video.IsFullscreen", false );
	m_bBorderless = xmlTree.get( L"GameConfig.Video.Borderless", false );
	m_bVsync = xmlTree.get( L"GameConfig.Video.VerticalSync", false );
	m_bAntialiasing = xmlTree.get( L"GameConfig.Video.MultisampleAntialiasing", true );
	m_multisampleSamples = xmlTree.get( L"GameConfig.Video.MultisamplSamples", 2 );

	// Load the keybinds
	m_keybinds[KEYBIND_JUMP] = xmlTree.get( L"GameConfig.Keybinds.Jump", (unsigned short)SDL_SCANCODE_W );
	m_keybinds[KEYBIND_CROUCH] = xmlTree.get( L"GameConfig.Keybinds.Crouch", (unsigned short)SDL_SCANCODE_S );
	m_keybinds[KEYBIND_MOVE_LEFT] = xmlTree.get( L"GameConfig.Keybinds.MoveLeft", (unsigned short)SDL_SCANCODE_A );
	m_keybinds[KEYBIND_MOVE_RIGHT] = xmlTree.get( L"GameConfig.Keybinds.MoveRight", (unsigned short)SDL_SCANCODE_D );
	m_keybinds[KEYBIND_WALK] = xmlTree.get( L"GameConfig.Keybinds.Walk", (unsigned short)SDL_SCANCODE_LCTRL );
	m_keybinds[KEYBIND_RUN] = xmlTree.get( L"GameConfig.Keybinds.Run", (unsigned short)SDL_SCANCODE_LSHIFT );
	m_keybinds[KEYBIND_TOGGLEWIREFRAME] = xmlTree.get( L"GameConfig.Keybinds.ToggleWireframe", (unsigned short)SDL_SCANCODE_F2 );
	m_keybinds[KEYBIND_TOGGLEDEBUGDRAW] = xmlTree.get( L"GameConfig.Keybinds.ToggleDebugDraw", (unsigned short)SDL_SCANCODE_F3 );
	m_keybinds[KEYBIND_TOGGLEFLY] = xmlTree.get( L"GameConfig.Keybinds.ToggleFly", (unsigned short)SDL_SCANCODE_F8 );

	return true;
}
bool CGameConfig::save()
{
	boost::filesystem::path configPath;
	boost::property_tree::wptree xmlTree;

	// Save all the properties
	xmlTree.put( L"GameConfig.Video.WindowSize.X", m_windowWidth );
	xmlTree.put( L"GameConfig.Video.WindowSize.Y", m_windowHeight );
	xmlTree.put( L"GameConfig.Video.Resolution.X", m_resolutionX );
	xmlTree.put( L"GameConfig.Video.Resolution.Y", m_resolutionY );
	xmlTree.put( L"GameConfig.Video.IsFullscreen", m_bFullscreen );
	xmlTree.put( L"GameConfig.Video.Borderless", m_bBorderless );
	xmlTree.put( L"GameConfig.Video.VerticalSync", m_bVsync );
	xmlTree.put( L"GameConfig.Video.MultisampleAntialiasing", m_bAntialiasing );
	xmlTree.put( L"GameConfig.Video.MultisamplSamples", m_multisampleSamples );

	// Save the keybinds
	xmlTree.put( L"GameConfig.Keybinds.Jump", m_keybinds[KEYBIND_JUMP] );
	xmlTree.put( L"GameConfig.Keybinds.Crouch", m_keybinds[KEYBIND_CROUCH] );
	xmlTree.put( L"GameConfig.Keybinds.MoveLeft", m_keybinds[KEYBIND_MOVE_LEFT] );
	xmlTree.put( L"GameConfig.Keybinds.MoveRight", m_keybinds[KEYBIND_MOVE_RIGHT] );
	xmlTree.put( L"GameConfig.Keybinds.Walk", m_keybinds[KEYBIND_WALK] );
	xmlTree.put( L"GameConfig.Keybinds.Run", m_keybinds[KEYBIND_RUN] );
	xmlTree.put( L"GameConfig.Keybinds.ToggleWireframe", m_keybinds[KEYBIND_TOGGLEWIREFRAME] );
	xmlTree.put( L"GameConfig.Keybinds.ToggleDebugDraw", m_keybinds[KEYBIND_TOGGLEDEBUGDRAW] );
	xmlTree.put( L"GameConfig.Keybinds.ToggleFly", m_keybinds[KEYBIND_TOGGLEFLY] );

	// Get the config path
	configPath = GameFilesystem::ConstructPath( GAME_CONFIG_FILE, FILESYSTEM_DIR_CONFIG );
	// Write it
	try {
		boost::property_tree::write_xml( configPath.string(), xmlTree );
	}
	catch( const boost::property_tree::xml_parser_error& e ) {
		PrintError( L"Failed to write game config (%hs)\n", e.what() );
		return false;
	}

	return true;
}

bool CGameConfig::initialize()
{
	m_keybinds.resize( KEYBIND_COUNT );

	// Load the config
	if( !this->load() )
		return false;

	return true;
}
void CGameConfig::destroy()
{
	// Save the config
	this->save();
}

unsigned int CGameConfig::getKeybind( unsigned int keybind ) const
{
	if( keybind > m_keybinds.size() ) {
		PrintWarn( L"Tried to get invalid keybind\n" );
		return 0;
	}
	return m_keybinds[keybind];
}