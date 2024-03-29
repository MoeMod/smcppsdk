#pragma once


#include "util/EventDispatcher.hpp"

#include "hack_takedamageinfo.h"
#include "sourcemod_types.h"

#include "hook_result.h"

#include <typeindex>

namespace sm {
    namespace sdkhooks {
        using hack::TakeDamageInfo;
        namespace detail {
            struct HookTagBase {};
            template<class FuncType> struct HookTag;
            template<class Ret, class...Args> struct HookTag<HookResult<Ret>(Args...)> : HookTagBase
            {
                using DelegateType = EventDispatcher<HookResult<Ret>(Args...)>;
                using ReturnType = Ret;
            };
            template<class...Args> struct HookTag<void(Args...)> : HookTagBase
            {
                using DelegateType = EventDispatcher<void(Args...)>;
                using ReturnType = void;
            };
            template<class Tag>
            inline auto GetHookDelegateSingleton() -> typename Tag::DelegateType&
            {
                static typename Tag::DelegateType x;
                return x;
            }

            bool AddGlobalListenerKeeper(EventListener listener);
            bool RemoveGlobalListenerKeeper(EventListener listener);
            void Hook(CBaseEntity* pEnt, const std::type_index& WrappedHookTagType);
            void UnHook(CBaseEntity* pEnt, const std::type_index& WrappedHookTagType);

            bool SDK_OnLoad(char* error, size_t maxlength, bool late);
            void SDK_OnUnload();
        }

