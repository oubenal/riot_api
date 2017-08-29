#include "riot_api/LEA_Project.h"
#include "riot_api/RiotObject.h"
#include "riot_api/Riot.h"
#include "riot_api/URLReader.h"

#include "riot_api/serverBD.h"

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <ctime>

namespace LEA_Project
{
	bool mycomparison(Riot::MatchReference& first, Riot::MatchReference& second)
	{
		return (first.timestamp>second.timestamp);
	}

	int getPositionInMatch(Riot::Match& match, int64_t summoner_id)
	{
		for (auto participant : match.participantIdentities)
			if (participant.player.summonerId == summoner_id)
				return participant.participantId;
		return -1;
	}

	Summoner getSummonerSummoners(const std::string& name)
	{
		auto serverBD = ServerBD::getInstance();
		// GetSummoner
		Summoner summoner = {};
		if (!serverBD->getFromDBSummonerByName(name, summoner))
		{
			summoner.riotSummoner = Riot::getSummonerSummonersByName(name);
			summoner.lastUpdate = time(nullptr) * 1000;
			// insert into DB
			serverBD->setInDBSummoner(summoner);
		}

		return summoner;
	}

	std::list<Riot::LeaguePosition> getLeaguePositions(const LEA_Project::Summoner& summoner)
	{
		auto serverBD = ServerBD::getInstance();
		// GetLeaguePosition
		std::list<Riot::LeaguePosition> league_positions;
		if (!serverBD->getFromDBLeaguesBySummonerId(summoner.riotSummoner.id, league_positions))
		{
			league_positions = Riot::getLeaguePositionsBySummoner(summoner.riotSummoner.id);
			// insert into DB
			serverBD->setInDBLeagues(league_positions);
		}

		return league_positions;
	}

	Riot::Matchlist getMatchHistory(const LEA_Project::Summoner& summoner)
	{
		auto serverBD = ServerBD::getInstance();

		Riot::Matchlist match_history = {};
		if (!serverBD->getFromDBMatchReference(summoner.riotSummoner.accountId, match_history))
		{
			match_history = Riot::getMatchlistsByAccountRecent(summoner.riotSummoner.accountId, 5);
			// insert into DB
			serverBD->setInDBMatchReference(summoner.riotSummoner.accountId, match_history);
		}

		return match_history;
	}

	std::vector<Riot::Match> getMatchList(const Riot::Matchlist& match_history)
	{
		auto serverBD = ServerBD::getInstance();

		std::vector<Riot::Match> matchs;
		matchs.reserve(match_history.totalGames);

		for (auto match_ref : match_history.matches)
		{
			Riot::Match match = {};
			if (!serverBD->getFromDBMatch(match_ref.gameId, match))
			{
				try
				{
					match = Riot::getMatchbyMatchId(match_ref.gameId);
					// insert into DB
					serverBD->setInDBMatch(match);
				}
				catch (Riot::URLReader::URLReaderException& e)
				{
					std::cout << "# ERR: " << e.what();
				}
			}
			if (!serverBD->getFromDBParticipantStats(match_ref.gameId, match))
			{
				try
				{
					match = Riot::getMatchbyMatchId(match_ref.gameId);
					// insert into DB
					serverBD->setInDBMatch(match);
					serverBD->setInDBParticipantsStats(match);
				}
				catch (Riot::URLReader::URLReaderException& e)
				{
					std::cout << "# ERR: " << e.what();
				}
			}
			//
			for(auto& participant : match.participantIdentities)
			{
				auto name = participant.player.summonerName;
				LEA_Project::Summoner summoner;
				if (!serverBD->getFromDBSummonerByName(name, summoner))
					participant.player.country = "nan";
				else
					participant.player.country = summoner.country;
			}
			matchs.push_back(match);
		}

		return matchs;
	}

	std::pair<std::vector<Riot::Match>, Riot::LeaguePosition> getSummonerHistory(const std::string &name)// , int dept_history)
	{
		auto serverBD = ServerBD::getInstance();
		// GetSummoner
		auto summoner = getSummonerSummoners(name);

		// GetLeaguePosition
		auto league_positions = getLeaguePositions(summoner);
		

		// GetMatchHistoryReference
		auto match_history = getMatchHistory(summoner);

		//GetMatchHistory
		auto matchs = getMatchList(match_history);

		Riot::LeaguePosition league_position = {};
		for (auto lp : league_positions)
			if (!lp.queueType.compare("RANKED_SOLO_5x5"))
				league_position = lp;
		return std::make_pair(matchs, league_position);
	}

