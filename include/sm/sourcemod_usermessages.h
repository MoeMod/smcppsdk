#pragma once

#include <tier0/wchartypes.h>
#include <irecipientfilter.h>
#include <ranges>
#include <string>
#include <iterator>
#include <cassert>

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
                void HudText(IRecipientFilter& crf, const hud_text_parms& textparms, const char* pMessage);
                void ShakeScreen(IRecipientFilter& crf, float flAmplitude, float flFrequency, float flDurationTime);
                void FadeScreen(IRecipientFilter& crf, int iDuration, int iHoldTime, int iFlags, Color color);
                void SayText(IRecipientFilter& crf, int ent_idx, const char* msg, bool chat);
                void HintText(IRecipientFilter& crf, bool hasColor, const char* pMessage);
                void SayText2(IRecipientFilter& crf, int ent_idx, bool chat, const char* pMessage);
                void ConstructHintTextAttribute(IRecipientFilter& crf, const char* message);
        		
                template<class T> concept PlayerSingle_c = std::convertible_to<T, AutoEntity<int>>;
                template<class T> concept PlayerRange_c = std::ranges::range<std::decay_t<T>> && PlayerSingle_c<std::ranges::range_value_t<std::decay_t<T>>>;

                class CRecipientFilter_Single : public IRecipientFilter
                {
                    cell_t m_Player;
                public:
                	explicit CRecipientFilter_Single(cell_t player) : m_Player(player) {}
                    bool IsReliable(void) const override { return false; }
                    bool IsInitMessage(void) const override { return false; }
                    int GetRecipientCount(void) const override { return 1; }
                    int GetRecipientIndex(int slot) const override { assert(!slot); return m_Player; }
                };
        		
        		inline CRecipientFilter_Single CreateCrf(AutoEntity<int> client)
                {
                    return CRecipientFilter_Single(client);
                }

        		template<PlayerRange_c T> class CRecipientFilter_Multi : public IRecipientFilter
        		{
                    std::reference_wrapper<typename std::add_const<typename std::remove_reference<T>::type>::type> m_PlayersRangeRef;
                public:
        			template<class U>
                    explicit CRecipientFilter_Multi(U &&player) : m_PlayersRangeRef(std::forward<U>(player)) {}
                    bool IsReliable(void) const override { return false; }
                    bool IsInitMessage(void) const override { return false; }
                    int GetRecipientCount(void) const override { return std::ranges::ssize(m_PlayersRangeRef.get()); }
                    int GetRecipientIndex(int slot) const override
        			{
                        auto iter = std::ranges::begin(m_PlayersRangeRef.get());
                        std::advance(iter, slot);
        				return ent_cast<int>(*iter);
        			}
        		};
                template<PlayerRange_c T> CRecipientFilter_Multi(T) -> CRecipientFilter_Multi<T>; // CTAD
                
        		template<PlayerRange_c T> auto CreateCrf(T &&players) requires std::ranges::random_access_range<typename std::remove_reference<T>::type>
                {
                    return CRecipientFilter_Multi<T>(std::forward<T>(players));
                }
                
        		template<PlayerRange_c T> auto CreateCrf(T &&players)
                {
                    std::basic_string<cell_t> arr;
                    std::ranges::transform(players, std::back_inserter(arr), ent_cast<int>);
                    return CRecipientFilter_Multi<std::basic_string<cell_t>>(arr);
                }

        	}

            template<class T> concept MessageReceiver_c = detail::PlayerSingle_c<T> || detail::PlayerRange_c<T>;
        	
        	template<MessageReceiver_c T> void SendHudText(T&& client, const hud_text_parms &textparms, const char *pMessage)
        	{
                auto crf = detail::CreateCrf(std::forward<T>(client));
                return detail::HudText(crf, textparms, pMessage);
        	}

            template<MessageReceiver_c T> void SendSayText(T&& client, int ent_idx, const char* msg, bool chat)
            {
                auto crf = detail::CreateCrf(std::forward<T>(client));
                return detail::SayText(crf, ent_idx, msg, chat);
            }
        	
            template<MessageReceiver_c T> void SendSayText2(T&& client, int ent_idx, const char* msg)
            {
                auto crf = detail::CreateCrf(std::forward<T>(client));
                return detail::SayText2(crf, ent_idx, msg);
            }

            template<MessageReceiver_c T> void CreateShakeScreen(T&& client, float flAmplitude, float flFrequency, float flDurationTime)
        	{
                auto crf = detail::CreateCrf(std::forward<T>(client));
                return detail::ShakeScreen(crf, flAmplitude, flFrequency, flDurationTime);
        	}
        	
            template<MessageReceiver_c T> void CreateFadeScreen(T&& client, int iDuration, int iHoldTime, int iFlags, Color color)
        	{
                auto crf = detail::CreateCrf(std::forward<T>(client));
                return detail::FadeScreen(crf, iDuration, iHoldTime, iFlags, color);
        	}
            template<MessageReceiver_c T> void CreateHintText(T &&client, const char* pMessage)
            {
                auto crf = detail::CreateCrf(std::forward<T>(client));
                return detail::ConstructHintTextAttribute(crf, pMessage);
            }
        }
    }
}