        constexpr struct : detail::HookTag<Action(CBaseEntity*, CBaseEntity*)> {} SDKHook_EndTouch;
        constexpr struct : detail::HookTag<void(CBaseEntity*, const FireBulletsInfo_t&)> {} SDKHook_FireBulletsPost;
        constexpr struct : detail::HookTag<HookResult<int>(CBaseEntity*, TakeDamageInfo&)> {} SDKHook_OnTakeDamage;
        constexpr struct : detail::HookTag<void(CBaseEntity*, TakeDamageInfo&)> {} SDKHook_OnTakeDamagePost;
        constexpr struct : detail::HookTag<Action(CBaseEntity*)> {} SDKHook_PreThink;
        constexpr struct : detail::HookTag<Action(CBaseEntity*)> {} SDKHook_PostThink;
        constexpr struct : detail::HookTag<Action(CBaseEntity*, CCheckTransmitInfo*, bool)> {} SDKHook_SetTransmit;
        constexpr struct : detail::HookTag<Action(CBaseEntity*)> {} SDKHook_Spawn;
        constexpr struct : detail::HookTag<Action(CBaseEntity*, CBaseEntity*)> {} SDKHook_StartTouch;
        constexpr struct : detail::HookTag<Action(CBaseEntity*)> {} SDKHook_Think;
        constexpr struct : detail::HookTag<Action(CBaseEntity*, CBaseEntity*)> {} SDKHook_Touch;
        constexpr struct : detail::HookTag<Action(CBaseEntity*, TakeDamageInfo&, const Vector&, trace_t*)> {} SDKHook_TraceAttack;
        constexpr struct : detail::HookTag<void(CBaseEntity*, TakeDamageInfo&, const Vector&, trace_t*)> {} SDKHook_TraceAttackPost;
        constexpr struct : detail::HookTag<HookResult<bool>(CBaseEntity*, CBaseCombatWeapon*)> {} SDKHook_WeaponCanSwitchTo;
        constexpr struct : detail::HookTag<HookResult<bool>(CBaseEntity*, CBaseCombatWeapon*)> {} SDKHook_WeaponCanUse;
        constexpr struct : detail::HookTag<Action(CBaseEntity*, CBaseCombatWeapon*, const Vector*, const Vector*)> {} SDKHook_WeaponDrop;
        constexpr struct : detail::HookTag<Action(CBaseEntity*, CBaseCombatWeapon*)> {} SDKHook_WeaponEquip;
        constexpr struct : detail::HookTag<HookResult<bool>(CBaseEntity*, CBaseCombatWeapon*, int)> {} SDKHook_WeaponSwitch;
        constexpr struct : detail::HookTag<HookResult<bool>(CBaseEntity*, int, int)> {} SDKHook_ShouldCollide;
        constexpr struct : detail::HookTag<void(CBaseEntity*)> {} SDKHook_PreThinkPost;
        constexpr struct : detail::HookTag<void(CBaseEntity*)> {} SDKHook_PostThinkPost;
        constexpr struct : detail::HookTag<void(CBaseEntity*)> {} SDKHook_ThinkPost;
        constexpr struct : detail::HookTag<void(CBaseEntity*, CBaseEntity*)> {} SDKHook_EndTouchPost;
        constexpr struct : detail::HookTag<void(CBaseEntity*, void*)> {} SDKHook_GroundEntChangedPost;
        constexpr struct : detail::HookTag<void(CBaseEntity*)> {} SDKHook_SpawnPost;
        constexpr struct : detail::HookTag<void(CBaseEntity*, CBaseEntity*)> {} SDKHook_StartTouchPost;
        constexpr struct : detail::HookTag<void(CBaseEntity*, CBaseEntity*)> {} SDKHook_TouchPost;
        constexpr struct : detail::HookTag<Action(CBaseEntity*, IPhysicsObject*)> {} SDKHook_VPhysicsUpdate;
        constexpr struct : detail::HookTag<void(CBaseEntity*, IPhysicsObject*)> {} SDKHook_VPhysicsUpdatePost;
        constexpr struct : detail::HookTag<void(CBaseEntity*, CBaseCombatWeapon*)> {} SDKHook_WeaponCanSwitchToPost;
        constexpr struct : detail::HookTag<void(CBaseEntity*, CBaseCombatWeapon*)> {} SDKHook_WeaponCanUsePost;
        constexpr struct : detail::HookTag<void(CBaseEntity*, CBaseCombatWeapon*, const Vector*, const Vector*)> {} SDKHook_WeaponDropPost;
        constexpr struct : detail::HookTag<void(CBaseEntity*, CBaseCombatWeapon*)> {} SDKHook_WeaponEquipPost;
        constexpr struct : detail::HookTag<void(CBaseEntity*, CBaseCombatWeapon*, int)> {} SDKHook_WeaponSwitchPost;
        constexpr struct : detail::HookTag<Action(CBaseEntity*, CBaseEntity*, CBaseEntity*, USE_TYPE, float)> {} SDKHook_Use;
        constexpr struct : detail::HookTag<void(CBaseEntity*, CBaseEntity*, CBaseEntity*, USE_TYPE, float)> {} SDKHook_UsePost;
        constexpr struct : detail::HookTag<HookResult<bool>(CBaseEntity*)> {} SDKHook_Reload;
        constexpr struct : detail::HookTag<void(CBaseEntity*)> {} SDKHook_ReloadPost;
        constexpr struct : detail::HookTag<HookResult<int>(CBaseEntity*)> {} SDKHook_GetMaxHealth;
        constexpr struct : detail::HookTag<Action(CBaseEntity*, CBaseEntity*)> {} SDKHook_Blocked;
        constexpr struct : detail::HookTag<void(CBaseEntity*, CBaseEntity*)> {} SDKHook_BlockedPost;
        constexpr struct : detail::HookTag<HookResult<int>(CBaseEntity*, TakeDamageInfo&)> {} SDKHook_OnTakeDamage_Alive;
        constexpr struct : detail::HookTag<void(CBaseEntity*, TakeDamageInfo&)> {} SDKHook_OnTakeDamage_AlivePost;
        constexpr struct : detail::HookTag<HookResult<bool>(CBaseEntity*)> {} SDKHook_CanBeAutobalanced;

        // 注意：返回值必须保存到变量，否则事件会直接销毁
        template<class Tag, class...CallBackArgs>
        [[nodiscard]] EventListener SDKHookRAII(CBaseEntity* pEnt, Tag type, CallBackArgs &&...callback)
        {
            auto listener = detail::GetHookDelegateSingleton<Tag>().subscribe(std::forward<CallBackArgs>(callback)...);
            detail::Hook(pEnt, typeid(Tag));
            return listener;
        }

        inline void SDKUnhookRAII(EventListener& eventListener)
        {
            eventListener = nullptr;
        }

        void SDKHooks_TakeDamage(CBaseEntity* pVictim, const TakeDamageInfo& info);
        void SDKHooks_DropWeapon(CBaseEntity* pPlayer, CBaseCombatWeapon* pWeapon, const Vector* pvecTarget, const Vector* pVelocity);

    }

}