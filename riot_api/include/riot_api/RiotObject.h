#pragma once
#ifndef RIOTOBJECT_HPP
#define RIOTOBJECT_HPP

#include <list>
#include <map>
#include <vector>
#include <array>

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

	struct Spell
	{
		int id;
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

	struct MatchReference
	{
		/* Example :
		{
		"lane": "TOP",
		"gameId": 3271042895,
		"champion": 133,
		"platformId": "EUW1",
		"timestamp": 1500503770677,
		"queue": 420,
		"role": "SOLO",
		"season": 9
		}
		 */
		std::string lane;
		int64_t gameId;
		int champion;
		std::string platformId;
		int season;
		int queue;
		std::string role;
		int64_t timestamp;

		
	};
	
	struct Player
	{
		/* Example :
		{
        "currentPlatformId": "EUW1",
        "summonerName": "Ale Riven",
        "matchHistoryUri": "/v1/stats/player_history/EUW1/24788418",
        "platformId": "EUW1",
        "currentAccountId": 24788418,
        "profileIcon": 2081,
        "summonerId": 21306477,
        "accountId": 24788418
        }
		*/
		std::string currentPlatformId;
		std::string summonerName;
		std::string matchHistoryUri;
		std::string platformId;
		int64_t currentAccountId;
		int profileIcon;
		int64_t summonerId;
		int64_t accountId;
		//TODO : remove this champ adecquatly
		std::string country;
	};

	struct ParticipantIdentity
	{
		/* Example : 
		 {
         "player": {...},
		 "participantId": 1
         }
		 */
		Player player;
		int participantId;
	};

	struct TeamBans
	{
		/* Example :
		{
		"pickTurn": 1,
		"championId": 141
		}
		*/
		int pickTurn;
		int championId;
	};

	struct TeamStats
	{
		/* Example :
		{
		"firstDragon": false,		
		"firstInhibitor": true,
		"bans": [...],
		"win": "Win",
		"firstRiftHerald": false,
		"firstBaron": true,
		"baronKills": 1,
		"riftHeraldKills": 0,
		"firstBlood": false,
		"teamId": 100,
		"firstTower": true,
		"vilemawKills": 0,
		"inhibitorKills": 1,
		"towerKills": 9,
		"dominionVictoryScore": 0,
		"dragonKills": 3
		}
		*/
		bool firstDragon;
		bool firstInhibitor;
		std::list<TeamBans> bans;
		int baronKills;
		bool firstRiftHerald;
		bool firstBaron;
		int riftHeraldKills;
		bool firstBlood;
		int teamId;
		bool firstTower;
		int vilemawKills;
		int inhibitorKills;
		int towerKills;
		int dominionVictoryScore;
		std::string win;
		int dragonKills;
	};

	struct Rune
	{
		/* Example :
		{
		"runeId": 5245,
		"rank": 9
		}
		*/
		int runeId;
		int rank;
	};

	struct ParticipantStats
	{
		/* Example :
		{
		"item1": 3142,
		"totalPlayerScore": 0,
		"visionScore": 22,
		"unrealKills": 0,
		"win": true,
		"objectivePlayerScore": 0,
		"largestCriticalStrike": 0,
		"totalDamageDealt": 112525,
		"magicDamageDealtToChampions": 976,
		"largestMultiKill": 3,
		"largestKillingSpree": 5,
		"quadraKills": 0,
		"totalTimeCrowdControlDealt": 96,
		"magicalDamageTaken": 7457,
		"longestTimeSpentLiving": 931,
		"neutralMinionsKilledEnemyJungle": 6,
		"firstTowerAssist": false,
		"neutralMinionsKilledTeamJungle": 6,
		"goldEarned": 12346,
		"item2": 3147,
		"item3": 3047,
		"item0": 3814,
		"deaths": 6,
		"item6": 3340,
		"wardsPlaced": 13,
		"item4": 1033,
		"item5": 1029,
		"turretKills": 2,
		"tripleKills": 1,
		"damageSelfMitigated": 17705,
		"goldSpent": 11400,
		"magicDamageDealt": 1460,
		"kills": 7,
		"doubleKills": 1,
		"firstInhibitorKill": false,
		"trueDamageTaken": 344,
		"firstBloodAssist": false,
		"firstBloodKill": false,
		"assists": 6,
		"totalScoreRank": 0,
		"neutralMinionsKilled": 12,
		"combatPlayerScore": 0,
		"visionWardsBoughtInGame": 0,
		"damageDealtToTurrets": 4486,
		"physicalDamageDealtToChampions": 14179,
		"pentaKills": 0,
		"trueDamageDealt": 0,
		"trueDamageDealtToChampions": 0,
		"champLevel": 15,
		"participantId": 1,
		"firstInhibitorAssist": true,
		"wardsKilled": 2,
		"firstTowerKill": false,
		"totalHeal": 1421,
		"totalMinionsKilled": 166,
		"physicalDamageDealt": 111065,
		"damageDealtToObjectives": 4486,
		"sightWardsBoughtInGame": 0,
		"totalDamageDealtToChampions": 15156,
		"totalUnitsHealed": 1,
		"inhibitorKills": 0,
		"totalDamageTaken": 19329,
		"killingSprees": 1,
		"timeCCingOthers": 15,
		"physicalDamageTaken": 11527
		}
		*/
		int64_t physicalDamageDealt;
		int neutralMinionsKilledTeamJungle;
		int64_t magicDamageDealt;
		int totalPlayerScore;
		int deaths;
		bool win;
		int neutralMinionsKilledEnemyJungle;
		int altarsCaptured;
		int largestCriticalStrike;
		int64_t totalDamageDealt;
		int64_t magicDamageDealtToChampions;
		int visionWardsBoughtInGame;
		int64_t damageDealtToObjectives;
		int largestKillingSpree;
		int item1;
		int quadraKills;
		int teamObjective;
		int totalTimeCrowdControlDealt;
		int longestTimeSpentLiving;
		int wardsKilled;
		bool firstTowerAssist;
		bool firstTowerKill;
		int item2;
		int item3;
		int item0;
		bool firstBloodAssist;
		int64_t visionScore;
		int wardsPlaced;
		int item4;
		int item5;
		int item6;
		int turretKills;
		int tripleKills;
		int64_t damageSelfMitigated;
		int champLevel;
		int nodeNeutralizeAssist;
		bool firstInhibitorKill;
		int goldEarned;
		int64_t magicalDamageTaken;
		int kills;
		int doubleKills;
		int nodeCaptureAssist;
		int64_t trueDamageTaken;
		int nodeNeutralize;
		bool firstInhibitorAssist;
		int assists;
		int unrealKills;
		int neutralMinionsKilled;
		int objectivePlayerScore;
		int combatPlayerScore;
		int64_t damageDealtToTurrets;
		int altarsNeutralized;
		int64_t physicalDamageDealtToChampions;
		int goldSpent;
		int64_t trueDamageDealt;
		int64_t trueDamageDealtToChampions;
		int participantId;
		int pentaKills;
		int64_t totalHeal;
		int totalMinionsKilled;
		bool firstBloodKill;
		int nodeCapture;
		int largestMultiKill;
		int sightWardsBoughtInGame;
		int64_t totalDamageDealtToChampions;
		int totalUnitsHealed;
		int inhibitorKills;
		int totalScoreRank;
		int64_t totalDamageTaken;
		int killingSprees;
		int64_t timeCCingOthers;
		int64_t physicalDamageTaken;
	};

	struct ParticipantTimeline
	{
		/* Example :
		{
		"lane": "TOP",
		"participantId": 1,
		"csDiffPerMinDeltas": {
			"20-30": -1.3,
            "0-10": -0.09999999999999987,
            "10-20": 1.4
		},
		"goldPerMinDeltas": {...},
		"xpDiffPerMinDeltas": {...},
		"creepsPerMinDeltas": {...},
		"xpPerMinDeltas": {...},
		"role": "SOLO",
		"damageTakenDiffPerMinDeltas": {...},
		"damageTakenPerMinDeltas": {...}
		*/
		std::string lane;
		int participantId;
		std::map<std::string, double> csDiffPerMinDeltas;
		std::map<std::string, double> goldPerMinDeltas;
		std::map<std::string, double> xpDiffPerMinDeltas;
		std::map<std::string, double> creepsPerMinDeltas;
		std::map<std::string, double> xpPerMinDeltas;
		std::string role;
		std::map<std::string, double> damageTakenDiffPerMinDeltas;
		std::map<std::string, double> damageTakenPerMinDeltas;
	};

	struct Mastery
	{
		/* Example :
		"masteryId": 6111,
		"rank": 5
		*/
		int masteryId;
		int rank;
	};

	struct Participant
	{
		/* Example :
		{
		"stats": {...},
		"spell1Id": 4,
		"participantId": 1,
		"runes": [...],
		"highestAchievedSeasonTier": "PLATINUM",
		"masteries": [...],
		"spell2Id": 12,
		"teamId": 100,
		"timeline": {...},
		"championId": 92
		}
		*/
		ParticipantStats stats;
		int participantId;
		std::list<Rune> runes;
		ParticipantTimeline timeline;
		int teamId;
		int spell2Id;
		std::list<Mastery> masteries;
		std::string highestAchievedSeasonTier;
		int spell1Id;
		int championId;
	};

	struct Match
	{
		/* Example :
		{
		"seasonId": 9,
		"queueId": 420,
		"gameId": 3271042895,
		"participantIdentities": [...],
		"gameVersion": "7.14.195.3579",
		"platformId": "EUW1",
		"gameMode": "CLASSIC",
		"mapId": 11,
		"gameType": "MATCHED_GAME",
		"teams": [...],
		"participants": [...],
		"gameDuration": 1838,
		"gameCreation": 1500503770677
		}
		 */
		int seasonId;
		int queueId;
		int64_t gameId;
		std::array<ParticipantIdentity, 10> participantIdentities;
		std::string gameVersion;
		std::string platformId;
		std::string gameMode;
		int mapId;
		std::string gameType;
		std::pair<TeamStats, TeamStats> teams;
		std::vector<Participant> participants;
		int64_t gameDuration;
		int64_t gameCreation;
	};

	struct Matchlist
	{
		std::list<MatchReference> matches;
		int totalGames;
		int startIndex;
		int endIndex;
	};
}

#endif //  RIOTOBJECT_HPP