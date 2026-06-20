/*
	RECODED NLIB, FOR BUILDING DMO LEAKED SOURCES

	@author Arves100
	@date 17/02/2022
	@file cPacket.h
	@brief asio stream packet
*/
#pragma once

#include <stdint.h>

#include <vector>

#include "asio/streambuf.hpp"

enum DebugState
{
	RECV,
	SEND,
};

class cClient;

class cPacket
{
public:
	friend cClient;

	cPacket();
	~cPacket();

	template <typename T>
	void push(T& val)
	{
		push(&val, sizeof(val), false);
	}

	template <>
	void push<std::string>(std::string& str)
	{
		push((char*)str.c_str(), str.length(), true);
	}

	template <>
	void push<std::wstring>(std::wstring& str)
	{
		auto str2 = nBase::w2m(str);
		push(str2);
	}

	void push(void* data, size_t len, bool wlen = false);

	template <typename T>
	void pop(T& val)
	{
		pop(&val, sizeof(val), false);
	}

	void pop(void* data, size_t len, bool wlen = false);

	void newp(uint16_t id);
	void endp(uint16_t id);

	void mark(void* data, size_t len);

	template <typename T>
	void mark(T& val)
	{
		mark(&val, sizeof(val));
	}

	template <>
	void push<const std::wstring>(const std::wstring& str)
	{
		auto str2 = nBase::w2m(str);
		push(str2);
	}

	void push(const char* str)
	{
		push((char*)str, strlen(str), true);
	}

	template <>
	void pop<std::string>(std::string& str)
	{
		uint8_t len;
		pop(len);
		str.resize(len);

		pop((char*)&str[0], len, false);
		pop(len);
	}


	template <>
	void pop<std::wstring>(std::wstring& str)
	{
		std::string str2;
		pop(str2);
		str = nBase::m2w(str2);
	}

	void pop(char* str)
	{
		pop((char*)str, 0, true);
		uint8_t nt;
		pop(nt);
	}

	// basic type push
	void push(n4 n)
	{
		push<n4>(n);
	}

	void push(n2 n)
	{
		push<n2>(n);
	}

	void push(n8 n)
	{
		push<n8>(n);
	}

	void push(n1 n)
	{
		push<n1>(n);
	}

	void push(u4 n)
	{
		push<u4>(n);
	}

	void push(u2 n)
	{
		push<u2>(n);
	}

	void push(u8 n)
	{
		push<u8>(n);
	}

	void push(u1 n)
	{
		push<u1>(n);
	}

protected:
	/*int MaxBufSize;
	uint8_t* m_pIDX;
	uint8_t* m_pEND;
	uint8_t* m_pAll;
	int m_nDebugState;*/

	uint16_t real_len;
	asio::streambuf m_buf;
};

