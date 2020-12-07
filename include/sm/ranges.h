#pragma once

#include <ranges>
#include <numeric>
#include <span>
#include "sourcemod.h"
#include "cstrike_const.h"

namespace sm {
	namespace ranges {
		namespace detail {
			template<class T, T...I> constexpr auto IntegerSequenceView(std::integer_sequence<T, I...>)
			{
				constexpr std::array<T, sizeof...(I)> arr = { I... };
				return std::views::all(arr);
			}

			inline CSTeam_e GetTeam(int player)
			{
				auto entity = ent_cast<CBaseEntity*>(player);
				return entity ? static_cast<CSTeam_e>(sm::GetEntProp<int>(entity, sm::Prop_Data, "m_iTeamNum")) : CS_TEAM_NONE;
			}
		}

		template<class EntityType = int>
		inline auto Players()
		{
			return detail::IntegerSequenceView(std::make_integer_sequence<int, SM_MAXPLAYERS + 1>())
				| std::views::drop(1)
				| std::views::take(playerhelpers->GetMaxClients())
				| std::views::transform(make_auto_ent<EntityType>);
		}

		inline auto Connected()
		{
			return std::views::filter(sm::IsClientConnected);
		}

		inline auto Alive()
		{
			return std::views::filter(sm::IsPlayerAlive);
		}

		inline auto Team(CSTeam_e team)
		{
			return std::views::filter([team](int player) { return detail::GetTeam(player) == team; });
		}
	}
}