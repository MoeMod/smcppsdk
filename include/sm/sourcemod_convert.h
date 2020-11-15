#pragma once

#include <iserverunknown.h>
#include <iservernetworkable.h>
#include <IPlayerHelpers.h>

namespace sm {
    inline namespace convert {
        namespace detail {
            template<class To, class = void> struct Converter
            {
                To operator()(edict_t *edict) const = delete;
                To operator()(CBaseEntity *pEntity) const = delete;
                To operator()(int id) const = delete;
                To operator()(IGamePlayer *id) const = delete;
                To operator()(const CBaseHandle &id) const = delete;
                To operator()(IServerUnknown *id) const = delete;
            };

            template<> struct Converter<CBaseEntity *, void>
            {
                CBaseEntity *operator()(edict_t *edict) const { return (*this)(edict->GetUnknown()); }
                CBaseEntity *operator()(CBaseEntity *pEntity) const { return pEntity; }
                CBaseEntity *operator()(int id) const { return gamehelpers->ReferenceToEntity(gamehelpers->IndexToReference(id)); }
                CBaseEntity *operator()(IGamePlayer *igp) const { return (*this)(igp->GetEdict()); } // not direct
                CBaseEntity *operator()(const CBaseHandle &handle) const { return gamehelpers->ReferenceToEntity(handle.ToInt() | (1<<31));; }
                CBaseEntity *operator()(IServerUnknown *unknown) const { return unknown->GetBaseEntity(); }
            };

            template<> struct Converter<edict_t *, void>
            {
                edict_t *operator()(edict_t *edict) const { return edict; }
                edict_t *operator()(CBaseEntity *pEntity) const { return (*this)(reinterpret_cast<IServerUnknown *>(pEntity)); }
                edict_t *operator()(int id) const { return gamehelpers->EdictOfIndex(id); }
                edict_t *operator()(IGamePlayer *igp) const { return igp->GetEdict(); }
                edict_t *operator()(const CBaseHandle &handle) const { return (*this)(Converter<CBaseEntity *>()(handle)); }
                edict_t *operator()(IServerUnknown *unknown) const { return unknown->GetNetworkable()->GetEdict(); }
            };

            template<> struct Converter<int, void>
            {
                int operator()(edict_t *edict) const { return gamehelpers->IndexOfEdict(edict); }
                int operator()(CBaseEntity *pEntity) const { return gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference(pEntity)); }
                int operator()(int id) const { return id; }
                int operator()(IGamePlayer *igp) const { return igp->GetIndex(); }
                int operator()(const CBaseHandle &handle) const { return gamehelpers->ReferenceToIndex(handle.ToInt() | (1<<31)); }
                int operator()(IServerUnknown *unknown) const { return (*this)(Converter<edict_t *>()(unknown)); }
            };

            template<> struct Converter<IGamePlayer *, void>
            {
                IGamePlayer *operator()(edict_t *edict) const { return playerhelpers->GetGamePlayer(edict); }
                IGamePlayer *operator()(CBaseEntity *pEntity) const { return (*this)(Converter<edict_t *>()(pEntity)); }
                IGamePlayer *operator()(int id) const { return playerhelpers->GetGamePlayer(id); }
                IGamePlayer *operator()(IGamePlayer *igp) const { return igp; }
                IGamePlayer *operator()(const CBaseHandle &handle) const { return (*this)(Converter<int>()(handle)); }
                IGamePlayer *operator()(IServerUnknown *unknown) const { return (*this)(Converter<edict_t *>()(unknown)); }
            };

            template<> struct Converter<IServerUnknown *, void>
            {
                IServerUnknown *operator()(edict_t *edict) const { return edict->GetUnknown(); }
                IServerUnknown *operator()(CBaseEntity *pEntity) const { return reinterpret_cast<IServerUnknown *>(pEntity); }
                IServerUnknown *operator()(int id) const { return (*this)(Converter<CBaseEntity *>()(id)); }
                IServerUnknown *operator()(IGamePlayer *igp) const { return (*this)(Converter<CBaseEntity *>()(igp)); }
                IServerUnknown *operator()(const CBaseHandle &handle) const { return (*this)(Converter<CBaseEntity *>()(handle)); }
            };

            template<> struct Converter<CBaseHandle, void>
            {
                const CBaseHandle &operator()(edict_t *edict) const { return (*this)(edict->GetUnknown()); }
                const CBaseHandle &operator()(CBaseEntity *pEntity) const { return (*this)(Converter<IServerUnknown *>()(pEntity)); }
                CBaseHandle operator()(int id) const { return CBaseHandle(gamehelpers->IndexToReference(id) & ~(1<<31)); }
                CBaseHandle operator()(IGamePlayer *igp) const { return (*this)(Converter<int>()(igp)); }
                const CBaseHandle &operator()(const CBaseHandle &handle) const { return handle; }
                const CBaseHandle &operator()(IServerUnknown *unknown) const { return unknown->GetRefEHandle(); }
            };

            template<class CBaseEntitySubClass> struct Converter<CBaseEntitySubClass *, typename std::enable_if<std::is_base_of<CBaseEntity, CBaseEntitySubClass>::value>::type> : Converter<CBaseEntity *>
            {
                template<class T> CBaseEntitySubClass *operator()(T &&x) const {
                    // TODO : safe check?
                    return static_cast<CBaseEntitySubClass *>(static_cast<const Converter<CBaseEntity *> &>(*this)(std::forward<T>(x)));
                }
            };
        }

        using detail::Converter;

        template<class TargetType, class InputType> auto ent_cast(InputType &&in) -> decltype(Converter<typename std::decay<TargetType>::type>()(in))
        {
            return Converter<typename std::decay<TargetType>::type>()(in);
        }

        template<class TargetType> struct AutoEntity
        {
            template<class InputType> AutoEntity(InputType &&in) : value(Converter<typename std::decay<TargetType>::type>()(std::forward<InputType>(in))) {}
            operator TargetType() const { return value; }
            TargetType value;
        };

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
