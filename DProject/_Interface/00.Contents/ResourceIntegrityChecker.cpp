#include "StdAfx.h"
#include "ResourceIntegrityChecker.h"
#include "../../../SimpleSecurity/SecuritySelf.h"
#include "../13.ResourceIntegrityChecker/ResourceChecker.h"

// 리소스 검사시 최대 대기 시간.
// 리소스 검사 스레드가 외부에서 종료 됬거나 중지 됬을 때
// 위 시간이 지나도 완료가 안되면 클라이언트 종료 시킴.
#define RESOURCECHECK_MAX_TITME_LIMIT 1000*60*10	// 10분



void CALLBACK cResourectIntegrityChecker::EventTimeoutProc( HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime )
{
	if( TIMER_IDEVENT_TIMEOUT == idEvent )
	{
		GAME_EVENT_ST.OnEvent( EVENT_CODE::TIMEOUT_RESOURCECHECKER );
		::KillTimer( GAMEAPP_ST.GetHWnd(), idEvent );
	}
}

int const cResourectIntegrityChecker::IsContentsIdentity(void)
{
	return E_CT_RESOURCEINTEGRITYCHECKER;
}

cResourectIntegrityChecker::cResourectIntegrityChecker(void)
:m_pResourceChecker(NULL),
#ifdef SDM_RESOURCE_CHECKER_ENABLE
m_nCheckState(eWait),m_nCheckError(CResurecChecker::eRESULT_WAIT)
#else
m_nCheckState(eFinish),m_nCheckError(CResurecChecker::eSUCCESS)
#endif
{
	// 리소스 검사하는 국가가 아니면 이벤트를 연결 안함
#ifdef SDM_RESOURCE_CHECKER_ENABLE
	GAME_EVENT_ST.AddEvent( EVENT_CODE::START_RESOURCECHECKER, this, &cResourectIntegrityChecker::_StartResourceChecker );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::TIMEOUT_RESOURCECHECKER, this, &cResourectIntegrityChecker::_ResourChekTimeOut );
	EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_NotMatch_MapNameResource, this );

	// 52.79.64.198
	SimpleSecurity::SecuritySelf::Decript("JC*5xG%@p[eST2,Vqmli01rn|B.ud:k\^#v6Rf-FjzOgoQMP9hL&Y/`}=w>AD;4I]t<'UK(!Z)s_y{WNb~38H+?EX7\"$a L7}}Q)JC3-y=D", m_DefaultUploadFtpInfo.s_Host);
	std::string port;//14147
	SimpleSecurity::SecuritySelf::Decript("D9o +aA4\Q/ebG\"gk_R*zx,]q}NhVF@$!PBwL-=8Idf:O<lM3nsE0)|#2(~ryZ%tKS?v5{^Jm6X[YWj1p&TiCuU'>7;H.`?/OL{F", port);
	m_DefaultUploadFtpInfo.s_port = atoi(port.c_str());

	//movegamesdumpuser
	SimpleSecurity::SecuritySelf::Decript(" \'!V0pz*}>1MtPfHm=4,QA)$3uI\"5J`|_iS:9yhgRe?F+/2EG&a<78]so~XlZ#[w^%;{@T.KCx6UDkNB-OqnWv\\YLdj(br+$>~=ZfzTz9>K#GWN?", m_DefaultUploadFtpInfo.s_id);
	//8AgLx9DlzFxN
	SimpleSecurity::SecuritySelf::Decript("Vd?=(PTh5a-CSI\"s 3^$g%o]N}`O20Me,\'.i<~4H+nRjfyXr9{mq|WEz*>Kl)#B&Jk;bvu1/tAZ@:_GxUpQLY86\\!D7Fw[w5}J\'R@^;R8}x", m_DefaultUploadFtpInfo.s_pw);

	m_DefaultUploadFtpInfo.s_dir = GLOBALDATA_ST.GetCountryName();

	std::string subDir;
#ifdef VERSION_QA
	//   "/default_qa/Log_v1.%04d/%s/"	
	SimpleSecurity::SecuritySelf::Decript("\\j1-XH:mE./GlSQp?@`TNbM3F*\"%!C=+[Ws5)80}I&zkyB6L2<7 {;wJe#,Y\'tUV4ra(ARZuhO^d9$vKnqigxD|>~]o_fP) +-QFn\"1a#mX>&uULU*(tiTr84.", subDir);	
#else
	//  "/default_live/Log_v1.%04d/%s/"
	SimpleSecurity::SecuritySelf::Decript(".D?;\"\\8C/2*PSb@BfZt:)R&aFx~U^7H9[nmQI`,6e{} z%TJ4j>sy=LO35_-W]M(<dYE1prhiKuG\'lNqAoX+|g#kvV0!$w10iZ&n\'#8>H4Iyp`0i[Ot=Kf#Qa#u", subDir);
