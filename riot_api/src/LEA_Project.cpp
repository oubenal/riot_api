#include "riot_api/LEA_Project.h"
#include "riot_api/RiotObject.h"
#include "riot_api/Riot.h"
#include "riot_api/URLReader.h"

#include "riot_api/serverBD.h"

#include <windows.h>
#include <tchar.h>
#include <ctime>

namespace LEA_Project
{
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
			//TODO: do something first
			return summoner;
		}
		//GetRank
		if (!serverBD->getRankByCountry(summoner))
		{
			//TODO: do something first
			return summoner;
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
			//TODO: do something first
			return league_positions;
		}

		return league_positions;
	}

	std::vector<Riot::Match> getMatchList(const LEA_Project::Summoner& summoner)
	{
		auto serverBD = ServerBD::getInstance();

		Riot::Matchlist match_history = {};
		if (!serverBD->getFromDBMatchReference(summoner.riotSummoner.accountId, match_history))
		{
			//TODO: do something first
			;
		}

		std::vector<Riot::Match> matchs;
		matchs.reserve(match_history.matches.size());

		for (auto match_ref : match_history.matches)
		{
			Riot::Match match = {};
			if (!serverBD->getFromDBMatch(match_ref.gameId, match))
			{
				//TODO: do something first
				continue;
			}
			if (!serverBD->getFromDBParticipantStats(match_ref.gameId, match))
			{
				//TODO: do something first
				continue;
			}

			for(auto& participant : match.participantIdentities)
			{
				auto name = participant.player.summonerName;
				Summoner local_summoner;
				if (!serverBD->getFromDBSummonerByName(name, local_summoner))
					participant.player.country = "nan";
				else
					participant.player.country = local_summoner.country;
			}
			matchs.push_back(match);
		}

		return matchs;
	}

	std::map<int, ChampionStats> getSummonerChampionsStats(const std::string &name)
	{
		auto serverBD = ServerBD::getInstance();

		// GetSummoner
		auto summoner = getSummonerSummoners(name);
		std::map<int, ChampionStats> champions_stats;
		if (!serverBD->getFromDBChampionStats(summoner.riotSummoner.accountId, champions_stats))
		{
			//TODO: do something first
			return champions_stats;
		}
		for(auto &champion_stats : champions_stats)
		{
			champion_stats.second.rank_by_all = serverBD->getChampionStatsRankByAllFromDB(summoner.riotSummoner.accountId, champion_stats.first);
			champion_stats.second.rank_by_country = serverBD->getChampionStatsRankByCountryFromDB(summoner.riotSummoner.accountId, summoner.country, champion_stats.first);

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
				match_history.matches.merge(Riot::getMatchlistsByAccount(summoner.riotSummoner.accountId, begin_index, -1, 420, 9).matches, [](Riot::MatchReference& first, Riot::MatchReference& second) {return (first.timestamp > second.timestamp); });
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

	std::map<int, ChampionStats> getSummonerChampionsStatsFromRiot(const std::string& name, Riot::Matchlist& match_history)
	{
		auto serverBD = ServerBD::getInstance();

		auto summoner = getSummonerSummoners(name);
		std::map<int, ChampionStats> champions_stats;
		//serverBD->getFromDBChampionStats(summoner.riotSummoner.accountId, champions_stats);

		// Main loop

		static auto MAX_RETRY_COUNT = 5;
		for (auto match_ref : match_history.matches)
		{
			auto retryCount = 0;
			Riot::Match match = {};
			match.gameCreation = match_ref.timestamp;
			while (true)
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

			if (champions_stats.find(participant.championId) != champions_stats.end())
			{
				auto& champion_stats = champions_stats[participant.championId];
				champion_stats.cs += new_champion_stats.cs;
				champion_stats.kills += new_champion_stats.kills;
				champion_stats.deaths += new_champion_stats.deaths;
				champion_stats.assists += new_champion_stats.assists;
				champion_stats.wins += new_champion_stats.wins;
				champion_stats.losses += new_champion_stats.losses;
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

	int fetchSummonerData(const std::string& name, LEAData& data)
	{
		// GetSummoner
		data.summoner = getSummonerSummoners(name);
		if (data.summoner.riotSummoner.name.empty()) // summoner not found
		{
			//TODO: Queue (/_!_\)
			return EXIT_FAILURE;
		}

		// GetLeaguePosition
		for (auto lp : getLeaguePositions(data.summoner))
			if (!lp.queueType.compare("RANKED_SOLO_5x5"))
				data.league = lp;

		//GetMatchHistory
		data.matchs = getMatchList(data.summoner);

		//GetChampionsStats
		data.champions_stats = getSummonerChampionsStats(name);

		return EXIT_SUCCESS;
	}

	int updateSummonerhistory(const std::string &name)
	{
		auto serverBD = ServerBD::getInstance();
		//GetSummoner From RIOT API
		auto riot_summoner = Riot::getSummonerSummonersByName(name);
		//GetSummoner From DB
		auto summoner = getSummonerSummoners(name);
		// Check if new data available
		if (riot_summoner.revisionDate <= summoner.riotSummoner.revisionDate)
		{
			return 0;
		}
		// Get new league position
		auto league_positions = Riot::getLeaguePositionsBySummoner(summoner.riotSummoner.id);
		
		// Get new match history
		Riot::Matchlist match_history;
		if (riot_summoner.revisionDate - summoner.riotSummoner.revisionDate < 604800000)
		{
			auto total_games = 1;
			auto begin_index = 0;
			while (total_games > begin_index)
			{
				auto list = Riot::getMatchlistsByAccount(riot_summoner.accountId, begin_index, -1, 420, 9, summoner.riotSummoner.revisionDate, riot_summoner.revisionDate);
				total_games = list.totalGames;
				begin_index = list.endIndex;
				match_history.matches.merge(list.matches, [](Riot::MatchReference& first, Riot::MatchReference& second) {return (first.timestamp > second.timestamp); });
			}
		}
		else
		{
			auto current_date = riot_summoner.revisionDate - 604800000; // 1000*60*60*24*7 = 1 week
			auto previous_date = riot_summoner.revisionDate;
			while (current_date > summoner.riotSummoner.revisionDate)
			{
				auto total_games = 1;
				auto begin_index = 0;
				while (total_games > begin_index)
				{
					auto list = Riot::getMatchlistsByAccount(riot_summoner.accountId, begin_index, -1, 420, 9, current_date, previous_date);
					total_games = list.totalGames;
					begin_index = list.endIndex;
					match_history.matches.merge(list.matches, [](Riot::MatchReference& first, Riot::MatchReference& second) {return (first.timestamp > second.timestamp); });
					
				}
				previous_date = current_date;
				current_date -= 604800000;
			}
		}

		// Get new matchs
		std::vector<Riot::Match> matchs;
		for (auto match_ref : match_history.matches)
		{
			matchs.push_back(Riot::getMatchbyMatchId(match_ref.gameId));
		}

		// Insert data in DB
		summoner.riotSummoner = riot_summoner;
		summoner.lastUpdate = time(nullptr) * 1000;
		serverBD->setInDBSummoner(summoner);

		serverBD->setInDBLeagues(league_positions);

		serverBD->setInDBMatchReference(summoner.riotSummoner.accountId, match_history);

		for (auto match : matchs)
		{
			serverBD->setInDBMatch(match);
			serverBD->setInDBParticipantsStats(match);
		}

		getSummonerChampionsStatsFromRiot(name, match_history);

		return 1;
	}

	int queueSummoner(const std::string& name)
	{
		auto serverBD = ServerBD::getInstance();

		serverBD->queueSummoner(name);

		return 1;
	}

	int insertSummoners()
	{
		auto serverBD = ServerBD::getInstance();
		QueueEl queue_el = {};
		while (serverBD->popSummoner(queue_el))
		{
			// Get Summoners Data
			auto riot_summoner = Riot::getSummonerSummonersById(queue_el.summonerId);
			Summoner summoner = {};
			summoner.riotSummoner = riot_summoner;
			summoner.country = queue_el.country;
			summoner.lastUpdate = time(nullptr) * 1000;

			// Get League Data
			auto league_positions = Riot::getLeaguePositionsBySummoner(summoner.riotSummoner.id);
			
			// Get History Data
			Riot::Matchlist match_history;
			auto total_games = 1;
			auto begin_index = 0;
			while (total_games > begin_index)
			{
				auto list = Riot::getMatchlistsByAccount(riot_summoner.accountId, begin_index, -1, 420, 9);
				total_games = list.totalGames;
				begin_index = list.endIndex;
				match_history.matches.merge(list.matches, [](Riot::MatchReference& first, Riot::MatchReference& second) {return (first.timestamp > second.timestamp); });
			}

			// Get Matchs Data
			std::vector<Riot::Match> matchs;
			for (auto match_ref : match_history.matches)
			{
				matchs.push_back(Riot::getMatchbyMatchId(match_ref.gameId));
			}

			// Get Champions Stats Data
			std::map<int, ChampionStats> champions_stats;
			for (auto match : matchs)
			{
				auto position = getPositionInMatch(match, summoner.riotSummoner.id);
				if (position < 0)
					continue;
				auto participant = match.participants[position - 1];

				ChampionStats new_champion_stats = {};
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

				if (champions_stats.find(participant.championId) != champions_stats.end())
				{
					auto& champion_stats = champions_stats[participant.championId];
					champion_stats.cs += new_champion_stats.cs;
					champion_stats.kills += new_champion_stats.kills;
					champion_stats.deaths += new_champion_stats.deaths;
					champion_stats.assists += new_champion_stats.assists;
					champion_stats.wins += new_champion_stats.wins;
					champion_stats.losses += new_champion_stats.losses;
					if (new_champion_stats.firstTimestamp < champion_stats.firstTimestamp)
						champion_stats.firstTimestamp = new_champion_stats.firstTimestamp;
					if (new_champion_stats.lastTimestamp > champion_stats.lastTimestamp)
						champion_stats.lastTimestamp = new_champion_stats.lastTimestamp;
				}
				else
				{
					champions_stats[participant.championId] = new_champion_stats;
				}
			}

			// Insert data in DB
			summoner.riotSummoner = riot_summoner;
			summoner.lastUpdate = time(nullptr) * 1000;
			serverBD->setInDBSummoner(summoner);

			serverBD->setInDBLeagues(league_positions);

			serverBD->setInDBMatchReference(summoner.riotSummoner.accountId, match_history);

			for (auto match : matchs)
			{
				serverBD->setInDBMatch(match);
				serverBD->setInDBParticipantsStats(match);
			}

			for(auto champion_stats : champions_stats)
			{
				serverBD->setInDBChampionStats(summoner.riotSummoner.accountId, champion_stats.second);
			}
		}
		return 1;
	}
}
