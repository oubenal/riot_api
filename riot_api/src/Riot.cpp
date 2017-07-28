#include <riot_api/Riot.h>
#include "riot_api/URLReader.h"
#include <rapidjson/document.h>
#include <iostream>
#include "riot_api/RiotParser.h"

namespace Riot
{
	

	std::string api_key = "RGAPI-d5872ef5-2b67-4557-bb32-0259f0e07762";
	std::string base_url = "https://euw1.api.riotgames.com/lol/";

	// ---------- API Resource: SUMMONER-V3 ---------- //
	Summoner getSummonerSummonersByName(const std::string &name)
	{
		auto url =
			base_url +
			"summoner/v3/summoners/by-name/" +
			name +
			"?api_key=" +
			api_key;

		// JSON parse
		rapidjson::Document json;
		json.Parse<0>(URLReader::read(url).c_str());

		return parseSummonerSummoners(json);
	}

	// ---------- API Resource: LEAGUE-V3 ---------- //
	std::list<LeaguePosition> getLeaguePositionsBySummoner(const Summoner &summoner)
	{
		auto url =
			base_url +
			"league/v3/positions/by-summoner/" +
			std::to_string(summoner.id) +
			"?api_key=" +
			api_key;

		// JSON parse
		rapidjson::Document json;
		json.Parse<0>(URLReader::read(url).c_str());

		return Riot::parseLeaguePositions(json);
	}
}