	std::map<int, ChampionStats> getSummonerChampionsStats(const std::string &name)
	{
		auto serverBD = ServerBD::getInstance();

		// GetSummoner
		auto summoner = getSummonerSummoners(name);
		std::map<int, ChampionStats> champions_stats;
		while (!serverBD->getFromDBChampionStats(summoner.riotSummoner.accountId, champions_stats))
		{
			getSummonerChampionsStatsFromRiot(name);
		}
		
		return champions_stats;
	}

	std::map<int, ChampionStats> getSummonerChampionsStatsFromRiot(const std::string& name)
	{
		auto serverBD = ServerBD::getInstance();

		auto summoner = getSummonerSummoners(name);
		std::map<int, ChampionStats> champions_stats;

		//Get champions list
		std::map<int, Riot::Champion> champions;
		serverBD->getListChampions(champions);
		
		// Get Match History References
		auto match_history = Riot::getMatchlistsByAccount(summoner.riotSummoner.accountId, 0, -1, 420, 9);;
		auto total_games = match_history.totalGames;
		if(total_games > 50)
		{
			for (auto begin_index = 50; begin_index < total_games; begin_index+=50)
				match_history.matches.merge(Riot::getMatchlistsByAccount(summoner.riotSummoner.accountId, begin_index, -1, 420, 9).matches, mycomparison);
		}	
		

		// Main loop
		
		static auto MAX_RETRY_COUNT = 5;
		for (auto match_ref : match_history.matches)
		{
			auto retryCount = 0;
			Riot::Match match = {};
			match.gameCreation = match_ref.timestamp;
			while(true)
			{
				try
				{
					match = Riot::getMatchbyMatchId(match_ref.gameId);
					break;
				}
				catch (Riot::URLReader::URLReaderException& e)
				{
					if (retryCount > MAX_RETRY_COUNT)
						std::cout << "# ERR: " << e.what();
				}
			}
			

			auto position = getPositionInMatch(match, summoner.riotSummoner.id);
			if (position < 0)
				continue;
			auto participant = match.participants[position - 1];

			LEA_Project::ChampionStats new_champion_stats = {};
			new_champion_stats.championId = participant.championId;
			new_champion_stats.accountId = summoner.riotSummoner.accountId;
			new_champion_stats.cs = match.participants[position - 1].stats.totalMinionsKilled;
			new_champion_stats.kills = match.participants[position - 1].stats.kills;
			new_champion_stats.deaths = match.participants[position - 1].stats.deaths;
			new_champion_stats.assists = match.participants[position - 1].stats.assists;
			new_champion_stats.wins = match.participants[position - 1].stats.win ? 1 : 0;
			new_champion_stats.losses = match.participants[position - 1].stats.win ? 0 : 1;
			new_champion_stats.firstTimestamp = match.gameCreation;
			new_champion_stats.lastTimestamp = match.gameCreation;
			
			if(champions_stats.find(participant.championId) != champions_stats.end())
			{
				auto& champion_stats = champions_stats[participant.championId];
				champion_stats.cs +=	new_champion_stats.cs	;
				champion_stats.kills += new_champion_stats.kills	;
				champion_stats.deaths += new_champion_stats.deaths	;
				champion_stats.assists += new_champion_stats.assists    ;
				champion_stats.wins +=	new_champion_stats.wins   ;
				champion_stats.losses += new_champion_stats.losses	;
				if (new_champion_stats.firstTimestamp < champion_stats.firstTimestamp)
					champion_stats.firstTimestamp = new_champion_stats.firstTimestamp;
				if (new_champion_stats.lastTimestamp > champion_stats.lastTimestamp)
					champion_stats.lastTimestamp = new_champion_stats.lastTimestamp;
			}
			else
			{
				champions_stats[participant.championId] = new_champion_stats;
			}

			serverBD->setInDBChampionStats(summoner.riotSummoner.accountId, new_champion_stats);
		}
		

		return champions_stats;
	}
}
