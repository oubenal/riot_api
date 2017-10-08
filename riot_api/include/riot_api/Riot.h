#pragma once
#ifndef RIOT_HPP
#define RIOT_HPP
#include "RiotObject.h"
#include <vector>
#include <list>

namespace Riot
{
	// ---------- API Resource: SUMMONER-V3 ---------- //
	Summoner getSummonerSummonersByName(const std::string &name);

	Summoner getSummonerSummonersById(int64_t &id);
	// ---------- API Resource: LEAGUE-V3 ---------- //
	std::list<LeaguePosition> getLeaguePositionsBySummoner(int64_t summonerId);


	Matchlist getMatchlistsByAccountRecent(int64_t account_id, int nb_game);

	Matchlist getMatchlistsByAccount(int64_t account_id, int begin_index, int end_index, int queue, int season);

	Matchlist getMatchlistsByAccount(int64_t account_id, int begin_index, int end_index, int queue, int season, int64_t begin_time, int64_t end_time);

	Match getMatchbyMatchId(int64_t game_id);
}

#endif // RIOT_API_HPP