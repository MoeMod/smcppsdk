
#include "extension.h"

#include "sm/sdktools.h"
#include "sm/call_helper.h"

#include <stdexcept>
#include <string>

namespace sm {
    namespace sdktools {
        IBinTools *g_pBinTools = nullptr;
        IGameConfig *g_pGameConf = nullptr;
        INetworkStringTableContainer* netstringtables = nullptr;
        IServerTools* servertools = nullptr;
        IServerGameClients* serverClients = nullptr;
        IServer* iserver = nullptr;
        variant_t g_Variant_t = { .fieldType = FIELD_VOID };
        bool detail::SDK_OnLoad(char *error, size_t maxlength, bool late) {
            char conf_error[255];
            if (!gameconfs->LoadGameConfigFile("sdktools.games", &g_pGameConf, conf_error, sizeof(conf_error)))
                return false;
            sharesys->AddDependency(myself, "bintools.ext", true, true);
            SM_GET_IFACE(BINTOOLS, g_pBinTools);
            return true;
        }
        int FindOffset(const char* name)
        {
            int offset;
            if (!g_pGameConf->GetOffset(name, &offset))
                throw std::runtime_error("hook : sig not found - " + std::string(name));
            return offset;
        }
        void* FindSig(const char* name)
        {
            void* addr;
            if (!g_pGameConf->GetMemSig(name, &addr))
                throw std::runtime_error("hook : sig not found - " + std::string(name));
            return addr;
        }
        void detail::SDK_OnUnload() {
            gameconfs->CloseGameConfigFile(g_pGameConf);
        }

        bool detail::SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late)
        {
            GET_V_IFACE_ANY(GetEngineFactory, netstringtables, INetworkStringTableContainer, INTERFACENAME_NETWORKSTRINGTABLESERVER);
            GET_V_IFACE_ANY(GetServerFactory, serverClients, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
#if SOURCE_ENGINE >= SE_ORANGEBOX
            GET_V_IFACE_ANY(GetServerFactory, servertools, IServerTools, VSERVERTOOLS_INTERFACE_VERSION);
#endif
            return true;
        }
    }
}