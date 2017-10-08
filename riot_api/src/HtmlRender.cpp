#include "riot_api/HtmlRender.h"

#include <cgicc/Cgicc.h>
#include <cgicc/HTMLClasses.h>
#include <cgicc/HTMLElementList.h>
#include <cgicc/HTMLElement.h>
#include <set>
#include <ctime>

#include <boost/asio.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "riot_api/serverBD.h"

#include <cgicc/HTTPRedirectHeader.h>
#include <cgicc/HTTPHTMLHeader.h>

namespace HtmlRender
{
	using namespace cgicc;
	cgicc::div* add_div(cgicc::div* outer_list, const std::string &&name)
	{
		cgicc::div * inner_list = new cgicc::div(cgicc::HTMLAttribute("class", name));
		outer_list->add(inner_list);
		return inner_list;
	}
	cgicc::div* add_div(cgicc::div* outer_list, const std::string &&name, const std::string &&value)
	{
		cgicc::div * inner_list = new cgicc::div(value, cgicc::HTMLAttribute("class", name));
		outer_list->add(inner_list);
		return inner_list;
	}
	cgicc::div* add_div(cgicc::div* outer_list, const std::string &&name, const std::string &value)
	{
		cgicc::div * inner_list = new cgicc::div(value, cgicc::HTMLAttribute("class", name));
		outer_list->add(inner_list);
		return inner_list;
	}
	
	std::map<int, Riot::Champion> champions;
	std::map<int, Riot::Spell> spells;

	bool mycomparison1(const LEA_Project::ChampionStats& lhs, const LEA_Project::ChampionStats& rhs) {
		return (lhs.wins + lhs.losses>rhs.wins + rhs.losses);
	}



