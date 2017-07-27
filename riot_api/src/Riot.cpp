#include <riot_api/Riot.h>
#include "riot_api/URLReader.h"
#include <rapidjson/document.h>
#include <iostream>

namespace Riot
{
	std::string api_key = "RGAPI-ae9a6b05-44dd-446c-814c-d41f12a97113";
	std::string base_url = "https://euw1.api.riotgames.com/lol/";

	// ---------- API Resource: SUMMONER-V3 ---------- //
	Summoner getSummonerData(std::string name)
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

		Summoner summoner = {};
		// parse adecquatly
		if (json.IsObject())
		{
			Helper::assignValue(summoner.name, json["name"]);
			Helper::assignValue(summoner.accountId, json["accountId"]);
			Helper::assignValue(summoner.id, json["id"]);
			Helper::assignValue(summoner.profileIconId, json["profileIconId"]);
			Helper::assignValue(summoner.revisionDate, json["revisionDate"]);
			Helper::assignValue(summoner.summonerLevel, json["summonerLevel"]);

		}
		
		return summoner;
	}



	namespace Helper
	{
		// ----------The less glorious wall of helper functions---------- //
		using namespace  rapidjson;
		void assignValue(bool& var, const Value& json)
		{
			if (json.IsBool())
				var = json.GetBool();
		}

		void assignValue(int& var, const Value& json)
		{
			if (json.IsInt())
				var = json.GetInt();
		}

		void assignValue(long long& var, const Value& json)
		{
			if (json.IsInt64())
				var = json.GetInt64();
		}

		void assignValue(double& var, const Value& json)
		{
			if (json.IsDouble())
				var = json.GetDouble();
		}

		void assignValue(std::string& var, const Value& json)
		{
			if (json.IsString())
				var = json.GetString();
		}
	}
}
