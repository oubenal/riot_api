#include <iostream>
#include <string>
#include <boost/asio.hpp>

//#include <boost/array.hpp>
#include <riot_api/URLReader.h>
#include <rapidjson/document.h>
#include <rapidjson/reader.h>
#include <riot_api/Riot.h>
/*
Include directly the different
headers from cppconn/ and mysql_driver.h + mysql_util.h
(and mysql_connection.h). This will reduce your build time!
*/
#include <mysql_connection.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>



using namespace std;

#define _HOST "localhost"
#define _USER "root"
#define _PASS "r4yzAnm5"
#define _DB "riot_api"

#include <new>
#include <vector>
#include <stdexcept>
#include <cstdlib>

#include "cgicc/CgiDefs.h"
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>

#if HAVE_SYS_UTSNAME_H
#  include <sys/utsname.h>
#endif

#if HAVE_SYS_TIME_H
#  include <sys/time.h>
#endif

#include "riot_api/styles.h"


using namespace std;
using namespace cgicc;

// Function prototypes
void dumpEnvironment(const CgiEnvironment& env);
void dumpList(const Cgicc& formData);
void showForm(const Cgicc& formData);
// Function prototypes
void WriteHeading(void);
void DrawImage(string s);
void WriteAddress();
void Err(const string s);

// Print the form for this CGI
void
printForm(const Cgicc& cgi)
{
	cout << "<form method=\"post\" action=\""
		<< cgi.getEnvironment().getScriptName() << "\">" << endl;

	cout << "<table>" << endl;

	cout << "<tr><td class=\"title\">Cookie Name</td>"
		<< "<td class=\"form\">"
		<< "<input type=\"text\" name=\"name\" />"
		<< "</td></tr>" << endl;

	cout << "<tr><td class=\"title\">Cookie Value</td>"
		<< "<td class=\"form\">"
		<< "<input type=\"text\" name=\"value\" />"
		<< "</td></tr>" << endl;

	cout << "</table>" << endl;

	cout << "<div class=\"center\"><p>"
		<< "<input type=\"submit\" name=\"submit\" value=\"Set the cookie\" />"
		<< "<input type=\"reset\" value=\"Nevermind\" />"
		<< "</p></div></form>" << endl;
}

// Main Street, USA
int
main(int argc,
	char ** argv)
{
	try {

		// Create a new Cgicc object containing all the CGI data
		Cgicc cgi;

		string name;
		// Get a pointer to the environment
		const CgiEnvironment& env = cgi.getEnvironment();

		ostringstream errMsg;
		const_form_iterator strItr = cgi.getElement("str");
		
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
		
		// Access Riot API
		// Search by summonner's Name ang get info


		// getElement and getValue
		
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
				
				auto summoner = Riot::getSummonerSummonersByName(name);
				auto league_positions = Riot::getLeaguePositionsBySummoner(summoner);

				
				// Output the HTTP headers for an HTML document,
				// and the HTML 4.0 DTD info
				cout << HTTPHTMLHeader() << HTMLDoctype(HTMLDoctype::eStrict) << endl;
				cout << html().set("lang", "en").set("dir", "ltr") << endl;

				// Set up the page's header and title.
				// I will put in lfs to ease reading of the produced HTML.
				cout << head("Project LEA - Alpha version") << endl;

				// Output the style sheet portion of the header
				cout << style() << comment() << endl;
				cout << styles;
				cout << comment() << style() << endl;

				cout << title() << "GNU cgicc v" << cgi.getVersion()
					<< " HTTPCookie" << title() << endl;

				cout << head() << endl;

				// show result
				cout << body() << endl;
				cout << h1() << summoner.name << "'s profile = \n" << h1() << endl;
				cout << h2() << "\t... Level = " << summoner.summonerLevel << "\n\n" << h2() << "\n";
				for (auto pos : league_positions)
				{
					cout << h3() << "\t... QueueType = " << pos.queueType << h3() << "\n";
					cout << p() << "\t... hotStreak = " << pos.hotStreak << p() << "\n";
					cout << p() << "\t... wins = " << pos.wins << p() << "\n";
					cout << p() << "\t... losses = " << pos.losses << p() << "\n";
					cout << p() << "\t... tier = " << pos.tier << " " << pos.rank << p() << "\n";
					cout << p() << "\t... LP = " << pos.leaguePoints << p() << "\n\n";
				}

				cout << p() << endl;

				WriteAddress();
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

		cout << h1() << "GNU cgi" << span("cc", set("class", "red"))
			<< " caught an exception" << h1() << endl;

		cout << cgicc::div().set("align", "center").set("class", "notice") << endl;

		cout << h2(e.what()) << endl;

		// End of document
		cout << cgicc::div() << endl;
		cout << hr().set("class", "half") << endl;
		cout << body() << html() << endl;

		return EXIT_SUCCESS;
	}
/*
	if (argc < 2)
		return 1;
	// Connect and Retrive/Update MySQL DataBase
	std::map<int, Riot::Champion> champions;

	try {
		cout << endl;
		cout << "Running 'SELECT 'Hello World!' AS _message'..." << endl;

		// Create a connection
		sql::Driver *driver = get_driver_instance();
		sql::Connection *con = driver->connect(_HOST, _USER, _PASS);
		// Connect to the MySQL database
		con->setSchema(_DB);
		// Query DB
		sql::Statement *stmt = con->createStatement();
		sql::ResultSet *res = stmt->executeQuery("SELECT * FROM riot_api.champions;");
		// Parse query result
		while (res->next()) {
			Riot::Champion champ;
			champ.id = res->getInt("id");
			champ.title = res->getString("title");
			champ.key = res->getString("key");
			champ.name = res->getString("name");
			champions[champ.id] = champ;
			// Access column data by numeric offset, 1 is the first column
			//cout << res->getString(1) << endl;
		}
		// Delete object used
		delete res;
		delete stmt;
		delete con;
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
*/
/*
	// Access Riot API
	// Search by summonner's Name ang get info

	
	cout << "Riot API SUMMONER-V3 for : " << argv[1] << "\n";

	auto summoner = Riot::getSummonerSummonersByName(argv[1]);
	auto league_positions = Riot::getLeaguePositionsBySummoner(summoner);

	// show result
	cout << "\t... " << summoner.name << "'s profile = \n";
	cout << "\t... Level = " << summoner.summonerLevel << "\n\n";
	for (auto pos : league_positions)
	{
		cout << "\t... QueueType = " << pos.queueType << "\n";
		cout << "\t... hotStreak = " << pos.hotStreak << "\n";
		cout << "\t... wins = " << pos.wins << "\n";
		cout << "\t... losses = " << pos.losses << "\n";
		cout << "\t... tier = " << pos.tier << " " << pos.rank << "\n";
		cout << "\t... LP = " << pos.leaguePoints << "\n\n";
	}

	cout << endl;

	return EXIT_SUCCESS;
	*/

}

