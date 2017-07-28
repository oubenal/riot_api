#ifndef RIOTOBJECT_HPP
#define RIOTOBJECT_HPP

#include <string>

namespace Riot
{
	struct MiniSeries;

	struct Summoner
	{
		/* Example :
{

}
		 */
		int profileIconId;
		std::string name;
		int64_t summonerLevel;
		int64_t revisionDate;
		int64_t id;
		int64_t accountId;
		int country = -1;
	};

	struct Champion
	{
		/* Example :
		{

		}
		*/
		int id;
		std::string title;
		std::string key;
		std::string name;
	};

	struct MiniSeries
	{
		/* Example :
		{

		}
		*/
		int wins;
		int losses;
		int target;
		std::string progress;
	};

	struct LeaguePosition
	{
		/* Example :
{
"queueType": "RANKED_SOLO_5x5",
"hotStreak": false,
"wins": 158,
"veteran": true,
"losses": 156,
"playerOrTeamId": "49238543",
"tier": "PLATINUM",
"playerOrTeamName": "Zammbi",
"inactive": false,
"rank": "II",
"freshBlood": false,
"leagueName": "Lee Sin's Captains",
"leaguePoints": 41
}
		 */
		std::string queueType; //
		bool hotStreak; //
		int wins; //
		bool veteran; //
		int losses; //
		std::string playerOrTeamId; //
		std::string tier;
		std::string playerOrTeamName; //
		bool inactive; //
		std::string rank; //
		bool freshBlood; //
		std::string leagueName; //
		int leaguePoints;

		MiniSeries miniSeries;
	};

	
}

#endif //  RIOTOBJECT_HPP