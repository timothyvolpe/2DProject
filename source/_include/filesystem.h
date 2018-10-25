#pragma once

#include <boost\filesystem.hpp>

#define FILESYSTEM_DIR_LOGS L"logs"
#define FILESYSTEM_DIR_CONFIG L"data\\config"
#define FILESYSTEM_DIR_SHADER L"data\\shader"
#define FILESYSTEM_DIR_TEXTURES L"data\\textures"
#define FILESYSTEM_DIR_FONTS L"data\\fonts"

namespace GameFilesystem
{
	bool ValidateGameDir( std::wstring dir );

	boost::filesystem::path ConstructPath( boost::filesystem::path relativePath, std::wstring dir );
	bool IsValidFile( boost::filesystem::path fullPath );
};