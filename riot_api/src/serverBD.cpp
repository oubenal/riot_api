#include <riot_api/ServerBD.h>



ServerBD::ServerBD()
{
	try
	{
		driver = get_driver_instance();
		con = driver->connect(_HOST, _USER, _PASS);
		con->setSchema(_DB);
	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
}


ServerBD::~ServerBD()
{
	delete con;
	//  Connector/C++ takes care of freeing driver
}

int ServerBD::getFromDBSummonerByName(const std::string& name, LEA_Project::Summoner& summoner) const
{
	// Query DB
	auto statement = con->createStatement();
	auto query_base = "SELECT * FROM riot_api.summoner as s WHERE s.name = '" + name + "';";
	try
	{
		auto result_set = statement->executeQuery(query_base.c_str());
		if (result_set->next())
		{
			Helper::assignValue(summoner.riotSummoner.id, result_set, "id");
			Helper::assignValue(summoner.riotSummoner.accountId, result_set, "accountId");
			Helper::assignValue(summoner.riotSummoner.name, result_set, "name");
			Helper::assignValue(summoner.riotSummoner.revisionDate, result_set, "revisionDate");
			Helper::assignValue(summoner.riotSummoner.summonerLevel, result_set, "summonerLevel");
			Helper::assignValue(summoner.riotSummoner.profileIconId, result_set, "profileIconId");
			Helper::assignValue(summoner.lastUpdate, result_set, "lastUpdate");
			delete result_set;
			delete statement;
			return 1;
		}
		delete result_set;
		delete statement;
		return 0; // Summoner not found in DB
	}
	catch(sql::SQLException e)
	{
		delete statement;
		return 0; // Summoner not found in DB
	}
}

void ServerBD::setInDBSummoner(const LEA_Project::Summoner& summoner) const
{
	std::string query_base = "INSERT INTO riot_api.summoner(profileIconId, name, summonerLevel, revisionDate, id, accountId, lastUpdate) VALUES(?, ?, ?, ?, ?, ?, ?) ON DUPLICATE KEY UPDATE profileIconId = VALUES(profileIconId), name = VALUES(name), summonerLevel = VALUES(summonerLevel), revisionDate = VALUES(revisionDate), lastUpdate = VALUES(lastUpdate);";
	auto prep_stmt = std::unique_ptr<sql::PreparedStatement>(con->prepareStatement(query_base));
	//query_base += to_string(summoner.profileIconId) + "', '" + summoner.name + "', '" + to_string(summoner.summonerLevel) + "', '" + to_string(summoner.revisionDate) + "', '" + to_string(summoner.id) + "', '" + to_string(summoner.accountId) + "');";

	Helper::setStatment(*prep_stmt, 1, summoner.riotSummoner.profileIconId);
	Helper::setStatment(*prep_stmt, 2, summoner.riotSummoner.name);
	Helper::setStatment(*prep_stmt, 3, summoner.riotSummoner.summonerLevel);
	Helper::setStatment(*prep_stmt, 4, summoner.riotSummoner.revisionDate);
	Helper::setStatment(*prep_stmt, 5, summoner.riotSummoner.id);
	Helper::setStatment(*prep_stmt, 6, summoner.riotSummoner.accountId);
	Helper::setStatment(*prep_stmt, 7, summoner.lastUpdate);

	try
	{
		prep_stmt->executeUpdate();
	}
	catch (sql::SQLException& e)
	{
		std::cout << "#Method : setInDBSummoner \n";
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
}

int ServerBD::getFromDBLeaguesBySummonerId(int64_t id, std::list<Riot::LeaguePosition>& league_positions) const
{
	// Query DB
	auto statement = con->createStatement();
	auto query_base = "SELECT * FROM riot_api.league as l WHERE l.playerOrTeamId = '" + std::to_string(id) + "' ;";
	try
	{
		auto result_set = statement->executeQuery(query_base);

		if (result_set->next())
		{
			do
			{
				Riot::LeaguePosition league_position = {};
				// Helper::assignValue(, result_set, "");
				Helper::assignValue(league_position.queueType, result_set, "queueType");
				Helper::assignValue(league_position.playerOrTeamId, result_set, "playerOrTeamId");
				Helper::assignValue(league_position.hotStreak, result_set, "hotStreak");
				Helper::assignValue(league_position.wins, result_set, "wins");
				Helper::assignValue(league_position.losses, result_set, "losses");
				Helper::assignValue(league_position.tier, result_set, "tier");
				Helper::assignValue(league_position.rank, result_set, "rank");
				Helper::assignValue(league_position.leaguePoints, result_set, "leaguePoints");
				Helper::assignValue(league_position.leagueName, result_set, "leagueName");

				league_positions.push_front(league_position);
			} while (result_set->next());
			delete result_set;
			delete statement;
			return 1;
		}
		delete result_set;
		delete statement;
		return 0; // Summoner not found in DB
	}
	catch (sql::SQLException e)
	{
		delete statement;
		return 0; // Summoner not found in DB
	}
}

void ServerBD::setInDBLeagues(const std::list<Riot::LeaguePosition>& league_positions) const
{
	std::string query_base = "INSERT INTO riot_api.league(playerOrTeamId, queueType, hotStreak, wins, losses, tier, rank, leaguePoints, leagueName) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?) ON DUPLICATE KEY UPDATE wins = VALUES(wins), losses = VALUES(losses), tier = VALUES(tier), rank = VALUES(rank), leaguePoints = VALUES(leaguePoints), hotStreak = VALUES(hotStreak), leagueName = VALUES(leagueName);";
	auto prep_stmt = std::unique_ptr<sql::PreparedStatement>(con->prepareStatement(query_base));
	for (auto league_position : league_positions)
	{
		Helper::setStatment(*prep_stmt, 1, league_position.playerOrTeamId);
		Helper::setStatment(*prep_stmt, 2, league_position.queueType);
		Helper::setStatment(*prep_stmt, 3, league_position.hotStreak);
		Helper::setStatment(*prep_stmt, 4, league_position.wins);
		Helper::setStatment(*prep_stmt, 5, league_position.losses);
		Helper::setStatment(*prep_stmt, 6, league_position.tier);
		Helper::setStatment(*prep_stmt, 7, league_position.rank);
		Helper::setStatment(*prep_stmt, 8, league_position.leaguePoints);
		Helper::setStatment(*prep_stmt, 9, league_position.leagueName);

		try
		{
			prep_stmt->executeUpdate();
		}
		catch (sql::SQLException& e)
		{
			std::cout << "#Method : setInDBLeagues \n";
			std::cout << "# ERR: SQLException in " << __FILE__;
			std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
			std::cout << "# ERR: " << e.what();
			std::cout << " (MySQL error code: " << e.getErrorCode();
			std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		}
		//query_base += league_position.playerOrTeamId + "', '" + league_position.queueType + "', '" + to_string(league_position.hotStreak) + "', '" + to_string(league_position.wins) + "', '" + to_string(league_position.losses) + "', '" + league_position.tier + "', '" + league_position.rank + "', '" + to_string(league_position.leaguePoints) + "');";
	}
}

int ServerBD::getFromDBMatchReference(int64_t accountId, Riot::Matchlist& match_history) const
{
	// Query DB
	auto statement = con->createStatement();
	auto query_base = "SELECT * FROM riot_api.matchreference as mr WHERE mr.accountId = '" + std::to_string(accountId) + "' ORDER BY timestamp DESC;";
	try
	{
		auto result_set = statement->executeQuery(query_base.c_str());

		if (result_set->next())
		{
			do
			{
				Riot::MatchReference match_reference = {};
				Helper::assignValue(match_reference.gameId, result_set, "gameId");
				Helper::assignValue(match_reference.lane, result_set, "lane");
				Helper::assignValue(match_reference.champion, result_set, "champion");
				Helper::assignValue(match_reference.platformId, result_set, "platformId");
				Helper::assignValue(match_reference.timestamp, result_set, "timestamp");
				Helper::assignValue(match_reference.queue, result_set, "queue");
				Helper::assignValue(match_reference.role, result_set, "role");
				Helper::assignValue(match_reference.season, result_set, "season");

				match_history.matches.push_back(match_reference);
			} while (result_set->next());
			match_history.totalGames = match_history.matches.size();
			match_history.startIndex = 0;
			match_history.endIndex = match_history.totalGames;

			delete result_set;
			delete statement;
			return match_history.totalGames;
		}
		delete result_set;
		delete statement;
		return 0; // Summoner not found in DB
	}
	catch (sql::SQLException e)
	{
		delete statement;
		return 0; // Summoner not found in DB
	}
}

void ServerBD::setInDBMatchReference(int64_t accountId, const Riot::Matchlist& match_history) const
{
	std::string query_base = "INSERT INTO riot_api.matchreference(lane, champion, platformId, season, queue, role, timestamp, gameId, accountId) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";// " ON DUPLICATE KEY UPDATE lane = VALUES(lane), champion = VALUES(champion), platformId = VALUES(platformId), season = VALUES(season), queue = VALUES(queue), role = VALUES(role), timestamp = VALUES(timestamp);";
	auto prep_stmt = std::unique_ptr<sql::PreparedStatement>(con->prepareStatement(query_base));
	for (auto match_ref : match_history.matches)
	{
		Helper::setStatment(*prep_stmt, 1, match_ref.lane);
		Helper::setStatment(*prep_stmt, 2, match_ref.champion);
		Helper::setStatment(*prep_stmt, 3, match_ref.platformId);
		Helper::setStatment(*prep_stmt, 4, match_ref.season);
		Helper::setStatment(*prep_stmt, 5, match_ref.queue);
		Helper::setStatment(*prep_stmt, 6, match_ref.role);
		Helper::setStatment(*prep_stmt, 7, match_ref.timestamp);
		Helper::setStatment(*prep_stmt, 8, match_ref.gameId);
		Helper::setStatment(*prep_stmt, 9, accountId);

		try
		{
			prep_stmt->executeUpdate();
		}
		catch (sql::SQLException& e)
		{
			std::cout << "";
			std::cout << "#Method : setInDBMatchReference \n";
			std::cout << "# ERR: SQLException in " << __FILE__;
			std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
			std::cout << "# ERR: " << e.what();
			std::cout << " (MySQL error code: " << e.getErrorCode();
			std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		}
		//query_base += match_ref.lane + "', '" + to_string(match_ref.champion) + "', '" + match_ref.platformId + "', '" + to_string(match_ref.season) + "', '" + to_string(match_ref.queue) + "', '" + match_ref.role + "', '" + to_string(match_ref.timestamp) + "', '" + to_string(match_ref.gameId) + "', '" + to_string(accountId) + "');";
	}
}

int ServerBD::getFromDBParticipantStats(int64_t gameId, Riot::Match& match) const
{
	// Query DB
	auto statement = con->createStatement();
	auto query_base = "SELECT * FROM riot_api.participantstats as mr WHERE mr.gameId = '" + std::to_string(gameId) + "';";
	
	try {
		auto result_set = statement->executeQuery(query_base.c_str());
		match.participants.resize(10);
		if (result_set->next())
		{
			do
			{
				Riot::Participant participant;
				Helper::assignValue(participant.participantId, result_set, "participantId");
				Helper::assignValue(participant.spell1Id, result_set, "spell1Id");
				Helper::assignValue(participant.spell2Id, result_set, "spell2Id");
				Helper::assignValue(participant.highestAchievedSeasonTier, result_set, "highestAchievedSeasonTier");
				Helper::assignValue(participant.stats.win, result_set, "win");
				Helper::assignValue(participant.stats.kills, result_set, "kills");
				Helper::assignValue(participant.stats.deaths, result_set, "deaths");
				Helper::assignValue(participant.stats.assists, result_set, "assists");
				Helper::assignValue(participant.stats.totalMinionsKilled, result_set, "totalMinionsKilled");
				Helper::assignValue(participant.teamId, result_set, "teamId");
				Helper::assignValue(participant.championId, result_set, "championId");
				Helper::assignValue(participant.stats.item0, result_set, "item0");
				Helper::assignValue(participant.stats.item1, result_set, "item1");
				Helper::assignValue(participant.stats.item2, result_set, "item2");
				Helper::assignValue(participant.stats.item3, result_set, "item3");
				Helper::assignValue(participant.stats.item4, result_set, "item4");
				Helper::assignValue(participant.stats.item5, result_set, "item5");
				Helper::assignValue(participant.stats.item6, result_set, "item6");
				Helper::assignValue(participant.stats.visionWardsBoughtInGame, result_set, "visionWards");
				Helper::assignValue(participant.stats.champLevel, result_set, "champLevel");
				Helper::assignValue(match.participantIdentities[participant.participantId - 1].participantId, result_set, "participantId");
				Helper::assignValue(match.participantIdentities[participant.participantId - 1].player.summonerId, result_set, "summonerId");
				Helper::assignValue(match.participantIdentities[participant.participantId - 1].player.summonerName, result_set, "summonerName");

				match.participants[participant.participantId - 1] = participant;

			} while (result_set->next());
			delete result_set;
			delete statement;
			return 1;
		}
		delete result_set;
		delete statement;
		return 0; // Summoner not found in DB
	}
	catch(sql::SQLException e)
	{
		delete statement;
		return 0;
	}
}

void ServerBD::setInDBParticipantsStats(const Riot::Match& match) const
{
	std::string query_base = "INSERT INTO riot_api.participantstats(summonerId, gameId, highestAchievedSeasonTier, spell1Id, spell2Id, teamId, championId, win, kills, deaths, assists, totalMinionsKilled, participantId, summonerName, item0, item1, item2, item3, item4, item5, item6, visionWards, champLevel) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) ON DUPLICATE KEY UPDATE highestAchievedSeasonTier = VALUES(highestAchievedSeasonTier), spell1Id = VALUES(spell1Id), spell2Id = VALUES(spell2Id), teamId = VALUES(teamId), championId = VALUES(championId), win = VALUES(win), kills = VALUES(kills), deaths = VALUES(deaths), assists= VALUES(assists), totalMinionsKilled = VALUES(totalMinionsKilled), participantId = VALUES(participantId), summonerName = VALUES(summonerName), item0 = VALUES(item0), item1 = VALUES(item1), item2 = VALUES(item2), item3 = VALUES(item3), item4 = VALUES(item4), item5 = VALUES(item5), item6 = VALUES(item6), visionWards = VALUES(visionWards), champLevel = VALUES(champLevel);";
	auto prep_stmt = std::unique_ptr<sql::PreparedStatement>(con->prepareStatement(query_base));

	for (auto participant : match.participants)
	{
		Helper::setStatment(*prep_stmt, 1, match.participantIdentities[participant.participantId - 1].player.summonerId);
		Helper::setStatment(*prep_stmt, 2, match.gameId);
		Helper::setStatment(*prep_stmt, 3, participant.highestAchievedSeasonTier);
		Helper::setStatment(*prep_stmt, 4, participant.spell1Id);
		Helper::setStatment(*prep_stmt, 5, participant.spell2Id);
		Helper::setStatment(*prep_stmt, 6, participant.teamId);
		Helper::setStatment(*prep_stmt, 7, participant.championId);
		Helper::setStatment(*prep_stmt, 8, participant.stats.win);
		Helper::setStatment(*prep_stmt, 9, participant.stats.kills);
		Helper::setStatment(*prep_stmt, 10, participant.stats.deaths);
		Helper::setStatment(*prep_stmt, 11, participant.stats.assists);
		Helper::setStatment(*prep_stmt, 12, participant.stats.totalMinionsKilled);
		Helper::setStatment(*prep_stmt, 13, participant.participantId);
		Helper::setStatment(*prep_stmt, 14, match.participantIdentities[participant.participantId - 1].player.summonerName);
		// summonerName, item0, item1, item2, item3, item4, item5, item6, visionWards, champLevel
		Helper::setStatment(*prep_stmt, 15, participant.stats.item0);
		Helper::setStatment(*prep_stmt, 16, participant.stats.item1);
		Helper::setStatment(*prep_stmt, 17, participant.stats.item2);
		Helper::setStatment(*prep_stmt, 18, participant.stats.item3);
		Helper::setStatment(*prep_stmt, 19, participant.stats.item4);
		Helper::setStatment(*prep_stmt, 20, participant.stats.item5);
		Helper::setStatment(*prep_stmt, 21, participant.stats.item6);
		Helper::setStatment(*prep_stmt, 22, participant.stats.visionWardsBoughtInGame);
		Helper::setStatment(*prep_stmt, 23, participant.stats.champLevel);
		//Helper::build_parameterised_query(*prep_stmt, participant.participantId, gameId, participant.highestAchievedSeasonTier, participant.spell1Id, participant.spell2Id, participant.teamId, participant.championId, participant.championId, participant.stats.win, participant.stats.kills, participant.stats.deaths, participant.stats.assists, participant.stats.totalMinionsKilled);
		try
		{
			prep_stmt->executeUpdate();
		}
		catch (sql::SQLException& e)
		{
			std::cout << match.participantIdentities[participant.participantId - 1].player.summonerName << "\n";
			std::cout << "#Method : setInDBParticipantsStats \n";
			std::cout << "# ERR: SQLException in " << __FILE__;
			std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
			std::cout << "# ERR: " << e.what();
			std::cout << " (MySQL error code: " << e.getErrorCode();
			std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		}

		//query_base += to_string(p.participantId) + "', '" + to_string(gameId) + "', '" + p.highestAchievedSeasonTier + "', '" + to_string(p.spell1Id) + "', '" + to_string(p.spell2Id) + "', '" + to_string(p.teamId) + "', '" + to_string(p.championId) + "', '" + to_string(p.stats.win ? 1:0) + "', '" + to_string(p.stats.kills) + "', '" + to_string(p.stats.deaths) + "', '" + to_string(p.stats.assists) + "', '" + to_string(p.stats.totalMinionsKilled) + "');";
	}
	//delete prep_stmt;
}

std::string ServerBD::loadKeyChampById(int champion_id) const
{
	try
	{
		std::string key;

		// Query DB
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* res = stmt->executeQuery("SELECT c.key FROM riot_api.champions as c WHERE id=" + std::to_string(champion_id) + ";");
		// Parse query result
		while (res->next())
		{
			Helper::assignValue(key, res, "key");
			// Access column data by numeric offset, 1 is the first column
			//cout << res->getString(1) << endl;
		}
		// Delete object used
		delete res;
		delete stmt;
		return key;
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
	return {};
}

int ServerBD::getFromDBChampionStats(int64_t accountId, std::map<int, LEA_Project::ChampionStats>& champions_stats) const
{
	// Query DB
	auto statement = con->createStatement();
	auto query_base = "SELECT * FROM riot_api.championsstats as mr WHERE mr.accountId = '" + std::to_string(accountId) + "';";
	try
	{
		auto result_set = statement->executeQuery(query_base.c_str());

		if (result_set->next())
		{
			do
			{
				LEA_Project::ChampionStats champion_stats = {};
				Helper::assignValue(champion_stats.championId, result_set, "championId");
				Helper::assignValue(champion_stats.kills, result_set, "kills");
				Helper::assignValue(champion_stats.deaths, result_set, "deaths");
				Helper::assignValue(champion_stats.assists, result_set, "assists");
				Helper::assignValue(champion_stats.cs, result_set, "cs");
				Helper::assignValue(champion_stats.wins, result_set, "wins");
				Helper::assignValue(champion_stats.losses, result_set, "losses");
				Helper::assignValue(champion_stats.accountId, result_set, "accountId");

				champions_stats[champion_stats.championId] = champion_stats;
			} while (result_set->next());
			delete result_set;
			delete statement;
			return 1;
		}
		delete result_set;
		delete statement;
		return 0;
	}
	catch(sql::SQLException e)
	{
		delete statement;
		return 0;
	}
}

void ServerBD::setInDBChampionStats(int64_t accountId, const std::map<int, LEA_Project::ChampionStats>& champions_stats) const
{
	std::string query_base = "INSERT INTO riot_api.championsstats(championId, kills, deaths, assists, cs, wins, losses, accountId) VALUES (?, ?, ?, ?, ?, ?, ?, ?) ON DUPLICATE KEY UPDATE kills = (kills*(wins+losses) + VALUES(kills))/(wins+losses+VALUES(wins)+VALUES(losses)), deaths = (deaths*(wins+losses) + VALUES(deaths))/(wins+losses+VALUES(wins)+VALUES(losses)), assists = (assists*(wins+losses) + VALUES(assists))/(wins+losses+VALUES(wins)+VALUES(losses)), cs = cs + VALUES(cs), wins = wins + VALUES(wins), losses = losses + VALUES(losses);";
	auto prep_stmt = std::unique_ptr<sql::PreparedStatement>(con->prepareStatement(query_base));

	for (auto champion_stats : champions_stats)
	{
		Helper::setStatment(*prep_stmt, 1, champion_stats.second.championId);
		Helper::setStatment(*prep_stmt, 2, champion_stats.second.kills);
		Helper::setStatment(*prep_stmt, 3, champion_stats.second.deaths);
		Helper::setStatment(*prep_stmt, 4, champion_stats.second.assists);
		Helper::setStatment(*prep_stmt, 5, champion_stats.second.cs);
		Helper::setStatment(*prep_stmt, 6, champion_stats.second.wins);
		Helper::setStatment(*prep_stmt, 7, champion_stats.second.losses);
		Helper::setStatment(*prep_stmt, 8, champion_stats.second.accountId);

		try
		{
			prep_stmt->executeUpdate();
		}
		catch (sql::SQLException& e)
		{
			std::cout << "#Method : setInDBParticipantsStats \n";
			std::cout << "# ERR: SQLException in " << __FILE__;
			std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
			std::cout << "# ERR: " << e.what();
			std::cout << " (MySQL error code: " << e.getErrorCode();
			std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		}

		//query_base += to_string(p.participantId) + "', '" + to_string(gameId) + "', '" + p.highestAchievedSeasonTier + "', '" + to_string(p.spell1Id) + "', '" + to_string(p.spell2Id) + "', '" + to_string(p.teamId) + "', '" + to_string(p.championId) + "', '" + to_string(p.stats.win ? 1:0) + "', '" + to_string(p.stats.kills) + "', '" + to_string(p.stats.deaths) + "', '" + to_string(p.stats.assists) + "', '" + to_string(p.stats.totalMinionsKilled) + "');";
	}
	//delete prep_stmt;
}

int ServerBD::getListChampions(std::map<int, Riot::Champion>& champion_list) const
{
	// Query DB
	auto statement = con->createStatement();
	auto result_set = statement->executeQuery("SELECT * FROM riot_api.champions;");

	if (result_set->next())
	{
		do
		{
			Riot::Champion champion = {};
			Helper::assignValue(champion.id, result_set, "id");
			Helper::assignValue(champion.key, result_set, "key");
			Helper::assignValue(champion.name, result_set, "name");
			Helper::assignValue(champion.title, result_set, "title");
			
			champion_list[champion.id] = champion;
		}
		while (result_set->next());
		delete result_set;
		delete statement;
		return 1;
	}
	delete result_set;
	delete statement;
	return 0; // Summoner not found in DB
}

int ServerBD::getFromDBMatch(int64_t gameId, Riot::Match& match) const
{
	// Query DB
	auto statement = con->createStatement();
	auto query_base = "SELECT * FROM riot_api.match as m WHERE m.gameId = '" + std::to_string(gameId) + "';";
	try
	{
		auto result_set = statement->executeQuery(query_base.c_str());
		if (result_set->next())
		{
			Helper::assignValue(match.gameId, result_set, "gameId");
			Helper::assignValue(match.gameCreation, result_set, "gameCreation");
			Helper::assignValue(match.gameDuration, result_set, "gameDuration");
			Helper::assignValue(match.queueId, result_set, "queueId");
			Helper::assignValue(match.seasonId, result_set, "seasonId");
			delete result_set;
			delete statement;
			return 1;
		}
		delete result_set;
		delete statement;
		return 0; // Summoner not found in DB
	}
	catch (sql::SQLException e)
	{
		delete statement;
		return 0; // Summoner not found in DB
	}
}

void ServerBD::setInDBMatch(const Riot::Match& match) const
{
	std::string query_base = "INSERT INTO riot_api.match(gameId, gameDuration, gameCreation, seasonId, queueId) VALUES (?, ?, ?, ?, ?)  ON DUPLICATE KEY UPDATE gameId = VALUES(gameId), gameDuration = VALUES(gameDuration), gameCreation = VALUES(gameCreation), seasonId = VALUES(seasonId), queueId = VALUES(queueId);"; 
	auto prep_stmt = std::unique_ptr<sql::PreparedStatement>(con->prepareStatement(query_base));
	Helper::setStatment(*prep_stmt, 1, match.gameId);
	Helper::setStatment(*prep_stmt, 2, match.gameDuration);
	Helper::setStatment(*prep_stmt, 3, match.gameCreation);
	Helper::setStatment(*prep_stmt, 4, match.seasonId);
	Helper::setStatment(*prep_stmt, 5, match.queueId);
	try
	{
		prep_stmt->executeUpdate();
	}
	catch (sql::SQLException& e)
	{
		std::cout << "#Method : setInDBMatchList \n";
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
	
}
