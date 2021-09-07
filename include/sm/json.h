// Note:
// 1. Referencing of this format: https://github.com/ErikMinekus/sm-ripext/blob/main/pawn/scripting/include/ripext/json.inc
// 2. In fact, you just need to use json_t typename.
// 3. The class functions is just a wrapper for object, but we are not recommend you to use it.
// 4. Warning: You must use try/catch to handle this part!

#pragma once
#include <nlohmann/json.hpp>
#include <fstream>
#include <iomanip>
namespace sm
{
	inline namespace json
	{
		using json_t = nlohmann::json;

		std::string json_to_string(json_t j) { return j.dump(); }

		// Basic JSON class
		class JSON
		{
		protected:
			json_t json;
		public:
			std::string to_string() { return json.dump(); }
			void to_file(std::string file_name, int indent = -1) { std::ofstream(file_name) << std::setw(indent) << json << std::endl; }
		};

		// You should always use try/catch to handle this!
		class JSONObject : public JSON
		{
		public:
			explicit JSONObject() {}
			JSONObject(std::string str_in) { json = json_t::parse(str_in); }
			JSONObject(json_t j) { json = j; }
			JSONObject(std::ifstream i) { i >> json; }
			std::vector<std::string> Key() {
				std::vector<std::string> ret;
				for (auto& obj : this->json.items())
				{
					ret.push_back(obj.key());
				}
				return ret;
			}
			template<typename T> T Get(std::string key) { return this->json.at(key).get<T>(); }
			template<typename T> void Set(std::string key, T value) { this->json.at(key) = value; }
			bool IsNull(std::string key) { return this->json.at(key).is_null(); }
			bool HasKey(std::string key) { return this->json.contains(key); }
			void Remove(std::string key) { this->json.erase(key); }
			void Clear() { this->json.clear(); }
		};

		// JSONArray = std::vector<T>
		// You should not use this class here.
		class JSONArray : public JSON {};
	}
}