	cgicc::div* sideContent(cgicc::div* summoner_layout, const std::string& country, const std::map<int, LEA_Project::ChampionStats> ss)
	{
		std::set<LEA_Project::ChampionStats, bool(*)(const LEA_Project::ChampionStats&, const LEA_Project::ChampionStats&)> stats_summoner(&mycomparison1);
		for (auto elem : ss)
		{
			stats_summoner.insert(elem.second);
		}

		auto side_content = add_div(summoner_layout, "SideContent");
		{
			auto _index_stats = 0;
			while (!stats_summoner.empty())
			{
				if (++_index_stats > 3)
					break;
				auto it = stats_summoner.begin();
				auto stat_champ = *it;
				stats_summoner.erase(it);
			
				auto box = add_div(side_content, "Box");
				{
					{
						box->add(cgicc::div(".").set("class", "ChampionBG").set("style", "background: url(https://ddragon.leagueoflegends.com/cdn/img/champion/splash/" + champions[stat_champ.championId].key + "_0.jpg) no-repeat;background-size: 100%;"));
					}
					auto most_champion_content = add_div(box, "MostChampionContent");
					{
						auto champion_box = add_div(most_champion_content, "ChampionBox Ranked");
						{
							//add title
							champion_box->add(cgicc::div().set("class", "Face").set("title", champions[stat_champ.championId].name).add(cgicc::div(".").set("class", "borderImage").set("style", "position: absolute;	left: -22px; top: -33px; width: 120px; height: 120px; background: url(http://localhost/LEAProject/stats_0"+ std::to_string(_index_stats) +".png) no-repeat center bottom")).add(a().add(img().set("class", "ChampionImage").set("src", "//opgg-static.akamaized.net/images/lol/champion/" + champions[stat_champ.championId].key + ".png?image=c_scale,w_45&amp;v=1"))));
							auto champion_info = add_div(champion_box, "ChampionInfo");
							{
								champion_info->add(cgicc::div().set("class", "ChampionName").set("title", champions[stat_champ.championId].name).add(a(champions[stat_champ.championId].name)));
								float ratio = stat_champ.cs / (stat_champ.wins + stat_champ.losses);
								std::stringstream stream;
								auto r1 = static_cast<int>(floor(ratio*100.)) % 100 == 0;
								auto r2 = static_cast<int>(floor(ratio*10.)) % 10 == 0;
								if (r1)
								{
									if (r2)
										stream << std::fixed << std::setprecision(0) << ratio;
									else
										stream << std::fixed << std::setprecision(1) << ratio;
								}
								else
									stream << std::fixed << std::setprecision(2) << ratio;
								champion_info->add(cgicc::div().set("class", "ChampionMinionKill").add(span(stream.str() + " CS").set("class", "cs average")));
							}
							auto personal_kda = add_div(champion_box, "PersonalKDA");
							{
								float ratio = stat_champ.kills + stat_champ.assists;
								if (stat_champ.deaths) ratio /= stat_champ.deaths;
								std::stringstream stream;
								auto r1 = static_cast<int>(floor(ratio*100.)) % 100 == 0;
								auto r2 = static_cast<int>(floor(ratio*10.)) % 10 == 0;
								if (r1)
								{
									if (r2)
										stream << std::fixed << std::setprecision(0) << ratio;
									else
										stream << std::fixed << std::setprecision(1) << ratio;
								}
								else
									stream << std::fixed << std::setprecision(2) << ratio;
								std::string kda_type;
								if (ratio < 3.)
									kda_type = "KDA normal tip";
								else if (ratio < 4.)
									kda_type = "KDA green tip";
								else if (ratio < 5.)
									kda_type = "KDA blue tip";
								else
									kda_type = "KDA orange tip";
								personal_kda->add(cgicc::div().set("class", kda_type).add(span(stream.str() + ":1").set("class", "KDA")).add(span("KDA").set("class", "Text")));

								auto kda_each = add_div(personal_kda, "KDAEach");
								{
									float nb_g = stat_champ.wins + stat_champ.losses;
									float ratio_k = stat_champ.kills / nb_g;
									float ratio_a = stat_champ.assists / nb_g;
									float ratio_d = stat_champ.deaths / nb_g;
									std::stringstream stream_k, stream_a, stream_d;
									stream_k << std::fixed << std::setprecision(1) << ratio_k;
									stream_d << std::fixed << std::setprecision(1) << ratio_d;
									stream_a << std::fixed << std::setprecision(1) << ratio_a;
									kda_each->add(span(stream_k.str()).set("class", "Kill"));
									kda_each->add(span("/").set("class", "Bar"));
									kda_each->add(span(stream_d.str()).set("class", "Death"));
									kda_each->add(span("/").set("class", "Bar"));
									kda_each->add(span(stream_a.str()).set("class", "Assist"));
								}
							}

							auto played = add_div(champion_box, "Played");
							{
								int ratio = stat_champ.wins * 100 / (stat_champ.wins + stat_champ.losses);
								std::string winratio_type;
								if (ratio < 60)
									winratio_type = "WinRatio normal tip";
								else
									winratio_type = "WinRatio red tip";
								played->add(cgicc::div(std::to_string(ratio) + "%").set("class", winratio_type).set("title", "Win Ratio"));
								played->add(cgicc::div(std::to_string(stat_champ.wins + stat_champ.losses) + " Played").set("class", "Title"));
							}
							champion_box->add(cgicc::div().set("class", "Rank").set("style", "display: table-cell;width: 70px;vertical-align: middle;").add(cgicc::div().set("class","CountryImage").set("style", "display: inline-block;vertical-align: middle;").add(img().set("src", "http://localhost/LEAProject/icons/"+ country +".png"))).add(p("<br>Rank#" + std::to_string(stat_champ.rank_by_country)).set("style", "margin: 0px")));
							champion_box->add(cgicc::div().set("class", "Rank").set("style", "display: table-cell;width: 70px;vertical-align: middle;").add(cgicc::div().set("class", "CountryImage").set("style", "display: inline-block;vertical-align: middle;").add(img().set("src", "http://localhost/LEAProject/icons/world.png"))).add(p("<br>Rank#" + std::to_string(stat_champ.rank_by_all)).set("style", "margin: 0px")));
						}						
					}
				}
			}
			
		}

		{
			auto _index_stats = 0;
			auto side_content_plus = add_div(summoner_layout, "SideContent");
			while (!stats_summoner.empty())
			{
				auto it = stats_summoner.begin();
				auto stat_champ = *it;
				stats_summoner.erase(it);

				if (++_index_stats > 3)
				{
					_index_stats = 1;
					side_content_plus = add_div(summoner_layout, "SideContent");
				}
				auto box = add_div(side_content_plus, "Box");
				{
					auto most_champion_content = add_div(box, "MostChampionContent");
					{
						auto champion_box = add_div(most_champion_content, "ChampionBox Ranked");
						{
							//add title
							champion_box->add(cgicc::div().set("class", "Face").set("title", champions[stat_champ.championId].name).add(a().add(img().set("class", "ChampionImage").set("src", "//opgg-static.akamaized.net/images/lol/champion/" + champions[stat_champ.championId].key + ".png?image=c_scale,w_45&amp;v=1"))));
							auto champion_info = add_div(champion_box, "ChampionInfo");
							{
								champion_info->add(cgicc::div().set("class", "ChampionName").set("title", champions[stat_champ.championId].name).add(a(champions[stat_champ.championId].name)));
								float ratio = stat_champ.cs / (stat_champ.wins + stat_champ.losses);
								std::stringstream stream;
								auto r1 = static_cast<int>(floor(ratio*100.)) % 100 == 0;
								auto r2 = static_cast<int>(floor(ratio*10.)) % 10 == 0;
								if (r1)
								{
									if (r2)
										stream << std::fixed << std::setprecision(0) << ratio;
									else
										stream << std::fixed << std::setprecision(1) << ratio;
								}
								else
									stream << std::fixed << std::setprecision(2) << ratio;
								champion_info->add(cgicc::div().set("class", "ChampionMinionKill").add(span(stream.str() + " CS").set("class", "cs average")));
							}
							auto personal_kda = add_div(champion_box, "PersonalKDA");
							{
								float ratio = stat_champ.kills + stat_champ.assists;
								if (stat_champ.deaths) ratio /= stat_champ.deaths;
								std::stringstream stream;
								auto r1 = static_cast<int>(floor(ratio*100.)) % 100 == 0;
								auto r2 = static_cast<int>(floor(ratio*10.)) % 10 == 0;
								if (r1)
								{
									if (r2)
										stream << std::fixed << std::setprecision(0) << ratio;
									else
										stream << std::fixed << std::setprecision(1) << ratio;
								}
								else
									stream << std::fixed << std::setprecision(2) << ratio;
								std::string kda_type;
								if (ratio < 3.)
									kda_type = "KDA normal tip";
								else if (ratio < 4.)
									kda_type = "KDA green tip";
								else if (ratio < 5.)
									kda_type = "KDA blue tip";
								else
									kda_type = "KDA orange tip";
								personal_kda->add(cgicc::div().set("class", kda_type).add(span(stream.str() + ":1").set("class", "KDA")).add(span("KDA").set("class", "Text")));

								auto kda_each = add_div(personal_kda, "KDAEach");
								{
									float nb_g = stat_champ.wins + stat_champ.losses;
									float ratio_k = stat_champ.kills / nb_g;
									float ratio_a = stat_champ.assists / nb_g;
									float ratio_d = stat_champ.deaths / nb_g;
									std::stringstream stream_k, stream_a, stream_d;
									stream_k << std::fixed << std::setprecision(1) << ratio_k;
									stream_d << std::fixed << std::setprecision(1) << ratio_d;
									stream_a << std::fixed << std::setprecision(1) << ratio_a;
									kda_each->add(span(stream_k.str()).set("class", "Kill"));
									kda_each->add(span("/").set("class", "Bar"));
									kda_each->add(span(stream_d.str()).set("class", "Death"));
									kda_each->add(span("/").set("class", "Bar"));
									kda_each->add(span(stream_a.str()).set("class", "Assist"));
								}
							}

							auto played = add_div(champion_box, "Played");
							{
								int ratio = stat_champ.wins * 100 / (stat_champ.wins + stat_champ.losses);
								std::string winratio_type;
								if (ratio < 60)
									winratio_type = "WinRatio normal tip";
								else
									winratio_type = "WinRatio red tip";
								played->add(cgicc::div(std::to_string(ratio) + "%").set("class", winratio_type).set("title", "Win Ratio"));
								played->add(cgicc::div(std::to_string(stat_champ.wins + stat_champ.losses) + " Played").set("class", "Title"));
							}
							champion_box->add(cgicc::div().set("class", "Rank").set("style", "display: table-cell;width: 70px;vertical-align: middle;").add(cgicc::div().set("class", "CountryImage").set("style", "display: inline-block;vertical-align: middle;").add(img().set("src", "http://localhost/LEAProject/icons/" + country + ".png"))).add(p("<br>Rank#" + std::to_string(stat_champ.rank_by_country)).set("style", "margin: 0px")));
							champion_box->add(cgicc::div().set("class", "Rank").set("style", "display: table-cell;width: 70px;vertical-align: middle;").add(cgicc::div().set("class", "CountryImage").set("style", "display: inline-block;vertical-align: middle;").add(img().set("src", "http://localhost/LEAProject/icons/world.png"))).add(p("<br>Rank#" + std::to_string(stat_champ.rank_by_all)).set("style", "margin: 0px")));
						}
					}
				}
			}
		}

		return side_content;
	}

