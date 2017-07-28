#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include <boost/array.hpp>
#include <riot_api/URLReader.h>
#include <rapidjson/document.h>
#include <rapidjson/reader.h>
#include <riot_api/Riot.h>
/*
Include directly the different
headers from cppconn/ and mysql_driver.h + mysql_util.h
(and mysql_connection.h). This will reduce your build time!
*/
#include <mysql_connection.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

#define _HOST "localhost"
#define _USER "root"
#define _PASS "r4yzAnm5"
#define _DB "riot_api"

int main(int argc, char* argv[])
{
	
	// Connect and Retrive/Update MySQL DataBase 
	map<int, Riot::Champion> champions;

	try {
		cout << endl;
		cout << "Running 'SELECT 'Hello World!' AS _message'..." << endl;

		// Create a connection 
		sql::Driver *driver = get_driver_instance();
		sql::Connection *con = driver->connect(_HOST, _USER, _PASS);
		// Connect to the MySQL database 
		con->setSchema(_DB);
		// Query DB
		sql::Statement *stmt = con->createStatement();
		sql::ResultSet *res = stmt->executeQuery("SELECT * FROM riot_api.champions;");
		// Parse query result
		while (res->next()) {
			Riot::Champion champ;
			champ.id = res->getInt("id");
			champ.title = res->getString("title");
			champ.key = res->getString("key");
			champ.name = res->getString("name");
			/*
			// Access column data by alias or column name 
			cout << "\t... id = " << champ.id << endl;
			cout << "\t... title = " << champ.title << endl;
			cout << "\t... key = " << champ.key << endl;
			cout << "\t... name = " << champ.name << endl;
			*/
			champions[champ.id] = champ;
			// Access column data by numeric offset, 1 is the first column 
			//cout << res->getString(1) << endl;
		}
		// Delete object used
		delete res;
		delete stmt;
		delete con;
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line "	<< __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	
	// Access Riot API
	// Search by summonner's Name ang get info

	auto summoner = Riot::getSummonerSummonersByName("Zammbi");
	auto league_positions = Riot::getLeaguePositionsBySummoner(summoner);

	// show result
	cout << "\t... " << summoner.name << "'s profile = \n";
	cout << "\t... Level = " << summoner.summonerLevel << "\n\n";
	for(auto pos : league_positions)
	{
		cout << "\t... QueueType = " << pos.queueType << "\n";
		cout << "\t... hotStreak = " << pos.hotStreak << "\n";
		cout << "\t... wins = " << pos.wins << "\n";
		cout << "\t... losses = " << pos.losses << "\n";
		cout << "\t... tier = " << pos.tier << " " << pos.rank << "\n";
		cout << "\t... LP = " << pos.leaguePoints << "\n\n";
	}
	
	cout << endl;

	return EXIT_SUCCESS;
}
