#ifndef RIOT_HPP
#define RIOT_HPP
#include <string>
#include "Summoner.h"
#include <rapidjson/document.h>
#include <map>
#include <vector>

namespace Riot
{
	/* By default, functions will use these fields
	unless otherwise specified. */
	extern std::string api_key; // ""
	//extern Region region;       // Region::NA
	//extern Locale locale;       // Locale::en_US
	extern std::string base_url;

	// ---------- API Resource: SUMMONER-V3 ---------- //
	Summoner getSummonerData(std::string name);

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

#endif // RIOT_API_HPP