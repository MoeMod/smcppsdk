#pragma once

namespace sm
{
	inline namespace sourcemod
	{
		inline namespace enums
		{
			enum NetFlow
			{
				NetFlow_Outgoing = 0,   /**< Outgoing traffic */
				NetFlow_Incoming,       /**< Incoming traffic */
				NetFlow_Both            /**< Both values added together */
			};

			// Must match clients.inc
			enum AuthIdType
			{
				AuthId_Engine = 0,     /**< The game-specific auth string as returned from the engine */

				// The following are only available on games that support Steam authentication.
				AuthId_Steam2,         /**< Steam2 rendered format, ex "STEAM_1:1:4153990" */
				AuthId_Steam3,         /**< Steam3 rendered format, ex "[U:1:8307981]" */
				AuthId_SteamID64,      /**< A SteamID64 (uint64) as a String, ex "76561197968573709" */
			};
		}
	}
}