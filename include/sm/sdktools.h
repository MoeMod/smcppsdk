// convert all SM original API into this repo, if possible

#pragma once

#include <extensions/IBinTools.h>
#include <smsdk_ext.h>
#include "sourcemod_types.h"
#include "variant_t.h"
#include "call_helper.h"
#include <networkstringtabledefs.h>
#include <stdexcept>

#if SOURCE_ENGINE >= SE_ORANGEBOX
#include <toolframework/itoolentity.h>
#endif

namespace sm {
    namespace sdktools {
        namespace detail {
            bool SDK_OnLoad(char* error, size_t maxlength, bool late);
            void SDK_OnUnload();
            bool SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late);
        }

        int FindOffset(const char* name);
        extern IBinTools* g_pBinTools;
        extern IGameConfig* g_pGameConf;
        extern INetworkStringTableContainer* netstringtables;
        extern IServerTools* servertools;
        
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
#pragma region sdktools_stringtables
        inline bool LockStringTables(bool lock) {
            return engine->LockNetworkStringTables(lock) ? true : false;
        }

        inline int FindStringTable(std::string _Table) 
        {
            INetworkStringTable* pTable = netstringtables->FindTable(_Table.c_str());

            if (!pTable)
            {
                return INVALID_STRING_TABLE;
            }

            return pTable->GetTableId();
        }

        inline bool AddToStringTable(TABLEID idx, std::string _AddString, std::string userdata = "", int length = -1) {
            INetworkStringTable* pTable = netstringtables->GetTable(idx);
            if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);

#if SOURCE_ENGINE >= SE_ORANGEBOX
            pTable->AddString(true, _AddString.c_str(), length, userdata.c_str());
#else
            pTable->AddString(true, length, userdata.c_str());
#endif
            return true;
        }

        inline int GetNumStringTables() { return netstringtables->GetNumTables(); }

        inline int GetStringTableNumStrings(TABLEID idx) {
            INetworkStringTable* pTable = netstringtables->GetTable(idx);
            if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);
            return pTable->GetNumStrings();
        }

        inline int GetStringTableMaxStrings(TABLEID idx) {
            INetworkStringTable* pTable = netstringtables->GetTable(idx);
            if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);
            return pTable->GetMaxStrings();
        }

        inline void GetStringTableName(TABLEID idx) {
            INetworkStringTable* pTable = netstringtables->GetTable(idx);
            if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);
            // HELP: Gets a suitable return.
            /*size_t numBytes;

            if (!pTable)
            {
                return pContext->ThrowNativeError("Invalid string table index %d", idx);
            }

            pContext->StringToLocalUTF8(params[2], params[3], pTable->GetTableName(), &numBytes);

            return numBytes;*/
            //return (std::string() + pTable->GetTableName());
        }

        inline int FindStringIndex(TABLEID idx, std::string _Str) {
            INetworkStringTable* pTable = netstringtables->GetTable(idx);
            if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);
            int strIndex = pTable->FindStringIndex(_Str.c_str());
            return (strIndex == INVALID_STRING_INDEX) ? -1 : strIndex;
        }

        inline int ReadStringTable(TABLEID idx, int stringIdx)
        {
            INetworkStringTable* pTable = netstringtables->GetTable(idx);
            if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);
            std::string value = (std::string() + *pTable->GetString(stringIdx));
            if (!value.size()) smutils->LogError(myself, "Invalid string index specified for table (index: %d) (table: \"%s\")", stringIdx, pTable->GetTableName());
            // HELP: NumBytes
            /*
            pContext->StringToLocalUTF8(params[3], params[4], value, &numBytes);

            return numBytes;
            */
        }
        //native int GetStringTableDataLength(int tableidx, int stringidx);
        inline int GetStringTableDataLength(TABLEID idx, int stringIdx)
        {
            INetworkStringTable* pTable = netstringtables->GetTable(idx);
            if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);
            if (stringIdx < 0 || stringIdx >= pTable->GetNumStrings())
                smutils->LogError(myself, "Invalid string index specified for table (index %d) (table \"%s\")", stringIdx, pTable->GetTableName());
            int datalen;
            const void* userdata = pTable->GetStringUserData(stringIdx, &datalen);
            return (!userdata) ? 0 : datalen;
        }

        // HELP: There are some problems in this function.
        // optimizate is required.
        inline std::size_t GetStringTableData(TABLEID idx, int stringIdx, int maxlength) {
            INetworkStringTable* pTable = netstringtables->GetTable(idx);
            if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);
            if (stringIdx < 0 || stringIdx >= pTable->GetNumStrings())
                smutils->LogError(myself, "Invalid string index specified for table (index %d) (table \"%s\")", stringIdx, pTable->GetTableName());
            int datalen = 0;
            std::string userdata = (std::string() + reinterpret_cast<const char*>(pTable->GetStringUserData(stringIdx, &datalen)));
            std::size_t numBytes = std::min(maxlength, datalen);
            void* addr = nullptr;
            if (userdata.c_str()) std::memcpy(addr, userdata.c_str(), numBytes);
            if (maxlength > 0) {
                //reinterpret_cast<const char*>(addr[0]) = '\0';
                numBytes = 0;
            }
            return numBytes;
            
        }

        // HELP ME
        inline void SetStringTableData(TABLEID idx) {
            INetworkStringTable* pTable = netstringtables->GetTable(idx);
            if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);
        }

        inline void AddFileToDownloadsTable(std::string _FileName) {
            static int table = INVALID_STRING_TABLE;
            if (table == INVALID_STRING_TABLE) {
                table = FindStringTable((std::string() + "downloadables"));
            }
            bool save = LockStringTables(false);
            AddToStringTable(table, _FileName);
            LockStringTables(save);
        }
