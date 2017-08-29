#pragma once
#ifndef URL_READER_HPP
#define URL_READER_HPP

#define CURL_STATICLIB
#include <map>

namespace Riot
{
	namespace URLReader
	{
		class HeaderResponse
		{
		public:
			static HeaderResponse* getInstance()
			{
				std::string value = "100:120, 20:1";
				static HeaderResponse headerResponse(value);

				return &headerResponse;
			}

			void handle_limit(std::map<std::string, std::string> &header_response);
		private:
			explicit HeaderResponse(std::string &entry);// default constructor available only to members or friends of this class
			HeaderResponse(const HeaderResponse &old) = delete; // disallow copy constructor
			const HeaderResponse &operator=(const HeaderResponse &old) = delete; //disallow assignment operator
			~HeaderResponse();

		private: // private data for an instance of this class
			std::string x_app_rate_limit;
		};

		class URLReaderException : public std::exception
		{
		private:
			int http_code;
		public:
			explicit URLReaderException(int http_code) : http_code(http_code) {}

			const char* what() const throw() override
			{
				return "\n\n \t\t ####RIOT_API RESPONSE ERROR ####\n\n";
			}
		};

		std::string read(const std::string& url);
	}
}

#endif // URL_READER_HPP