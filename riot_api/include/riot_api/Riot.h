#ifndef RIOT_HPP
#define RIOT_HPP
#include <string>
#include "RiotObject.h"
#include <rapidjson/document.h>
#include <map>
#include <vector>
#include <list>

namespace Riot
{
	/* By default, functions will use these fields
	unless otherwise specified. */
	extern std::string api_key; // ""
	//extern Region region;       // Region::NA
	//extern Locale locale;       // Locale::en_US
	extern std::string base_url;

	// ---------- API Resource: SUMMONER-V3 ---------- //
	Summoner getSummonerSummonersByName(const std::string &name);

	// ---------- API Resource: LEAGUE-V3 ---------- //
	std::list<LeaguePosition> getLeaguePositionsBySummoner(const Summoner &summoner);


	
}

#endif // RIOT_API_HPP