// convert all SM original API into this repo, if possible

#pragma once

#include <extensions/IBinTools.h>
#include <extensions/ISDKTools.h>
#include <smsdk_ext.h>
#include "sourcemod_types.h"
#include "variant_t.h"
#include "call_helper.h"
#include <networkstringtabledefs.h>
#include <stdexcept>
#include <cstring>
#include <string_view>
#include "sourcemod_convert.h"
#if SOURCE_ENGINE >= SE_ORANGEBOX
#include <toolframework/itoolentity.h>
#endif
#include <iserver.h>
#include "impl/sdktools_impl_global.hpp"
#include "impl/sdktools_impl.hpp"

namespace sm {
    namespace sdktools {
        namespace sdktools_client {
            inline void InactivateClient(IGamePlayer* player)
            {
                if (!player) smutils->LogError(myself, "Invalid client index: %d", player->GetIndex());

                if (!iserver) throw std::runtime_error("[InactivateClient] IServer interface is not supported. DO NOT USE THIS FUNCTION!");

                IClient* pClient = iserver->GetClient(player->GetIndex() - 1);
                if (pClient) pClient->Inactivate();
                else smutils->LogError(myself, "Cannot get IClient for client %d", player->GetIndex());
            }
            inline void ReconnectClient(IGamePlayer* player)
            {
                if (!player) smutils->LogError(myself, "Invalid client index: %d", player->GetIndex());

                if (!iserver) throw std::runtime_error("[ReconnectClient] IServer interface is not supported. DO NOT USE THIS FUNCTION!");
                IClient* pClient = iserver->GetClient(player->GetIndex() - 1);
                if (pClient) pClient->Reconnect();
                else smutils->LogError(myself, "Cannot get IClient for client %d", player->GetIndex());
            }
        }

        namespace sdktools_engine {
            inline Vector GetClientEyePosition(IGamePlayer* player)
            {
                if (player && player->IsInGame())
                {
                    Vector pos;
                    serverClients->ClientEarPosition(player->GetEdict(), &pos);
                    return pos;
                }
            }
            inline void SetClientViewEntity(IGamePlayer* player, edict_t* edict)
            {
                if (!player) smutils->LogError(myself, "Invalid client index: %d", player->GetIndex());
                if (!player->IsInGame()) smutils->LogError(myself, "Client index: %d is not in game", player->GetIndex());
                if (!edict || edict->IsFree()) smutils->LogError(myself, "Entity %d is invalid.", sm::ent_cast<int>(edict));
                engine->SetView(player->GetEdict(), edict);
            }

            inline void SetLightStyle(int style, const char* value)
            {
                return engine->LightStyle(style, value);
            }
        }

        namespace sdktools_entinput {
            inline bool AcceptEntityInput(CBaseEntity* dest, const char* input, CBaseEntity* activator = nullptr, CBaseEntity* pcaller = nullptr, int outputid = 0)
            {
                variant_t v = g_Variant_t;
                static VFuncCaller<bool(CBaseEntity::*)(const char*, CBaseEntity*, CBaseEntity*, variant_t, int)> caller(g_pBinTools, FindOffset("AcceptInput"));
                return caller(dest, input, activator, pcaller, v, outputid);
            }
        }

