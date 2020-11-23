#pragma once

#include <extensions/IBinTools.h>
#include <smsdk_ext.h>
#include "sourcemod_types.h"
#include "variant_t.h"
#include "call_helper.h"

namespace sm {
    namespace sdktools {
        namespace detail {
            bool SDK_OnLoad(char* error, size_t maxlength, bool late);
            void SDK_OnUnload();
        }

        int FindOffset(const char* name);
        extern IBinTools* g_pBinTools;
        extern IGameConfig* g_pGameConf;
    	
        inline void RemovePlayerItem(CBasePlayer * player, CBaseCombatWeapon *pItem)
        {
            static VFuncCaller<void(CBasePlayer::*)(CBaseCombatWeapon*)> caller(g_pBinTools, FindOffset("RemovePlayerItem"));
            return caller(player, pItem);
        }
    	
        inline CBaseEntity *GivePlayerItem(CBasePlayer *player, const char *item, int iSubType = 0)
        {
            static VFuncCaller<CBaseEntity* (CBasePlayer::*)(const char*, int, CEconItemView*, bool, CBaseEntity*)> caller(g_pBinTools, FindOffset("GiveNamedItem"));
            return caller(player, item, iSubType, nullptr, true, nullptr);
        }
    	
        inline void SetLightStyle(int style, const char *value)
        {
            return engine->LightStyle(style, value);
        }
    	
        inline int GivePlayerAmmo(CBasePlayer * player, int amount, int ammotype, bool suppressSound=false)
        {
            static VFuncCaller<int(CBasePlayer::*)(int, int, bool)> caller(g_pBinTools, FindOffset("GivePlayerAmmo"));
            return caller(player, amount, ammotype, suppressSound);
        }
    	
        inline void SetEntityModel(CBaseEntity *entity, const char *model)
        {
            static VFuncCaller<void(CBaseEntity::*)(const char*)> caller(g_pBinTools, FindOffset("SetEntityModel"));
            return caller(entity, model);
        }
    	
        inline bool AcceptEntityInput(CBaseEntity * dest, const char *input, CBaseEntity *activator=nullptr, CBaseEntity *pcaller=nullptr, int outputid=0)
        {
            variant_t value{};
            value.eVal = INVALID_EHANDLE_INDEX;
            value.fieldType = FIELD_VOID;

            static VFuncCaller<bool(CBaseEntity::*)(const char*, CBaseEntity*, CBaseEntity*, variant_t, int)> caller(g_pBinTools, FindOffset("AcceptInput"));
            return caller(dest, input, activator, pcaller, value, outputid);
        }
    	
        inline void ForcePlayerSuicide(CBasePlayer * player, bool bExplode = false, bool bForce = false)
        {
            static VFuncCaller<void(CBasePlayer::*)(bool, bool)> caller(g_pBinTools, FindOffset("CommitSuicide"));
            return caller(player, bExplode, bForce);
        }
    	
        inline void EquipPlayerWeapon(CBasePlayer * player, CBaseEntity *entity)
        {
            static VFuncCaller<void(CBasePlayer::*)(CBaseEntity*)> caller(g_pBinTools, FindOffset("WeaponEquip"));
            return caller(player, entity);
        }

        inline void TeleportEntity(CBaseEntity* pEntity, Vector newpos, Vector newang, Vector newVel)
        {
            static VFuncCaller<void(CBaseEntity::*)(Vector, Vector, Vector)> caller(g_pBinTools, FindOffset("Teleport"));
            return caller(pEntity, newpos, newang, newVel);
        }
    }
}