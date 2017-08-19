#include <riot_api/Riot.h>
#include "riot_api/URLReader.h"
#include <rapidjson/document.h>
#include "rapidjson/encodedstream.h"    // EncodedInputStream


#include "riot_api/RiotParser.h"
#include <iostream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <string>
#include <sstream>

namespace Riot
{

	std::string hex(unsigned int c)
	{
		std::ostringstream stm;
		if (c < 161)
			stm << '%' << std::hex << std::uppercase << c;
		else if (c < 192)
			stm << '%' << 'C' << '2' << '%' << std::hex << std::uppercase << c;
		else
			stm << '%' << 'C' << '3' << '%' << std::hex << std::uppercase << c - 64;
		return stm.str();
	}

	std::string name_encode(const std::string& str)
	{
		static const std::string unreserved = "0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"-_.~";
		std::string result;

		for (unsigned char c : str)
		{
			if (unreserved.find(c) != std::string::npos) result += c;
			else result += hex(c);
		}

		return result;
	}

	std::string api_key = "RGAPI-8a620b99-4faf-41ad-b9aa-d3a2b23094a0";
	std::string base_url = "https://euw1.api.riotgames.com/lol/";

	// ---------- API Resource: SUMMONER-V3 ---------- //
	Summoner getSummonerSummonersByName(const std::string &name)
	{
		auto n_name = name_encode(name);
		auto url =
			base_url +
			"summoner/v3/summoners/by-name/" +
			n_name +
			"?api_key=" +
			api_key;

		// JSON parse
		rapidjson::Document json;
		json.Parse<0>(URLReader::read(url).c_str());
		Summoner summoner = {};
		try
		{
			summoner = parseSummonerSummoners(json);
		}
		catch (Riot::URLReader::URLReaderException& e)
		{
			std::cout << e.what() << '\n';
		}
		return summoner;
	}

	// ---------- API Resource: LEAGUE-V3 ---------- //
	std::list<LeaguePosition> getLeaguePositionsBySummoner(int64_t summonerId)
	{
		auto url =
			base_url +
			"league/v3/positions/by-summoner/" +
			std::to_string(summonerId) +
			"?api_key=" +
			api_key;

		// JSON parse
		rapidjson::Document json;
		json.Parse<0>(URLReader::read(url).c_str());

		return Riot::parseLeaguePositions(json);
	}

	Matchlist getMatchlistsByAccountRecent(int64_t account_id, int nb_game)
	{
		return getMatchlistsByAccount(account_id, 0, nb_game, 420, 9); // 420, 440
	}

	Matchlist getMatchlistsByAccount(int64_t account_id, int begin_index, int end_index, int queue, int season)
	{
		///lol/match/v3/matchlists/by-account/{accountId}
		auto url =
			base_url +
			"match/v3/matchlists/by-account/" +
			std::to_string(account_id) +
			"?queue=" + std::to_string(queue) + "&endIndex=" + std::to_string(end_index) + "&season=" + std::to_string(season) + "&beginIndex=" + std::to_string(begin_index) + "&api_key=" +
			api_key;

		//JSON parse
		rapidjson::Document json;
		json.Parse<0>(URLReader::read(url).c_str());

		return Riot::parseMatchlist(json);
	}

	Match getMatchbyMatchId(int64_t game_id)
	{
		///lol/match/v3/matches/{matchId}
		auto url =
			base_url +
			"match/v3/matches/" +
			std::to_string(game_id) +
			"?api_key=" +
			api_key;

		Match match;
		//JSON parse
		rapidjson::Document json;
		auto res = URLReader::read(url);
		rapidjson::StringStream i_buffer(res.c_str());
		rapidjson::EncodedInputStream<rapidjson::UTF8<>, rapidjson::StringStream> s(i_buffer);
		json.ParseStream<0, rapidjson::UTF8<> >(s);
		rapidjson::StringBuffer o_buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(o_buffer);
		json.Accept(writer);
		
		return Riot::parseMatch(json);
	}
}
