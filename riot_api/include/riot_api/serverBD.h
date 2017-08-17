#pragma once
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include "RiotObject.h"
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "LEA_Project.h"

#define _HOST "localhost"
#define _USER "root"
#define _PASS "r4yzAnm5"
#define _DB "riot_api"

namespace Helper
{

	namespace detail {

		//
		// these overloads perform the correct action depending on the argument type
		//
		inline void parameterise(sql::PreparedStatement& stmt, std::size_t column, std::string const& value) {
			stmt.setString(column, value);
		}

		inline void parameterise(sql::PreparedStatement& stmt, std::size_t column, const char* value) {
			stmt.setString(column, value);
		}

		inline void parameterise(sql::PreparedStatement& stmt, std::size_t column, int value) {
			stmt.setInt(column, value);
		}

		inline void parameterise(sql::PreparedStatement& stmt, std::size_t column, long long value) {
			stmt.setInt64(column, value);
		}

		inline void parameterise(sql::PreparedStatement& stmt, std::size_t column, double value) {
			stmt.setDouble(column, value);
		}

		inline void parameterise(sql::PreparedStatement& stmt, std::size_t column, bool value) {
			stmt.setInt(column, value);
		}
		//
		// variadic parameteriser
		//
		template<class Tuple, std::size_t...Is>
		void parameterise(sql::PreparedStatement& stmt, Tuple&& tuple, std::index_sequence<Is...>, std::size_t initial_index = 1)
		{
			using expand = int[];
			void(expand{ 0,
				(parameterise(stmt, Is + initial_index, std::get<Is>(std::forward<Tuple>(tuple))),0)...
			});
		};
	}

	template<class...Args>
	void build_parameterised_query(sql::PreparedStatement& stmt, Args&&...args)
	{
		//
		// in mysqcppconn, this bit would be
		//
		//auto stmt = con->prepareStatement(sql);

		// but the rest would be the same

		detail::parameterise(stmt,
			std::forward_as_tuple(std::forward<Args>(args)...),
			std::make_index_sequence<sizeof...(Args)>());

	}


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

	int getFromDBSummonerByName(const std::string& name, Riot::Summoner& summoner) const;

	void setInDBSummoner(const Riot::Summoner& summoner) const;


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
	
	void setInDBChampionStats(int64_t accountId, const std::map<int, LEA_Project::ChampionStats>& champions_stats) const;

	int getListChampions(std::map<int, Riot::Champion>& champion_list) const;

private:
	ServerBD();// default constructor available only to members or friends of this class
	ServerBD(const ServerBD &old) = delete; // disallow copy constructor
	const ServerBD &operator=(const ServerBD &old) = delete; //disallow assignment operator
	~ServerBD();

private: // private data for an instance of this class
	sql::Driver *driver;
	sql::Connection *con;
};
