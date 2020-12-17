#pragma once

#include "filesystem.h"
#include <filesystem>

namespace sm
{
	inline namespace sourcemod
	{
		
		inline namespace filesystem
		{
			// You may want to ask: Why we construct SystemFile here, rather than using std::fstream
			// Well..definitely.. this is due to fstream.read does not accept any other char (int8_t, etc) expected of char*
			// But out term is for all cases, so that we have to construct a SystemFile class.
			class SystemFile
			{
			public:
				SystemFile(FILE* fp)
					: fp_(fp)
				{}
				~SystemFile() {
					Close();
				}

				static SystemFile* Open(const char* path, const char* mode) {
					FILE* fp = fopen(path, mode);
					if (!fp)
						return nullptr;
					return new SystemFile(fp);
				}
				static bool Delete(const char* path) {
					return _unlink(path) == 0;
				}
				size_t Read(void* pOut, int size) {
					return fread(pOut, 1, size, fp_);
				}
				char* ReadLine(char* pOut, int size) {
					return fgets(pOut, size, fp_);
				}
				size_t Write(const void* pData, int size) {
					return fwrite(pData, 1, size, fp_);
				}
				bool Seek(int pos, int seek_type) {
					return fseek(fp_, pos, seek_type) == 0;
				}
				int Tell() {
					return ftell(fp_);
				}
				bool HasError() {
					return ferror(fp_) != 0;
				}
				bool Flush() {
					return fflush(fp_) == 0;
				}
				bool EndOfFile() {
					return feof(fp_) != 0;
				}
				void Close() {
					if (!fp_)
						return;
					fclose(fp_);
					fp_ = nullptr;
				}
				virtual SystemFile* AsSystemFile() {
					return this;
				}
				FILE* fp() const {
					return fp_;
				}
			private:
				FILE* fp_;
				//std::string _filename;

			};

			// This is for binary this, for majority.
			inline int32_t ReadFileString(SystemFile* file, char* buffer, int size, int read_count = -1)
			{
				int32_t num_read = 0;
				if (read_count != -1)
				{
					if (std::size_t(read_count) > std::size_t(size))
					{
						smutils->LogError(myself, "read_count (%u) is greater than buffer size (%u)", read_count, size);
						return -1;
					}
					num_read = (int32_t)file->Read(buffer, read_count);
					if (num_read != read_count && file->HasError()) return -1;

					return num_read;
				}

				char val;
				while (true)
				{
					if (!size || num_read >= size - 1) break;
					if (file->Read(&val, sizeof(val)) != 1)
					{
						if (file->HasError()) return -1;

						break;
					}

					if (val == '\0') break;
					if (size > 0 && num_read < size - 1)
					{
						buffer[num_read++] = val;
					}
				}

				if (size > 0) buffer[num_read] = '\0';
				return num_read;
			}

			// IFileSystem <=> ValveFile
			// Same as Systemfile
			// so that we don't construct here.
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
			#undef DeleteFile
			inline bool DeleteFile(const char* path, bool use_valve_fs = false, const char* valve_path_id = "DEFAULT_WRITE_PATH")
			{
				if (use_valve_fs)
				{
					valvefs->RemoveFile(path, valve_path_id);
					return true;
				}

				char buffer[256];
				g_pSM->BuildPath(Path_Game, buffer, sizeof(buffer), "%s", path);
				std::filesystem::remove(buffer);
				return true;
			}
		}
	}
}