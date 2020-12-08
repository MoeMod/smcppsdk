#pragma once

#include <shared/shareddefs.h>
#include <engine/IEngineSound.h>
#include <memory>
#include <string>
#include <stdexcept>

namespace sm{
    inline namespace sourcemod {
        inline namespace halflife {

            extern IEngineSound* enginesound;
            void LogToGame(const char* format, ...);
#pragma endregion
#pragma region random
            void SetRandomSeed(int seed);
            float GetRandomFloat(float fMin = 0.0f, float fMax = 1.0f);
            int GetRandomInt(int nmin, int nmax);
#pragma endregion
#pragma region filesystem
            enum FindMapResult
            {
                // A direct match for this name was found
                FindMap_Found,
                // No match for this map name could be found.
                FindMap_NotFound,
                // A fuzzy match for this map name was found.
                // Ex: cp_dust -> cp_dustbowl, c1m1 -> c1m1_hotel
                // Only supported for maps that the engine knows about. (This excludes workshop maps on Orangebox).
                FindMap_FuzzyMatch,
                // A non-canonical match for this map name was found.
                // Ex: workshop/1234 -> workshop/cp_qualified_name.ugc1234
                // Only supported on "Orangebox" games with workshop support.
                FindMap_NonCanonical,
                // No currently available match for this map name could be found, but it may be possible to load
                // Only supported on "Orangebox" games with workshop support.
                FindMap_PossiblyAvailable
            };

            inline bool IsMapValid(const char *map)
            {
                return gamehelpers->IsMapValid(map);
            }

            FindMapResult FindMap(const char *map, char *foundmap, int maxlen);
            const char* GetMapDisplayNameSz(const char* map);

            inline bool GetMapDisplayName(const char *map, char *displayName, int maxlen)
            {
                strncpy(displayName, GetMapDisplayNameSz(map), maxlen);
                return strlen(displayName);
            }

            inline bool IsDedicatedServer()
            {
                return engine->IsDedicatedServer();
            }

            float GetEngineTime();

            inline float GetGameTime()
            {
                return gpGlobals->curtime;
            }

            inline int GetGameTickCount()
            {
                return gpGlobals->tickcount;
            }

            inline float GetGameFrameTime()
            {
                return gpGlobals->frametime;
            }

            inline const char* GetGameFolderNameSz()
            {
                return g_pSM->GetGameFolderName();
            }

            inline int GetGameFolderName(char* buffer, int maxlength)
            {
                strncpy(buffer, GetGameFolderNameSz(), maxlength);
                return strlen(buffer);
            }

            inline const char* GetCurrentMapSz()
            {
                return STRING(gpGlobals->mapname);
            }

            inline int GetCurrentMap(char* buffer, int maxlength)
            {
                strncpy(buffer, GetCurrentMapSz(), maxlength);
                return strlen(buffer);
            }
#pragma endregion
#pragma region precache
            inline int PrecacheModel(const char* model, bool preload = false)
            {
                return engine->PrecacheModel(model, preload);
            }

            inline int PrecacheSentenceFile(const char* file, bool preload = false)
            {
                return engine->PrecacheSentenceFile(file, preload);
            }

            inline int PrecacheDecal(const char* decal, bool preload = false)
            {
                return engine->PrecacheDecal(decal, preload);
            }

            inline int PrecacheGeneric(const char* generic, bool preload = false)
            {
                return engine->PrecacheGeneric(generic, preload);
            }

            inline bool IsModelPrecached(const char* model)
            {
                return engine->IsModelPrecached(model);
            }

            inline bool IsDecalPrecached(const char* decal)
            {
                return engine->IsDecalPrecached(decal);
            }

            inline bool IsGenericPrecached(const char* generic)
            {
                return engine->IsGenericPrecached(generic);
            }

            inline bool PrecacheSound(const char* sound, bool preload = false)
            {
                return enginesound->PrecacheSound(sound, preload);
            }

            [[deprecated]] inline bool IsSoundPrecached(const char* sound)
            {
                return enginesound->IsSoundPrecached(sound);
            }

#pragma endregion
#pragma region dialog
            enum DialogType
            {
                DialogType_Msg = 0,     /**< just an on screen message */
                DialogType_Menu,        /**< an options menu */
                DialogType_Text,        /**< a richtext dialog */
                DialogType_Entry,       /**< an entry box */
                DialogType_AskConnect   /**< ask the client to connect to a specified IP */
            };
            // fuck winapi
#undef CreateDialog
            void CreateDialog(int client, KeyValues* kv, DialogType type);
#pragma endregion
#pragma region engineversion
            enum EngineVersion
            {
                Engine_Unknown,             /**< Could not determine the engine version */
                Engine_Original,            /**< Original Source Engine (used by The Ship) */
                Engine_SourceSDK2006,       /**< Episode 1 Source Engine (second major SDK) */
                Engine_SourceSDK2007,       /**< Orange Box Source Engine (third major SDK) */
                Engine_Left4Dead,           /**< Left 4 Dead */
                Engine_DarkMessiah,         /**< Dark Messiah Multiplayer (based on original engine) */
                Engine_Left4Dead2 = 7,      /**< Left 4 Dead 2 */
                Engine_AlienSwarm,          /**< Alien Swarm (and Alien Swarm SDK) */
                Engine_BloodyGoodTime,      /**< Bloody Good Time */
                Engine_EYE,                 /**< E.Y.E Divine Cybermancy */
                Engine_Portal2,             /**< Portal 2 */
                Engine_CSGO,                /**< Counter-Strike: Global Offensive */
                Engine_CSS,                 /**< Counter-Strike: Source */
                Engine_DOTA,                /**< Dota 2 */
                Engine_HL2DM,               /**< Half-Life 2 Deathmatch */
                Engine_DODS,                /**< Day of Defeat: Source */
                Engine_TF2,                 /**< Team Fortress 2 */
                Engine_NuclearDawn,         /**< Nuclear Dawn */
                Engine_SDK2013,             /**< Source SDK 2013 */
                Engine_Blade,               /**< Blade Symphony */
                Engine_Insurgency,          /**< Insurgency (2013 Retail version)*/
                Engine_Contagion,           /**< Contagion */
                Engine_BlackMesa,           /**< Black Mesa Multiplayer */
                Engine_DOI                  /**< Day of Infamy */
            };
            inline int GuessSDKVersion() = delete;
            EngineVersion GetEngineVersion();

#pragma endregion
#pragma region print
            inline bool PrintToChat(int id, const char* buffer) {
                    return gamehelpers->TextMsg(id, HUD_PRINTTALK, buffer);
            }
            inline bool PrintCenterText(int id, const char* buffer) {
                    return gamehelpers->TextMsg(id, HUD_PRINTCENTER, buffer);
            }
            inline bool PrintHintText(int id, const char* buffer) {
                    return gamehelpers->HintTextMsg(id, buffer);
            }

#define PRINT_FN(FUNC_PREFIX) \
            inline bool FUNC_PREFIX##Str(int id, std::string str) { \
                return FUNC_PREFIX(id, str.c_str()); \
            } \
            inline void FUNC_PREFIX##All(const char* buffer) { \
                for (int i = 1; i <= playerhelpers->GetMaxClients(); i++) { \
                    if (playerhelpers->GetGamePlayer(i)->IsInGame()) { \
                        FUNC_PREFIX(i, buffer); \
                    } \
                } \
            } \
            inline void FUNC_PREFIX##AllStr(std::string buffer) { \
                return FUNC_PREFIX##All(buffer.c_str()); \
            }
            PRINT_FN(PrintToChat)
            PRINT_FN(PrintCenterText)
            PRINT_FN(PrintHintText)
