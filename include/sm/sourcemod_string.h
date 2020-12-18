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
			template<typename T> concept char_c = std::same_as<T, char> || std::same_as<T, wchar_t>;
			template<char_c charT>
			struct iequal {
				iequal(const std::locale& loc) : _loc(loc) {}
				bool operator()(charT c1, charT c2) { return std::tolower(c1, _loc) == std::tolower(c2, _loc); }
			private:
				const std::locale& _loc;
			};
			
			// but this passes std::string...
			template<typename T>
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
		}
	}
}