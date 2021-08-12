#pragma once

#include "sdktools_impl_global.hpp"

namespace sm {
    namespace sdktools {
    	namespace detail
    	{
            inline bool SDK_OnLoad(char* error, size_t maxlength, bool late) {
                char conf_error[255];
                if (!gameconfs->LoadGameConfigFile("sdktools.games", &g_pGameConf, conf_error, sizeof(conf_error)))
                    return false;
                sharesys->AddDependency(myself, "bintools.ext", true, true);
                sharesys->AddDependency(myself, "sdktools.ext", true, true);

                g_szGameRulesProxy = g_pGameConf->GetKeyValue("GameRulesProxy");
                if (!g_szGameRulesProxy || !strcmp(g_szGameRulesProxy, ""))
                {
                    smutils->LogError(myself, "Error looking up \"GameRulesProxy\".");
                    return false;
                }

                SM_GET_IFACE(BINTOOLS, g_pBinTools);
                SM_GET_IFACE(SDKTOOLS, g_pSDKTools);

                iserver = g_pSDKTools->GetIServer();

                return true;
            }
    		
            inline void SDK_OnUnload() {
                gameconfs->CloseGameConfigFile(g_pGameConf);
            }

            inline bool SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late)
            {
                GET_V_IFACE_ANY(GetEngineFactory, netstringtables, INetworkStringTableContainer, INTERFACENAME_NETWORKSTRINGTABLESERVER);
                GET_V_IFACE_ANY(GetServerFactory, serverClients, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
#if SOURCE_ENGINE >= SE_ORANGEBOX
                GET_V_IFACE_ANY(GetServerFactory, servertools, IServerTools, VSERVERTOOLS_INTERFACE_VERSION);
#endif
                return true;
            }
    		
    	}
        inline int FindOffset(const char* name)
        {
            int offset;
            if (!g_pGameConf->GetOffset(name, &offset))
                throw std::runtime_error("hook : sig not found - " + std::string(name));
            return offset;
        }
        inline void* FindSig(const char* name)
        {
            void* addr;
            if (!g_pGameConf->GetMemSig(name, &addr))
                throw std::runtime_error("hook : sig not found - " + std::string(name));
            return addr;
        }
    }
}