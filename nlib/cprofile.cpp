/*
	RECODED NLIB, FOR BUILDING DMO LEAKED SOURCES

	@author Arves100
	@date 17/02/2022
	@file cProfile.h
	@brief wrapper of ini++
*/
#include "base.h"
#include "cProfile.h"

// NOTE: replace to corsacbase ini

cProfile::cProfile(const char* name)
{
	std::ifstream i(name, std::ifstream::in);
	m_ini.parse(i);
	i.close();

	m_name = name;
}

cProfile::~cProfile()
{
	std::ofstream o(m_name, std::ofstream::out);
	m_ini.generate(o);
	o.close();
}

void cProfile::Write(const char* section, const char* value, int data)
{
	m_ini.sections[section][value] = std::to_string(data);
}

int cProfile::GetInt(const char* section, const char* value)
{
	int ret = -1;
	inipp::get_value(m_ini.sections[section], value, ret);
	return ret;
}

std::string cProfile::GetStr(const char* section, const char* value)
{
	std::string ret = "";
	inipp::get_value(m_ini.sections[section], value, ret);
	return ret;
}
