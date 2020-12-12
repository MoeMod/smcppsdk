#pragma once
// if possible, modife sourcemod_hudtext->sourcemod_usermsgs
#include <tier0/wchartypes.h>
#include "../core/CellRecipientFilter.h"
#include <ranges>
#include <string>
#include <iterator>

namespace sm{
    inline namespace sourcemod {
        inline namespace usermessages {

            struct hud_text_parms {
                float x;
                float y;
                int effect;
                byte r1, g1, b1, a1;
                byte r2, g2, b2, a2;
                float fadeinTime;
                float fadeoutTime;
                float holdTime;
                float fxTime;
                int channel;
            };

        	namespace detail {
                void HudText(CellRecipientFilter& crf, const hud_text_parms& textparms, const char* pMessage);
                void ShakeScreen(CellRecipientFilter& crf, float flAmplitude, float flFrequency, float flDurationTime);
                void FadeScreen(CellRecipientFilter& crf, int iDuration, float iHoldTime, int iFlags, Color color);
                void SayText(CellRecipientFilter& crf, int ent_idx, const char* msg, bool chat);
                void HintText(CellRecipientFilter& crf, bool hasColor, const char* pMessage);
                void SayText2(CellRecipientFilter& crf, int ent_idx, bool chat, const char* pMessage);
        		
                template<class T> concept PlayerSingle_c = std::convertible_to<T, AutoEntity<int>>;
                template<class T> concept PlayerRange_c = std::ranges::range<std::decay_t<T>> && PlayerSingle_c<std::ranges::range_value_t<std::decay_t<T>>>;

        		inline CellRecipientFilter CreateCrf(AutoEntity<int> client)
                {
                    cell_t player = client;
                    CellRecipientFilter crf;
                    crf.Initialize(&player, 1);
                    return crf;
                }
                
        		template<PlayerRange_c T> CellRecipientFilter CreateCrf(T &&players)
                {
                	// 用string可以使用小对象优化，而vector必分配内存。
                    std::basic_string<cell_t> arr;
                    auto last = std::transform(std::ranges::begin(players), std::ranges::end(players), std::back_inserter(arr), ent_cast<int>);
                    CellRecipientFilter crf;
                    crf.Initialize(arr.data(), arr.size());
                    return crf;
                }

        	}

            template<class T> concept MessageReceiver_c = detail::PlayerSingle_c<T> || detail::PlayerRange_c<T>;
        	
        	template<MessageReceiver_c T> void SendHudText(T&& client, const hud_text_parms &textparms, const char *pMessage)
        	{
                CellRecipientFilter crf = detail::CreateCrf(std::forward<T>(client));
                return detail::HudText(crf, textparms, pMessage);
        	}

            template<MessageReceiver_c T> void SendSayText(T&& client, int ent_idx, const char* msg, bool chat)
            {
                CellRecipientFilter crf = detail::CreateCrf(std::forward<T>(client));
                return detail::SayText(crf, ent_idx, msg, chat);
            }
        	
            template<MessageReceiver_c T> void SendSayText2(T&& client, int ent_idx, const char* msg)
            {
                CellRecipientFilter crf = detail::CreateCrf(std::forward<T>(client));
                return detail::SayText2(crf, ent_idx, msg);
            }

            template<MessageReceiver_c T> void CreateShakeScreen(T&& client, float flAmplitude, float flFrequency, float flDurationTime)
        	{
                CellRecipientFilter crf = detail::CreateCrf(std::forward<T>(client));
                return detail::ShakeScreen(crf, flAmplitude, flFrequency, flDurationTime);
        	}
        	
            template<MessageReceiver_c T> void CreateFadeScreen(T&& client, int iDuration, float iHoldTime, int iFlags, Color color)
        	{
                CellRecipientFilter crf = detail::CreateCrf(std::forward<T>(client));
                return detail::FadeScreen(crf, iDuration, iHoldTime, iFlags, color);
        	}
        }
    }
}