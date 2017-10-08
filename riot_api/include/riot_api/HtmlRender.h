#pragma once
#include <ostream>
#include <map>
#include <riot_api/LEA_Project.h>

namespace HtmlRender
{
	int init_cgi();
	void layoutWrapRender(std::ostream& out, const LEA_Project::Summoner &summoner, const std::map<int, LEA_Project::ChampionStats> ss, const std::vector<Riot::Match> &matchs, const Riot::LeaguePosition &league);
}