	cgicc::div* realContent(cgicc::div* summoner_layout, const Riot::Summoner &summoner, const std::vector<Riot::Match> &matchs)
	{
		auto real_content = add_div(summoner_layout, "RealContent");
		{
			auto game_list_container = add_div(real_content, "GameListContainer");
			{
				auto _content = add_div(game_list_container, "Content");
				{
					auto _index_list = 0;
					auto game_item_list = add_div(_content, "GameItemList");
					for(auto match : matchs)
					{
						if(++_index_list > 3)
						{
							_index_list = 1;
							game_item_list = add_div(_content, "GameItemList");
						}
						{
							auto position = LEA_Project::getPositionInMatch(match, summoner.id);
							auto win = match.participants[position - 1].stats.win;

							auto game_item_wrap = add_div(game_item_list, "GameItemWrap");

							cgicc::div* game_item;
							if (win) game_item = add_div(game_item_wrap, "GameItem Win");
							else game_item = add_div(game_item_wrap, "GameItem Lose");

							auto content = add_div(game_item, "Content");
							{
								auto follow_players_names_01 = add_div(content, "FollowPlayers Names");
								{
									auto result = add_div(follow_players_names_01, "Result");
									{
										if(position<=5 && win)
											result->add(img().set("src", "http://localhost/LEAProject/victory.png").set("class", "Image").set("alt", "Victory"));
										else if(position <= 5 && !win)
											result->add(img().set("src", "http://localhost/LEAProject/defeat.png").set("class", "Image").set("alt", "Defeat"));
										else
											result->add(h1(".").set("class", "Image").set("alt", "Empty"));
									}
									
									auto team01 = add_div(follow_players_names_01, "Team");
									{
										auto summoner01 = add_div(team01, "Summoner");
										{
											auto country_image = add_div(summoner01, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/" + match.participantIdentities[0].player.country + ".png")));
											}
											auto champion_image = add_div(summoner01, "ChampionImage");
											{
												auto champ_name = champions[match.participants[0].championId].name;
												champion_image->add(cgicc::div(champ_name, HTMLAttribute("class", champions[match.participants[0].championId].key + " splite")));
											}
											// TODO : link to summoner_name_page
											auto summoner_name = add_div(summoner01, "SummonerName", match.participantIdentities[0].player.summonerName);
										}
										auto summoner02 = add_div(team01, "Summoner");
										{
											auto country_image = add_div(summoner02, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/" + match.participantIdentities[1].player.country + ".png")));
											}
											auto champion_image = add_div(summoner02, "ChampionImage");
											{
												auto champ_name = champions[match.participants[1].championId].name;
												champion_image->add(cgicc::div(champ_name, HTMLAttribute("class", champions[match.participants[1].championId].key + " splite")));
											}
											// TODO : link to summoner_name_page
											auto summoner_name = add_div(summoner02, "SummonerName", match.participantIdentities[1].player.summonerName);

										}
										auto summoner03 = add_div(team01, "Summoner");
										{
											auto country_image = add_div(summoner03, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/" + match.participantIdentities[2].player.country + ".png")));
											}
											auto champion_image = add_div(summoner03, "ChampionImage");
											{
												auto champ_name = champions[match.participants[2].championId].name;
												champion_image->add(cgicc::div(champ_name, HTMLAttribute("class", champions[match.participants[2].championId].key + " splite")));
											}
											// TODO : link to summoner_name_page
											auto summoner_name = add_div(summoner03, "SummonerName", match.participantIdentities[2].player.summonerName);

										}
										auto summoner04 = add_div(team01, "Summoner");
										{
											auto country_image = add_div(summoner04, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/" + match.participantIdentities[3].player.country + ".png")));
											}
											auto champion_image = add_div(summoner04, "ChampionImage");
											{
												auto champ_name = champions[match.participants[3].championId].name;
												champion_image->add(cgicc::div(champ_name, HTMLAttribute("class", champions[match.participants[3].championId].key + " splite")));
											}
											// TODO : link to summoner_name_page
											auto summoner_name = add_div(summoner04, "SummonerName", match.participantIdentities[3].player.summonerName);

										}
										auto summoner05 = add_div(team01, "Summoner");
										{
											auto country_image = add_div(summoner05, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/" + match.participantIdentities[4].player.country + ".png")));
											}
											auto champion_image = add_div(summoner05, "ChampionImage");
											{
												auto champ_name = champions[match.participants[4].championId].name;
												champion_image->add(cgicc::div(champ_name, HTMLAttribute("class", champions[match.participants[4].championId].key + " splite")));
											}
											// TODO : link to summoner_name_page
											auto summoner_name = add_div(summoner05, "SummonerName", match.participantIdentities[4].player.summonerName);

										}
									}
								}
								auto game_stats = add_div(content, "GameStats");
								{
									auto champ_name = champions[match.participants[position - 1].championId].key;
									auto hours = static_cast<int>(match.gameDuration / 60 * 60);
									auto minutes = static_cast<int>((match.gameDuration / 60) % 60);
									auto seconds = static_cast<int>(match.gameDuration % 60);

									auto champion_name = add_div(game_stats, "ChampionName");
									{
										champion_name->add(a(champ_name).set("href", "#").set("target", "_self"));
									}
									auto champion_display = add_div(game_stats, "Championdisplay");
									{
										auto spell01 = add_div(champion_display, "Spell");
										{
											auto spell_name = spells[match.participants[position - 1].spell1Id].key;
											std::string src = "//opgg-static.akamaized.net/images/lol/spell/" + spell_name + ".png?image=c_scale,w_22&amp;v=1";
											spell01->add(img().set("src", src));
										}
										auto champion_image = add_div(champion_display, "ChampionImage");
										{
											auto src = "//opgg-static.akamaized.net/images/lol/champion/" + champ_name + ".png?image=c_scale,w_46&amp;v=1";
											champion_image->add(a(HTMLAttribute("href", "#")).add(img().set("src", src).set("class", "Image").set("alt", champ_name)));
										}
										auto spell02 = add_div(champion_display, "Spell");
										{
											auto spell_name = spells[match.participants[position - 1].spell2Id].key;
											std::string src = "//opgg-static.akamaized.net/images/lol/spell/" + spell_name + ".png?image=c_scale,w_22&amp;v=1";
											spell02->add(img().set("src", src));
										}
									}
									auto game_result = add_div(game_stats, "GameResult", win ? "Victory" : "Defeat");
									cgicc::div* timestamp;
									{
										int64_t time_now = time(nullptr) * 1000;
										auto diff = time_now - match.gameCreation;
										auto days = static_cast<int>(diff / (1000 * 60 * 60 * 24));
										auto hours = static_cast<int>(diff / (1000 * 60 * 60));
										auto minutes = static_cast<int>(diff / (1000 * 60));
										auto seconds = static_cast<int>(diff) / 1000;
										if (days)
											timestamp = add_div(game_stats, "TimeStamp", std::to_string(days) + " days ago");
										else
										{
											if (hours)
												timestamp = add_div(game_stats, "TimeStamp", std::to_string(hours) + " hours ago");
											else
											{
												if (minutes)
													timestamp = add_div(game_stats, "TimeStamp", std::to_string(minutes) + " minutes ago");
												else
													timestamp = add_div(game_stats, "TimeStamp", std::to_string(seconds) + " seconds ago");
											}
										}
									}
									auto game_length = add_div(game_stats, "TimeStamp", std::to_string(minutes) + "m:" + std::to_string(minutes) + "s");									
									auto bar = add_div(game_stats, "Bar", ".");
									auto kda_d = add_div(game_stats, "KDA");
									{
										auto sum_stats = match.participants[position - 1].stats;
										kda_d->add(span(std::to_string(sum_stats.kills) + "  /  ", HTMLAttribute("class", "Kill")));
										kda_d->add(span(std::to_string(sum_stats.deaths), HTMLAttribute("class", "Death")));
										kda_d->add(span("  /  " + std::to_string(sum_stats.assists), HTMLAttribute("class", "Assist")));
									}
									// TODO : kill participation
									auto ck_rate = add_div(game_stats, "CKRate", "P/Kill -1%");
									auto kda_ratio = add_div(game_stats, "KDARatio");
									{
										auto sum_stats = match.participants[position - 1].stats;
										float ratio = sum_stats.kills + sum_stats.assists;
										if (sum_stats.deaths) ratio /= sum_stats.deaths;
										std::stringstream stream;
										auto r1 = static_cast<int>(floor(ratio*100.)) % 100 == 0;
										auto r2 = static_cast<int>(floor(ratio*10.)) % 10 == 0;
										if (r1)
										{
											if (r2)
												stream << std::fixed << std::setprecision(0) << ratio;
											else
												stream << std::fixed << std::setprecision(1) << ratio;
										}
										else
											stream << std::fixed << std::setprecision(2) << ratio;
										kda_ratio->add(span(stream.str() + " KDA", HTMLAttribute("class", "KDARatio")));
									}
									auto level = add_div(game_stats, "Level", "Level" + std::to_string(match.participants[position - 1].stats.champLevel));
									auto cs = add_div(game_stats, "CS");
									{
										cs->add(span(std::to_string(match.participants[position - 1].stats.totalMinionsKilled) + "CS").set("class", "CS tip"));
									}

								}
								
								auto follow_players_names_02 = add_div(content, "FollowPlayers Names");
								{
									auto result = add_div(follow_players_names_02, "Result");
									{
										if (position>5 && win)
											result->add(img().set("src", "http://localhost/LEAProject/victory.png").set("class", "Image").set("alt", "Victory"));
										else if (position>5 && !win)
											result->add(img().set("src", "http://localhost/LEAProject/defeat.png").set("class", "Image").set("alt", "Defeat"));
										else 
											result->add(h1(".").set("class", "Image").set("alt", "Empty"));
									}
									auto team02 = add_div(follow_players_names_02, "Team");
									{
										auto summoner06 = add_div(team02, "Summoner");
										{
											auto country_image = add_div(summoner06, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/" + match.participantIdentities[5].player.country + ".png")));
											}
											auto champion_image = add_div(summoner06, "ChampionImage");
											{
												auto champ_name = champions[match.participants[5].championId].name;
												champion_image->add(cgicc::div(champ_name, HTMLAttribute("class", champions[match.participants[5].championId].key + " splite")));
											}
											// TODO : link to summoner_name_page
											auto summoner_name = add_div(summoner06, "SummonerName", match.participantIdentities[5].player.summonerName);

										}
										auto summoner07 = add_div(team02, "Summoner");
										{
											auto country_image = add_div(summoner07, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/" + match.participantIdentities[6].player.country + ".png")));
											}
											auto champion_image = add_div(summoner07, "ChampionImage");
											{
												auto champ_name = champions[match.participants[6].championId].name;
												champion_image->add(cgicc::div(champ_name, HTMLAttribute("class", champions[match.participants[6].championId].key + " splite")));
											}
											// TODO : link to summoner_name_page
											auto summoner_name = add_div(summoner07, "SummonerName", match.participantIdentities[6].player.summonerName);

										}
										auto summoner08 = add_div(team02, "Summoner");
										{
											auto country_image = add_div(summoner08, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/" + match.participantIdentities[7].player.country + ".png")));
											}
											auto champion_image = add_div(summoner08, "ChampionImage");
											{
												auto champ_name = champions[match.participants[7].championId].name;
												champion_image->add(cgicc::div(champ_name, HTMLAttribute("class", champions[match.participants[7].championId].key + " splite")));
											}
											// TODO : link to summoner_name_page
											auto summoner_name = add_div(summoner08, "SummonerName", match.participantIdentities[7].player.summonerName);

										}
										auto summoner09 = add_div(team02, "Summoner");
										{
											auto country_image = add_div(summoner09, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/" + match.participantIdentities[8].player.country + ".png")));
											}
											auto champion_image = add_div(summoner09, "ChampionImage");
											{
												auto champ_name = champions[match.participants[8].championId].name;
												champion_image->add(cgicc::div(champ_name, HTMLAttribute("class", champions[match.participants[8].championId].key + " splite")));
											}
											// TODO : link to summoner_name_page
											auto summoner_name = add_div(summoner09, "SummonerName", match.participantIdentities[8].player.summonerName);

										}
										auto summoner10 = add_div(team02, "Summoner");
										{
											auto country_image = add_div(summoner10, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/" + match.participantIdentities[9].player.country + ".png")));
											}
											auto champion_image = add_div(summoner10, "ChampionImage");
											{
												auto champ_name = champions[match.participants[9].championId].name;
												champion_image->add(cgicc::div(champ_name, HTMLAttribute("class", champions[match.participants[9].championId].key + " splite")));
											}
											// TODO : link to summoner_name_page
											auto summoner_name = add_div(summoner10, "SummonerName", match.participantIdentities[9].player.summonerName);

										}
									}
								}

							}
						}
					}
					if(++_index_list > 3)
					{
						game_item_list = add_div(_content, "GameItemList");
						
					}
					else
					{
						auto game_item_wrap = add_div(game_item_list, "GameItemWrap");
						auto game_item = add_div(game_item_wrap, "GameItem");
						auto content = add_div(game_item, "Content");
						content->add(cgicc::div().set("class", "GameMoreButton Box").set("title", "GIMMMME MOOOOOOORE").add(a("Show More").set("href", "#").set("class", "Button")));
					}
				}
			}

		}
		return real_content;
	}

