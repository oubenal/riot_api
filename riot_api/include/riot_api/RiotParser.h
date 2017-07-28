#ifndef RIOT_PARSER_HPP
#define RIOT_PARSER_HPP
#include <rapidjson/document.h>
#include "RiotObject.h"
#include <list>

namespace Riot
{


	std::list<LeaguePosition> parseLeaguePositions(const rapidjson::Value& json);
	Summoner parseSummonerSummoners(const rapidjson::Value& json);

	namespace Helper
	{
		// ----------The less glorious wall of helper functions---------- //
		using namespace  rapidjson;
		void assignValue(bool& var, const Value& json);
		void assignValue(int& var, const Value& json);
		void assignValue(long long& var, const Value& json);
		void assignValue(double& var, const Value& json);
		void assignValue(std::string& var, const Value& json);

	}
}

#endif // RIOT_PARSER_HPP