        namespace sdktools_entoutput {
            namespace detail {
                void* FindOutputPointerByName(CBaseEntity* pEntity, const char* outputname)
                {
                    datamap_t* pMap = gamehelpers->GetDataMap(pEntity);
                    while (pMap)
                    {
                        for (int i = 0; i < pMap->dataNumFields; i++)
                        {
                            if (pMap->dataDesc[i].flags & FTYPEDESC_OUTPUT)
                            {
                                if (!strcmp(pMap->dataDesc[i].externalName, outputname))
                                {
                                    return reinterpret_cast<void*>((unsigned char*)pEntity + GetTypeDescOffs(&pMap->dataDesc[i]));
                                }
                            }
                        }
                        pMap = pMap->baseMap;
                    }

                    return nullptr;
                }
            }
            // _ZN17CBaseEntityOutput10FireOutputE9variant_tP11CBaseEntityS2_f
            // CBaseEntityOutput::FireOutput(variant_t, CBaseEntity*, CBaseEntity*, float)
            // => FireEntityOutput(CBaseEntity* pActivator, CBaseEntity* pCaller, float delay)
            //void FireEntityOutput(int caller, const char[] output, int activator, float delay)
            inline void FireEntityOutput(CBaseEntity* pCaller, const char* output, CBaseEntity* pActivator, float flDelay = 0.0)
            {
                variant_t v = g_Variant_t;
                // void -> CBaseEntityOutput
                CBaseEntityOutput* pOutput = (CBaseEntityOutput*)detail::FindOutputPointerByName(pCaller, output);
                if (!pOutput) return;
                static MemFuncCaller<void(CBaseEntityOutput::*)(variant_t, CBaseEntity*, CBaseEntity*, float)> caller(g_pBinTools, FindSig("FireOutput"));
                return caller(pOutput, v, pActivator, pCaller, flDelay);
            }
            inline void HookEntityOutput(const char* classname, const char* output, void* callback) = delete;
            inline void HookSingleEntityOutput(CBaseEntity* entity, const char* output, void* callback, bool once = false) = delete;
            inline bool UnhookEntityOutput(const char* classname, const char* output, void* callback) = delete;
            inline void UnhookSingleEntityOutput(CBaseEntity* entity, const char* output, void* callback, bool once = false) = delete;
        }

        namespace sdktools_functions {
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
#if SOURCE_ENGINE >= SE_ORANGEBOX
            inline CBaseEntity* CreateEntityByName(const char* classname)
            {
                if (!g_pSM->IsMapRunning()) throw std::runtime_error("CANNOT create new entity when no map is running!");
#if SOURCE_ENGINE != SE_CSGO
                CBaseEntity* pEntity = (CBaseEntity*)servertools->CreateEntityByName(classname);
#else
                CBaseEntity* pEntity = (CBaseEntity*)servertools->CreateItemEntityByName(classname);

                if (!pEntity)
                {
                    pEntity = (CBaseEntity*)servertools->CreateEntityByName(classname);
                }
#endif
                return pEntity;
            }
#else
            inline CBaseEntity* CreateEntityByName(const char* classname, int forceEdictIndex = -1)
            {
                if (!g_pSM->IsMapRunning()) throw std::runtime_error("CANNOT create new entity when no map is running!");
            }
#endif
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
            inline void TeleportEntity(CBaseEntity* pEntity, Vector newpos, QAngle newang, Vector newVel)
            {
                static VFuncCaller<void(CBaseEntity::*)(Vector, QAngle, Vector)> caller(g_pBinTools, FindOffset("Teleport"));
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
            inline CBaseEntity* GetPlayerWeaponSlot(CBasePlayer* player, int slot)
            {
                static VFuncCaller<CBaseEntity* (CBasePlayer::*)(int)> caller(g_pBinTools, FindOffset("Weapon_GetSlot"));
                return caller(player, slot);
            }
            inline void IgniteEntity(CBaseEntity* entity, float time, bool npc = false, float size = 0.f, bool level = false)
            {

            }
            inline void ExtinguishEntity(CBaseEntity* entity)
            {

            }
            inline void SlapPlayer(CBasePlayer* client, int health = 5, bool sound = true)
            {

            }
            inline CBaseEntity* FindEntityByClassname(CBaseEntity* startEnt, const char* classname)
            {
                return nullptr;
            }
            inline Vector GetClientEyeAngles(CBasePlayer* player)
            {
                Vector vec;
                return vec;
            }
            inline auto GetClientAimTarget(CBasePlayer* client, bool only_clients = true)
            {
                CBasePlayer* player = nullptr;
                return player;
            }
            inline int GetTeamCount()
            {

            }
            inline const char* GetTeamName()
            {
                return nullptr;
            }
            inline int GetTeamScore()
            {
                return 0;
            }
            inline void SetTeamScore()
            {

            }
            inline int GetTeamClientCount()
            {
                return 0;
            }
            inline CBaseEntity* GetTeamEntity()
            {
                return nullptr;
            }
            inline const char* GetPlayerDecalFile(CBasePlayer* player)
            {
                return nullptr;
            }
            inline const char* GetPlayerJingleFile(CBasePlayer* player)
            {
                return nullptr;
            }
            inline void GetServerNetStats()
            {

            }
            inline void SetClientInfo()
            {

            }
            inline void SetClientName()
            {

            }
        }

