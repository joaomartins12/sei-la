#pragma once

/*
	라이브러리 헤더(Static, Dll 및 Library project, Client Project 공용)
	공용이니깐, 써야할것과 안써야 할 것을 잘 구분하여야 할 필요가 있음.

	------------------------------------------------------------------------------------------------
	매우 간단한 HTTP, HTTPS Request 및 그에 따른 Response를 받는 기능을 제공한다.
	현재 구현에선 해당 객체는 싱글스레드 기반이며, 첫 Request 이 후 TCP Connection을 유지한다.

	Written by seunghyun 
	2009-06-23.

	
	------------------------------------------------------------------------------------------------
	간단한 버전 정보를 추가

	Written by seunghyun
	2009-12-10.
*/

#include <windows.h>


#define JMX_HTTP_CONNECTION_VERSION	1

//Library 개발 프로젝트에서 공통으로 쓰기 때문임. Client 개발자는 무시해야함.
#ifdef _USRDLL
	#define JMX_LIB	extern "C" __declspec(dllexport)
#else
	#define JMX_LIB
#endif
	
/*/////////////////////////////////////////////////////////////////
// HTTP 관련 메소드의 인터페이스
// ----------------------------------------------------------------
// Dll 클라이언트에서 런타임 바인딩을 할 수 있도록, 인터페이스를 둔다.
// 이 헤더파일 가장 아래쪽의 CreateJmxHttpConnectionObject함수를 이용해 객체를 생성한다.
/////////////////////////////////////////////////////////////////*/

namespace JmxPlatform
{
	//추후 구현하도록 하자. 다양한 HTTP 기능을 활용 해야 한다면...
	//HTTP File download시 range헤더등을 이용하여 flash get같은 분할 다운로드 구현이 가능하다.
	struct HTTP_REQUEST
	{
	};

	struct HTTP_RESPONSE
	{
		int nStatus;
		HTTP_RESPONSE() : nStatus(0) {}
	};

	class IJmxHttpConnection
	{
	private:
		DWORD	m_dwLastErrorCode;

	public:
		//에러코드 정의
		enum ERROR_CODE
		{	
			JHC_SUCCESS					=0x1,						//성공

			JHC_NOT_ENOUGHT_BUFFER_SIZE	=0x2,						//버퍼 크기가 부족함
			JHC_CONNECTION_ERROR		=0x3,						//TCP 연결 관련 모든 에러
			JHC_HTTP_RESPONSE_ERROR		=0x4,						//Http Response의 status가 200이 아닌 모든 경우

			JHC_INTERNAL_ERROR			=0x100,						//Assertion 성격의 내부 에러( 라이브러리 디버깅이 필요한 에러)

			JHC_IMPL_DEFINED			=0x0000FFFF					//구현에 따른 에러정의 => JHC_IMPL_DEFINED < 구현에따른에러코드
		};

	public:
		//생성/소멸자
		IJmxHttpConnection() : m_dwLastErrorCode(JHC_SUCCESS) { }
		virtual ~IJmxHttpConnection() {}

		virtual bool Connect(LPCTSTR lpszDomainURL)			=0;
		virtual bool Disconnect()							=0;
		virtual bool IsConnected()							=0;
		virtual bool Request(LPCTSTR szURL, DWORD& nRecvSize, BYTE* pOutBuf, const DWORD nOutBufSize)	=0;

		//추후 구현, 단지 Concept 일 뿐..
		virtual bool Request() =0;
		virtual const HTTP_RESPONSE& GetResponse() = 0;
		virtual const HTTP_REQUEST& GetRequest() = 0;
		virtual HTTP_RESPONSE& SetResponse() = 0;
		virtual HTTP_REQUEST& SetRequest() = 0;
		virtual PBYTE GetPayload() = 0;
		virtual DWORD GetPayloadLen() = 0;
		
		virtual DWORD	GetLastError()
		{
			return m_dwLastErrorCode;
		}


	protected:
		virtual void SetLastError(DWORD dwErrorCode)
		{
			m_dwLastErrorCode=dwErrorCode;
		}
	};
}

/*/////////////////////////////////////////////////////////////////
// 라이브러리 인터페이스
/////////////////////////////////////////////////////////////////*/

namespace JmxPlatform
{
	enum JMX_HTTP_CONNECTION_IMPL_TYPE
	{
		JMX_HTTP_IMPL_ATL = 0x0
	};

	//DLL전용
	typedef IJmxHttpConnection* (*CreateJmxHttpConnectionObject_fn_t)(const JMX_HTTP_CONNECTION_IMPL_TYPE emImplType);
	typedef void (*DeleteJmxHttpConnectionObject_fn_t)(IJmxHttpConnection* ObjToDelete);
	typedef bool (*SetDllCrtHeapLFH_fn_t)(void);
}

//객체 생성/소멸 함수
JMX_LIB	JmxPlatform::IJmxHttpConnection* CreateJmxHttpConnectionObject(const JmxPlatform::JMX_HTTP_CONNECTION_IMPL_TYPE emImplType = JmxPlatform::JMX_HTTP_IMPL_ATL);
JMX_LIB	void DeleteJmxHttpConnectionObject(JmxPlatform::IJmxHttpConnection* ObjToDelete);

#ifdef _USRDLL
JMX_LIB bool SetDllCrtHeapLFH(void);
#endif

//라이브러리 버전검사
JMX_LIB int GetJmxHttpConnectionVer();
