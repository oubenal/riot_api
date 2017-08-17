#include "riot_api/LEA_Project.h"
#include "riot_api/RiotObject.h"
#include "riot_api/Riot.h"
#include "riot_api/URLReader.h"

#include "riot_api/serverBD.h"

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

	Riot::Summoner getSummonerSummoners(const std::string& name)
	{
		auto serverBD = ServerBD::getInstance();
		// GetSummoner
		Riot::Summoner summoner = {};
		if (!serverBD->getFromDBSummonerByName(name, summoner))
		{
			summoner = Riot::getSummonerSummonersByName(name);
			// insert into DB
			serverBD->setInDBSummoner(summoner);
		}

		return summoner;
	}

	std::list<Riot::LeaguePosition> getLeaguePositions(const Riot::Summoner& summoner)
	{
		auto serverBD = ServerBD::getInstance();
		// GetLeaguePosition
		std::list<Riot::LeaguePosition> league_positions;
		if (!serverBD->getFromDBLeaguesBySummonerId(summoner.id, league_positions))
		{
			league_positions = Riot::getLeaguePositionsBySummoner(summoner.id);
			// insert into DB
			serverBD->setInDBLeagues(league_positions);
		}

		return league_positions;
	}

	Riot::Matchlist getMatchHistory(const Riot::Summoner& summoner)
	{
		auto serverBD = ServerBD::getInstance();

		Riot::Matchlist match_history = {};
		if (!serverBD->getFromDBMatchReference(summoner.accountId, match_history))
		{
			match_history = Riot::getMatchlistsByAccountRecent(summoner.accountId);
			// insert into DB
			serverBD->setInDBMatchReference(summoner.accountId, match_history);
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
		Riot::Summoner summoner = {};
		if (!serverBD->getFromDBSummonerByName(name, summoner))
		{
			summoner = Riot::getSummonerSummonersByName(name);
			// insert into DB
			serverBD->setInDBSummoner(summoner);
		}
		std::map<int, ChampionStats> champions_stats;
		auto result = serverBD->getFromDBChampionStats(summoner.accountId, champions_stats);
		if (false)
		{
			// GetMatchHistoryReference
			Riot::Matchlist match_history = {};
			auto nb_game_found = serverBD->getFromDBMatchReference(summoner.accountId, match_history);
			auto match_history0 = Riot::getMatchlistsByAccount(summoner.accountId, nb_game_found, 10000, 420, 9);
			//serverBD->setInDBMatchReference(summoner.accountId, match_history0);
			match_history.matches.merge(match_history0.matches, mycomparison);

			//GetMatchHistory

			std::map<int, Riot::Champion> champions;

			serverBD->getListChampions(champions);
			for (auto match_ref : match_history.matches)
			{
				Riot::Match match = {};
				match.gameCreation = match_ref.timestamp;
				try
				{
					match = Riot::getMatchbyMatchId(match_ref.gameId);
					// insert into DB
					//serverBD->setInDBParticipantsStats(match);
				}
				catch (Riot::URLReader::URLReaderException& e)
				{
					std::cout << "# ERR: " << e.what();
				}

				auto position = getPositionInMatch(match, summoner.id);
				if (position < 0)
					continue;
				auto participant = match.participants[position - 1];

				auto& champion_stats = champions_stats[participant.championId];
				champion_stats.championId = participant.championId;
				champion_stats.accountId = summoner.accountId;
				champion_stats.cs += match.participants[position - 1].stats.totalMinionsKilled;
				champion_stats.kills += match.participants[position - 1].stats.kills;
				champion_stats.deaths += match.participants[position - 1].stats.deaths;
				champion_stats.assists += match.participants[position - 1].stats.assists;
				champion_stats.wins += match.participants[position - 1].stats.win ? 1 : 0;
				champion_stats.losses += match.participants[position - 1].stats.win ? 0 : 1;
			}
			serverBD->setInDBChampionStats(summoner.accountId, champions_stats);
		}
		
		return champions_stats;
	}
}
