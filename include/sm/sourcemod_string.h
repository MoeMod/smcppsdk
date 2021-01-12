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
			template<Char_c charT>
			struct iequal {
				iequal(const std::locale& loc) : _loc(loc) {}
				bool operator()(charT c1, charT c2) { return std::tolower(c1, _loc) == std::tolower(c2, _loc); }
			private:
				const std::locale& _loc;
			};
			
			template<typename T> concept String_c = std::same_as<T, std::string> || std::same_as<T, std::wstring>;
			template<String_c T>
			int StrContains(const T& str1, const T& str2, const std::locale& loc = std::locale())
			{
				typename T::const_iterator it = std::search(
					str1.begin(), str1.end(),
					str2.begin(), str2.end(),
					iequal<typename T::value_type>(loc)
				);

				if (it != str1.end()) return (it - str1.begin());
				else return -1;
			}

			template<String_c T>
			int FindCharInString(T& in, std::size_t keyword) { return (keyword != std::string::npos) ? keyword : -1; }
			
			// trim from start (in place)
			template<String_c T>
			static inline void ltrim(T& s) {
				s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
					return !std::isspace(ch);
					}));
			}

			// trim from end (in place)
			template<String_c T>
			static inline void rtrim(T& s) {
				s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
					return !std::isspace(ch);
					}).base(), s.end());
			}

			// trim from both ends (in place)
			template<String_c T>
			static inline void trim(T& s) {
				ltrim(s);
				rtrim(s);
			}

			// trim from start (copying)
			template<String_c T>
			static inline std::string ltrim_copy(T s) {
				ltrim(s);
				return s;
			}

			// trim from end (copying)
			template<String_c T>
			static inline std::string rtrim_copy(T s) {
				rtrim(s);
				return s;
			}

			// trim from both ends (copying)
			template<String_c T>
			static inline std::string trim_copy(T s) {
				trim(s);
				return s;
			}

		}
	}
}