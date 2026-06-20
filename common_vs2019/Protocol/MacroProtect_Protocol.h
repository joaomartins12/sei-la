#pragma once
#include "MacroProtect_Define.h"

class C2GS_SEND_MACRO_QUESTION
{
public:
	C2GS_SEND_MACRO_QUESTION():m_nPacketId( pGame::MacroQuestion )
	{}
	~C2GS_SEND_MACRO_QUESTION()
	{}

public:	
	std::wstring	m_TargetName;
	WORD			GetPacketID() const	{return m_nPacketId;}

private:
	WORD			m_nPacketId;
};

class C2GS_RECV_MACRO_QUESTION_RESULT 
{
public:
	C2GS_RECV_MACRO_QUESTION_RESULT(): m_nResult(0)
	{}
	~C2GS_RECV_MACRO_QUESTION_RESULT()
	{}

public:	
	n4		m_nResult;
};

class C2GS_SEND_MACRO_ANSWER
{
public:
#ifdef SERVER_KSW_MACRO_RENEW_181214
	C2GS_SEND_MACRO_ANSWER():m_nPacketId( pGame::MacroAnswer )
#else
	C2GS_SEND_MACRO_ANSWER():m_nPacketId( pGame::MacroAnswer ),m_nRandNo(0),m_nAnswer(0)
#endif	
	{}
	~C2GS_SEND_MACRO_ANSWER()
	{}

public:	

#ifdef SERVER_KSW_MACRO_RENEW_181214
	std::string m_szAnswer;
#else
	n4				m_nRandNo;
	n4				m_nAnswer;
#endif	

	WORD			GetPacketID() const	{return m_nPacketId;}

private:
	WORD			m_nPacketId;
};


// 다른 유저의 신고로 내가 질문을 받을 패킷
class GS2C_NTF_MACRO_QUESTION_REQUEST
{
public:
#ifdef SERVER_KSW_MACRO_RENEW_181214
	GS2C_NTF_MACRO_QUESTION_REQUEST():m_pData(NULL) , m_nSize(0) , m_nTimeMax(0) , m_nTimeLeft(0) , m_nChanceLeft(0)
#else
	GS2C_NTF_MACRO_QUESTION_REQUEST():m_nResult(0),m_nRandNum(0),m_nHint(0)
#endif	
	{}

	~GS2C_NTF_MACRO_QUESTION_REQUEST()
	{
#ifdef SERVER_KSW_MACRO_RENEW_181214
		if( m_pData )
		{
			delete[] m_pData;
			m_pData = NULL;
		}
#endif		
	}

public:	

#ifdef SERVER_KSW_MACRO_RENEW_181214
	n2		m_nTimeMax;
	n2		m_nTimeLeft;
	n1		m_nChanceLeft;

	n4		m_nSize;
	char*	m_pData;	
#ifdef SERVER_KSW_MACRO_QUESTION_190517
	n1		m_nQuestionType;	//nsMacroProtector::QuestionType
	n2	m_nQuestion[CAPTCHA_QUESTION_LENGTH];
#endif
#else
	n4		m_nResult;
	n4		m_nRandNum;
	n4		m_nHint;
#endif	

};

// 대상이 매크로 유저 인지 아닌지에 대한 결과값
class GS2C_RECV_MACRO_PLAYER_RESULT
{
public:
	GS2C_RECV_MACRO_PLAYER_RESULT():m_nResult(0)
	{}
	~GS2C_RECV_MACRO_PLAYER_RESULT()
	{}

public:	
	n4		m_nResult;
};