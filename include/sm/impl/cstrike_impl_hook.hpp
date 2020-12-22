#pragma once

namespace sm {
    namespace cstrike {

#if SOURCE_ENGINE == SE_CSS
        class DetourTerminateRoundClass
        {
        public:
            void DetourTerminateRound(float delay, int reason);
            static inline void (DetourTerminateRoundClass::* DetourTerminateRound_Actual)(float delay, int reason) = nullptr;
        };
        inline void DetourTerminateRoundClass::DetourTerminateRound(float delay, int reason)
        {
            if (g_pIgnoreTerminateDetour)
            {
                g_pIgnoreTerminateDetour = false;
                DETOUR_MEMBER_CALL(DetourTerminateRound)(delay, reason);
                return;
            }
#elif SOURCE_ENGINE == SE_CSGO && !defined(WIN32)
        class DetourTerminateRoundClass
        {
        public:
            void DetourTerminateRound(float delay, int reason, int unknown, int unknown2);
            static inline void (DetourTerminateRoundClass::* DetourTerminateRound_Actual)(float delay, int reason, int unknown, int unknown2) = nullptr;
        };
        inline void DetourTerminateRoundClass::DetourTerminateRound(float delay, int reason, int unknown, int unknown2)
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
        class DetourTerminateRoundClass
        {
        public:
            void DetourTerminateRound(int reason, int unknown, int unknown2);
            static inline void (DetourTerminateRoundClass::* DetourTerminateRound_Actual)(int reason, int unknown, int unknown2) = nullptr;
        };
        inline void DetourTerminateRoundClass::DetourTerminateRound(int reason, int unknown, int unknown2)
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

        inline bool CreateTerminateRoundDetour()
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

        inline void RemoveTerminateRoundDetour()
        {
            if (DTerminateRound != NULL)
            {
                DTerminateRound->Destroy();
                DTerminateRound = NULL;
            }
            g_pTerminateRoundDetoured = false;
        }
    }
}