// Print out a table of the CgiEnvironment
void
dumpEnvironment(const CgiEnvironment& env)
{
	// This is just a brain-dead dump of information.
	// Almost all of this code is for HTML formatting
	cout << h2("Environment information from CgiEnvironment") << endl;

	cout << cgicc::div().set("align", "center") << endl;

	cout << table() << endl;

	cout << tr() << td("Request Method").set("class", "title")
		<< td(env.getRequestMethod()).set("class", "data") << tr() << endl;
	cout << tr() << td("Path Info").set("class", "title")
		<< td(env.getPathInfo()).set("class", "data") << tr() << endl;
	cout << tr() << td("Path Translated").set("class", "title")
		<< td(env.getPathTranslated()).set("class", "data") << tr() << endl;
	cout << tr() << td("Script Name").set("class", "title")
		<< td(env.getScriptName()).set("class", "data") << tr() << endl;
	cout << tr() << td("HTTP Referrer").set("class", "title")
		<< td(env.getReferrer()).set("class", "data") << tr() << endl;
	cout << tr() << td("HTTP Cookie").set("class", "title")
		<< td(env.getCookies()).set("class", "data") << tr() << endl;
	cout << tr() << td("Query String").set("class", "title")
		<< td(env.getQueryString()).set("class", "data") << tr() << endl;
	cout << tr() << td("Content Length").set("class", "title")
		<< td().set("class", "data") << env.getContentLength()
		<< td() << tr() << endl;
	cout << tr() << td("Post Data").set("class", "title")
		<< td().set("class", "data")
		<< pre(env.getPostData()).set("class", "data") << td()
		<< tr() << endl;
	cout << tr() << td("Remote Host").set("class", "title")
		<< td(env.getRemoteHost()).set("class", "data") << tr() << endl;
	cout << tr() << td("Remote Address").set("class", "title")
		<< td(env.getRemoteAddr()).set("class", "data") << tr() << endl;
	cout << tr() << td("Authorization Type").set("class", "title")
		<< td(env.getAuthType()).set("class", "data") << tr() << endl;
	cout << tr() << td("Remote User").set("class", "title")
		<< td(env.getRemoteUser()).set("class", "data") << tr() << endl;
	cout << tr() << td("Remote Identification").set("class", "title")
		<< td(env.getRemoteIdent()).set("class", "data") << tr() << endl;
	cout << tr() << td("Content Type").set("class", "title")
		<< td(env.getContentType()).set("class", "data") << tr() << endl;
	cout << tr() << td("HTTP Accept").set("class", "title")
		<< td(env.getAccept()).set("class", "data") << tr() << endl;
	cout << tr() << td("User Agent").set("class", "title")
		<< td(env.getUserAgent()).set("class", "data") << tr() << endl;
	cout << tr() << td("Server Software").set("class", "title")
		<< td(env.getServerSoftware()).set("class", "data") << tr() << endl;
	cout << tr() << td("Server Name").set("class", "title")
		<< td(env.getServerName()).set("class", "data") << tr() << endl;
	cout << tr() << td("Gateway Interface").set("class", "title")
		<< td(env.getGatewayInterface()).set("class", "data") << tr() << endl;
	cout << tr() << td("Server Protocol").set("class", "title")
		<< td(env.getServerProtocol()).set("class", "data") << tr() << endl;
	cout << tr() << td("Server Port").set("class", "title")
		<< td().set("class", "data") << env.getServerPort()
		<< td() << tr() << endl;
	cout << tr() << td("HTTPS").set("class", "title")
		<< td().set("class", "data") << (env.usingHTTPS() ? "true" : "false")
		<< td() << tr() << endl;
	cout << tr() << td("Redirect Request").set("class", "title")
		<< td(env.getRedirectRequest()).set("class", "data") << tr() << endl;
	cout << tr() << td("Redirect URL").set("class", "title")
		<< td(env.getRedirectURL()).set("class", "data") << tr() << endl;
	cout << tr() << td("Redirect Status").set("class", "title")
		<< td(env.getRedirectStatus()).set("class", "data") << tr() << endl;

	cout << table() << cgicc::div() << endl;
}

