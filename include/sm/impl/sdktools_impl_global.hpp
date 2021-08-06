#pragma once

namespace SourceMod {
    class IBinTools;
    class ISDKTools;
    class IGameConfig;
    class IGameConfig;
}

class INetworkStringTableContainer;
class IServerTools;
class IServerGameClients;
class IServer;

namespace sm {
    namespace sdktools {
        inline IBinTools* g_pBinTools = nullptr;
        inline IGameConfig* g_pGameConf = nullptr;
        inline INetworkStringTableContainer* netstringtables = nullptr;
        inline IServerTools* servertools = nullptr;
        inline IServerGameClients* serverClients = nullptr;
        inline IServer* iserver = nullptr;
        inline variant_t g_Variant_t = { .fieldType = FIELD_VOID };
        inline const char* g_sGameRulesProxy = nullptr;
        inline static void* s_pGameRules = nullptr;
        inline static void** g_ppGameRules = nullptr;
    }
}