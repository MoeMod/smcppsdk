#pragma once

#include <utility>
#include <tuple>
#include <concepts>
#include <iserverunknown.h>
#include <iservernetworkable.h>
#include <IPlayerHelpers.h>

namespace sm {
    inline namespace convert {
        // forward decl
        template<class From, class To> struct ConvertFunc;
        template<class To> struct Converter;
        template<class TargetType> struct AutoEntity;
    	template<class TargetType>
        constexpr auto ent_cast = [](const auto& x) { return Converter<typename std::decay<TargetType>::type>()(x); };
        template<class T>
        constexpr auto make_auto_ent = [](const auto& x) { return AutoEntity<T>(x); };

    	// impl
        namespace detail {
        	template<class From, class...Dest> struct AdapterFunc;
        	template<class From> struct AdapterFunc<From> : std::identity {};
            template<class From, class To> struct AdapterFunc<From, To> : ConvertFunc<From, To> {};
            template<class From, class Mid, class...To> struct AdapterFunc<From, Mid, To...>
            {
	            auto operator()(From in) const
	            {
		            return AdapterFunc<Mid, To...>()(AdapterFunc<From, Mid>()(in));
	            };
            };

            template<class From, class To, class = void> struct ConvertFuncSingle;
            template<class T> struct ConvertFuncSingle<T, T> : std::identity {};
            template<> struct ConvertFuncSingle<int, CBaseEntity*> { CBaseEntity* operator()(int id) const { return gamehelpers->ReferenceToEntity(gamehelpers->IndexToReference(id)); } };
            template<> struct ConvertFuncSingle<CBaseHandle, CBaseEntity*> { CBaseEntity* operator()(const CBaseHandle& handle) const { return gamehelpers->ReferenceToEntity(handle.ToInt() | (1 << 31));; } };
            template<> struct ConvertFuncSingle<IServerUnknown*, CBaseEntity*> { CBaseEntity* operator()(IServerUnknown* unknown) const { return unknown->GetBaseEntity(); } };
            template<> struct ConvertFuncSingle<int, edict_t*> { edict_t* operator()(int id) const { return gamehelpers->EdictOfIndex(id); } };
            template<> struct ConvertFuncSingle<IGamePlayer*, edict_t*> { edict_t* operator()(IGamePlayer* igp) const { return igp->GetEdict(); } };
            template<> struct ConvertFuncSingle<IServerUnknown*, edict_t*> { edict_t* operator()(IServerUnknown* unknown) const { return unknown->GetNetworkable()->GetEdict(); } };
            template<> struct ConvertFuncSingle<edict_t*, int> { int operator()(edict_t* edict) const { return gamehelpers->IndexOfEdict(edict); } };
            template<> struct ConvertFuncSingle<CBaseEntity*, int> { int operator()(CBaseEntity* pEntity) const { return gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference(pEntity)); } };
            template<> struct ConvertFuncSingle<IGamePlayer*, int> { int operator()(IGamePlayer* igp) const { return igp->GetIndex(); } };
            template<> struct ConvertFuncSingle<CBaseHandle, int> { int operator()(const CBaseHandle& handle) const { return gamehelpers->ReferenceToIndex(handle.ToInt() | (1 << 31)); } };
            template<> struct ConvertFuncSingle<edict_t*, IGamePlayer*> { IGamePlayer* operator()(edict_t* edict) const { return playerhelpers->GetGamePlayer(edict); } };
            template<> struct ConvertFuncSingle<int, IGamePlayer*> { IGamePlayer* operator()(int id) const { return playerhelpers->GetGamePlayer(id); } };
            template<> struct ConvertFuncSingle<edict_t*, IServerUnknown*> { IServerUnknown* operator()(edict_t* edict) const { return edict->GetUnknown(); } };
            template<> struct ConvertFuncSingle<CBaseEntity*, IServerUnknown*> { IServerUnknown* operator()(CBaseEntity* pEntity) const { return reinterpret_cast<IServerUnknown*>(pEntity); } };
            template<> struct ConvertFuncSingle<int, CBaseHandle> { CBaseHandle operator()(int id) const { return CBaseHandle(gamehelpers->IndexToReference(id) & ~(1 << 31)); } };
            template<> struct ConvertFuncSingle<IServerUnknown*, CBaseHandle> { const CBaseHandle& operator()(IServerUnknown* unknown) const { return unknown->GetRefEHandle(); } };

            // use adapters
            template<> struct ConvertFuncSingle<edict_t*, CBaseEntity*> : AdapterFunc<edict_t*, IServerUnknown*, CBaseEntity*> {};
            template<> struct ConvertFuncSingle<IGamePlayer*, CBaseEntity*> : AdapterFunc<IGamePlayer*, edict_t*, CBaseEntity*> {};
            template<> struct ConvertFuncSingle<CBaseEntity*, edict_t*> : AdapterFunc<CBaseEntity*, IServerUnknown*, edict_t*> {};
            template<> struct ConvertFuncSingle<CBaseHandle, edict_t*> : AdapterFunc<CBaseHandle, CBaseEntity*, edict_t*> {};
            template<> struct ConvertFuncSingle<IServerUnknown*, int> : AdapterFunc<IServerUnknown*, edict_t*, int> {};
            template<> struct ConvertFuncSingle<CBaseEntity*, IGamePlayer*> : AdapterFunc<CBaseEntity*, IServerUnknown*, edict_t*, IGamePlayer*> {};
            template<> struct ConvertFuncSingle<CBaseHandle, IGamePlayer*> : AdapterFunc<CBaseHandle, int, IGamePlayer*> {};
            template<> struct ConvertFuncSingle<IServerUnknown*, IGamePlayer*> : AdapterFunc<IServerUnknown*, edict_t*, IGamePlayer*> {};
            template<> struct ConvertFuncSingle<int, IServerUnknown*> : AdapterFunc<int, CBaseEntity*, IServerUnknown*> {};
            template<> struct ConvertFuncSingle<IGamePlayer*, IServerUnknown*> : AdapterFunc<IGamePlayer*, CBaseEntity*, IServerUnknown*> {};
            template<> struct ConvertFuncSingle<CBaseHandle, IServerUnknown*> : AdapterFunc<CBaseHandle, CBaseEntity*, IServerUnknown*> {};
            template<> struct ConvertFuncSingle<edict_t*, CBaseHandle> : AdapterFunc<edict_t*, IServerUnknown*, CBaseHandle> {};
            template<> struct ConvertFuncSingle<CBaseEntity*, CBaseHandle> : AdapterFunc<CBaseEntity*, IServerUnknown*, CBaseHandle> {};
            template<> struct ConvertFuncSingle<IGamePlayer*, CBaseHandle> : AdapterFunc<IGamePlayer*, int, CBaseHandle> {};
        	
            // covariant
            template<class CBaseEntitySubClass> struct ConvertFuncSingle<CBaseEntitySubClass*, CBaseEntity*, typename std::enable_if<std::is_base_of<CBaseEntity, CBaseEntitySubClass>::value && !std::is_same<CBaseEntity, CBaseEntitySubClass>::value>::type>
            {
                CBaseEntity* operator()(CBaseEntitySubClass* in) const {
                    return static_cast<CBaseEntity*>(in);
                }
            };
            template<class CBaseEntitySubClass> struct ConvertFuncSingle<CBaseEntity*, CBaseEntitySubClass*, typename std::enable_if<std::is_base_of<CBaseEntity, CBaseEntitySubClass>::value && !std::is_same<CBaseEntity, CBaseEntitySubClass>::value>::type>
            {
                CBaseEntitySubClass* operator()(CBaseEntity* in) const {
                    return static_cast<CBaseEntitySubClass*>(in); // down-cast!!! no checked.
                }
            };
            template<class From, class CBaseEntitySubClass> struct ConvertFuncSingle<From, CBaseEntitySubClass*, typename std::enable_if<std::is_base_of<CBaseEntity, CBaseEntitySubClass>::value && !std::is_same<CBaseEntity, CBaseEntitySubClass>::value && !std::is_same<CBaseEntity*, From>::value>::type>
                : AdapterFunc<From, CBaseEntity *, CBaseEntitySubClass *> {};
            template<class CBaseEntitySubClass, class To> struct ConvertFuncSingle<CBaseEntitySubClass*, To, typename std::enable_if<std::is_base_of<CBaseEntity, CBaseEntitySubClass>::value && !std::is_same<CBaseEntity, CBaseEntitySubClass>::value && !std::is_same<CBaseEntity *, To>::value>::type>
                : AdapterFunc<CBaseEntitySubClass *, CBaseEntity*, To> {};

        	// auto type
        	template<class From, class To> struct ConvertFuncSingle<AutoEntity<From>, To> : ConvertFuncSingle<From, To> {};
        	template<class From, class To> struct ConvertFuncSingle<From, AutoEntity<To>> : ConvertFuncSingle<From, To> {};
        	template<class From, class To> struct ConvertFuncSingle<AutoEntity<From>, AutoEntity<To>> : ConvertFuncSingle<From, To> {};
        	
            template<class FromList, class To> struct ConverterGenerator;
            template<class To, template<class...> class TypeList, class...Froms> struct ConverterGenerator<TypeList<Froms...>, To> : ConvertFunc<Froms, To>... {};
        }