// Print out the value of every form element
void
dumpList(const Cgicc& formData)
{
	cout << h2("Form Data via vector") << endl;

	cout << cgicc::div().set("align", "center") << endl;

	cout << table() << endl;

	cout << tr().set("class", "title") << td("Element Name")
		<< td("Element Value") << tr() << endl;

	// Iterate through the vector, and print out each value
	const_form_iterator iter;
	for (iter = formData.getElements().begin();
		iter != formData.getElements().end();
		++iter) {
		cout << tr().set("class", "data") << td(iter->getName())
			<< td(iter->getValue()) << tr() << endl;
	}
	cout << table() << cgicc::div() << endl;
}

// Print out information customized for each element
void
showForm(const Cgicc& formData) {

	// I am using an if statement to check if each element is found
	cout << h2("Form Data via Cgicc") << endl;

	cout << cgicc::div().set("class", "notice") << endl;

	//getElement
	const_form_iterator name = formData.getElement("name");
	if (name != (*formData).end() && !name->isEmpty())
		cout << "Your name is " << **name << '.' << br() << endl;
	else
		cout << "You don't have a name." << br() << endl;

	// getElement and getDoubleValue
	const_form_iterator salary = formData.getElement("bucks");
	if (salary != (*formData).end() && !salary->isEmpty())
		cout << "You make " << (*salary).getDoubleValue(80, 120)
		<< " million dollars." << br() << endl;
	else
		cout << "You don't have a salary." << br() << endl;

	// getElement and getIntegerValue
	const_form_iterator hours = formData.getElement("time");
	if (hours != (*formData).end() && !(*hours).isEmpty())
		cout << "You've wasted " << (*hours).getIntegerValue()
		<< " hours on the web." << br() << endl;
	else
		cout << "You haven't wasted any time on the web." << br() << endl;

	// getElement and getStrippedValue
	const_form_iterator thoughts = formData.getElement("thoughts");
	if (thoughts != (*formData).end() && !(*thoughts).isEmpty()) {
		std::string temp = (*thoughts).getStrippedValue();
		cout << "Your thoughts: " << temp << br() << endl;
	}
	else
		cout << "You don't have any thoughts!?" << br() << endl;

	// queryCheckbox
	if (formData.queryCheckbox("hungry"))
		cout << "You are hungry." << br() << endl;
	else
		cout << "You are not hungry." << br() << endl;

	// getElement
	std::vector<FormEntry> flavors;
	formData.getElement("flavors", flavors);
	if (!flavors.empty()) {
		cout << "You like ";
		for (std::string::size_type i = 0; i < flavors.size(); i++) {
			cout << flavors[i].getValue();
			if (i < flavors.size() - 2)
				cout << ", ";
			else if (i == flavors.size() - 2)
				cout << " and ";
		}
		cout << " ice cream." << br() << endl;
	}
	else
		cout << "You don't like ice cream!?" << br() << endl;

	// getElement
	const_form_iterator hair = formData.getElement("hair");
	if (hair != (*formData).end())
		cout << "Your hair is " << **hair << '.' << br() << endl;
	else
		cout << "You don't have any hair." << br() << endl;

	cout << "You surf the web with " << formData("browser") << '.'
		<< br() << endl;

	// getElement
	std::vector<FormEntry> authors;
	formData.getElement("authors", authors);
	if (!authors.empty()) {
		cout << "You like to read books by ";
		for (std::string::size_type i = 0; i < authors.size(); ++i) {
			cout << authors[i].getValue();
			if (i < authors.size() - 2)
				cout << ", ";
			else if (i == authors.size() - 2)
				cout << " and ";
		}
		cout << "." << br() << endl;
	}
	else
		cout << "You don't like to read!?" << br() << endl;

	cout << cgicc::div() << endl;


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
	cout << p() << hr() << p() << address()
		<< a("Redouane Oubenal").set("href", "http://www.github.com/oubenal")
		<< br() << endl
		<< a("redouane@oubenal.com").set("href", "mailto:redouane@oubenal.com")
		<< br() << endl;

	// Information on this query
	cout << address() << body() << html() << endl;
}