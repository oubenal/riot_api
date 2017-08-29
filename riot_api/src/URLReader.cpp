#include <string>
#include <riot_api/URLReader.h>

#include <curl/curl.h>
#include <iostream>

#include <exception>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>


namespace Riot
{
	namespace URLReader
	{
		static std::string readBuffer;
		size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
		{
			static_cast<std::string*>(userp)->append(static_cast<char*>(contents), size * nmemb);
			return size * nmemb;
		}

		std::string read(const std::string& url)
		{
			CURL * curl = curl_easy_init();
			std::string readBuffer;
			if (curl) {
				curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
				curl_easy_setopt(curl, CURLOPT_TRANSFERTEXT, 2);
				auto curl_code = curl_easy_perform(curl); // auto res = curl_easy_perform(curl);
				long http_code = 0;
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
				if (http_code != 200 || curl_code == CURLE_ABORTED_BY_CALLBACK)
				
					curl_easy_cleanup(curl);
			}
			return readBuffer;
		}
	}
}