#undef PRINT_FN
#pragma endregion
#pragma region vguipanel
            void ShowVGUIPanel(int client, const char* name, KeyValues* Kv = nullptr, bool show = true);
            enum MOTDPANEL_TYPE {
                MOTDPANEL_TYPE_TEXT =        0,      /**< Treat msg as plain text */
                MOTDPANEL_TYPE_INDEX =       1,      /**< Msg is auto determined by the engine */
                MOTDPANEL_TYPE_URL =         2,      /**< Treat msg as an URL link */
                MOTDPANEL_TYPE_FILE =        3,      /**< Treat msg as a filename to be opened */
            };
            inline void ShowMOTDPanel(int client, const char *title, const char *msg, MOTDPANEL_TYPE type = MOTDPANEL_TYPE_INDEX)
            {
                KeyValues::AutoDeleteInline kv(new KeyValues("data"));
                kv->SetString("title", title);
                kv->SetString("type", std::to_string(static_cast<int>(type)).c_str());
                kv->SetString("msg", msg);
                ShowVGUIPanel(client, "info", kv);
            }
            inline void DisplayAskConnectBox(int client, float time, const char *ip, const char *password = "")
            {
                char destination[288];
                snprintf(destination, sizeof(destination), "%s/%s", ip, password);

                KeyValues::AutoDeleteInline kv(new KeyValues("data"));
                kv->SetFloat("time", time);
                kv->SetString("title", destination);
                CreateDialog(client, kv, DialogType_AskConnect);
            }