        namespace sdktools_gamerules {
            namespace detail {
                inline void* GameRules()
                {
                    return g_pSDKTools->GetGameRules();
                }

                CBaseEntity* FindEntityByNetClass(int start, const char* classname)
                {
                    int maxEntities = gpGlobals->maxEntities;
                    for (int i = start; i < maxEntities; i++)
                    {
                        edict_t* current = ent_cast<edict_t*>(i);
                        if (!current || current->IsFree()) continue;

                        IServerNetworkable* pNet = current->GetNetworkable();
                        if (!pNet) continue;
                        
                        if (!pNet->GetEntityHandle()) continue;

                        if (!strcmp(pNet->GetServerClass()->GetName(), classname))
                            return gamehelpers->ReferenceToEntity(ent_cast<cell_t>(current));
                    }
                    return nullptr;
                }

                CBaseEntity* GetGameRulesProxyEnt()
                {
                    static cell_t proxyEntRef = -1;
                    CBaseEntity* pProxy;
                    if (proxyEntRef == -1 || (pProxy = gamehelpers->ReferenceToEntity(proxyEntRef)) == nullptr)
                    {
                        pProxy = FindEntityByNetClass(playerhelpers->GetMaxClients(), g_szGameRulesProxy);
                        if (pProxy) proxyEntRef = gamehelpers->EntityToReference(pProxy);
                    }
                    
                    return pProxy;
                }

                template<class T = cell_t>
                T& GameRulesProp(const char* prop, int size = sizeof(T), int element = 0) {
                    void* pGameRules = GameRules();
                    assert(pGameRules != nullptr);
                    sm_sendprop_info_t info = {};
                    if (!gamehelpers->FindSendPropInfo(g_szGameRulesProxy, prop, &info)) 
                        throw std::runtime_error(std::string() + "Prop not found in the gamerules: " + prop);
                    SendProp* pProp = info.prop;
                    ptrdiff_t offset = info.actual_offset;
                    T* data = (T*)(reinterpret_cast<intptr_t>(static_cast<CBaseEntity*>(pGameRules)) + offset);
                    return *data;
                }
            }

            template<class T = cell_t>
            const T& GameRulesGetProp(const char* prop, int size = sizeof(T), int element = 0) {
                return detail::GameRulesProp<T>(prop, size, element);
            }

            template<class T = cell_t>
            T& GameRulesSetProp(const char* prop, const T& value, int size = sizeof(T), int element = 0) {
                const char* sValue = g_pSM->GetCoreConfigValue("FollowCSGOServerGuidelines");
                if (sValue && !strcasecmp(sValue, "no")) {
                    throw std::runtime_error("You must set \"FollowCSGOServerGuidelines\" true to process this function.");
                }
                return detail::GameRulesProp<T>(prop, size, element) = value;
            }

            template<EntType_c T = CBaseHandle>
            T GameRulesGetPropEnt(const char* prop, int element = 0) {
                CBaseHandle ent = detail::GameRulesProp<CBaseHandle>(prop, sizeof(T), element);
                return ent_cast<T>(ent);
            }

            template<EntType_c T = CBaseHandle>
            void GameRulesSetPropEnt(const char* prop, const T& other, int element = 0) {
                CBaseHandle& ent = detail::GameRulesProp<CBaseHandle>(prop, sizeof(T), element);
                CBaseEntity* ent_set = ent_cast<CBaseEntity*>(other);
                IHandleEntity* ent_set2 = (IHandleEntity*)ent_set;
                ent.Set(ent_set2);
            }