        template<class From, class To> struct ConvertFunc : detail::ConvertFuncSingle<From, To> {};
        template<class To> struct Converter
        {
            template<class InputType> To operator()(InputType&&in) const {
                return ConvertFunc<typename std::decay<InputType>::type, To>()(std::forward<InputType>(in));
            }
        };

#if SMCPP_STATIC_UNIT_TEST
        using AllEntTypeList = std::tuple<edict_t*, CBaseEntity*, int, IGamePlayer*, CBaseHandle, IServerUnknown*>;
    	template<class From, class To> struct ConverterUnitTest3 : std::is_invocable_r<To, ConvertFunc<From, To>, From> {};
        template<class FromList, class To> struct ConverterUnitTest2;
        template<class To, template<class...> class TypeList, class...Froms> struct ConverterUnitTest2<TypeList<Froms...>, To> : std::conjunction<ConverterUnitTest3<Froms, To>...> {};
        template<class EntTypeList> struct ConverterUnitTest1;
        template<template<class...> class TypeList, class...T> struct ConverterUnitTest1<TypeList<T...>> : std::conjunction<ConverterUnitTest2<TypeList<T...>, T>...> {};
        static_assert(ConverterUnitTest1<AllEntTypeList>::value);
#endif

        template<class TargetType> struct AutoEntity
        {
            template<class InputType> AutoEntity(InputType&& in) : value(Converter<TargetType>()(std::forward<InputType>(in))) {}
            operator TargetType() const { return value; }
            template<class Ret = TargetType> decltype(&*std::declval<Ret>()) operator->() { return &*value; }
            TargetType value;
        };
        template<class TargetType> struct AutoEntity<AutoEntity<TargetType>> : AutoEntity<TargetType> {};

