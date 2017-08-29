#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <utility>

#include <rapidjson/reader.h>
#include <riot_api/Riot.h>
#include <riot_api/serverBD.h>
#include <riot_api/LEA_Project.h>
#include <riot_api/HtmlRender.h>
/*
Include directly the different
headers from cppconn/ and mysql_driver.h + mysql_util.h
(and mysql_connection.h). This will reduce your build time!
*/
#include <mysql_connection.h>

using namespace std;

#define _HOST "localhost"
#define _USER "root"
#define _PASS "r4yzAnm5"
#define _DB "riot_api"

#include <vector>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>
#include <cgicc/HTMLElementList.h>
#include <cgicc/HTMLElement.h>
#include <cgicc/HTTPRedirectHeader.h>


using namespace std;
using namespace cgicc;

// Function prototypes
void WriteAddress();
void Err(const string s);
void cgicc_render(LEA_Project::Summoner &summoner, Riot::LeaguePosition &league, std::vector<Riot::Match> &matchs, std::map<int, LEA_Project::ChampionStats> &ss);

int init_cgicc()
{
	// Create a new Cgicc object containing all the CGI data
	Cgicc cgi;
	string name;
	// Get a pointer to the environment
	ostringstream errMsg;
	const_form_iterator strItr = cgi.getElement("userName");

	if (strItr == cgi.getElements().end())
	{
		errMsg.str("");
		errMsg << "variable " << "str" << " was missing";
		Err(errMsg.str());
	}
	else if (strItr->isEmpty())
	{
		auto redirect = HTTPRedirectHeader("http://localhost/LEAProject/index.html");
		redirect.render(cout);
	}
	else
		name = strItr->getValue();


	const_form_iterator whichFormItr = cgi.getElement("whichForm");
	if (whichFormItr == cgi.getElements().end())
	{
		errMsg.str("");
		errMsg << "variable " << "whichForm" << " was missing";
		Err(errMsg.str());
	}
	else if (whichFormItr->isEmpty())
	{
		errMsg.str("");
		errMsg << "variable " << "whichForm" << " was empty";
		Err(errMsg.str());
	}
	else
	{
		string form = whichFormItr->getValue();
		if (form == "Search")
		{
			auto summoner = LEA_Project::getSummonerSummoners(name);
			if (summoner.riotSummoner.name.empty())
			{
				auto redirect = HTTPRedirectHeader("http://localhost/LEAProject/not_found.html");
				redirect.render(cout);
				//TODO: Queue (/_!_\)
				return EXIT_SUCCESS;
			}
			auto pair = LEA_Project::getSummonerHistory(name);
			auto matchs = pair.first;
			auto league = pair.second;
			auto ss = LEA_Project::getSummonerChampionsStats(name);

			cgicc_render(summoner, league, matchs, ss);
		}

		else
		{
			errMsg.str("");
			errMsg << "variable " << "whichForm" << " which has a value of";
			errMsg << form << " is illegal";
			Err(errMsg.str());
		}
	}
	return EXIT_SUCCESS;
}
void cgicc_render(LEA_Project::Summoner &summoner, Riot::LeaguePosition &league, std::vector<Riot::Match> &matchs, std::map<int, LEA_Project::ChampionStats> &ss)
{
	// Output the HTTP headers for an HTML document,
	// and the HTML 4.0 DTD info
	cout << HTTPHTMLHeader() << HTMLDoctype(HTMLDoctype::eStrict) << endl;
	cout << meta().set("charset", "UTF-16") << endl;
	cout << html() << endl;
	// Set up the page's header and title.
	cout << head() << link().set("rel", "stylesheet").set("href", "http://localhost/LEAProject/styles.css") << endl << head() << endl;
	// Start the HTML body
	cout << body() << endl;
	HtmlRender::layoutWrapRender(cout, summoner, ss, matchs, league);	
}

// Main Street, USA
int
main(int argc,
	char ** argv)
{	
	return init_cgicc();
}

/* -------------------- Function: Err ------------------- */
/* Returns an error to the client */
void Err(const string s)
{
	// Output the HTTP headers for an HTML document, and the HTML 4.0 DTD info
	cout << HTTPHTMLHeader() << HTMLDoctype(HTMLDoctype::eStrict) << endl;
	cout << html().set("lang", "EN").set("dir", "LTR") << endl;

	// Set up the page's header and title.
	cout << head(title("LEAProject ERROR"))
		<< endl;

	// Start the HTML body
	cout << body().set("BGCOLOR", "#FFFFFF") << endl;

	cout << h1("Error : ") << endl
		<< "Your form results could not be processed because "
		<< s << "." << endl;
	WriteAddress();
	exit(EXIT_FAILURE);
}

/* -------------------- Function: WriteAddress ---------- */
void WriteAddress()
{
	cout << cgicc::div().set("class", "rightpane") << endl;
	cout << address()
		<< a("Redouane Oubenal").set("href", "http://www.github.com/oubenal")
		<< br() << endl
		<< a("redouane@oubenal.com").set("href", "mailto:redouane@oubenal.com")
		<< br() << endl;
	cout << address() << endl;
	cout << cgicc::div() << endl;
	// Information on this query
	cout << body() << html() << endl;
}