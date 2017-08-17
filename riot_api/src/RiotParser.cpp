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

	std::list<MatchReference> parseMatchReference(const rapidjson::Value& json)
	{
		std::list<MatchReference> match_references;
		// parse adecquatly
		if (json.IsArray())
		{
			for (size_t i = 0; i < json.Size(); ++i)
			{
				MatchReference match;
				Helper::assignValue(match.champion, json[i]["champion"]);
				Helper::assignValue(match.gameId, json[i]["gameId"]);
				Helper::assignValue(match.lane, json[i]["lane"]);
				Helper::assignValue(match.platformId, json[i]["platformId"]);
				Helper::assignValue(match.queue, json[i]["queue"]);
				Helper::assignValue(match.role, json[i]["role"]);
				Helper::assignValue(match.season, json[i]["season"]);
				Helper::assignValue(match.timestamp, json[i]["timestamp"]);

				match_references.push_back(match);
			}
		}
		return match_references;
	}

	Matchlist parseMatchlist(const rapidjson::Value& json)
	{
		Matchlist matchlist;
		// parse adecquatly
		if(json.IsObject())
		{
			Helper::assignValue(matchlist.totalGames, json["totalGames"]);
			Helper::assignValue(matchlist.startIndex, json["startIndex"]);
			Helper::assignValue(matchlist.endIndex, json["endIndex"]);
			matchlist.matches = parseMatchReference(json["matches"]);
		}
		return matchlist;
	}

	Player parsePlayer(const rapidjson::Value& json)
	{
		Player player = {};
		if (json.IsObject())
		{
			Helper::assignValue(player.currentPlatformId, json["currentPlatformId"]);
			Helper::assignValue(player.summonerName, json["summonerName"]);
			Helper::assignValue(player.matchHistoryUri, json["matchHistoryUri"]);
			Helper::assignValue(player.platformId, json["platformId"]);
			Helper::assignValue(player.currentAccountId, json["currentAccountId"]);
			Helper::assignValue(player.profileIcon, json["profileIcon"]);
			Helper::assignValue(player.summonerId, json["summonerId"]);
			Helper::assignValue(player.accountId, json["accountId"]);
		}
		return player;
	}

	std::array<ParticipantIdentity, 10> parseParticipantIdentities(const rapidjson::Value& json)
	{
		std::array<ParticipantIdentity, 10> participant_identities;
		// parse adecquatly
		if (json.IsArray())
		{
			for (size_t i = 0; i < json.Size(); ++i)
			{
				ParticipantIdentity participant_identity;
				Helper::assignValue(participant_identity.participantId, json[i]["participantId"]);
				participant_identity.player = parsePlayer(json[i]["player"]);
				participant_identities[participant_identity.participantId-1] = participant_identity;
			}
		}
		return participant_identities;
	}

	std::list<TeamBans> parseBans(const rapidjson::Value& json)
	{
		std::list<TeamBans> team_banses;
		// parse adecquatly
		if (json.IsArray())
		{
			for (size_t i = 0; i < json.Size(); ++i)
			{
				TeamBans team_bans;
				Helper::assignValue(team_bans.championId, json[i]["championId"]);
				Helper::assignValue(team_bans.pickTurn, json[i]["pickTurn"]);
				team_banses.push_front(team_bans);
			}
		}

		return team_banses;
	}

	std::pair<TeamStats, TeamStats> parseTeams(const rapidjson::Value& json)
	{
		std::pair<TeamStats, TeamStats> team_statses;
		// parse adecquatly
		if (json.IsArray())
		{
			int i = 0;
			Helper::assignValue(team_statses.first.firstDragon, json[i]["firstDragon"]);
			Helper::assignValue(team_statses.first.firstInhibitor, json[i]["firstInhibitor"]);
			Helper::assignValue(team_statses.first.baronKills, json[i]["baronKills"]);
			Helper::assignValue(team_statses.first.firstRiftHerald, json[i]["firstRiftHerald"]);
			Helper::assignValue(team_statses.first.firstBaron, json[i]["firstBaron"]);
			Helper::assignValue(team_statses.first.riftHeraldKills, json[i]["riftHeraldKills"]);
			Helper::assignValue(team_statses.first.firstBlood, json[i]["firstBlood"]);
			Helper::assignValue(team_statses.first.teamId, json[i]["teamId"]);
			Helper::assignValue(team_statses.first.firstTower, json[i]["firstTower"]);
			Helper::assignValue(team_statses.first.vilemawKills, json[i]["vilemawKills"]);
			Helper::assignValue(team_statses.first.inhibitorKills, json[i]["inhibitorKills"]);
			Helper::assignValue(team_statses.first.towerKills, json[i]["towerKills"]);
			Helper::assignValue(team_statses.first.dominionVictoryScore, json[i]["dominionVictoryScore"]);
			Helper::assignValue(team_statses.first.win, json[i]["win"]);
			Helper::assignValue(team_statses.first.dragonKills, json[i]["dragonKills"]);
			team_statses.first.bans = parseBans(json[i]["bans"]);
			i++;
			Helper::assignValue(team_statses.second.firstDragon, json[i]["firstDragon"]);
			Helper::assignValue(team_statses.second.firstInhibitor, json[i]["firstInhibitor"]);
			Helper::assignValue(team_statses.second.baronKills, json[i]["baronKills"]);
			Helper::assignValue(team_statses.second.firstRiftHerald, json[i]["firstRiftHerald"]);
			Helper::assignValue(team_statses.second.firstBaron, json[i]["firstBaron"]);
			Helper::assignValue(team_statses.second.riftHeraldKills, json[i]["riftHeraldKills"]);
			Helper::assignValue(team_statses.second.firstBlood, json[i]["firstBlood"]);
			Helper::assignValue(team_statses.second.teamId, json[i]["teamId"]);
			Helper::assignValue(team_statses.second.firstTower, json[i]["firstTower"]);
			Helper::assignValue(team_statses.second.vilemawKills, json[i]["vilemawKills"]);
			Helper::assignValue(team_statses.second.inhibitorKills, json[i]["inhibitorKills"]);
			Helper::assignValue(team_statses.second.towerKills, json[i]["towerKills"]);
			Helper::assignValue(team_statses.second.dominionVictoryScore, json[i]["dominionVictoryScore"]);
			Helper::assignValue(team_statses.second.win, json[i]["win"]);
			Helper::assignValue(team_statses.second.dragonKills, json[i]["dragonKills"]);
			team_statses.second.bans = parseBans(json[i]["bans"]);
			
		}
		return team_statses;
	}

	ParticipantStats parseStats(const rapidjson::Value& json)
	{
		ParticipantStats participant_stats = {};
		// parse adecquatly
		if (json.IsObject())
		{
			Helper::assignValue(participant_stats.physicalDamageDealt, json["physicalDamageDealt"]);
			Helper::assignValue(participant_stats.neutralMinionsKilledTeamJungle, json["neutralMinionsKilledTeamJungle"]);
			Helper::assignValue(participant_stats.magicDamageDealt, json["magicDamageDealt"]);
			Helper::assignValue(participant_stats.totalPlayerScore, json["totalPlayerScore"]);
			Helper::assignValue(participant_stats.deaths, json["deaths"]);
			Helper::assignValue(participant_stats.win, json["win"]);
			Helper::assignValue(participant_stats.neutralMinionsKilledEnemyJungle, json["neutralMinionsKilledEnemyJungle"]);
			Helper::assignValue(participant_stats.altarsCaptured, json["altarsCaptured"]);
			Helper::assignValue(participant_stats.largestCriticalStrike, json["largestCriticalStrike"]);
			Helper::assignValue(participant_stats.totalDamageDealt, json["totalDamageDealt"]);
			Helper::assignValue(participant_stats.magicDamageDealtToChampions, json["magicDamageDealtToChampions"]);
			Helper::assignValue(participant_stats.visionWardsBoughtInGame, json["visionWardsBoughtInGame"]);
			Helper::assignValue(participant_stats.damageDealtToObjectives, json["damageDealtToObjectives"]);
			Helper::assignValue(participant_stats.largestKillingSpree, json["largestKillingSpree"]);
			Helper::assignValue(participant_stats.item1, json["item1"]);
			Helper::assignValue(participant_stats.quadraKills, json["quadraKills"]);
			Helper::assignValue(participant_stats.teamObjective, json["teamObjective"]);
			Helper::assignValue(participant_stats.totalTimeCrowdControlDealt, json["totalTimeCrowdControlDealt"]);
			Helper::assignValue(participant_stats.longestTimeSpentLiving, json["longestTimeSpentLiving"]);
			Helper::assignValue(participant_stats.wardsKilled, json["wardsKilled"]);
			Helper::assignValue(participant_stats.firstTowerAssist, json["firstTowerAssist"]);
			Helper::assignValue(participant_stats.firstTowerKill, json["firstTowerKill"]);
			Helper::assignValue(participant_stats.item2, json["item2"]);
			Helper::assignValue(participant_stats.item3, json["item3"]);
			Helper::assignValue(participant_stats.item0, json["item0"]);
			Helper::assignValue(participant_stats.firstBloodAssist, json["firstBloodAssist"]);
			Helper::assignValue(participant_stats.visionScore, json["visionScore"]);
			Helper::assignValue(participant_stats.wardsPlaced, json["wardsPlaced"]);
			Helper::assignValue(participant_stats.item4, json["item4"]);
			Helper::assignValue(participant_stats.item5, json["item5"]);
			Helper::assignValue(participant_stats.item6, json["item6"]);
			Helper::assignValue(participant_stats.turretKills, json["turretKills"]);
			Helper::assignValue(participant_stats.tripleKills, json["tripleKills"]);
			Helper::assignValue(participant_stats.damageSelfMitigated, json["damageSelfMitigated"]);
			Helper::assignValue(participant_stats.champLevel, json["champLevel"]);
			Helper::assignValue(participant_stats.nodeNeutralizeAssist, json["nodeNeutralizeAssist"]);
			Helper::assignValue(participant_stats.firstInhibitorKill, json["firstInhibitorKill"]);
			Helper::assignValue(participant_stats.goldEarned, json["goldEarned"]);
			Helper::assignValue(participant_stats.magicalDamageTaken, json["magicalDamageTaken"]);
			Helper::assignValue(participant_stats.kills, json["kills"]);
			Helper::assignValue(participant_stats.doubleKills, json["doubleKills"]);
			Helper::assignValue(participant_stats.nodeCaptureAssist, json["nodeCaptureAssist"]);
			Helper::assignValue(participant_stats.trueDamageTaken, json["trueDamageTaken"]);
			Helper::assignValue(participant_stats.nodeNeutralize, json["nodeNeutralize"]);
			Helper::assignValue(participant_stats.firstInhibitorAssist, json["firstInhibitorAssist"]);
			Helper::assignValue(participant_stats.assists, json["assists"]);
			Helper::assignValue(participant_stats.unrealKills, json["unrealKills"]);
			Helper::assignValue(participant_stats.neutralMinionsKilled, json["neutralMinionsKilled"]);
			Helper::assignValue(participant_stats.objectivePlayerScore, json["objectivePlayerScore"]);
			Helper::assignValue(participant_stats.combatPlayerScore, json["combatPlayerScore"]);
			Helper::assignValue(participant_stats.damageDealtToTurrets, json["damageDealtToTurrets"]);
			Helper::assignValue(participant_stats.altarsNeutralized, json["altarsNeutralized"]);
			Helper::assignValue(participant_stats.physicalDamageDealtToChampions, json["physicalDamageDealtToChampions"]);
			Helper::assignValue(participant_stats.goldSpent, json["goldSpent"]);
			Helper::assignValue(participant_stats.trueDamageDealt, json["trueDamageDealt"]);
			Helper::assignValue(participant_stats.trueDamageDealtToChampions, json["trueDamageDealtToChampions"]);
			Helper::assignValue(participant_stats.participantId, json["participantId"]);
			Helper::assignValue(participant_stats.pentaKills, json["pentaKills"]);
			Helper::assignValue(participant_stats.totalHeal, json["totalHeal"]);
			Helper::assignValue(participant_stats.totalMinionsKilled, json["totalMinionsKilled"]);
			Helper::assignValue(participant_stats.firstBloodKill, json["firstBloodKill"]);
			Helper::assignValue(participant_stats.nodeCapture, json["nodeCapture"]);
			Helper::assignValue(participant_stats.largestMultiKill, json["largestMultiKill"]);
			Helper::assignValue(participant_stats.sightWardsBoughtInGame, json["sightWardsBoughtInGame"]);
			Helper::assignValue(participant_stats.totalDamageDealtToChampions, json["totalDamageDealtToChampions"]);
			Helper::assignValue(participant_stats.totalUnitsHealed, json["totalUnitsHealed"]);
			Helper::assignValue(participant_stats.inhibitorKills, json["inhibitorKills"]);
			Helper::assignValue(participant_stats.totalScoreRank, json["totalScoreRank"]);
			Helper::assignValue(participant_stats.totalDamageTaken, json["totalDamageTaken"]);
			Helper::assignValue(participant_stats.killingSprees, json["killingSprees"]);
			Helper::assignValue(participant_stats.timeCCingOthers, json["timeCCingOthers"]);
			Helper::assignValue(participant_stats.physicalDamageTaken, json["physicalDamageTaken"]);
		}
		return participant_stats;
	}

	std::list<Rune> parseRunes(const rapidjson::Value& json)
	{
		// TODO
		std::list<Rune> runes;
		return runes;
	}

	ParticipantTimeline parseTimeline(const rapidjson::Value& json)
	{
		// TODO
		ParticipantTimeline participant_timeline = {};
		return participant_timeline;
	}

	std::list<Mastery> parseMasteries(const rapidjson::Value& json)
	{
		// TODO
		std::list<Mastery> masteries;
		return masteries;
	}

	std::vector<Participant> parseParticipants(const rapidjson::Value& json)
	{
		std::vector<Participant> participants;
		participants.resize(10);
		// parse adecquatly
		if (json.IsArray())
		{
			for (size_t i = 0; i < json.Size(); ++i)
			{
				Participant participant = {};
				Helper::assignValue(participant.participantId, json[i]["participantId"]);
				if (participant.participantId < 0)
					continue;
				Helper::assignValue(participant.teamId, json[i]["teamId"]);
				Helper::assignValue(participant.spell1Id, json[i]["spell1Id"]);
				Helper::assignValue(participant.spell2Id, json[i]["spell2Id"]);
				Helper::assignValue(participant.championId, json[i]["championId"]);
				Helper::assignValue(participant.highestAchievedSeasonTier, json[i]["highestAchievedSeasonTier"]);
				participant.stats = parseStats(json[i]["stats"]);
				participant.runes = parseRunes(json[i]["runes"]);
				participant.timeline = parseTimeline(json[i]["timeline"]);
				participant.masteries = parseMasteries(json[i]["masteries"]);


				participants[participant.participantId-1] = participant;
			}
		}

		return participants;
	}

	Match parseMatch(const rapidjson::Value& json)
	{
		Match match = {};
		// parse adecquatly
		if (json.IsObject())
		{
			Helper::assignValue(match.seasonId, json["SeasonId"]);
			Helper::assignValue(match.queueId, json["queueId"]);
			Helper::assignValue(match.gameId, json["gameId"]);
			Helper::assignValue(match.gameVersion, json["gameVersion"]);
			Helper::assignValue(match.platformId, json["platformId"]);
			Helper::assignValue(match.gameMode, json["gameMode"]);
			Helper::assignValue(match.mapId, json["mapId"]);
			Helper::assignValue(match.gameType, json["gameType"]);
			Helper::assignValue(match.gameDuration, json["gameDuration"]);
			Helper::assignValue(match.gameCreation, json["gameCreation"]);

			match.participantIdentities = parseParticipantIdentities(json["participantIdentities"]);
			match.teams = parseTeams(json["teams"]);
			match.participants = parseParticipants(json["participants"]);

		}
		return match;
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
			if (json.IsInt() || json.IsUint())
				var = json.GetInt();
		}

		void assignValue(long long& var, const Value& json)
		{
			if (json.IsInt64() || json.IsUint64())
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
