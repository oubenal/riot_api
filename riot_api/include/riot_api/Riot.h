#pragma once
#ifndef RIOT_HPP
#define RIOT_HPP
#include "RiotObject.h"
#include <vector>
#include <list>

namespace Riot
{
	/* By default, functions will use these fields
	unless otherwise specified. */
	extern std::string api_key; // ""
	//extern Region region;       // Region::NA
	//extern Locale locale;       // Locale::en_US
	extern std::string base_url;

	// ---------- API Resource: SUMMONER-V3 ---------- //
	Summoner getSummonerSummonersByName(const std::string &name);

	// ---------- API Resource: LEAGUE-V3 ---------- //
	std::list<LeaguePosition> getLeaguePositionsBySummoner(int64_t summonerId);


	Matchlist getMatchlistsByAccountRecent(int64_t account_id);

	Matchlist getMatchlistsByAccount(int64_t account_id, int begin_index, int end_index, int queue, int season);

	Match getMatchbyMatchId(int64_t game_id);
}

#endif // RIOT_API_HPP