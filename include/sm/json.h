// WARNING: The whole part is still under the DRAFT!
// Referencing of this format: https://github.com/ErikMinekus/sm-ripext/blob/main/pawn/scripting/include/ripext/json.inc
#pragma once
#include <nlohmann/json.hpp>
namespace sm::json
{
	using json_t = nlohmann::json;
	class JSON {
	public:
		inline FILE* ToFile() { return nullptr; }
		inline std::string ToString() { return {}; }

	protected:
		json_t json;
	};

	class JSONObject : public JSON {
	public:
		explicit JSONObject(std::string from_string) {}
		JSONObject(FILE* from_file) {}
		inline json_t Get() { return this->json; }
		template<typename T> inline T Get(std::string key) { return {  }; }
		template<typename T> inline bool Set(std::string key, T val) { return true; }
		//inline std::size_t Size() { return this->json.size(); }
		inline bool HasKey(std::string keyname) {
			return true;
		}
		inline bool Remove(std::string keyname) { return true; }
		inline bool Clear() { return true; }
		inline std::vector<std::string> Keys() {
			return {};
		}
		inline bool IsNull(std::string key) { return true; }
		std::size_t Size = this->json.size();
	};
	class JSONArray : public JSON {
	public:
		explicit JSONArray(std::string from_string) {}
		JSONArray(FILE* from_file) {}
		template<typename T> inline T Get(std::size_t index) { return {}; }
		template<typename T> inline bool Set(std::size_t index, T val) { return true; }
		template<typename T> inline bool Push(T val) { return true; }
		inline bool IsNull(std::size_t index) { return true; }
		inline bool Remove(std::size_t index) { return true; }
		inline bool Clear() { return true; }
		// This part should be std::vector.size()!!
		// Still in draft!
		std::size_t Length = this->json.size();
	};
}