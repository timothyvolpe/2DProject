#include "base.h"
#include "filesystem.h"

namespace GameFilesystem
{
	bool ValidateGameDir( std::wstring dir )
	{
		boost::filesystem::path fullPath;

		fullPath = boost::filesystem::current_path();
		fullPath /= dir;

		// Make sure the dir exists, if not create it
		if( boost::filesystem::exists( fullPath ) ) {
			if( boost::filesystem::is_directory( fullPath ) )
				return true;
		}
		// create it
		try {
			boost::filesystem::create_directories( fullPath );
		}
		catch( const boost::filesystem::filesystem_error& e ) {
			PrintError( L"Failed to create directories (%hs)\n", e.what() );
			return false;
		}

		return true;
	}

	boost::filesystem::path ConstructPath( boost::filesystem::path relativePath, std::wstring dir )
	{
		boost::filesystem::path fullPath;

		fullPath = boost::filesystem::current_path();
		fullPath /= dir;
		fullPath /= relativePath;

		return fullPath;
	}
	bool IsValidFile( boost::filesystem::path fullPath )
	{
		if( boost::filesystem::exists( fullPath ) ) {
			if( boost::filesystem::is_regular_file( fullPath ) )
				return true;
			else
				return false;
		}
		else
			return false;
	}
};