#endif
	m_DefaultUploadFtpInfo.s_dir += subDir;
	// "HashLog"
	SimpleSecurity::SecuritySelf::Decript("F;Y!#B|KbZqt>P3X.&r1[Cmj:fsh7]Wo\"4J^0RQS@_i%zxn$\'D }wG(`pVOl2U8*)\\T,y~NdM96u{kIeEH+/vL=gA5?-<aP?Au5C/", m_DefaultUploadFtpInfo.s_file);
#endif
}

cResourectIntegrityChecker::~cResourectIntegrityChecker(void)
{
	SAFE_DELETE( m_pResourceChecker );
	GAME_EVENT_ST.DeleteEventAll( this );
	EventRouter()->UnRegisterAll( this );
	::KillTimer( GAMEAPP_ST.GetHWnd(), TIMER_IDEVENT_TIMEOUT );
}

int const cResourectIntegrityChecker::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool cResourectIntegrityChecker::Initialize(void)
{

	return true;
}

void cResourectIntegrityChecker::UnInitialize(void)
{
}

bool cResourectIntegrityChecker::IntraConnection(ContentsSystem& System)
{ 
	return true; 
}

void cResourectIntegrityChecker::Update(float const& fElapsedTime)
{
}

void cResourectIntegrityChecker::NotifyEvent(int const& iNotifiedEvt)
{
}


void cResourectIntegrityChecker::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case CONTENTS_EVENT::EStreamEvt_NotMatch_MapNameResource:
		{
			uint nMapID;
			std::string nameName;
			kStream >> nMapID >> nameName;

			m_nCheckError = CResurecChecker::eMemoryModulation;
			GAMEAPP_ST.SetProcessState( App::CWin32App::PROCESS_STOP );

			_SendErrorLog( m_nCheckError, nameName );
			_ShowErrorMessage( m_nCheckError );
			PostQuitMessage(0);
		}
		break;
	}
}

// 로비에서 게임으로 진입시 호출
void cResourectIntegrityChecker::MakeWorldData(void)
{
	switch( m_nCheckState )
	{
	case eFinish:	
		{
			if( CResurecChecker::eSUCCESS != m_nCheckError )
			{
				GAMEAPP_ST.SetProcessState( App::CWin32App::PROCESS_STOP );
				m_nCheckError = CResurecChecker::eSERVER_HASH_DATA_NOT_RECV;
				_SendErrorLog( m_nCheckError, "" );
				_ShowErrorMessage( m_nCheckError );	
				PostQuitMessage(0);
			}
			else
				SAFE_DELETE( m_pResourceChecker );
		}break;// 모든 리소스 검사가 끝남.
	case eWait:	// 리소스 검사를 시작 안했을 경우 에러 메시지 출력후 종료
		{
			GAMEAPP_ST.SetProcessState( App::CWin32App::PROCESS_STOP );
			m_nCheckError = CResurecChecker::eSERVER_HASH_DATA_NOT_RECV;
			_SendErrorLog( m_nCheckError, "" );
			_ShowErrorMessage( m_nCheckError );	
			PostQuitMessage(0);
		}break;	
	default:	// 그외 리소스 검사를 진행하고 있는 중								
		{
			if(( NULL == m_pResourceChecker || (m_pResourceChecker && !m_pResourceChecker->IsThreadActive())) )// 리소스 감사 스레드가 죽어 있는 경우
			{
				m_nCheckError = CResurecChecker::eThread_Stop;
				GAMEAPP_ST.SetProcessState( App::CWin32App::PROCESS_STOP );
				_SendErrorLog( m_nCheckError, "" );
				_ShowErrorMessage(m_nCheckError );
				PostQuitMessage(0);
			}
		}break;		
	}
}

void cResourectIntegrityChecker::ClearWorldData(void)
{
	ClearAllRegistered();
}

void cResourectIntegrityChecker::MakeMainActorData(void)
{
	
}

void cResourectIntegrityChecker::ClearMainActorData(void)
{
}

//////////////////////////////////////////////////////////////////////////

void cResourectIntegrityChecker::_StartResourceChecker( void* pData )
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_Resource_Hash* pRecv = static_cast<GS2C_RECV_Resource_Hash*>(pData);

	if( eCheck_Start == m_nCheckState && NULL != m_pResourceChecker )
		return;

	if( eFinish == m_nCheckState )
	{
		if( CResurecChecker::eSUCCESS == m_nCheckError )
			return;
		
		GAMEAPP_ST.SetProcessState( App::CWin32App::PROCESS_STOP );
		_SendErrorLog(m_nCheckError, "");
		_ShowErrorMessage( m_nCheckError );
		PostQuitMessage(0);
		return;
	}
	
	m_nCheckState = eCheck_Start;
	SAFE_DELETE( m_pResourceChecker );	
	m_pResourceChecker = new CResurecChecker;
	m_pResourceChecker->SetAccountID( GLOBALDATA_ST.GetAccountID() );
	m_pResourceChecker->SetCallbackFunc( this, &cResourectIntegrityChecker::callback_Function );
	m_pResourceChecker->SetHashInfo( pRecv->szHash, m_DefaultUploadFtpInfo );
	m_pResourceChecker->CreateThread();

	// 리소스 체크 스레드가 죽었을 경우를 대비하여
	// 리소스 체크 시간을 10분 이상 못하도록 추가
	::SetTimer( GAMEAPP_ST.GetHWnd(), TIMER_IDEVENT_TIMEOUT, RESOURCECHECK_MAX_TITME_LIMIT, (TIMERPROC)(cResourectIntegrityChecker::EventTimeoutProc) );
}

