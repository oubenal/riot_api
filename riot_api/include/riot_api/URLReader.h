#pragma once
#ifndef URL_READER_HPP
#define URL_READER_HPP

#define CURL_STATICLIB
#include <string>

namespace Riot
{
	namespace URLReader
	{
		class URLReaderException : public std::exception
		{
		private:
			int http_code;
		public:
			explicit URLReaderException(int http_code) : http_code(http_code) {}
			virtual const char* what() const throw()
			{
				return "\n\n \t\t ####RIOT_API RESPONSE ERROR ####\n\n";
			}
		};

		std::string read(const std::string& url);
	}
}

#endif // URL_READER_HPP