#pragma endregion
#pragma region entityref
            inline cell_t EntIndexToEntRef(int entity)
            {
                return gamehelpers->IndexToReference(entity);
            }
            inline int EntRefToEntIndex(cell_t ref)
            {
                return gamehelpers->ReferenceToIndex(ref);
            }
            inline cell_t MakeCompatEntRef(cell_t ref)
            {
                return gamehelpers->ReferenceToBCompatRef(ref);
            }

            enum ClientRangeType
            {
                RangeType_Visibility = 0,
                RangeType_Audibility
            };
            inline int GetClientsInRange(const Vector& vOrigin, ClientRangeType rangeType, int* outPlayers, int maxPlayers) {


                CBitVec<ABSOLUTE_PLAYER_LIMIT> players;
                engine->Message_DetermineMulticastRecipients(rangeType == RangeType_Audibility, vOrigin, players);

                int curPlayers = 0;

                int index = players.FindNextSetBit(0);
                while (index > -1 && curPlayers < maxPlayers)
                {
                    int entidx = index + 1;
                    IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(entidx);
                    if (pPlayer && pPlayer->IsInGame())
                    {
                        outPlayers[curPlayers++] = entidx;
                    }

                    index = players.FindNextSetBit(index + 1);
                }

                return curPlayers;

            }
            inline std::vector<int> GetClientsInRangeVec(const Vector& vOrigin, ClientRangeType rangeType)
            {
                std::vector<int> result(ABSOLUTE_PLAYER_LIMIT, 0);
                int size = GetClientsInRange(vOrigin, rangeType, result.data(), result.size());
                result.resize(size);
                return result;
            }
#pragma endregion
#pragma region auth
            // Must match clients.inc
            enum AuthIdType
            {
                AuthId_Engine = 0,     /**< The game-specific auth string as returned from the engine */

                // The following are only available on games that support Steam authentication.
                AuthId_Steam2,         /**< Steam2 rendered format, ex "STEAM_1:1:4153990" */
                AuthId_Steam3,         /**< Steam3 rendered format, ex "[U:1:8307981]" */
                AuthId_SteamID64,      /**< A SteamID64 (uint64) as a String, ex "76561197968573709" */
            };
            inline void GetServerAuthId(AuthIdType authType, char *auth, size_t maxlen)
            {
                switch (authType)
                {
                    case AuthId_Steam3:
                        gamehelpers->GetServerSteam3Id(auth, maxlen);
                        break;

                    case AuthId_SteamID64:
                        ke::SafeSprintf(auth, maxlen, "%" PRIu64, gamehelpers->GetServerSteamId64());
                        break;
                    default:
                        throw std::runtime_error("Unsupported AuthIdType for GetServerAuthId.");
                }
            }
            inline uint64_t GetServerSteamAccountId()
            {
                return gamehelpers->GetServerSteamId64();
            }
#pragma endregion
        }
    }
}
