#ifndef SUMMONER_HPP
#define SUMMONER_HPP

#include <string>

namespace Riot
{
	struct Summoner
	{
		int profileIconId;
		std::string name;
		int64_t summonerLevel;
		int64_t revisionDate;
		int64_t id;
		int64_t accountId;
		int country = -1;
	};
}

#endif // SUMMONER_HPP