#include "riot_api/RiotParser.h"
#include "riot_api/Riot.h"

namespace Riot
{
	Summoner parseSummonerSummoners(const rapidjson::Value& json)
	{
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

	std::list<LeaguePosition> parseLeaguePositions(const rapidjson::Value& json)
	{
		std::list<LeaguePosition> league_positions;
		// parse adecquatly
		if (json.IsArray())
		{
			for (size_t i = 0; i < json.Size(); ++i)
			{
				LeaguePosition league_position = {};
				Helper::assignValue(league_position.queueType, json[i]["queueType"]);
				Helper::assignValue(league_position.hotStreak, json[i]["hotStreak"]);
				Helper::assignValue(league_position.wins, json[i]["wins"]);
				Helper::assignValue(league_position.veteran, json[i]["veteran"]);
				Helper::assignValue(league_position.losses, json[i]["losses"]);
				Helper::assignValue(league_position.playerOrTeamId, json[i]["playerOrTeamId"]);
				Helper::assignValue(league_position.tier, json[i]["tier"]);
				Helper::assignValue(league_position.playerOrTeamName, json[i]["playerOrTeamName"]);
				Helper::assignValue(league_position.inactive, json[i]["inactive"]);
				Helper::assignValue(league_position.rank, json[i]["rank"]);
				Helper::assignValue(league_position.freshBlood, json[i]["freshBlood"]);
				Helper::assignValue(league_position.leagueName, json[i]["leagueName"]);
				Helper::assignValue(league_position.leaguePoints, json[i]["leaguePoints"]);

				league_positions.push_front(league_position);
			}

		}

		return league_positions;
	}


	// ----------The less glorious wall of helper functions---------- //
	namespace Helper
	{
		
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
