#include <string>
#include <riot_api/URLReader.h>

#include <curl/curl.h>
#include <iostream>

#include <exception>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <queue>
#include <thread>

namespace Riot
{
	namespace URLReader
	{
		void HeaderResponse::handle_limit(std::map<std::string, std::string>& header_response)
		{
			std::vector<std::string> tokens;
			boost::split(tokens, header_response["X-App-Rate-Limit-Count"], boost::is_any_of(","));
			std::map<int, int> count_limits;
			for (auto value : tokens)
			{
				auto found = value.find(":");
				auto v1 = std::stoi(value.substr(0, found));
				if (found != std::string::npos)
					count_limits.insert(std::make_pair(std::stoi(value.substr(found + 1)), std::stoi(value.substr(0, found))));
			}
			tokens.clear();
			boost::split(tokens, x_app_rate_limit, boost::is_any_of(","));
			std::map<int, int> limits;
			for (auto value : tokens)
			{
				auto found = value.find(":");
				auto v1 = std::stoi(value.substr(0, found));
				if (found != std::string::npos)
					limits.insert(std::make_pair(std::stoi(value.substr(found + 1)), std::stoi(value.substr(0, found))));
			}
			for(auto key_val : limits)
			{
				auto allowed_calls = key_val.second;
				auto total_time = key_val.first;
				if(allowed_calls == count_limits[total_time])
				{
					std::this_thread::sleep_for(std::chrono::seconds(20));
					continue;
				}					
				auto left_calls = allowed_calls - count_limits[total_time];
				auto time_to_sleep = total_time / allowed_calls * 2;
				if (0.2*allowed_calls > left_calls)
					std::this_thread::sleep_for(std::chrono::seconds(time_to_sleep));
			}
		}

		HeaderResponse::HeaderResponse(std::string& entry)
		{
			x_app_rate_limit = entry;
		}

		HeaderResponse::~HeaderResponse()
		{
		}


		std::map<std::string, std::string> parseHeader(std::string& line)
		{
			std::map<std::string, std::string> header_response;

			std::vector<std::string> tokens;
			boost::split(tokens, line, boost::is_any_of("\r\n"));
			if (tokens.empty())
				return header_response;

			auto response_code = tokens[0];
			for(auto i=1; i<tokens.size(); ++i)
			{
				auto header = tokens[i];
				auto found = header.find(":");
				if (found != std::string::npos)
					header_response.insert(std::make_pair(header.substr(0,found), header.substr(found+2)));
			}
			return header_response;
		}
		

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
			std::string headBuffer;
			if (curl) {
				curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
				curl_easy_setopt(curl, CURLOPT_TRANSFERTEXT, 2);
				curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, writeCallback);
				curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headBuffer);
				auto curl_code = curl_easy_perform(curl); // auto res = curl_easy_perform(curl);
				auto header_response = parseHeader(headBuffer);
				long http_code = 0;
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
				if(http_code == 429)
				{
					std::this_thread::sleep_for(std::chrono::seconds(stoi(header_response["Retry-After"])));
					curl_easy_perform(curl);
				}
				if ((http_code != 200 && http_code != 404) || curl_code == CURLE_ABORTED_BY_CALLBACK)
				{
					//Failed
					throw URLReaderException(http_code);
				}
				curl_easy_cleanup(curl);
				auto hr = HeaderResponse::getInstance();
				hr->handle_limit(header_response);
			}
			return readBuffer;
		}
	}
}
