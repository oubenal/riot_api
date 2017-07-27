#ifndef URL_READER_HPP
#define URL_READER_HPP

#define CURL_STATICLIB
#include <curl/curl.h>
#include <string>

namespace Riot
{
	namespace URLReader
	{
		std::string read(const std::string& url);
	}
}

#endif // URL_READER_HPP