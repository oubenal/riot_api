#pragma once
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include "RiotObject.h"
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "LEA_Project.h"

namespace Helper
{

	inline void assignValue(bool& var, const sql::ResultSet* result_set, const std::string&& column_name)
	{
		var = result_set->getBoolean(column_name);
	}

	inline void assignValue(int& var, const sql::ResultSet* result_set, const std::string&& column_name)
	{
		var = result_set->getInt(column_name);
	}

	inline void assignValue(long long& var, const sql::ResultSet* result_set, const std::string&& column_name)
	{
		var = result_set->getInt64(column_name);
	}

	inline void assignValue(double& var, const sql::ResultSet* result_set, const std::string&& column_name)
	{
		var = result_set->getDouble(column_name);
	}

	inline void assignValue(std::string& var, const sql::ResultSet* result_set, const std::string&& column_name)
	{
		var = result_set->getString(column_name);
	}

	inline void setStatment(sql::PreparedStatement &prep_stmt, int pos, const bool& val)
	{
		prep_stmt.setInt(pos, val);
	}

	inline void setStatment(sql::PreparedStatement &prep_stmt, int pos, const int& val)
	{
		prep_stmt.setInt(pos, val);
	}

	inline void setStatment(sql::PreparedStatement &prep_stmt, int pos, const long long& val)
	{
		prep_stmt.setInt64(pos, val);
	}

	inline void setStatment(sql::PreparedStatement &prep_stmt, int pos, const double&val)
	{
		prep_stmt.setDouble(pos, val);

	}

	inline void setStatment(sql::PreparedStatement &prep_stmt, int pos, const std::string& val)
	{
		prep_stmt.setString(pos, val);
	}
}


class ServerBD
{
public:
	static ServerBD* getInstance()
	{
		static ServerBD serverBD;

		return &serverBD;
	}

	int getFromDBSummonerByName(const std::string& name, LEA_Project::Summoner& summoner) const;

	void setInDBSummoner(const LEA_Project::Summoner& summoner) const;

	int getFromDBLeaguesBySummonerId(int64_t id, std::list<Riot::LeaguePosition>& league_positions) const;

	void setInDBLeagues(const std::list<Riot::LeaguePosition>& league_positions) const;

	int getFromDBMatchReference(int64_t accountId, Riot::Matchlist& match_history) const;

	void setInDBMatchReference(int64_t accountId, const Riot::Matchlist& match_history) const;

	int getFromDBParticipantStats(int64_t gameId, Riot::Match& match) const;

	void setInDBParticipantsStats(const Riot::Match& match) const;
	
	int getFromDBMatch(int64_t gameId, Riot::Match& match) const;

	void setInDBMatch(const Riot::Match& match) const;

	std::string loadKeyChampById(int champion_id) const;

	int getFromDBChampionStats(int64_t accountId, std::map<int, LEA_Project::ChampionStats>& champions_stats) const;
	
	void setInDBChampionStats(int64_t accountId, const LEA_Project::ChampionStats& champion_stats) const;

	int getListChampions(std::map<int, Riot::Champion>& champion_list) const;

	int getListSpells(std::map<int, Riot::Spell>& spell_list) const;

	int getChampionStatsRankByAllFromDB(int64_t accountId, int championId) const;

	int getChampionStatsRankByCountryFromDB(int64_t accountId, std::string country, int championId) const;
	
	void queueSummoner(const std::string& name) const;

	int popSummoner(LEA_Project::QueueEl& summoner) const;

	int getRankByCountry(LEA_Project::Summoner& summoner) const;

private:
	ServerBD();// default constructor available only to members or friends of this class
	ServerBD(const ServerBD &old) = delete; // disallow copy constructor
	const ServerBD &operator=(const ServerBD &old) = delete; //disallow assignment operator
	~ServerBD();

private: // private data for an instance of this class
	sql::Driver *driver;
	sql::Connection *con;
};

