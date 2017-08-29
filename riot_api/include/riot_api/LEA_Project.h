#pragma once
#ifndef LEA_HPP
#define LEA_HPP

#include <vector>
#include <map>
#include "RiotObject.h"

namespace LEA_Project
{

	struct Summoner
	{
		Riot::Summoner riotSummoner;
		int64_t lastUpdate;
		std::string country;
	};

	struct ChampionStats
	{
		int64_t accountId;
		int championId;
		double cs, kills, deaths, assists;
		int wins, losses;
		int64_t firstTimestamp, lastTimestamp;
		int rank_by_country, rank_by_all;
	};

	int getPositionInMatch(Riot::Match& match, int64_t summoner_id);

	Summoner getSummonerSummoners(const std::string& name);

	std::list<Riot::LeaguePosition> getLeaguePositions(const LEA_Project::Summoner& summoner);

	Riot::Matchlist getMatchHistory(const LEA_Project::Summoner& summoner);

	std::vector<Riot::Match> getMatchList(const Riot::Matchlist& match_history);

	std::pair<std::vector<Riot::Match>, Riot::LeaguePosition> getSummonerHistory(const std::string &name);

	std::map<int, ChampionStats> getSummonerChampionsStats(const std::string &name);

	std::map<int, ChampionStats> getSummonerChampionsStatsFromRiot(const std::string &name);
}

#endif //  LEA_HPP