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

	bool mycomparison1(const LEA_Project::ChampionStats& lhs, const LEA_Project::ChampionStats& rhs) {
		return (lhs.wins + lhs.losses>rhs.wins + rhs.losses);
	}



	cgicc::div* sideContent(cgicc::div* summoner_layout, const std::map<int, LEA_Project::ChampionStats> ss)
	{
		std::set<LEA_Project::ChampionStats, bool(*)(const LEA_Project::ChampionStats&, const LEA_Project::ChampionStats&)> stats_summoner(&mycomparison1);
		for (auto elem : ss)
		{
			stats_summoner.insert(elem.second);
		}

		auto side_content = add_div(summoner_layout, "SideContent");
		{
			auto box = add_div(side_content, "Box");
			{
				auto most_champion_content = add_div(box, "MostChampionContent");
				{
					for (auto stat_champ : stats_summoner)
					{
						auto champion_box = add_div(most_champion_content, "ChampionBox Ranked");
						{
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
						}
					}
				}

				most_champion_content->add(cgicc::div().set("class", "MoreButton").add(a("Show More").set("href", "#").set("class", "Action")));
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
					auto game_item_list = add_div(_content, "GameItemList");
					{
						int max_list = 22;
						for (auto match : matchs)
						{
							max_list--;
							if (!max_list)
								break;
							auto position = LEA_Project::getPositionInMatch(match, summoner.id);
							auto win = match.participants[position - 1].stats.win;

							auto game_item_wrap = add_div(game_item_list, "GameItemWrap");

							cgicc::div* game_item;
							if (win) game_item = add_div(game_item_wrap, "GameItem Win");
							else game_item = add_div(game_item_wrap, "GameItem Lose");

							auto content = add_div(game_item, "Content");
							{
								auto game_stats = add_div(content, "GameStats");
								{
									auto game_type = add_div(game_stats, "GameType", "Ranked Solo");
									{
										int64_t time_now = time(nullptr) * 1000;
										auto diff = time_now - match.gameCreation;
										auto days = static_cast<int>(diff / (1000 * 60 * 60 * 24));
										auto hours = static_cast<int>(diff / (1000 * 60 * 60));
										auto minutes = static_cast<int>(diff / (1000 * 60));
										auto seconds = static_cast<int>(diff) / 1000;
										if (days)
											auto timestamp = add_div(game_stats, "TimeStamp", std::to_string(days) + " days ago");
										else
										{
											if (hours)
												auto timestamp = add_div(game_stats, "TimeStamp", std::to_string(hours) + " hours ago");
											else
											{
												if (minutes)
													auto timestamp = add_div(game_stats, "TimeStamp", std::to_string(minutes) + " minutes ago");
												else
													auto timestamp = add_div(game_stats, "TimeStamp", std::to_string(seconds) + " seconds ago");
											}
										}
									}
									auto bar = add_div(game_stats, "Bar", ".");
									auto game_result = add_div(game_stats, "GameResult", win ? "Victory" : "Defeat");
									{
										auto hours = static_cast<int>(match.gameDuration / 60 * 60);
										auto minutes = static_cast<int>((match.gameDuration / 60) % 60);
										auto seconds = static_cast<int>(match.gameDuration % 60);
										auto game_length = add_div(game_stats, "GameLength", std::to_string(minutes) + "m:" + std::to_string(minutes) + "s");
									}
								}
								auto game_setting_info = add_div(content, "GameSettingInfo");
								{
									auto champ_name = champions[match.participants[position - 1].championId].key;

									auto champion_image = add_div(game_setting_info, "ChampionImage");
									{
										auto src = "//opgg-static.akamaized.net/images/lol/champion/" + champ_name + ".png?image=c_scale,w_46&amp;v=1";
										champion_image->add(a(HTMLAttribute("href", "#")).add(img().set("src", src).set("class", "Image").set("alt", champ_name)));
									}
									auto summoner_spell = add_div(game_setting_info, "SummonerSpell");
									{
										auto spell01 = add_div(summoner_spell, "Spell");
										{
											auto spell_name = match.participants[position - 1].spell1Id;
											// TODO : get spell_name
											std::string src = "//opgg-static.akamaized.net/images/lol/spell/SummonerFlash.png?image=c_scale,w_22&amp;v=1";
											spell01->add(img().set("src", src));
										}
										auto spell02 = add_div(summoner_spell, "Spell");
										{
											auto spell_name = match.participants[position - 1].spell2Id;
											// TODO : get spell_name
											std::string src = "//opgg-static.akamaized.net/images/lol/spell/SummonerTeleport.png?image=c_scale,w_22&amp;v=1";
											spell02->add(img().set("src", src));
										}
									}
									auto champion_name = add_div(game_setting_info, "ChampionName");
									{
										champion_name->add(a(champ_name).set("href", "#").set("target", "_self"));
									}
								}
								auto kda = add_div(content, "KDA");
								{
									auto kda_d = add_div(kda, "KDA");
									{
										auto sum_stats = match.participants[position - 1].stats;
										kda_d->add(span(std::to_string(sum_stats.kills) + "  /  ", HTMLAttribute("class", "Kill")));
										kda_d->add(span(std::to_string(sum_stats.deaths), HTMLAttribute("class", "Death")));
										kda_d->add(span("  /  " + std::to_string(sum_stats.assists), HTMLAttribute("class", "Assist")));
									}
									auto kda_ratio = add_div(kda, "KDARatio");
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
								}
								auto stats = add_div(content, "Stats");
								{
									auto level = add_div(stats, "Level", "Level" + std::to_string(match.participants[position - 1].stats.champLevel));
									auto cs = add_div(stats, "CS");
									{
										cs->add(span(std::to_string(match.participants[position - 1].stats.totalMinionsKilled) + "CS").set("class", "CS tip"));
									}
									auto ward = add_div(stats, "Ward");
									{
										ward->add(span(std::to_string(match.participants[position - 1].stats.visionWardsBoughtInGame) + " Control Ward", HTMLAttribute("class", "wards vision")));
									}
									// TODO : kill participation
									auto ck_rate = add_div(stats, "CKRate", "P/Kill -1%");
								}
								auto items = add_div(content, "Items");
								{
									auto item01 = add_div(items, "Item");
									{
										std::string src = "//opgg-static.akamaized.net/images/lol/item/" + std::to_string(match.participants[position - 1].stats.item0) + ".png?image=c_scale,w_32&amp;v=1";
										item01->add(img(HTMLAttribute("src", src)));
									}
									auto item02 = add_div(items, "Item");
									{
										std::string src = "//opgg-static.akamaized.net/images/lol/item/" + std::to_string(match.participants[position - 1].stats.item1) + ".png?image=c_scale,w_32&amp;v=1";
										item02->add(img(HTMLAttribute("src", src)));
									}
									auto item03 = add_div(items, "Item");
									{
										std::string src = "//opgg-static.akamaized.net/images/lol/item/" + std::to_string(match.participants[position - 1].stats.item2) + ".png?image=c_scale,w_32&amp;v=1";
										item03->add(img(HTMLAttribute("src", src)));
									}
									auto item04 = add_div(items, "Item");
									{
										std::string src = "//opgg-static.akamaized.net/images/lol/item/" + std::to_string(match.participants[position - 1].stats.item3) + ".png?image=c_scale,w_32&amp;v=1";
										item04->add(img(HTMLAttribute("src", src)));
									}
									auto item05 = add_div(items, "Item");
									{
										std::string src = "//opgg-static.akamaized.net/images/lol/item/" + std::to_string(match.participants[position - 1].stats.item4) + ".png?image=c_scale,w_32&amp;v=1";
										item05->add(img(HTMLAttribute("src", src)));
									}
									auto item06 = add_div(items, "Item");
									{
										std::string src = "//opgg-static.akamaized.net/images/lol/item/" + std::to_string(match.participants[position - 1].stats.item5) + ".png?image=c_scale,w_32&amp;v=1";
										item06->add(img(HTMLAttribute("src", src)));
									}
								}
								auto trinket_with_item = add_div(content, "TrinketWithItem");
								{
									auto item00 = add_div(trinket_with_item, "Item");
									{
										std::string src = "//opgg-static.akamaized.net/images/lol/item/" + std::to_string(match.participants[position - 1].stats.item6) + ".png?image=c_scale,w_32&amp;v=1";
										item00->add(img(HTMLAttribute("src", src)));
									}
								}
								auto follow_players_names = add_div(content, "FollowPlayers Names");
								{
									auto team01 = add_div(follow_players_names, "Team");
									{
										auto summoner01 = add_div(team01, "Summoner");
										{
											// TODO : use country tag
											auto country_image = add_div(summoner01, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/ma.png")));
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
											// TODO : use country tag
											auto country_image = add_div(summoner02, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/ma.png")));
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
											// TODO : use country tag
											auto country_image = add_div(summoner03, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/ma.png")));
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
											// TODO : use country tag
											auto country_image = add_div(summoner04, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/ma.png")));
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
											// TODO : use country tag
											auto country_image = add_div(summoner05, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/ma.png")));
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
									auto team02 = add_div(follow_players_names, "Team");
									{
										auto summoner06 = add_div(team02, "Summoner");
										{
											// TODO : use country tag
											auto country_image = add_div(summoner06, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/ma.png")));
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
											// TODO : use country tag
											auto country_image = add_div(summoner07, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/ma.png")));
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
											// TODO : use country tag
											auto country_image = add_div(summoner08, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/ma.png")));
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
											// TODO : use country tag
											auto country_image = add_div(summoner09, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/ma.png")));
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
											// TODO : use country tag
											auto country_image = add_div(summoner10, "CountryImage");
											{
												country_image->add(img(HTMLAttribute("src", "http://localhost/LEAProject/ma.png")));
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
							//cout << *game_item_wrap << endl;
						}
					}

					_content->add(cgicc::div().set("class", "GameMoreButton Box").set("title", "GIMMMME MOOOOOOORE").add(a("Show More").set("href", "#").set("class", "Button")));
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
					auto past_rank = add_div(header, "PastRank");
					{
						auto past_rank_list = new ul(HTMLAttribute("class", "PastRankList"));
						past_rank->add(past_rank_list);
						past_rank_list->add(li(HTMLAttribute("class", "Item tip")).add(b("S4 Unranked")));
						past_rank_list->add(li(HTMLAttribute("class", "Item tip")).add(b("S5 Unranked")));
						past_rank_list->add(li(HTMLAttribute("class", "Item tip")).add(b("S6 Unranked")));
					}
					auto face = add_div(header, "Face");
					{
						auto profile_icon = add_div(face, "ProfileIcon");
						{
							profile_icon->add(cgicc::div(".").set("class", "borderImage").set("style", "background-image: url(//opgg-static.akamaized.net/images/borders2/" + boost::to_lower_copy(league.tier) + ".png);"));
							profile_icon->add(img().set("src", "//opgg-static.akamaized.net/images/profile_icons/profileIcon1666.jpg").set("class", "ProfileImage"));
						}
					}
					auto profile = add_div(header, "Profile");
					{
						int64_t time_now = time(nullptr) * 1000;
						auto diff = time_now - summoner.lastUpdate;
						auto days = static_cast<int>(diff / (1000 * 60 * 60 * 24));
						auto hours = static_cast<int>(diff / (1000 * 60 * 60));
						auto minutes = static_cast<int>(diff / (1000 * 60));
						auto seconds = static_cast<int>(diff) / 1000;
						if (days)
							header->add(cgicc::div().set("class", "LastUpdate").add(span("Last updated:" + std::to_string(days) + " days ago")));
						else
						{
							if (hours)
								header->add(cgicc::div().set("class", "LastUpdate").add(span("Last updated:" + std::to_string(hours) + " days ago")));
							else
							{
								if (minutes)
									header->add(cgicc::div().set("class", "LastUpdate").add(span("Last updated:" + std::to_string(minutes) + " days ago")));
								else
									header->add(cgicc::div().set("class", "LastUpdate").add(span("Last updated:" + std::to_string(seconds) + " days ago")));
							}
						}
					}
					{
						profile->add(cgicc::div(".").set("class", "Information").add(span(summoner.riotSummoner.name).set("class", "Name")));
						//auto buttons = add_div(profile, "Buttons");
						profile->add(cgicc::div().set("class", "Buttons").add(button("Update").set("class", "Button SemiRound Blue").set("onclick", ";").set("style", "position: relative;")));
					}

				}
				auto box = add_div(summoner_layout, "Box");
				{
					// TODO : add Title
					int _id = 0;
					auto summoner_rating_medium = add_div(box, "SummonerRatingMedium");
					{
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

									// TODO : Compute Win Ratio
									tier_info->add(span().set("class", "WinLose").add(span(" / " + std::to_string(league.wins) + "W").set("class", "wins")).add(span(" " + std::to_string(league.losses) + "L").set("class", "losses")).add(br()).add(span("Win Ration ?%").set("class", "winratio")));
								}

							}
							tier_rank_info->add(cgicc::div(league.leagueName).set("class", "Leaguename"));
						}

					}
				}
				auto summoner_layout_content = add_div(summoner_layout, "Content SummonerLayoutContent");

				auto footer_wrap = add_div(layout_wrap, "FooterWrap");
				{
					footer_wrap->add(cgicc::div("2012-2017 OP.GG.   Data based on League of legends Europe West.").set("class", "FooterCopyRight"));
				}

				{
					serverBD->getListChampions(champions);

					auto real_content = realContent(summoner_layout_content, summoner.riotSummoner, matchs);

					auto side_content = sideContent(summoner_layout_content, ss);
				}
			}

		}
		setlocale(LC_ALL, "");
		//ofstream myfile;
		//myfile.open("example.json");
		//myfile << *layout_wrap << endl;
		//myfile.close();

		out << *layout_wrap << "\n";
		delete layout_wrap;
		out << body() << html() << "\n";
	}
}
