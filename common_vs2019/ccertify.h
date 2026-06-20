#pragma once

#include "nScope.h"
#include <iostream>

namespace pCertify
{
	enum
	{
		SvrCertify = nScope::Certify,		// 서버 인증
		HeartBeat,							// 주기적 확인
		Close,	
		IntegrityHash,						//클라이언트 리소스 해시 패킷
		End,
	};
};

namespace cCertify
{
#define MAX_SECURITY_CODE	64

	enum CHECK_RESULT
	{
		CHECK_NOT_REQUEST	= 0,				// 질의 번호가 없는 경우		
		CHECK_SUCCESS		= 1,				// 정상

		// 이 이하는 에러
		CHECK_FAIL			= 60000,			// 암호 코드 틀림
		CHECK_REQUEST_NO_ER = 60001,			// 인덱스 번호가 잘못됨
	};

	class cSecurityCode
	{
	private:
		const	double	m_nCertifyVersion;				// 인증버전
		const	__int64	m_nBuildDate;					// 빌드일자
		const	double	m_nRanNo;						// 임의 번호
		
		short			m_nRequestIndex;				// 질의 인덱스
		double			m_szSecurityCode[MAX_SECURITY_CODE];		// 보안 코드
	public:
		cSecurityCode() : m_nCertifyVersion(0.1), m_nBuildDate(131004), m_nRanNo(3.5)
		{
			m_nRequestIndex = -1;			
		}

		~cSecurityCode()
		{

		}
		
//		cSecurityCode & operator=(const cSecurityCode &) { return *this; }

		void CodeGenerator()
		{
			memset( m_szSecurityCode, 0, sizeof(m_szSecurityCode) );

			for(int i = 0; i < MAX_SECURITY_CODE; i++)
			{
				m_szSecurityCode[i]	= m_nBuildDate + m_nRanNo * m_nCertifyVersion * i;				
			}
		}

		short MakeRequestNo()
		{	// 랜덤 코드를 사용하여 결과를 리턴
			m_nRequestIndex = (SHORT)nBase::rand(0,MAX_SECURITY_CODE-1);
			return m_nRequestIndex;
		}

		double GetSecurity(short nIndex)
		{
			if( nIndex >= MAX_SECURITY_CODE )
			{
				return 0;
			}

			return m_szSecurityCode[nIndex];			
		}

		// 수신 한 응답 코드를 체크
		unsigned short CheckAnswer(double nSecurityCode)
		{
			if( m_nRequestIndex < 0 )
			{				
				return CHECK_NOT_REQUEST;
			}

			if( m_nRequestIndex >= MAX_SECURITY_CODE )
			{
				return CHECK_REQUEST_NO_ER;
			}

			unsigned short nResult = CHECK_FAIL;

			if( m_szSecurityCode[m_nRequestIndex] == nSecurityCode )
			{				
				nResult = CHECK_SUCCESS;
			}

			LOG("%f,%f",m_szSecurityCode[m_nRequestIndex],nSecurityCode);

			m_nRequestIndex = -1;
			return nResult;
		}
	};
};