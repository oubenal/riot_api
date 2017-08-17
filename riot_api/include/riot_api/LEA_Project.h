#pragma once
#ifndef LEA_HPP
#define LEA_HPP

#include <vector>
#include <map>
#include "RiotObject.h"

namespace LEA_Project
{
	struct ChampionStats
	{
		int64_t accountId;
		int championId;
		double cs, kills, deaths, assists;
		int wins, losses;
	};

	int getPositionInMatch(Riot::Match& match, int64_t summoner_id);

	Riot::Summoner getSummonerSummoners(const std::string& name);

	std::list<Riot::LeaguePosition> getLeaguePositions(const Riot::Summoner& summoner);

	Riot::Matchlist getMatchHistory(const Riot::Summoner& summoner);

	std::vector<Riot::Match> getMatchList(const Riot::Matchlist& match_history);

	std::pair<std::vector<Riot::Match>, Riot::LeaguePosition> getSummonerHistory(const std::string &name);

	std::map<int, ChampionStats> getSummonerChampionsStats(const std::string &name);
}

#endif //  LEA_HPP