void cResourectIntegrityChecker::callback_Function( int const& nErrorCode )
{
	m_nCheckState = eFinish;
	m_nCheckError = nErrorCode;

	if( CResurecChecker::eSUCCESS != m_nCheckError )
	{
		GAMEAPP_ST.SetProcessState( App::CWin32App::PROCESS_STOP );		
		_ShowErrorMessage( m_nCheckError );
		PostQuitMessage(0);
	}
}

//////////////////////////////////////////////////////////////////////////

void cResourectIntegrityChecker::_ResourChekTimeOut(void* pData)
{
	if( eFinish == m_nCheckState && CResurecChecker::eSUCCESS == m_nCheckError )
	{
		SAFE_DELETE( m_pResourceChecker );
		return;
	}

	m_nCheckError = CResurecChecker::eResourceCheckTimeOut;
	GAMEAPP_ST.SetProcessState( App::CWin32App::PROCESS_STOP );
	_SendErrorLog(m_nCheckError, "");
	_ShowErrorMessage( m_nCheckError );
	PostQuitMessage(0);
}

void cResourectIntegrityChecker::_SendErrorLog( int const& nErrorType, std::string const& msg )
{
	std::string accountID = GLOBALDATA_ST.GetAccountID();
	if( accountID.empty() )
		return;

	int nVersion = GLOBALDATA_ST.GetDownloadPatchVersion();
	std::string userKey = DmCS::sha256(accountID);
	std::string uploadDir;
	DmCS::StringFn::FormatA( uploadDir, m_DefaultUploadFtpInfo.s_dir.c_str(), nVersion, userKey.c_str() );

	std::string saveData;
	saveData = accountID;
	saveData += "\r\n";
	char buffer[65] = {0};
	itoa( nErrorType, buffer, 10 );
	saveData += buffer;
	if( !msg.empty() )
	{
		saveData += "\r\n";
		saveData += msg;
	}

	std::string saveFileName = makeSaveFile( m_DefaultUploadFtpInfo.s_file );
	if( SaveSendFile( saveFileName,  saveData ) )
		_FTPFileUpload( m_DefaultUploadFtpInfo.s_Host, m_DefaultUploadFtpInfo.s_port, m_DefaultUploadFtpInfo.s_id, m_DefaultUploadFtpInfo.s_pw, uploadDir, saveFileName);
	::DeleteFileA( saveFileName.c_str() );
}

void cResourectIntegrityChecker::_ShowErrorMessage( int const& nErrorType )
{
	std::wstring msg;
	switch( nErrorType )
	{
	case CResurecChecker::eFILE_DOWNLOAD_FAIL:					// 파일이 없다
	case CResurecChecker::eRESOURCE_HASH_NOT_EQUAL:			// 받은 해쉬 데이터의 해쉬 값이 서버에서 받은 해쉬와 다르다.
		msg = UISTRING_TEXT("RESOUREC_CHECK_RESULT_HASHDATA_FILE_DOWNLOAD_FAILE");
		break;
	case CResurecChecker::eRESOURCE_MODEULATION:
	case CResurecChecker::eRESOURCE_HASHDATA_NOT_compressed:
	case CResurecChecker::eSERVER_HASH_DATA_NOT_RECV:
	case CResurecChecker::eSERVER_HASH_DATA_EMPTY:
	case CResurecChecker::eSERVER_HASH_DOWNLOADURL_EMPTY:
	case CResurecChecker::eSERVER_HASH_DOWNLOADURL_DECRIPT_ERROR:
	case CResurecChecker::eSERVER_HASH_CUT_SIZE_ERROR:
	case CResurecChecker::eDownloadHashDataEmpty:
	case CResurecChecker::eThread_Stop: // 외부에서 리소스 체크 스레드가 종료 됬을 경우 
	case CResurecChecker::eResourceCheckTimeOut:		// 리소스 체그 최대 시간 초과 
	case CResurecChecker::eCLIENT_HASH_DATA_NOT_EQUAL:
	case CResurecChecker::eMemoryModulation:
	case CResurecChecker::eRESULT_WAIT:
		msg = UISTRING_TEXT("RESOUREC_CHECK_RESULT_RESOURCE_MODEULATION");
		break;
	}
	
	CsMessageBox(MB_OK, msg.c_str() );
}