#pragma endregion

#pragma region sdktools_functions
        //CreateEntityByName(const char[] classname, int ForceEdictIndex=-1);
        inline CBaseEntity* CreateEntityByName(std::string classname, int forceEdictIndex = -1)
        {
            if (!g_pSM->IsMapRunning()) throw std::runtime_error("CANNOT create entity when no map is running!");
#if SOURCE_ENGINE >= SE_ORANGEBOX
#if SOURCE_ENGINE != SE_CSGO
            CBaseEntity* pEntity = (CBaseEntity*)servertools->CreateEntityByName(classname.c_str());
#else // SE==CSGO
            CBaseEntity* pEntity = (CBaseEntity*)servertools->CreateItemEntityByName(classname.c_str());
            if (!pEntity)
            {
                pEntity = (CBaseEntity*)servertools->CreateEntityByName(classname.c_str());
            }
#endif //endif::SE!=CSGO
            return pEntity;
#else // SE< ORANGEBOX
            throw std::runtime_error("This function is incomplete. DO NOT USE IT!");
            return nullptr;
#endif // endif::SE>=SE_ORANGEBOX
        }
        
        // native bool DispatchKeyValue(int entity, const char[] keyName, const char[] value);
        template<class T>
        inline bool DispatchKeyValue(CBaseEntity* pEntity, std::string keyName, T Value)
        {
            if (!pEntity) throw std::runtime_error("Called entity is invalid.");
            return (servertools->SetKeyValue(pEntity, keyName.c_str(), Value) ? true : false);
            
            // TODO: When SE_VERSION < ORANGEBOX: VFuncCaller
        }

        //native bool DispatchSpawn(int entity);
        inline void DispatchSpawn(CBaseEntity* pEntity)
        {
            if (!pEntity) throw std::runtime_error("The entity what you want to spawn is invalid.");

            servertools->DispatchSpawn(pEntity);

            // TODO: When SE_VERSION < ORANGEBOX: VFuncCaller
        }


#pragma endregion
    }
}