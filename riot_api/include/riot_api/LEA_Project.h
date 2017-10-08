#pragma once
#ifndef LEA_HPP
#define LEA_HPP

#include <vector>
#include <map>
#include "RiotObject.h"

namespace LEA_Project
{

	struct QueueEl
	{
		int64_t summonerId;
		std::string name, country;
	};

	struct Summoner
	{
		Riot::Summoner riotSummoner;
		int64_t lastUpdate;
		std::string country;
		int rank_country;
		int rank_all;
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

	struct LEAData
	{
		Summoner summoner;
		std::vector<Riot::Match> matchs;
		Riot::LeaguePosition league;
		std::map<int, ChampionStats> champions_stats;
	};

	int getPositionInMatch(Riot::Match& match, int64_t summoner_id);

	int updateSummonerhistory(const std::string &name);

	int fetchSummonerData(const std::string& name, LEAData& data);

	int queueSummoner(const std::string& name);

	int insertSummoners();
}

#endif //  LEA_HPP