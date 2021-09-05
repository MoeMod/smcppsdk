#pragma once

#include <string>

namespace sm
{
	inline namespace sourcemod
	{
		inline namespace string
		{
			// only accepts char/wchar_t
			// otherwise is not accepted
			template<typename T> concept Char_c = std::same_as<T, char> || std::same_as<T, wchar_t>;
			
			template<typename T> concept String_c = std::same_as<T, std::string> || std::same_as<T, std::wstring>;

			inline int StrContains(const String_c auto& str1, const String_c auto& str2, const std::locale& loc = std::locale())
			{
				auto it = std::search(
					str1.begin(), str1.end(),
					str2.begin(), str2.end(),
					[&loc](auto c1, auto c2) { return std::tolower(c1, loc) == std::tolower(c2, loc); }
				);

				if (it != str1.end()) return (it - str1.begin());
				else return -1;
			}

			inline int FindCharInString(String_c auto & in, std::size_t keyword) { return (keyword != std::string::npos) ? keyword : -1; }
			
			// trim from start (in place)
			inline void ltrim(String_c auto& s) {
				s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
					return !std::isspace(ch);
					}));
			}

			// trim from end (in place)
			inline void rtrim(String_c auto& s) {
				s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
					return !std::isspace(ch);
					}).base(), s.end());
			}

			// trim from both ends (in place)
			inline void trim(String_c auto& s) {
				ltrim(s);
				rtrim(s);
			}

			// trim from start (copying)
			inline auto ltrim_copy(String_c auto s) {
				ltrim(s);
				return s;
			}

			// trim from end (copying)
			inline auto rtrim_copy(String_c auto s) {
				rtrim(s);
				return s;
			}

			// trim from both ends (copying)
			inline auto trim_copy(String_c auto s) {
				trim(s);
				return s;
			}

		}
	}
}