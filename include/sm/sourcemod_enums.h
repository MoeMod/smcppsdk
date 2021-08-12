#pragma once

namespace sm
{
	inline namespace sourcemod
	{
		inline namespace enums
		{
			enum RoundState {
				// initialize the game, create teams
				RoundState_Init,

				//Before players have joined the game. Periodically checks to see if enough players are ready
				//to start a game. Also reverts to this when there are no active players
				RoundState_Pregame,

				//The game is about to start, wait a bit and spawn everyone
				RoundState_StartGame,

				//All players are respawned, frozen in place
				RoundState_Preround,

				//Round is on, playing normally
				RoundState_RoundRunning,

				//Someone has won the round
				RoundState_TeamWin,

				//Noone has won, manually restart the game, reset scores
				RoundState_Restart,

				//Noone has won, restart the game
				RoundState_Stalemate,

				//Game is over, showing the scoreboard etc
				RoundState_GameOver,

				//Game is over, doing bonus round stuff
				RoundState_Bonus,

				//Between rounds
				RoundState_BetweenRounds
			};

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