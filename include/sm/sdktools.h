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

#pragma region sdktools_client
        //InactivateClient
        //ReconnectClient
#pragma endregion

#pragma region sdktools_engine
        //GetClientEyePosition
        //SetClientViewEntity
        inline void SetLightStyle(int style, const char* value)
        {
            return engine->LightStyle(style, value);
        }
#pragma endregion

#pragma region sdktools_entinput
        inline bool AcceptEntityInput(CBaseEntity* dest, const char* input, CBaseEntity* activator = nullptr, CBaseEntity* pcaller = nullptr, int outputid = 0)
        {
            variant_t value{};
            value.eVal = INVALID_EHANDLE_INDEX;
            value.fieldType = FIELD_VOID;

            static VFuncCaller<bool(CBaseEntity::*)(const char*, CBaseEntity*, CBaseEntity*, variant_t, int)> caller(g_pBinTools, FindOffset("AcceptInput"));
            return caller(dest, input, activator, pcaller, value, outputid);
        }
#pragma endregion

#pragma region sdktools_entoutput
        //FireEntityOutput
        //HookEntityOutput
        //HookSingleEntityOutput
        //UnhookEntityOutput
        //UnhookSingleEntityOutput
#pragma endregion

#pragma region sdktools_functions
        inline void RemovePlayerItem(CBasePlayer* player, CBaseCombatWeapon* pItem)
        {
            static VFuncCaller<void(CBasePlayer::*)(CBaseCombatWeapon*)> caller(g_pBinTools, FindOffset("RemovePlayerItem"));
            return caller(player, pItem);
        }
        inline CBaseEntity* GivePlayerItem(CBasePlayer* player, const char* item, int iSubType = 0)
        {
            static VFuncCaller<CBaseEntity* (CBasePlayer::*)(const char*, int, CEconItemView*, bool, CBaseEntity*)> caller(g_pBinTools, FindOffset("GiveNamedItem"));
            return caller(player, item, iSubType, nullptr, true, nullptr);
        }
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
        template<class T>
        inline bool DispatchKeyValue(CBaseEntity* pEntity, std::string keyName, T Value)
        {
            if (!pEntity) throw std::runtime_error("Called entity is invalid.");
            return (servertools->SetKeyValue(pEntity, keyName.c_str(), Value) ? true : false);

            // TODO: When SE_VERSION < ORANGEBOX: VFuncCaller
        }
        inline void DispatchSpawn(CBaseEntity* pEntity)
        {
            if (!pEntity) throw std::runtime_error("The entity what you want to spawn is invalid.");

            servertools->DispatchSpawn(pEntity);

            // TODO: When SE_VERSION < ORANGEBOX: VFuncCaller
        }
        inline void TeleportEntity(CBaseEntity* pEntity, Vector newpos, Vector newang, Vector newVel)
        {
            static VFuncCaller<void(CBaseEntity::*)(Vector, Vector, Vector)> caller(g_pBinTools, FindOffset("Teleport"));
            return caller(pEntity, newpos, newang, newVel);
        }
        inline void ActivateEntity(CBaseEntity* entity)
        {
            static VFuncCaller<void(CBaseEntity::*)()> caller(g_pBinTools, FindOffset("Activate"));
            return caller(entity);
        }
        inline void SetEntityModel(CBaseEntity* entity, const char* model)
        {
            static VFuncCaller<void(CBaseEntity::*)(const char*)> caller(g_pBinTools, FindOffset("SetEntityModel"));
            return caller(entity, model);
        }
        inline void EquipPlayerWeapon(CBasePlayer* player, CBaseEntity* entity)
        {
            static VFuncCaller<void(CBasePlayer::*)(CBaseEntity*)> caller(g_pBinTools, FindOffset("WeaponEquip"));
            return caller(player, entity);
        }
        inline void ForcePlayerSuicide(CBasePlayer* player, bool bExplode = false, bool bForce = false)
        {
            static VFuncCaller<void(CBasePlayer::*)(bool, bool)> caller(g_pBinTools, FindOffset("CommitSuicide"));
            return caller(player, bExplode, bForce);
        }
        inline int GivePlayerAmmo(CBasePlayer* player, int amount, int ammotype, bool suppressSound = false)
        {
            static VFuncCaller<int(CBasePlayer::*)(int, int, bool)> caller(g_pBinTools, FindOffset("GivePlayerAmmo"));
            return caller(player, amount, ammotype, suppressSound);
        }
        //    GetPlayerWeaponSlot
        //    IgniteEntity
        //    ExtinguishEntity
        //    SlapPlayer
        //    FindEntityByClassname
        //    GetClientEyeAngles
        //    GetClientAimTarget
        //    GetTeamCount
        //    GetTeamName
        //    GetTeamScore
        //    SetTeamScore
        //    GetTeamClientCount
        //    GetTeamEntity
        //    GetPlayerDecalFile
        //    GetPlayerJingleFile
        //    GetServerNetStats
        //    SetClientInfo
        //    SetClientName
#pragma endregion
#pragma region sdktools_gamerules
        //GameRules_GetProp
        //    GameRules_SetProp
        //    GameRules_GetPropFloat
        //    GameRules_SetPropFloat
        //    GameRules_GetPropEnt
        //    GameRules_SetPropEnt
        //    GameRules_GetPropVector
        //    GameRules_SetPropVector
        //    GameRules_GetPropString
        //    GameRules_SetPropString
        //    GameRules_GetRoundState
#pragma endregion

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
#pragma region sdktools_varient_t
        inline void SetVariant(bool Val)
        {
            variant_t* v{};

            v->bVal = Val;
            v->fieldType = FIELD_BOOLEAN;
        }
        inline void SetVariant(int Val)
        {
            variant_t* v{};
            v->iVal = Val;
            v->fieldType = FIELD_INTEGER;
        }
        inline void SetVariant(std::string str)
        {
            variant_t* v{};
            v->iszVal = MAKE_STRING(str.c_str());
            v->fieldType = FIELD_STRING;
        }
        inline void SetVariant(float val)
        {
            variant_t* v{};
            v->flVal = val;
            v->fieldType = FIELD_FLOAT;
        }
        // true=PosVec, false = vec
        inline void SetVariant(Vector vec, bool IsPos = false)
        {
            variant_t* v{};
            v->vecVal[0] = vec.x;
            v->vecVal[1] = vec.y;
            v->vecVal[2] = vec.z;
            v->fieldType = IsPos ? FIELD_POSITION_VECTOR : FIELD_VECTOR;
            

        }
        inline void SetVariant(Color color)
        {
            variant_t* v{};
            v->rgbaVal.r = color.r();
            v->rgbaVal.g = color.g();
            v->rgbaVal.b = color.b();
            v->rgbaVal.a = color.a();

            v->fieldType = FIELD_COLOR32;
        }
        inline void SetVariant(CBaseEntity* entity)
        {
            variant_t* v{};
            CBaseHandle handle;
            handle = reinterpret_cast<IHandleEntity*>(entity)->GetRefEHandle();
            v->eVal = (unsigned long)(handle.ToInt());
            v->fieldType = FIELD_EHANDLE;
        }
#pragma endregion
    }
}