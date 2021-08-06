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
                SM_GET_IFACE(BINTOOLS, g_pBinTools);

                char* addr;
                if (g_pGameConf->GetMemSig("g_pGameRules", (void**)&addr) && addr)
                {
                    g_ppGameRules = reinterpret_cast<void**>(addr);
                }
                else if (g_pGameConf->GetMemSig("CreateGameRulesObject", (void**)&addr) && addr)
                {
                    int offset;
                    if (!g_pGameConf->GetOffset("g_pGameRules", &offset) || !offset)
                    {
                        return false;
                    }
#ifdef PLATFORM_X86
                    g_ppGameRules = *reinterpret_cast<void***>(addr + offset);
#else
                    int32_t varOffset = *(int32_t*)((unsigned char*)addr + offset);
                    g_ppGameRules = *reinterpret_cast<void***>((unsigned char*)addr + offset + sizeof(int32_t) + varOffset);
#endif
                }
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