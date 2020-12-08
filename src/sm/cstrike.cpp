
#include "extension.h"
#include "sm/cstrike.h"
#include "sm/call_helper.h"
#include "sm/sdktools.h"
#include <extensions/IBinTools.h>
#include <extensions/ISDKHooks.h>
#include <extensions/ISDKTools.h>

#include "CDetour/detours.h"

#include <string>
#include <stdexcept>

namespace sm {
    namespace cstrike {
        IBinTools *g_pBinTools = nullptr;
        ISDKTools *g_pSDKTools = nullptr;
        IGameConfig *g_pGameConf = nullptr;

        inline namespace forwards {
            EventDispatcher<Action(float&, CSRoundEndReason_e&)> CS_OnTerminateRound;
        }

        bool g_pIgnoreTerminateDetour = false;
        bool g_pTerminateRoundDetoured = false;
        CDetour* DTerminateRound = NULL;
        bool g_TerminateRoundDetourEnabled = false;

        void *FindSig(const char *name)
        {
            void *addr;
            if(!g_pGameConf->GetMemSig(name, &addr))
                throw std::runtime_error("hook : sig not found - " + std::string(name));
            return addr;
        }

        static void TerminateRound(CGameRules *gamerules, float delay, int reason) {
#if SOURCE_ENGINE == SE_CSGO
            reason++;
#endif
#ifndef _WIN32
            static MemFuncCaller<void (CGameRules::*)(float, int, int, int)> caller(g_pBinTools, FindSig("TerminateRound"));
            return caller(gamerules, delay, reason, 0, 0);
#else
            static void *addr = FindSig("TerminateRound");
			__asm
			{
			push 0
			push 0
			push reason
			movss xmm1, delay
			mov ecx, gamerules
			call addr
			}
#endif
        }

        void CS_TerminateRound(float delay, CSRoundEndReason_e reason, bool blockhook) {
            if (blockhook && g_pTerminateRoundDetoured)
                g_pIgnoreTerminateDetour = true;
            return TerminateRound(static_cast<CGameRules *>(g_pSDKTools->GetGameRules()), delay, reason);
        }

        void CS_RespawnPlayer(CBasePlayer *pEntity) {
            static MemFuncCaller<void (CBasePlayer::*)()> caller(g_pBinTools, FindSig("RoundRespawn"));
            return caller(pEntity);
        }

        void CS_UpdateClientModel(CBasePlayer *pEntity) {
            static MemFuncCaller<void (CBasePlayer::*)()> caller(g_pBinTools, FindSig("SetModelFromClass"));
            return caller(pEntity);
        }

        void SwitchTeam(CGameRules *gamerules, CBasePlayer *pEntity, CSTeam_e team) {

#ifndef _WIN32
            static MemFuncCaller<void (CBasePlayer::*)(int)> caller(g_pBinTools, FindSig("SwitchTeam"));
            return caller(pEntity, team);
#else
            static void *addr = FindSig("SwitchTeam");
			__asm
			{
			push team
			mov ecx, pEntity
			mov ebx, gamerules
			call addr
			}
#endif
        }

        void CS_SwitchTeam(CBasePlayer *pEntity, CSTeam_e team) {
            return SwitchTeam(static_cast<CGameRules *>(g_pSDKTools->GetGameRules()), pEntity, team);
        }

