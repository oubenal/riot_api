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


using namespace std;
using namespace cgicc;

// Function prototypes
void WriteAddress();
void Err(const string s);

int cgicc_render()
{
	try {

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
			errMsg.str("");
			errMsg << "No summoner's name was written " << "str" << " was empty";
			Err(errMsg.str());
		}
		else
			name = strItr->getValue();

		//string name = "smurfAtlas";
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
				// Output the HTTP headers for an HTML document,
				// and the HTML 4.0 DTD info
				cout << HTTPHTMLHeader() << HTMLDoctype(HTMLDoctype::eStrict) << endl;
				cout << meta().set("charset", "UTF-16") << endl;
				cout << html() << endl;
				// Set up the page's header and title.
				cout << head() << link().set("rel", "stylesheet").set("href", "http://localhost/LEAProject/styles.css") << endl << head() << endl;
				// Start the HTML body
				cout << body() << endl;

				auto summoner = LEA_Project::getSummonerSummoners(name);
				auto pair = LEA_Project::getSummonerHistory(name);
				auto matchs = pair.first;
				auto league = pair.second;
				auto ss = LEA_Project::getSummonerChampionsStats(name);

				HtmlRender::layoutWrapRender(cout, summoner, ss, matchs, league);
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

	// Did any errors occur?
	catch (const std::exception& e) {

		// This is a dummy exception handler, as it doesn't really do
		// anything except print out information.

		// Reset all the HTML elements that might have been used to
		// their initial state so we get valid output
		html::reset();      head::reset();          body::reset();
		title::reset();     h1::reset();            h4::reset();
		comment::reset();   td::reset();            tr::reset();
		table::reset();     cgicc::div::reset();    p::reset();
		a::reset();         h2::reset();            colgroup::reset();

		// Output the HTTP headers for an HTML document, and the HTML 4.0 DTD info
		cout << HTTPHTMLHeader() << HTMLDoctype(HTMLDoctype::eStrict) << endl;
		cout << html().set("lang", "en").set("dir", "ltr") << endl;

		// Set up the page's header and title.
		// I will put in lfs to ease reading of the produced HTML.
		cout << head() << endl;

		// Output the style sheet portion of the header
		cout << style() << comment() << endl;
		cout << "body { color: black; background-color: white; }" << endl;
		cout << "hr.half { width: 60%; align: center; }" << endl;
		cout << "span.red, strong.red { color: red; }" << endl;
		cout << "div.notice { border: solid thin; padding: 1em; margin: 1em 0; "
			<< "background: #ddd; }" << endl;

		cout << comment() << style() << endl;

		cout << title("GNU cgicc exception") << endl;
		cout << head() << endl;

		cout << body() << endl;

		cout << cgicc::div().set("align", "center").set("class", "notice") << endl;

		cout << h2(e.what()) << endl;

		// End of document
		cout << cgicc::div() << endl;
		cout << hr().set("class", "half") << endl;
		cout << body() << html() << endl;

		return EXIT_SUCCESS;
	}
}

// Main Street, USA
int
main(int argc,
	char ** argv)
{
	return cgicc_render();
}

/* -------------------- Function: Err ------------------- */
/* Returns an error to the client */
void Err(const string s)
{
	// Output the HTTP headers for an HTML document, and the HTML 4.0 DTD info
	cout << HTTPHTMLHeader() << HTMLDoctype(HTMLDoctype::eStrict) << endl;
	cout << html().set("lang", "EN").set("dir", "LTR") << endl;

	// Set up the page's header and title.
	cout << head(title("Mark B. Motl - CS 4312 - Mixed Content Types"))
		<< endl;

	// Start the HTML body
	cout << body().set("BGCOLOR", "#FFFFFF") << endl;

	cout << h1("Mixed Content Types Error") << endl
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