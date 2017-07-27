#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include <boost/array.hpp>
#include <riot_api/URLReader.h>
#include <rapidjson/document.h>
#include <rapidjson/reader.h>
#include <riot_api/Riot.h>

int main(int argc, char* argv[])
{
/*	// Verify if player already in DB
	sql::mysql::MySQL_Driver *driver;
	sql::Connection *con;

	driver = sql::mysql::get_mysql_driver_instance();
	con = driver->connect("tcp://127.0.0.1:3306", "user", "password");

	delete con;
	*/
	// Search by summonner's Name ang get info

	auto res = Riot::getSummonerData("Zammbi");


	return 0;
}