            inline RoundState GetRoundState() {
                return static_cast<RoundState>(GameRulesGetProp<int>("m_iRoundState"));
            }
        }

        namespace sdktools_stringtables {
            inline bool LockStringTables(bool lock) {
                return engine->LockNetworkStringTables(lock) ? true : false;
            }

            inline int FindStringTable(const char* _Table)
            {
                INetworkStringTable* pTable = netstringtables->FindTable(_Table);

                if (!pTable)
                {
                    return INVALID_STRING_TABLE;
                }

                return pTable->GetTableId();
            }

            inline bool AddToStringTable(TABLEID idx, const char* _AddString, const char* userdata = "", int length = -1) {
                INetworkStringTable* pTable = netstringtables->GetTable(idx);
                if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);

#if SOURCE_ENGINE >= SE_ORANGEBOX
                pTable->AddString(true, _AddString, length, userdata);
#else
                pTable->AddString(true, length, userdata);
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

            // native int GetStringTableName(int tableidx, char[] name, int maxlength);
            inline const char* GetStringTableNameSz(TABLEID idx) {
                INetworkStringTable* pTable = netstringtables->GetTable(idx);
                if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);
                return pTable->GetTableName();
            }
            inline std::size_t GetStringTableName(TABLEID idx, char* name = nullptr, std::size_t maxlength = 0) {
                const char* src = GetStringTableNameSz(idx);
                auto size = std::strlen(src);
                std::size_t numBytes = maxlength ? std::min<std::size_t>(size, maxlength) : size;
                if (name) std::strncpy(name, src, numBytes);
                return numBytes;
            }

            inline int FindStringIndex(TABLEID idx, std::string _Str) {
                INetworkStringTable* pTable = netstringtables->GetTable(idx);
                if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);
                int strIndex = pTable->FindStringIndex(_Str.c_str());
                return (strIndex == INVALID_STRING_INDEX) ? -1 : strIndex;
            }

            // native int ReadStringTable(int tableidx, int stringidx, char[] str, int maxlength);
            inline const char* ReadStringTableSz(TABLEID idx, int stringIdx)
            {
                INetworkStringTable* pTable = netstringtables->GetTable(idx);
                if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);
                return pTable->GetString(stringIdx);
            }
            inline std::string ReadStringTableStr(TABLEID idx, int stringIdx)
            {
                return ReadStringTableSz(idx, stringIdx);
            }
            inline std::string_view ReadStringTableSv(TABLEID idx, int stringIdx)
            {
                const char* str = ReadStringTableSz(idx, stringIdx);
                return std::string_view(str, std::strlen(str));
            }
            inline std::size_t ReadStringTable(TABLEID idx, int stringIdx, char* str = nullptr, std::size_t maxlength = 0)
            {
                std::string_view sv = ReadStringTableSv(idx, stringIdx);
                const char* src = sv.data();
                auto size = sv.size();
                auto numBytes = maxlength ? std::min<std::size_t>(size, maxlength) : size;
                if (str) std::strncpy(str, src, numBytes);
                return numBytes;
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

            // native int GetStringTableData(int tableidx, int stringidx, char[] userdata, int maxlength);
            inline std::string_view GetStringTableDataSv(TABLEID idx, int stringIdx)
            {
                INetworkStringTable* pTable = netstringtables->GetTable(idx);
                if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);
                if (stringIdx < 0 || stringIdx >= pTable->GetNumStrings())
                    smutils->LogError(myself, "Invalid string index specified for table (index %d) (table \"%s\")", stringIdx, pTable->GetTableName());
                int datalen = 0;
                const char* userdata = reinterpret_cast<const char*>(pTable->GetStringUserData(stringIdx, &datalen));
                return std::string_view(userdata, datalen);
            }
            inline std::size_t GetStringTableData(TABLEID idx, int stringIdx, char* userdata = nullptr, std::size_t maxlength = 0) {
                std::string_view sv = GetStringTableDataSv(idx, stringIdx);
                const char* src = sv.data();
                auto size = sv.size();
                auto numBytes = maxlength ? std::min<std::size_t>(size, maxlength) : size;
                if (userdata) std::strncpy(userdata, src, numBytes);
                return numBytes;
            }
            inline std::string GetStringTableDataStr(TABLEID idx, int stringIdx)
            {
                return std::string(GetStringTableDataSv(idx, stringIdx));
            }

