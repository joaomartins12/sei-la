/*
	RECODED NLIB, FOR BUILDING DMO LEAKED SOURCES

	@author Arves100
	@date 17/02/2022
	@file map.h
	@brief unordered map alias
*/
#pragma once

#include <unordered_map>

template <typename A, typename B>
class cMap : public std::unordered_map<A, B> {};
