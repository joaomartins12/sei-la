#pragma once

#include "Account_Define.h"
#include <list>
#include <minwindef.h>

/************************************************************************/
/* 서버 리스트 정보 패킷												*/
/************************************************************************/
class GS2C_RECV_SERVERLIST
{
public:
	GS2C_RECV_SERVERLIST() : m_dwResult(0)
	{}
	~GS2C_RECV_SERVERLIST(){};

public:	
	n4						m_dwResult;
	std::list<sServerInfo>	m_listServer;
};

/************************************************************************/
/* 캐릭터 리스트 정보 패킷												*/
/************************************************************************/
class GS2C_RECV_CHARACTER_LIST
{
public:
	GS2C_RECV_CHARACTER_LIST() : m_dwResult(0)
	{}

	~GS2C_RECV_CHARACTER_LIST(){};

public:
	n4									m_dwResult;
	std::list<sSimpleCharacterInfo>		m_listCharacterInfo;
};

/************************************************************************/
/* 새로 생성한 캐릭터 정보 패킷											*/
/************************************************************************/
class GS2C_RECV_NEW_CHARACTER_INFO
{
public:
	GS2C_RECV_NEW_CHARACTER_INFO() : m_dwResult(0)
	{}

	~GS2C_RECV_NEW_CHARACTER_INFO(){};

public:
	n4								m_dwResult;
	sSimpleCharacterInfo			m_NewCharInfo;
};

/************************************************************************/
/* 2차 비밀번호 여부 정보 패킷											*/
/************************************************************************/
class GS2C_RECV_CHECK_SECONDPASSWORD
{
public:
	GS2C_RECV_CHECK_SECONDPASSWORD() : m_nResult(0),m_u2SecondPasswordType(0)
	{}

	~GS2C_RECV_CHECK_SECONDPASSWORD(){};

public:
	n4								m_nResult;
	u2								m_u2SecondPasswordType;
};



/************************************************************************/
/* 로그인 요청 응답 패킷												*/
/************************************************************************/
 class GS2C_RECV_LOGIN_REQUEST
 {
 public:
	 GS2C_RECV_LOGIN_REQUEST(): nProtocol(pLogin::Request)
	 {}

	 WORD GetProtocol(){return nProtocol;}

 public:
	 WORD nProtocol;

	 u4 nResult;
	 u1 nSubType;
	
 };

 /************************************************************************/
 /* 2차 비밀번호 변경 응답 패킷	   										 */
 /************************************************************************/

 class GS2C_RECV_2ndPass_Change
 {
 public:
	 GS2C_RECV_2ndPass_Change(): nProtocol(pPass2::ChangePass)
	 {}

	 WORD GetProtocol(){return nProtocol;}

 public:
	 WORD nProtocol;

	 u4 nResult;	 
 };



 /************************************************************************/
 /* 2차 비밀번호 검증 응답 패킷											 */
 /************************************************************************/

 class GS2C_RECV_2ndPass_Certify
 {
 public:
	 GS2C_RECV_2ndPass_Certify(): nProtocol(pPass2::Check)
	 {}

	 WORD GetProtocol(){return nProtocol;}

 public:
	 WORD nProtocol;

	 u4 nResult;	 
 };


 /************************************************************************/
 /* 2차 비밀번호 등록													 */
 /************************************************************************/

 class GS2C_RECV_2ndPass_Register
 {
 public:
	 GS2C_RECV_2ndPass_Register(): nProtocol(pPass2::Register)
	 {}

	 WORD GetProtocol(){return nProtocol;}

 public:
	 WORD nProtocol;

	 u4 nResult;	 
 };

 /************************************************************************/
 /* 리소스 변조 해시값 전송												 */
 /************************************************************************/

 class GS2C_RECV_Resource_Hash
 {
 public:
	 GS2C_RECV_Resource_Hash(): nProtocol(pCertify::IntegrityHash)
	 {}

	 WORD GetProtocol(){return nProtocol;}

 public:
	 WORD nProtocol;

	 std::string szHash;
	 //std::string szURL;
 };


 /************************************************************************/
 /* 계정 밴 시간 전송													 */
 /************************************************************************/

 class GS2C_RECV_Account_Ban
 {
 public:
	 GS2C_RECV_Account_Ban(): nProtocol(pLogin::AccountBan)
	 {}

	 WORD GetProtocol(){return nProtocol;}

 public:
	 WORD nProtocol;

	u4 nTimeLeft;
	std::string szReason;
 };


 /************************************************************************/
 /* 셧다운제 생년월일 전송												 */
 /************************************************************************/

 class GS2C_RECV_Shutdown_Birthday
 {
 public:
	 GS2C_RECV_Shutdown_Birthday(): nProtocol(pLogin::Birthday)
	 {}

	 WORD GetProtocol(){return nProtocol;}

 public:
	 WORD nProtocol;

	 std::string szBirthDay;
 };


 /************************************************************************/
 /* 서버 접속 요청에 대한 결과 패킷										 */
 /************************************************************************/
 class AC2C_RECV_SERVERSELECT_CLUSTER
 {
 public:
	 AC2C_RECV_SERVERSELECT_CLUSTER():nResult(0) {};
	 ~AC2C_RECV_SERVERSELECT_CLUSTER() {};

 public:
	 n4 nResult;
 };