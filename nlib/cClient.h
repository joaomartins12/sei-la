/*
	RECODED NLIB, FOR BUILDING DMO LEAKED SOURCES

	@author Arves100
	@date 17/02/2022
	@file cClient.h
	@brief ASIO TCP CLIENT
*/
#pragma once

#include "cpacket.h"

#include <thread>

#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include <regex>

namespace nSession
{
	enum _
	{
		User = 1,
	};
}

class cClient
{
public:
	cClient(uint16_t protocol);
	~cClient();

	void DoSend(cPacket& p);

	void DoSend()
	{
		DoSend(m_write);
	}

	void send()
	{
		DoSend();
	}

	uint32_t GetTimeTS() const { return m_timets; }

	void SetSendBlock(bool g) { m_sb = g; }
	bool IsSendBlock() const { return m_sb; }

	void DoDisconnect();

	bool IsActive() const { return m_a; }

	uint16_t GetProtocol() const { return m_sproto; }

	void Stop();

	bool Bind();
	bool Connect(const char* ip, uint16_t port);
	bool Run(int unk);

	void EnableEncryption(unsigned int version);

	void DoExecute();

	virtual void OnConnected(void) = 0;
	virtual void OnDisconnected(char* msg) = 0;
	virtual void OnExecute(void) = 0;

	template <typename T>
	void push(T& val)
	{
		push((void*) & val, sizeof(val), false);
	}

	template <>
	void push<std::string>(std::string& str)
	{
		push((char*)str.c_str(), str.length(), true);
	}

	void push(const char* str)
	{
		push((char*)str, strlen(str), true);
	}

	template <>
	void push<std::wstring>(std::wstring& str)
	{
		auto str2 = nBase::w2m(str);
		push(str2);
	}

	template <>
	void push<const std::wstring>(const std::wstring& str)
	{
		auto str2 = nBase::w2m(str);
		push(str2);
	}

	void push(const wchar_t* str)
	{
		std::wstring wstr(str);
		auto str2 = nBase::w2m(wstr);
		push(str2);
	}

	void push(void* data, size_t len, bool wlen = false);

	template <typename T>
	void pop(T& val)
	{
		pop(&val, sizeof(val), false);
	}

	void pop(char* str)
	{
		pop((char*)str, 0, true);
		uint8_t nt;
		pop(nt);
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

	void pop(void* data, size_t len, bool wlen = false);


	void newp(uint16_t id);
	void endp(uint16_t id);

	void mark(void* data, size_t len);

	template <typename T>
	void mark(T& val)
	{
		mark(&val, sizeof(val));
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

	std::string ltrim(const std::string& s) {
		return std::regex_replace(s, std::regex("^\\s+"), std::string(""));
	}

	std::string rtrim(const std::string& s) {
		return std::regex_replace(s, std::regex("\\s+$"), std::string(""));
	}

	std::string trim(const std::string& s) {
		return ltrim(rtrim(s));
	}

protected:
	void StartRead();
	void ReadAll(int bt);
	void ReadAll2();
	bool enc;
	uint32_t m_v;
	uint32_t m_timets;
	bool m_a;
	bool m_sb;
	uint16_t m_sproto;
	std::thread m_t;

	cPacket m_read;
	cPacket m_write;

	/*!
		The TCP socket
	*/
	asio::ip::tcp::socket m_sock;

};
