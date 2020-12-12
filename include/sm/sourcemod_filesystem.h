#pragma once

#include "filesystem.h"
#include <filesystem>
namespace sm
{
	inline namespace sourcemod
	{
		
		inline namespace filesystem
		{
			extern IFileSystem* valvefs;
			inline bool FileExists(const char* path, bool use_valve_fs = false, const char* valve_path_id = "GAME")
			{
				if (use_valve_fs)
				{
					return valvefs->FileExists(path, valve_path_id);
				}

				char realpath[256];
				g_pSM->BuildPath(Path_Game, realpath, sizeof(realpath), "%s", path);
				return std::filesystem::exists(realpath);
			}
			inline bool DirExists(const char* path, bool use_valve_fs = false, const char* valve_path_id = "GAME")
			{
				if (use_valve_fs)
				{
					return valvefs->IsDirectory(path, valve_path_id);
				}
				char realpath[256];
				g_pSM->BuildPath(Path_Game, realpath, sizeof(realpath), "%s", path);
				return std::filesystem::is_directory(realpath);
			}
			// Register functions.
			class File
			{

			};
		}
	}
}