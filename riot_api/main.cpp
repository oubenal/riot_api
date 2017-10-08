#include <riot_api/HtmlRender.h>

#define _HOST "localhost"
#define _USER "root"
#define _PASS "r4yzAnm5"
#define _DB "riot_api"

// Main Street, USA
int
main(int argc,
	char ** argv)
{	
	//LEA_Project::insertSummoners();
	return HtmlRender::init_cgi();
}
