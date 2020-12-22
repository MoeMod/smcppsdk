#pragma once

#include <CDetour/detours.h>

namespace SourceMod {
    class IBinTools;
    class ISDKTools;
    class IGameConfig;
}

namespace sm {
    namespace cstrike
    {
        inline SourceMod::IBinTools* g_pBinTools = nullptr;
        inline SourceMod::ISDKTools* g_pSDKTools = nullptr;
        inline SourceMod::IGameConfig* g_pGameConf = nullptr;

        inline bool g_pIgnoreTerminateDetour = false;
        inline bool g_pTerminateRoundDetoured = false;
        inline CDetour* DTerminateRound = NULL;
        inline bool g_TerminateRoundDetourEnabled = false;
    }
}