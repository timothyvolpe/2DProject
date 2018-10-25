#pragma once

#include <boost\filesystem.hpp>
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\xml_parser.hpp>

#define GAME_CONFIG_FILE L"game.xml"

enum : unsigned int
{
	KEYBIND_JUMP = 0,
	KEYBIND_CROUCH,
	KEYBIND_MOVE_LEFT,
	KEYBIND_MOVE_RIGHT,
	KEYBIND_WALK,
	KEYBIND_RUN,
	KEYBIND_TOGGLEWIREFRAME,
	KEYBIND_TOGGLEDEBUGDRAW,
	KEYBIND_TOGGLEFLY,

	KEYBIND_COUNT
};

class CGameConfig
{
private:
	bool load();
	bool save();

	std::vector<unsigned int> m_keybinds;
public:
	CGameConfig();
	~CGameConfig();

	bool initialize();
	void destroy();

	int m_windowWidth, m_windowHeight;
	int m_resolutionX, m_resolutionY;
	bool m_bFullscreen;
	bool m_bBorderless;
	bool m_bVsync;
	bool m_bAntialiasing;
	int m_multisampleSamples;

	unsigned int getKeybind( unsigned int keybind ) const;
};