#pragma once

#include <extensions/IBinTools.h>
#include <smsdk_ext.h>
#include "sourcemod_types.h"
#include <variant>

namespace sm {
	inline namespace variant
	{
	    class variant_t
	    {
	    public:
	        // float[3] = 12U
	        // CBaseHandle
	        union
	        {
	            bool bVal;
	            string_t iszVal;
	            int iVal;
	            float flVal;
	            float vecVal[3];
	            color32 rgbaVal;
	        };

	        CBaseHandle eVal; // unsigned long
	        fieldtype_t fieldType;
	    };

	#ifdef _WIN32
	    static_assert(sizeof(variant_t) == 20);
	#endif

	    using std_variant_t = std::variant<bool, string_t, int, float, Vector, Color, CBaseHandle>;

		namespace detail {
	        template<class F, class V> struct make_common_ret_type;
	        template<class F, class...Args> struct make_common_ret_type<F, std::variant<Args...>> {
	            using type = typename std::common_type<typename std::invoke_result<F, Args>::type...>::type;
	        };
	        template<class...Args> struct join_func : Args... {};
	        template<class...Args> join_func(Args...args)->join_func<Args...>; // CTAD
		}

		template<class F, class R = typename detail::make_common_ret_type<F, std_variant_t>::type>
	    inline R Visit(F&& func, const variant_t &v)
	    {
	        std_variant_t res;
		    switch(v.fieldType)
		    {
	        case FIELD_BOOLEAN:
	            return func(v.bVal);
	        case FIELD_INTEGER:
	            return func(v.iVal);
	        case FIELD_STRING:
	            return func(v.iszVal);
	        case FIELD_FLOAT:
	            return func(v.flVal);
	        case FIELD_VECTOR:
	        case FIELD_POSITION_VECTOR:
	            return func(Vector(v.vecVal[0], v.vecVal[1], v.vecVal[2]));
	        case FIELD_COLOR32:
	            return func(Color(v.rgbaVal.r, v.rgbaVal.g, v.rgbaVal.b, v.rgbaVal.a));
	        case FIELD_EHANDLE:
	            return func(v.eVal);
	        default:
	            throw std::bad_variant_access();
		    }
	    }

	    inline std_variant_t ToStdVariant(const variant_t& v)
		{
	        return Visit([](const auto& x) { return std_variant_t(std::in_place_type<typename std::decay<decltype(x)>::type>, x); }, v);
		}
		
		inline variant_t FromStdVariant(const std_variant_t &sv)
		{
	        return std::visit(detail::join_func(
	            [](bool x) { return variant_t{ .bVal = x, .fieldType = FIELD_BOOLEAN }; },
	            [](int x) { return variant_t{ .iVal = x, .fieldType = FIELD_INTEGER }; },
	            [](string_t x) { return variant_t{ .iszVal = x, .fieldType = FIELD_STRING }; },
	            [](float x) { return variant_t{ .flVal = x, .fieldType = FIELD_FLOAT }; },
	            [](Vector x) { return variant_t{ .vecVal = { x.x, x.y, x.z }, .fieldType = FIELD_VECTOR }; },
	            [](Color x) { return variant_t{ .rgbaVal = x.ToColor32(), .fieldType = FIELD_COLOR32 }; },
	            [](const CBaseHandle &x) { return variant_t{ .eVal = x, .fieldType = FIELD_EHANDLE }; }
	            ), sv);
		}
    }
}