        void CS_DropWeapon(CBasePlayer *pEntity, CBaseCombatWeapon *pWeapon, bool toss) {
            static MemFuncCaller<void (CBasePlayer::*)(CBaseCombatWeapon*, bool)> caller(g_pBinTools, FindSig("DropWeaponBB"));
            return caller(pEntity, pWeapon, toss);
        }

#if SOURCE_ENGINE == SE_CSS
        DETOUR_DECL_MEMBER2(DetourTerminateRound, void, float, delay, int, reason)
        {
            if (g_pIgnoreTerminateDetour)
            {
                g_pIgnoreTerminateDetour = false;
                DETOUR_MEMBER_CALL(DetourTerminateRound)(delay, reason);
                return;
            }
#elif SOURCE_ENGINE == SE_CSGO && !defined(WIN32)
        DETOUR_DECL_MEMBER4(DetourTerminateRound, void, float, delay, int, reason, int, unknown, int, unknown2)
        {
            if (g_pIgnoreTerminateDetour)
            {
                g_pIgnoreTerminateDetour = false;
                DETOUR_MEMBER_CALL(DetourTerminateRound)(delay, reason, unknown, unknown2);
                return;
            }
#else
        //Windows CSGO
        //char __userpurge TerminateRound(int a1@<ecx>, float a2@<xmm1>, int *a3)
        // a1 - this
        // a2 - delay
        // a3 - reason
        // a4 - unknown
        // a5 - unknown
        DETOUR_DECL_MEMBER3(DetourTerminateRound, void, int, reason, int, unknown, int, unknown2)
        {
            float delay;

            if (g_pIgnoreTerminateDetour)
            {
                g_pIgnoreTerminateDetour = false;
                return DETOUR_MEMBER_CALL(DetourTerminateRound)(reason, unknown, unknown2);
            }

            //Save the delay
            __asm
            {
                movss delay, xmm1
            }
#endif

            const float orgdelay = delay;
            const int orgreason = reason;

#if SOURCE_ENGINE == SE_CSGO
            reason--;
#endif

            CSRoundEndReason_e reason_e = static_cast<CSRoundEndReason_e>(reason);
            
            Action result = sm::CallForwardHook(CS_OnTerminateRound, delay, reason_e);
            reason = reason_e;

            if (result >= Pl_Handled)
                return;

#if SOURCE_ENGINE == SE_CSGO
            reason++;
#endif

#if SOURCE_ENGINE == SE_CSS
            if (result == Pl_Changed)
                return DETOUR_MEMBER_CALL(DetourTerminateRound)(delay, reason);

            return DETOUR_MEMBER_CALL(DetourTerminateRound)(orgdelay, orgreason);
#elif SOURCE_ENGINE == SE_CSGO && !defined(WIN32)
            if (result == Pl_Changed)
                return DETOUR_MEMBER_CALL(DetourTerminateRound)(delay, reason, unknown, unknown2);

            return DETOUR_MEMBER_CALL(DetourTerminateRound)(orgdelay, orgreason, unknown, unknown2);
#else
            if (result == Pl_Changed)
            {
                __asm
                {
                    movss xmm1, delay
                }
                return DETOUR_MEMBER_CALL(DetourTerminateRound)(reason, unknown, unknown2);
            }
            __asm
            {
                movss xmm1, orgdelay
            }
            return DETOUR_MEMBER_CALL(DetourTerminateRound)(orgreason, unknown, unknown2);
#endif
        }

        bool CreateTerminateRoundDetour()
        {
            DTerminateRound = DETOUR_CREATE_MEMBER(DetourTerminateRound, "TerminateRound");

            if (DTerminateRound != NULL)
            {
                DTerminateRound->EnableDetour();
                g_pTerminateRoundDetoured = true;
                return true;
            }
            g_pSM->LogError(myself, "TerminateRound detour could not be initialized - Disabled OnTerminateRound forward");
            return false;
        }

        void RemoveTerminateRoundDetour()
        {
            if (DTerminateRound != NULL)
            {
                DTerminateRound->Destroy();
                DTerminateRound = NULL;
            }
            g_pTerminateRoundDetoured = false;
        }

        bool detail::SDK_OnLoad(char *error, size_t maxlength, bool late) {
            char conf_error[255];
            if (!gameconfs->LoadGameConfigFile("sm-cstrike.games", &g_pGameConf, conf_error, sizeof(conf_error)))
            {
                if (error)
                {
                    ke::SafeSprintf(error, maxlength, "Could not read sm-cstrike.games: %s", conf_error);
                }
                return false;
            }
            sharesys->AddDependency(myself, "bintools.ext", true, true);
            sharesys->AddDependency(myself, "sdktools.ext", true, true);

            SM_GET_IFACE(BINTOOLS, g_pBinTools);
            SM_GET_IFACE(SDKTOOLS, g_pSDKTools); 
            
            CDetourManager::Init(g_pSM->GetScriptingEngine(), g_pGameConf);
            g_TerminateRoundDetourEnabled = CreateTerminateRoundDetour();

            return true;

        }

        void detail::SDK_OnUnload() {

            if (g_TerminateRoundDetourEnabled)
            {
                RemoveTerminateRoundDetour();
                g_TerminateRoundDetourEnabled = false;
            }

            gameconfs->CloseGameConfigFile(g_pGameConf);
        }

    }
}