	constexpr unsigned int str2int(const char* str, int h = 0)
	{
		return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
	}

	void layoutWrapRender(std::ostream& out, const LEA_Project::Summoner &summoner, const std::map<int, LEA_Project::ChampionStats> ss, const std::vector<Riot::Match> &matchs, const Riot::LeaguePosition &league)
	{
		auto serverBD = ServerBD::getInstance();
		serverBD->getListChampions(champions);
		serverBD->getListSpells(spells);


		cgicc::div* layout_wrap = new cgicc::div(HTMLAttribute("class", "LayoutWrap"));

		{
			auto global_header = add_div(layout_wrap, "GlobalHeader");
			{
				global_header->add(h1().set("class", "Logo").add(a().set("href", "#").set("class", "Link").add(span("Lea-Project", HTMLAttribute("class", "Image __spSite __spSite-2")))));
			}
			auto summoner_layout = add_div(layout_wrap, "SummonerLayout");
			{
				auto header = add_div(summoner_layout, "Header");
				{
					std::set<LEA_Project::ChampionStats, bool(*)(const LEA_Project::ChampionStats&, const LEA_Project::ChampionStats&)> stats_summoner(&mycomparison1);
					for (auto elem : ss)
					{
						stats_summoner.insert(elem.second);
					}
					header->set("style", "background: url(https://ddragon.leagueoflegends.com/cdn/img/champion/splash/" + champions[stats_summoner.begin()->championId].key + "_0.jpg) no-repeat;");
					auto header_box = add_div(header, "HeaderBox");
					{
						
						auto face = add_div(header_box, "Face");
						{
							auto profile_icon = add_div(face, "ProfileIcon");
							{
								profile_icon->add(cgicc::div(".").set("class", "borderImage").set("style", "background-image: url(//opgg-static.akamaized.net/images/borders2/" + boost::to_lower_copy(league.tier) + ".png);"));
								profile_icon->add(img().set("src", "//opgg-static.akamaized.net/images/profile_icons/profileIcon" + std::to_string(summoner.riotSummoner.profileIconId) + ".jpg").set("class", "ProfileImage"));
							}
						}
						auto profile = add_div(header_box, "Profile");
						{
							int64_t time_now = time(nullptr) * 1000;
							auto diff = time_now - summoner.lastUpdate;
							auto days = static_cast<int>(diff / (1000 * 60 * 60 * 24));
							auto hours = static_cast<int>(diff / (1000 * 60 * 60));
							auto minutes = static_cast<int>(diff / (1000 * 60));
							auto seconds = static_cast<int>(diff) / 1000;
							if (days)
								header_box->add(cgicc::div().set("class", "LastUpdate").add(span("Last updated:" + std::to_string(days) + " days ago")));
							else
							{
								if (hours)
									header_box->add(cgicc::div().set("class", "LastUpdate").add(span("Last updated:" + std::to_string(hours) + " days ago")));
								else
								{
									if (minutes)
										header_box->add(cgicc::div().set("class", "LastUpdate").add(span("Last updated:" + std::to_string(minutes) + " days ago")));
									else
										header_box->add(cgicc::div().set("class", "LastUpdate").add(span("Last updated:" + std::to_string(seconds) + " days ago")));
								}
							}
						}
						{
							auto rank = add_div(profile, "Rank");
							{
								rank->add(cgicc::div().set("class", "CountryImage").set("style", "display: inline-block; vertical-align: middle; width: 18px;").add(img().set("src", "http://localhost/LEAProject/icons/" + summoner.country + ".png")));
							}
							profile->add(cgicc::div(".").set("class", "Information").add(span(summoner.riotSummoner.name).set("class", "Name")));
							profile->add(cgicc::div().set("class", "Buttons").add(button("Update").set("class", "Button SemiRound Blue").set("onclick", ";").set("style", "position: relative;")));
						}
						auto summoner_rating_medium = add_div(header_box, "SummonerRatingMedium");
						{
							auto _id = 0;

							switch (str2int(league.rank.c_str()))
							{
							case str2int("I"):
								_id = 1;
								break;
							case str2int("II"):
								_id = 2;
								break;
							case str2int("III"):
								_id = 3;
								break;
							case str2int("IV"):
								_id = 4;
								break;
							case str2int("V"):
								_id = 5;
								break;
							default:
								break;
							}

							summoner_rating_medium->add(cgicc::div().set("class", "Medal").add(img().set("class", "Image").set("src", "//opgg-static.akamaized.net/images/medals/" + boost::to_lower_copy(league.tier) + "_" + std::to_string(_id) + ".png")));
							auto tier_rank_info = add_div(summoner_rating_medium, "TierRankInfo");
							{
								tier_rank_info->add(cgicc::div().set("class", "TierRank").add(span(league.tier + " " + league.rank).set("class", "tierRank")));
								{
									auto tier_info = add_div(tier_rank_info, "TierInfo");
									{
										tier_info->add(span(std::to_string(league.leaguePoints) + " LP").set("class", "LeaguePoints"));

										auto ratio = league.wins * 100 / (league.wins + league.losses);
										tier_info->add(span().set("class", "WinLose").add(span(" / " + std::to_string(league.wins) + "W").set("class", "wins")).add(span(" " + std::to_string(league.losses) + "L").set("class", "losses")).add(br()).add(span("Win Ration " + std::to_string(ratio) + "%").set("class", "winratio")));
									}

								}
								tier_rank_info->add(cgicc::div(league.leagueName).set("class", "Leaguename"));
							}
						}

						auto summoner_rating_country = add_div(header_box, "SummonerRatingCountry");
						{
							{
								auto medal = add_div(summoner_rating_country, "Flag");
								{
									medal->add(img().set("class", "Image").set("src", "//localhost/LEAProject/flags/png/" + summoner.country + ".png"));
								}
								auto tier_rank_info = add_div(summoner_rating_country, "TierRankInfo");
								{
									auto tier_rank = add_div(tier_rank_info, "TierRank");
									{
										tier_rank->add(span("Rank #" + std::to_string(summoner.rank_country)).set("class", "LeaguePoints"));
									}
								}
							}
							{
								auto medal = add_div(summoner_rating_country, "Flag");
								{
									medal->add(img().set("class", "Image").set("src", "//localhost/LEAProject/flags/png/europe.png"));
								}
								auto tier_rank_info = add_div(summoner_rating_country, "TierRankInfo");
								{
									auto tier_rank = add_div(tier_rank_info, "TierRank");
									{
										tier_rank->add(span("Rank #" + std::to_string(summoner.rank_all)).set("class", "LeaguePoints"));
									}
								}
							}
						}
					}
					
				}
				
				auto summoner_layout_content = add_div(summoner_layout, "Content SummonerLayoutContent");

				auto footer_wrap = add_div(layout_wrap, "FooterWrap");
				{
					footer_wrap->add(cgicc::div("2017 LEAProject.   Data based on League of legends Europe West.").set("class", "FooterCopyRight"));
				}

				{
					
					auto side_content = sideContent(summoner_layout_content, summoner.country, ss);

					auto real_content = realContent(summoner_layout_content, summoner.riotSummoner, matchs);

				}
			}

		}
		setlocale(LC_ALL, "");
		std::ofstream myfile;
		myfile.open("example.json");
		myfile << *layout_wrap << std::endl;
		myfile.close();

		out << *layout_wrap << "\n";
		delete layout_wrap;
	}

