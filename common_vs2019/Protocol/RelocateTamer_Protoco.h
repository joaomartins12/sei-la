#pragma once

#include "common_vs2019/pItem.h"
#include "RelocateTamer_Define.h"

//////////////////////////////////////////////////////////////
// 테이머 서버 이전 요청
//////////////////////////////////////////////////////////////
struct C2GS_SEND_REQUEST_RELOCATE_ITEM_USE
{
public:
	C2GS_SEND_REQUEST_RELOCATE_ITEM_USE() :m_nPacketId(pItem::RelocateTamer),m_nNewServerIDX(0),m_nItemInvenPos(-1)
	{}

	~C2GS_SEND_REQUEST_RELOCATE_ITEM_USE(){};

	WORD			GetPacketID() const{ return m_nPacketId; }

	n2				m_nItemInvenPos;
	n4				m_nNewServerIDX;
	std::wstring	m_nTamerName;

private:
	WORD			m_nPacketId;
};

//////////////////////////////////////////////////////////////
// 테이머 서버 이전 요청에 대한 응답
//////////////////////////////////////////////////////////////
struct GS2C_RECV_REQUEST_RELOCATE_ITEM_USE
{
public:
	GS2C_RECV_REQUEST_RELOCATE_ITEM_USE() : m_nResult(0)
	{}
	~GS2C_RECV_REQUEST_RELOCATE_ITEM_USE(){};

public:	
	int m_nResult;
};

//////////////////////////////////////////////////////////////
// 클라이언트로 부터 이전하고자하는 서버에 같은 테이머 이름이 
// 있는지 확인 요청
//////////////////////////////////////////////////////////////
struct C2GS_SEND_CHECK_RELOCATE_NEW_TAMER_NAME
{
public:
	C2GS_SEND_CHECK_RELOCATE_NEW_TAMER_NAME() :m_nPacketId(pRelocateTamer::CheckTamerName),m_nNewServerIDX(0)
	{}

	~C2GS_SEND_CHECK_RELOCATE_NEW_TAMER_NAME(){};

	WORD			GetPacketID() const{	return m_nPacketId; }

	n4				m_nNewServerIDX;
	std::wstring	m_nTamerName;

private:
	WORD			m_nPacketId;
};

//////////////////////////////////////////////////////////////
// 테이머 이름 중복 확인에 대한 응답
//////////////////////////////////////////////////////////////
struct GS2C_RECV_CHECK_RELOCATE_NEW_TAMER_NAME
{
public:
	GS2C_RECV_CHECK_RELOCATE_NEW_TAMER_NAME() : m_nResult(0)
	{}
	~GS2C_RECV_CHECK_RELOCATE_NEW_TAMER_NAME(){};

public:	
	int			m_nResult;
};