        inline namespace [[deprecated("Try using ent_cast() instead of this namespace's functions.")]] deprecated {

            inline CBaseEntity *edict2cbase(edict_t *edict) {
                return ent_cast<CBaseEntity *>(edict);
            }

            inline edict_t *cbase2edict(CBaseEntity *pEntity) {
                return ent_cast<edict_t *>(pEntity);
            }

            inline edict_t *id2edict(int id) {
                return ent_cast<edict_t *>(id);
            }

            inline int edict2id(edict_t *edict) {
                return ent_cast<int>(edict);
            }

            inline CBaseEntity *id2cbase(int id) {
                return ent_cast<CBaseEntity *>(id);
            }

            inline int cbase2id(CBaseEntity *pEntity) {
                return ent_cast<int>(pEntity);
            }

            inline CBaseEntity *handle2ent(const CBaseHandle &handle) {
                return ent_cast<CBaseEntity *>(handle);
            }

            inline CBaseEntity *CBaseEntityFrom(int client) {
                return ent_cast<CBaseEntity *>(client);
            }

            inline IGamePlayer *IGamePlayerFrom(int client) {
                return ent_cast<IGamePlayer *>(client);
            }

            inline IGamePlayer *IGamePlayerFrom(edict_t *edict) {
                return ent_cast<IGamePlayer *>(edict);
            }
        }
    }
}
