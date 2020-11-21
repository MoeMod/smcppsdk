#pragma once

#include "smsdk_ext.h"
#include <../core/logic/ShareSys.h>

namespace sm {
	namespace hack {
		namespace detail {
			using NativeCache = NameHashSet<ke::RefPtr<Native>, Native>;
			constexpr auto GetNativeCacheMemPtr()->NativeCache ShareSystem::*;
			template<NativeCache ShareSystem::* Ptr> struct hack_m_NtvCache
			{
				friend constexpr auto GetNativeCacheMemPtr()->NativeCache ShareSystem::*
				{
					return Ptr;
				}
			};
			template struct hack_m_NtvCache<&ShareSystem::m_NtvCache>; // hack private member m_NtvCache

			inline ke::RefPtr<Native> FindNative(ShareSystem *pthis, const char* name)
			{
				auto& m_NtvCache = pthis->*GetNativeCacheMemPtr();
				auto r = m_NtvCache.find(name);
				if (!r.found())
					return nullptr;
				return *r;
			}
		}

		inline SPVM_NATIVE_FUNC FindGlobalNative(const char* name)
		{
			ShareSystem* derived = static_cast<ShareSystem*>(sharesys);
			return detail::FindNative(derived, name)->native->func;
		}
	}
}
