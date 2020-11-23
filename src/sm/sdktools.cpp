
#include "extension.h"

#include "sm/sdktools.h"
#include "sm/call_helper.h"

#include <stdexcept>
#include <string>

namespace sm {
    namespace sdktools {
        IBinTools *g_pBinTools = nullptr;
        IGameConfig *g_pGameConf = nullptr;

        bool detail::SDK_OnLoad(char *error, size_t maxlength, bool late) {
            char conf_error[255];
            if (!gameconfs->LoadGameConfigFile("sdktools.games", &g_pGameConf, conf_error, sizeof(conf_error)))
                return false;
            sharesys->AddDependency(myself, "bintools.ext", true, true);
            SM_GET_IFACE(BINTOOLS, g_pBinTools);
            return true;
        }

        int FindOffset(const char *name)
        {
            int offset;
            if(!g_pGameConf->GetOffset(name, &offset))
                throw std::runtime_error("hook : sig not found - " + std::string(name));
            return offset;
        }

        void detail::SDK_OnUnload() {
            gameconfs->CloseGameConfigFile(g_pGameConf);
        }
    }
}