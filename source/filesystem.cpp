#include "base.h"
#include "filesystem.h"

namespace GameFilesystem
{
	bool ValidateGameDir( std::wstring dir )
	{
		boost::filesystem::path fullPath;

		fullPath = dir;

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

		fullPath = dir;
		fullPath /= relativePath;

		return fullPath;
	}
	bool IsValidFile( boost::filesystem::path fullPath )
	{
		// Check if it existss
		if( boost::filesystem::exists( fullPath ) )
		{
			// Check if it is a regular file
			if( boost::filesystem::is_regular_file( fullPath ) )
			{
				// Make sure it is in the current directory
				boost::filesystem::path absPath = boost::filesystem::canonical( fullPath );
				boost::filesystem::path curparent = absPath.parent_path();
				while( !curparent.empty() ) {
					if( curparent == boost::filesystem::current_path() )
						return true;
					curparent = curparent.parent_path();
				}
				return false;
			}
			else
				return false;
		}
		else
			return false;
	}
};