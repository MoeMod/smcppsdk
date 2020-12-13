#pragma once

#include "filesystem.h"
#include <filesystem>

// perhaps using std::fs will be easier..
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
			//native File OpenFile(const char[] file, const char[] mode, bool use_valve_fs = false, const char[] valve_path_id = "GAME");
			//native bool DeleteFile(const char[] path, bool use_valve_fs=false, const char[] valve_path_id="DEFAULT_WRITE_PATH");
			//native bool ReadFileLine(Handle hndl, char[] buffer, int maxlength);
			//native int ReadFile(Handle hndl, any[] items, int num_items, int size);
		}
	}
}