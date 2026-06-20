#pragma once

#include <string>

namespace SimpleSecurity
{
	struct SecuritySelf
	{
		static bool EncriptHex(std::string const& msg, std::string& out);
		static bool DecriptHex(std::string const& msg, std::string& out);
		static bool Encript(std::string const& msg, std::string& out);
		static bool Decript(std::string const& msg, std::string& out);
		static size_t GetSecuritySize( std::string const& AuthKey );
	};
};