            // native int SetStringTableData(int tableidx, int stringidx, const char[] userdata, int length);
            inline void SetStringTableData(TABLEID idx, int stringIdx, const char* userdata, int length) {
                INetworkStringTable* pTable = netstringtables->GetTable(idx);
                if (!pTable) smutils->LogError(myself, "Invalid string table index: %d", idx);
                pTable->SetStringUserData(stringIdx, length, userdata);
            }

            inline void AddFileToDownloadsTable(const char* _FileName) {
                static int table = INVALID_STRING_TABLE;
                if (table == INVALID_STRING_TABLE) {
                    table = FindStringTable("downloadables");
                }
                bool save = LockStringTables(false);
                AddToStringTable(table, _FileName);
                LockStringTables(save);
            }
        }

        namespace sdktools_variant_t {
            inline void SetVariantBool(bool Val)
            {
                variant_t v{};
                v.bVal = Val;
                v.fieldType = FIELD_BOOLEAN;
                g_Variant_t = v;
            }
            inline void SetVariantInt(int Val)
            {
                variant_t v{};

                v.iVal = Val;
                v.fieldType = FIELD_INTEGER;
                g_Variant_t = v;
            }
            inline void SetVariantString(const char* buffer)
            {
                variant_t v{};
                v.iszVal = MAKE_STRING(buffer);
                v.fieldType = FIELD_STRING;
                g_Variant_t = v;
            }
            inline void SetVariantFloat(float val)
            {
                variant_t v{};
                v.flVal = val;
                v.fieldType = FIELD_FLOAT;
                g_Variant_t = v;
            }
            // true=PosVec, false = vec
            inline void SetVariantVector3D(Vector vec, bool IsPos = false)
            {
                variant_t v{};
                v.vecVal[0] = vec.x;
                v.vecVal[1] = vec.y;
                v.vecVal[2] = vec.z;
                v.fieldType = IsPos ? FIELD_POSITION_VECTOR : FIELD_VECTOR;
                g_Variant_t = v;
            }
            inline void SetVariantColor(Color color)
            {
                variant_t v{ .rgbaVal = color.ToColor32() };
                v.fieldType = FIELD_COLOR32;
                g_Variant_t = v;
            }
            inline void SetVariantEntity(CBaseEntity* entity)
            {
                variant_t v{};
                CBaseHandle handle;
                handle = reinterpret_cast<IHandleEntity*>(entity)->GetRefEHandle();

                v.eVal = (unsigned long)(handle.ToInt());
                v.fieldType = FIELD_EHANDLE;
                g_Variant_t = v;
            }
        }

        namespace sdktools_misc {
            inline int FindTeamByName(const char* name)
            {

            }
        }
        
        namespace sdktools_sound {
            inline void PrefetchSound(const char* name)
            {

            }
            [[deprecated("Does not work, may crash")]]
            inline float GetSoundDuration(const char* name)
            {

            }
            inline void EmitAmbientSound()
            {

            }
            inline void FadeClientVolume()
            {

            }
            inline void StopSound()
            {

            }
            inline void EmitSound()
            {

            }
            inline void EmitSoundEntry()
            {

            }
            inline void EmitSentence()
            {

            }
            inline void GetDistGainFromSoundLevel()
            {

            }
        }
        
        using namespace sdktools_client;
        using namespace sdktools_engine;
        using namespace sdktools_entinput;
        using namespace sdktools_entoutput;
        using namespace sdktools_functions;
        using namespace sdktools_gamerules;
        using namespace sdktools_stringtables;
        using namespace sdktools_variant_t;
        using namespace sdktools_misc;
        using namespace sdktools_sound;
    }
}