#include <string>
#include <riot_api/URLReader.h>

namespace Riot
{
	namespace URLReader
	{
		size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
		{
			((std::string*)userp)->append((char*)contents, size * nmemb);
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
				CURLcode res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
			}

			return readBuffer;
		}
	}
}