	void cgicc_render(LEA_Project::Summoner &summoner, Riot::LeaguePosition &league, std::vector<Riot::Match> &matchs, std::map<int, LEA_Project::ChampionStats> &ss)
	{
		using namespace cgicc;
		using namespace std;
		// Output the HTTP headers for an HTML document,
		// and the HTML 4.0 DTD info
		cout << HTTPHTMLHeader() << HTMLDoctype(HTMLDoctype::eStrict) << endl;
		cout << meta().set("charset", "UTF-16") << endl;
		cout << html() << endl;
		// Set up the page's header and title.
		cout << head() << link().set("rel", "stylesheet").set("href", "http://localhost/LEAProject/styles.css") << endl << head() << endl;
		// Start the HTML body
		cout << body() << endl;
		layoutWrapRender(cout, summoner, ss, matchs, league);
		cout << body() << html() << "\n";
	}

	int init_cgi()
	{
		using namespace cgicc;
		using namespace std;
		// Create a new Cgicc object containing all the CGI data
		Cgicc cgi;
		// Get a pointer to the environment
		auto name_itr = cgi.getElement("userName");
		if (name_itr == cgi.getElements().end() || name_itr->isEmpty())
		{
			auto redirect = HTTPRedirectHeader("http://localhost/LEAProject/index.html");
			redirect.render(cout);
			return EXIT_FAILURE;
		}
		
		// Get "userName" value
		auto name = name_itr->getValue();

		// Fetch Summoner Data
		LEA_Project::LEAData data;
		auto status = LEA_Project::fetchSummonerData(name, data);
		if(status)
		{
			auto redirect = HTTPRedirectHeader("http://localhost/LEAProject/not_found.html");
			redirect.render(cout);
			//TODO: Queue (/_!_\)
			return EXIT_SUCCESS;
		}

		cgicc_render(data.summoner, data.league, data.matchs, data.champions_stats);
		return EXIT_SUCCESS;
	}

}
