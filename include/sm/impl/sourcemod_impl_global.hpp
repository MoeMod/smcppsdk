#pragma once

#include "sm/hook_result.h"
#include <memory>

#include <boost/asio/io_context.hpp>

namespace SourceMod {
    class IGameConfig;
	class ITranslator;
    class IPhraseCollection;
}

enum CSRoundEndReason_e : int;

class IEngineSound;
class ICvar;
class IServerPluginHelpers;
class IFileSystem;

namespace sm {
    inline namespace sourcemod {
        inline IGameConfig* g_pGameConf = nullptr;
        inline ICvar* icvar = nullptr;
        inline IServerPluginHelpers* serverpluginhelpers = nullptr;

        inline namespace halflife {
            inline IEngineSound* enginesound = nullptr;
        }

        inline namespace filesystem {
            inline IFileSystem* g_pFullFileSystem = nullptr;
        }
    	
        inline namespace translate {
            inline ITranslator* translator = nullptr;
            inline IPhraseCollection* collection = nullptr;
        }
    	
        inline namespace usermessages {
            inline int g_HudMsgNum = -1;
            inline int g_ShakeMsg = -1;
            inline int g_FadeMsg = -1;
            inline int g_SayTextMsg = -1;
            inline int g_SayText2Msg = -1;
            inline int g_HintTextMsg = -1;
            inline int g_TextMsg = -1; // just coincidence, with same name with usermessage.
            inline bool g_SkipNextChatConSound = false;
            inline bool g_ChatConSnd = true;
        }
    	
        inline namespace functions {
            inline std::shared_ptr<boost::asio::io_context> GetGameFrameContext()
            {
                static std::shared_ptr<boost::asio::io_context> ioc = std::make_shared<boost::asio::io_context>();
                return ioc;
            }
        }

        inline void OnGameFrame(bool simulating)
        {
            auto ioc = GetGameFrameContext();
            ioc->poll();
        }
    	
    	
    }

}
