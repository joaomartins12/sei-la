
#include "stdafx.h"
#include <windows.h>
#include <initguid.h>
#include <wchar.h>

#include "DeviceInfo.h"
#include "../Flow/Flow.h"
#include "../Flow/FlowMgr.h"

#define EXPAND(x) x, sizeof(x)/sizeof(char)
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

HRESULT GetStringValue( IDxDiagContainer* pObject, char* wstrName, char* strValue, int nStrLen ) ;
HRESULT GetUIntValue( IDxDiagContainer* pObject, char* wstrName, DWORD* pdwValue ) ;
HRESULT GetIntValue( IDxDiagContainer* pObject, char* wstrName, LONG* pnValue ) ;
HRESULT GetBoolValue( IDxDiagContainer* pObject, char* wstrName, BOOL* pbValue ) ;
HRESULT GetInt64Value( IDxDiagContainer* pObject, char* wstrName, ULONGLONG* pullValue ) ;

//-----------------------------------------------------------------------------
GbDxDiagInfo::GbDxDiagInfo( void ) 
: m_bInited(false)
{
}

//-----------------------------------------------------------------------------
GbDxDiagInfo::~GbDxDiagInfo( void )
{
}

//-----------------------------------------------------------------------------
void GbDxDiagInfo::Initialize( void ) 
{
	HRESULT hR;

	CoInitialize( NULL );
	hR = CoCreateInstance( CLSID_DxDiagProvider, NULL, 
		CLSCTX_INPROC_SERVER, IID_IDxDiagProvider, (LPVOID*)&m_pkDxProvider ) ;
	if( FAILED(hR) ) return ;

	DXDIAG_INIT_PARAMS dxDiagInitParam;
	ZeroMemory( &dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS) );

	dxDiagInitParam.dwSize                  = sizeof(DXDIAG_INIT_PARAMS);
	dxDiagInitParam.dwDxDiagHeaderVersion   = DXDIAG_DX9_SDK_VERSION;
	dxDiagInitParam.bAllowWHQLChecks        = false;
	dxDiagInitParam.pReserved               = NULL;

	hR = m_pkDxProvider->Initialize( &dxDiagInitParam );
	if( FAILED(hR) ) assert_cs( false ) ;

	hR = m_pkDxProvider->GetRootContainer( &m_pkDxRoot );
	if( FAILED(hR) ) assert_cs( false ) ;

	m_listDisplayInformation.clear() ;

	m_bInited = true ;
}

//-----------------------------------------------------------------------------
void GbDxDiagInfo::Destroy( void ) 
{
	m_listDisplayInformation.clear() ;
}

//-----------------------------------------------------------------------------
void GbDxDiagInfo::QueryDxDiagValue( void ) 
{
	HRESULT hR ;
	DWORD dwInstance=0 ;
	WCHAR wszContainer[256] ;
	IDxDiagContainer* pkSystemCT=0 ;
	IDxDiagContainer* pkDisplatCT=0 ;
	IDxDiagContainer* pkChildCT=0 ;

	if( !m_bInited ) return ;
	hR = m_pkDxRoot->GetChildContainer( _T( "DxDiag_SystemInfo" ), &pkSystemCT ) ;
	if( FAILED(hR) || pkSystemCT==NULL ) assert_cs( false ) ;

	hR = GetStringValue( pkSystemCT, "szProcessorEnglish", EXPAND(m_kSystemInformation.m_szProcessorEnglish) ) ;
	if( FAILED(hR) ) assert_cs( false ) ;
	hR = GetStringValue( pkSystemCT, "szPhysicalMemoryEnglish", EXPAND(m_kSystemInformation.m_szPhysicalMemoryEnglish) ) ;
	if( FAILED(hR) ) assert_cs( false ) ;
	hR = GetStringValue( pkSystemCT, "szCSDVersion", EXPAND(m_kSystemInformation.m_szCSDVersion) ) ;
	if( FAILED(hR) ) assert_cs( false ) ;
	hR = GetStringValue( pkSystemCT, "szDirectXVersionEnglish", EXPAND(m_kSystemInformation.m_szDirectXVersionEnglish) ) ;
	if( FAILED(hR) ) assert_cs( false ) ;
	hR = GetStringValue( pkSystemCT, "szDirectXVersionLongEnglish", EXPAND(m_kSystemInformation.m_szDirectXVersionLongEnglish) ) ;
	if( FAILED(hR) ) assert_cs( false ) ;
	hR = GetStringValue( pkSystemCT, "szOSEnglish", EXPAND(m_kSystemInformation.m_szOSEnglish) ) ;
	if( FAILED(hR) ) assert_cs( false ) ;
	hR = GetStringValue( pkSystemCT, "szOSExEnglish", EXPAND(m_kSystemInformation.m_szOSExEnglish) ) ;
	if( FAILED(hR) ) assert_cs( false ) ;
	hR = GetStringValue( pkSystemCT, "szOSExLongEnglish", EXPAND(m_kSystemInformation.m_szOSExLongEnglish) ) ;
	if( FAILED(hR) ) assert_cs( false ) ;

	SAFE_RELEASE( pkSystemCT ) ;

	hR = m_pkDxRoot->GetChildContainer( _T( "DxDiag_DisplayDevices" ), &pkDisplatCT ) ;
	if( FAILED(hR) || pkDisplatCT==NULL ) assert_cs( false ) ;
	hR = pkDisplatCT->GetNumberOfChildContainers( &dwInstance ) ;
	if( FAILED(hR) ) assert_cs( false ) ;

	for( DWORD dwItem=0; dwItem<dwInstance; dwItem++ )
	{
		DispalyInformation kDPInfo ;

		hR = pkDisplatCT->EnumChildContainerNames( dwItem, wszContainer, 256 );
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = pkDisplatCT->GetChildContainer( wszContainer, &pkChildCT );
		if( FAILED(hR) ) assert_cs( false ) ;

		hR = GetStringValue( pkChildCT, "szDescription", EXPAND(kDPInfo.m_szDescription) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szManufacturer", EXPAND(kDPInfo.m_szManufacturer) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szChipType", EXPAND(kDPInfo.m_szChipType) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szDACType", EXPAND(kDPInfo.m_szDACType) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szRevision", EXPAND(kDPInfo.m_szRevision) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szDisplayMemoryLocalized", EXPAND(kDPInfo.m_szDisplayMemoryLocalized) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szDisplayMemoryEnglish", EXPAND(kDPInfo.m_szDisplayMemoryEnglish) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szDisplayModeLocalized", EXPAND(kDPInfo.m_szDisplayModeLocalized) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szDisplayModeEnglish", EXPAND(kDPInfo.m_szDisplayModeEnglish) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;

		hR = GetUIntValue( pkChildCT, "dwWidth", &kDPInfo.m_dwWidth ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetUIntValue( pkChildCT, "dwHeight", &kDPInfo.m_dwHeight ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetUIntValue( pkChildCT, "dwBpp", &kDPInfo.m_dwBpp ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetUIntValue( pkChildCT, "dwRefreshRate", &kDPInfo.m_dwRefreshRate ) ;
		if( FAILED(hR) ) assert_cs( false ) ;

		hR = GetStringValue( pkChildCT, "szMonitorName", EXPAND(kDPInfo.m_szMonitorName) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szMonitorMaxRes", EXPAND(kDPInfo.m_szMonitorMaxRes) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;

		hR = GetStringValue( pkChildCT, "szDriverName", EXPAND(kDPInfo.m_szDriverName) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szDriverVersion", EXPAND(kDPInfo.m_szDriverVersion) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szDriverAttributes", EXPAND(kDPInfo.m_szDriverAttributes) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szDriverLanguageEnglish", EXPAND(kDPInfo.m_szDriverLanguageEnglish) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szDriverLanguageLocalized", EXPAND(kDPInfo.m_szDriverLanguageLocalized) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szDriverDateEnglish", EXPAND(kDPInfo.m_szDriverDateEnglish) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szDriverDateLocalized", EXPAND(kDPInfo.m_szDriverDateLocalized) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetIntValue( pkChildCT, "lDriverSize", &kDPInfo.m_lDriverSize ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szMiniVdd", EXPAND(kDPInfo.m_szMiniVdd) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szMiniVddDateLocalized", EXPAND(kDPInfo.m_szMiniVddDateLocalized) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szMiniVddDateEnglish", EXPAND(kDPInfo.m_szMiniVddDateEnglish) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetIntValue( pkChildCT, "lMiniVddSize", &kDPInfo.m_lMiniVddSize ) ;
		if( FAILED(hR) ) assert_cs( false ) ;
		hR = GetStringValue( pkChildCT, "szVdd", EXPAND(kDPInfo.m_szVdd) ) ;
		if( FAILED(hR) ) assert_cs( false ) ;

		SAFE_RELEASE( pkChildCT ) ;

		m_listDisplayInformation.push_back( kDPInfo ) ;
	}

	SAFE_RELEASE( pkDisplatCT ) ;

}

void GbDxDiagInfo::Write( char* szFile )
{
	char szBuf[ MAX_PATH ];
	std::string szLastError, szSystem, szDisplay, szUser ;

	bool bLastError = false;
	szLastError += "[ Last Error : ";
	switch( g_CsLastError.m_eType )
	{
	case CsLastError::__ThreadLoad1:
		{
			bLastError = true;

			szLastError += " __ThreadLoad1 ]";
			szLastError += "\r\n";						

			sprintf_s( szBuf, MAX_PATH, "    type				   : %d \r\n", g_CsLastError.m_nIntValue1 );	
			szLastError += szBuf;
			sprintf_s( szBuf, MAX_PATH, "    bool값				   : %d \r\n", g_CsLastError.m_nIntValue2 );	
			szLastError += szBuf;	
			sprintf_s( szBuf, MAX_PATH, "    Process			   : %d \r\n", g_CsLastError.m_nIntValue3 );	
			szLastError += szBuf;
			sprintf_s( szBuf, MAX_PATH, "    ModelDataLoad		   : %d \r\n", g_CsLastError.m_nIntValue4 );	
			szLastError += szBuf;			
			sprintf_s( szBuf, MAX_PATH, "    ModelID			   : %d \r\n", g_CsLastError.m_nIntValue5 );	
			szLastError += szBuf;			

			szLastError += "\r\n\n\n\n\n" ;
		}
		break;

	case CsLastError::__ThreadLoad2:
		{
			bLastError = true;

			szLastError += " __ThreadLoad2 ]";
			szLastError += "\r\n";						

			sprintf_s( szBuf, MAX_PATH, "    type				   : %d \r\n", g_CsLastError.m_nIntValue1 );	
			szLastError += szBuf;
			sprintf_s( szBuf, MAX_PATH, "    bool값				   : %d \r\n", g_CsLastError.m_nIntValue2 );	
			szLastError += szBuf;			
			sprintf_s( szBuf, MAX_PATH, "    Process			   : %d \r\n", g_CsLastError.m_nIntValue3 );	
			szLastError += szBuf;
			sprintf_s( szBuf, MAX_PATH, "    ModelDataLoad		   : %d \r\n", g_CsLastError.m_nIntValue4 );	
			szLastError += szBuf;			
			sprintf_s( szBuf, MAX_PATH, "    ModelID			   : %d \r\n", g_CsLastError.m_nIntValue5 );	
			szLastError += szBuf;			

			szLastError += "\r\n\n\n\n\n" ;
		}
		break;

	case CsLastError::__CalAniConstant:
		{
			bLastError = true;

			szLastError += " __CalAniConstant ]";
			szLastError += "\r\n";						

			sprintf_s( szBuf, MAX_PATH, "    pLoadSequence	전반   : %p \r\n", g_CsLastError.m_pValue1 );	
			szLastError += szBuf;
			sprintf_s( szBuf, MAX_PATH, "    m_pSequence		   : %p \r\n", g_CsLastError.m_pValue2 );	
			szLastError += szBuf;			
			sprintf_s( szBuf, MAX_PATH, "    pLoadSequence	후반   : %p \r\n", g_CsLastError.m_pValue3 );
			szLastError += szBuf;
			sprintf_s( szBuf, MAX_PATH, "    pLoadSequence->GetSequence  : %p \r\n", g_CsLastError.m_pValue4 );
			szLastError += szBuf;			
			sprintf_s( szBuf, MAX_PATH, "    GetFTID			   : %d \r\n", g_CsLastError.m_nIntValue1 );	
			szLastError += szBuf;			

			szLastError += "\r\n\n\n\n\n" ;
		}
		break;	

	case  CsLastError::__attack:
		{
			bLastError = true;

			szLastError += " __attack ]";
			szLastError += "\r\n";

			sprintf_s( szBuf, MAX_PATH, "	 model				   : %d \r\n", g_CsLastError.m_nIntValue1 );	
			szLastError += szBuf;
			sprintf_s( szBuf, MAX_PATH, "    ANI				   : %d \r\n", g_CsLastError.m_nIntValue2 );	
			szLastError += szBuf;	
			sprintf_s( szBuf, MAX_PATH, "    IsLoad				   : %d \r\n", g_CsLastError.m_nIntValue3 );	
			szLastError += szBuf;	
			sprintf_s( szBuf, MAX_PATH, "    Ref				   : %d \r\n", g_CsLastError.m_nIntValue4 );	
			szLastError += szBuf;	
			sprintf_s( szBuf, MAX_PATH, "    MapCnt				   : %d \r\n", g_CsLastError.m_nIntValue5 );	
			szLastError += szBuf;	

			sprintf_s( szBuf, MAX_PATH, "    GetLoadSequence	   : %p \r\n", g_CsLastError.m_pValue1 );	
			szLastError += szBuf;
			sprintf_s( szBuf, MAX_PATH, "    GetSequenceMng		   : %p \r\n", g_CsLastError.m_pValue2 );	
			szLastError += szBuf;


			std::map< DWORD, CsC_LoadSequence::sSEQUENCE_INFO* >* mapSequence = ( (CsC_LoadSequence*)g_CsLastError.m_pValue1 )->GetSequenceMap();

			std::map< DWORD, CsC_LoadSequence::sSEQUENCE_INFO* >::iterator it = mapSequence->begin();		
			std::map< DWORD, CsC_LoadSequence::sSEQUENCE_INFO* >::iterator itEnd = mapSequence->end();			

			for( ; it != itEnd; it++ )
			{
				sprintf_s( szBuf, MAX_PATH, "    Key		   : %d \r\n", it->first );	
				szLastError += szBuf;
			}
			
			std::map< DWORD, CsModelData::sSEQUENCE* >::iterator it2 = g_pModelDataMng->GetData( g_CsLastError.m_nIntValue1 )->GetSequenceMap()->begin();
			std::map< DWORD, CsModelData::sSEQUENCE* >::iterator it2End = g_pModelDataMng->GetData( g_CsLastError.m_nIntValue1 )->GetSequenceMap()->end();

			for( ; it2 != it2End; it2++ )
			{
				sprintf_s( szBuf, MAX_PATH, "    Key		   : %d \r\n", it->first );	
				szLastError += szBuf;
			}

		}
		break;	
	}

	SystemInformation* pkSystemInformation ;	
	std::list<DispalyInformation>* plistDisplayInformation ;
	std::list<DispalyInformation>::iterator kIter ;

	// system information
	pkSystemInformation = GetSystemInfo() ;
	szSystem+= pkSystemInformation->m_szOSExLongEnglish;
	szSystem+= "\r\n" ;
	szSystem+= pkSystemInformation->m_szProcessorEnglish;
	szSystem+= " " ;
	szSystem+= pkSystemInformation->m_szPhysicalMemoryEnglish;
	szSystem+= "\r\n" ;
	szSystem+= pkSystemInformation->m_szDirectXVersionLongEnglish;
	szSystem+= "\r\n" ;

	// display information
	plistDisplayInformation = GetDisplayInfo() ;
	kIter = plistDisplayInformation->begin() ;
	for( ; kIter!=plistDisplayInformation->end(); kIter++ )
	{
		DispalyInformation kDInfo=*kIter ;
		szDisplay += kDInfo.m_szDescription;
		szDisplay += kDInfo.m_szDisplayMemoryLocalized;	
		szDisplay += kDInfo.m_szDisplayMemoryEnglish;	
		szDisplay += "\r\n" ;
	}

	
	GetCurrentDirectoryA( MAX_PATH, szBuf );

	szUser += "CurDirectory   : ";
	szUser += szBuf;
	szUser += "\r\n" ;	

	szUser += "AccumTime      : ";
	sprintf( szBuf, "%.1f [ %.2f ]", g_fAccumTime, g_fAccumTime/60.0f );
	szUser += szBuf;
	szUser += "\r\n\n" ;	

	

	if( g_pResist )
	{
		szUser += "Account        : ";
		szUser += nBase::w2m( g_pResist->m_szID );
		szUser += "\r\n" ;

		szUser += "Server         : ";
		szUser += nBase::w2m( g_pResist->m_szServer );
		szUser += "\r\n" ;

		szUser += "TamerName      : ";
		szUser += nBase::w2m( g_pResist->m_szChar );
		szUser += "\r\n" ;
		
		if( g_pCharMng )
		{
			if( g_pCharMng->GetTamerUser() )
			{
				szUser += "TamerType      : ";
				sprintf( szBuf, "%d", g_pCharMng->GetTamerUser()->GetFTID() );
				szUser += szBuf;
				szUser += "\r\n" ;
			}
			if( g_pCharMng->GetDigimonUser( 0 ) )
			{
				szUser += "DigimonName    : ";
				szUser += nBase::w2m( g_pCharMng->GetDigimonUser( 0 )->GetName() );
				szUser += "\r\n" ;

				szUser += "DigimonType    : ";
				sprintf( szBuf, "%d", g_pCharMng->GetDigimonUser( 0 )->GetFTID() );
				szUser += szBuf;
				szUser += "\r\n" ;
			}
		}

		if( nsCsGBTerrain::g_pCurRoot )
		{
			DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
			szUser += "MapID          : ";

			if( nsCsMapTable::g_pMapListMng->IsList( dwMapID ) )
			{
				sprintf( szBuf, "%d ( %s )", dwMapID, nBase::w2m( nsCsMapTable::g_pMapListMng->GetList( dwMapID )->GetInfo()->s_szMapDiscript.c_str() ) );
			}
			else
			{
				sprintf( szBuf, "%d", dwMapID );
			}
			szUser += szBuf;
			szUser += "\r\n" ;
		}		
	}

	szUser += "CurGameStep    : ";

	switch( FLOWMGR_ST.GetCurTopFlowID() )
	{
	case Flow::CFlow::FLW_NONE:			szUser += "GAME_FLW_NONE";			break;
	case Flow::CFlow::FLW_INTRO:		szUser += "GAME_FLW_INTRO";			break;
	case Flow::CFlow::FLW_LOGO:			szUser += "GAME_FLW_LOGO";			break;
	case Flow::CFlow::FLW_LOGIN:		szUser += "GAME_FLW_LOGIN";			break;
	case Flow::CFlow::FLW_SERVERSEL:	szUser += "GAME_FLW_SERVERSEL";			break;
	case Flow::CFlow::FLW_SECURITY:		szUser += "GAME_FLW_SECURITY";			break;
	case Flow::CFlow::FLW_MAINGAME:		szUser += "GAME_FLW_MAINGAME";			break;
	case Flow::CFlow::FLW_LOADING:		szUser += "GAME_FLW_LOADING";			break;
	case Flow::CFlow::FLW_CHARCREATE:	szUser += "GAME_FLW_CHARCREATE";		break;
	case Flow::CFlow::FLW_CHARSEL:		szUser += "GAME_FLW_CHARSEL";		break;

// 	case GAME_STEP_NONE:		szUser += "GAME_STEP_NONE";			break;
// 	case GAME_LOGIN:			szUser += "GAME_LOGIN";				break;
// 	case GAME_CHARACTER_CREATE:	szUser += "GAME_CHARACTER_CREATE";	break;
// 	case GAME_CHARACTER_SELECT:	szUser += "GAME_CHARACTER_SELECT";	break;
// 	case GAME_LOADING:			szUser += "GAME_LOADING";			break;
// 	case GAME_PLAY:				szUser += "GAME_PLAY";				break;
// 	case GAME_TEST:				szUser += "GAME_TEST";				break;
// 	case GAME_CLIENT_PORTAL:	szUser += "GAME_CLIENT_PORTAL";		break;
// 	case GAME_EXIT:				szUser += "GAME_EXIT";				break;
// 	case GAME_LOGOUT:			szUser += "GAME_LOGOUT";			break;
	}
	szUser += "\r\n" ;


	szUser += "\r\n";
	szUser += "\r\n" ;

	FILE* fp = NULL;
	fopen_s( &fp, szFile, "wt" );
	if( bLastError )
	{
		fwrite( szLastError.data(), sizeof( char ), szLastError.size(), fp );
	}

	if( szUser.size() )
	{
		fwrite( szUser.data(), sizeof( char ), szUser.size(), fp );
	}
	fwrite( szSystem.data(), sizeof( char ), szSystem.size(), fp );
	fwrite( szDisplay.data(), sizeof( char ), szDisplay.size(), fp );
	fclose( fp );
}

//-----------------------------------------------------------------------------
HRESULT GetStringValue( IDxDiagContainer* pObject, char* wstrName, char* strValue, int nStrLen )
{
	HRESULT hr;
	VARIANT var;
	VariantInit( &var );

	if( FAILED( hr = pObject->GetProp( nBase::m2w( wstrName ).c_str(), &var ) ) )
		return hr;

	if( var.vt != VT_BSTR )
		return E_INVALIDARG;

	wcstombs( strValue, var.bstrVal, nStrLen );   

	strValue[nStrLen-1] = TEXT('\0');
	VariantClear( &var );

	return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT GetUIntValue( IDxDiagContainer* pObject, char* wstrName, DWORD* pdwValue )
{
	HRESULT hr;
	VARIANT var;
	VariantInit( &var );

	if( FAILED( hr = pObject->GetProp( nBase::m2w( wstrName ).c_str(), &var)))
		return hr;

	if( var.vt != VT_UI4 )
		return E_INVALIDARG;

	*pdwValue = var.ulVal;
	VariantClear( &var );

	return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT GetIntValue( IDxDiagContainer* pObject, char* wstrName, LONG* pnValue )
{
	HRESULT hr;
	VARIANT var;
	VariantInit( &var );

	if( FAILED( hr = pObject->GetProp( nBase::m2w( wstrName ).c_str(), &var ) ) )
		return hr;

	if( var.vt != VT_I4 )
		return E_INVALIDARG;

	*pnValue = var.lVal;
	VariantClear( &var );

	return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT GetBoolValue( IDxDiagContainer* pObject, char* wstrName, BOOL* pbValue )
{
	HRESULT hr;
	VARIANT var;
	VariantInit( &var );

	if( FAILED( hr = pObject->GetProp( nBase::m2w( wstrName ).c_str(), &var ) ) )
		return hr;

	if( var.vt != VT_BOOL )
		return E_INVALIDARG;

	*pbValue = ( var.boolVal != 0 );
	VariantClear( &var );

	return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT GetInt64Value( IDxDiagContainer* pObject, char* wstrName, ULONGLONG* pullValue )
{
	HRESULT hr;
	VARIANT var;
	VariantInit( &var );

	if( FAILED( hr = pObject->GetProp( nBase::m2w( wstrName ).c_str(), &var ) ) )
		return hr;

	if( var.vt != VT_BSTR )
		return E_INVALIDARG;

	*pullValue = _wtoi64( var.bstrVal );
	VariantClear( &var );

	return S_OK;
}



DxResolutionInfo*		g_pResolutionInfo = NULL;

void DxResolutionInfo::GlobalInit()
{
	assert_cs( g_pResolutionInfo == NULL );
	g_pResolutionInfo = NiNew DxResolutionInfo;
	g_pResolutionInfo->Init();
}

void DxResolutionInfo::GlobalShotDown()
{
	if( g_pResolutionInfo == NULL )
		return;
	g_pResolutionInfo->Delete();
	SAFE_NIDELETE( g_pResolutionInfo );
}

void DxResolutionInfo::Init()
{
	LPDIRECT3D9 pD3D;
	HRESULT hr = g_pEngine->m_spRenderer->GetD3DDevice()->GetDirect3D( &pD3D );
	if( pD3D == NULL )
	{
		DWORD dwError = GetLastError();
		CsMessageBox( MB_OK, _T( "Resolution : GetDevice Failed! ( ErrorCode = %d )" ), dwError );
		return;
	}
	
	DWORD dwModeCount = pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8 );
	for( DWORD i=0; i<dwModeCount; ++i )
	{
		D3DDISPLAYMODE* pMode = csnew D3DDISPLAYMODE;		
		pD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, pMode );

		// width 는 최하 1024보다 커야 한다.
		if( pMode->Width < 1024 || pMode->Height < 768 )
		{
			SAFE_DELETE( pMode );
			continue;
		}

		// 같은 값이 존재 해선 안된다.
		if( GetResolution( pMode->Width, pMode->Height ) != NULL )
		{
			SAFE_DELETE( pMode );
			continue;
		}

		m_list.push_back( pMode );
	}

	// 설정된 해상도가 존재 하지 않는다면
	if( GetResolution( g_pResist->m_Global.s_nResolutionWidth, g_pResist->m_Global.s_nResolutionHeight ) == NULL )
	{		
		g_pResist->m_Global.ResetResolution();
	}

	// 멀티 샘플 타입	
	if( SUCCEEDED( pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, TRUE, D3DMULTISAMPLE_8_SAMPLES, NULL ) ) )
	{
		m_mapMultisample[ NiDX9Renderer::FBMODE_MULTISAMPLES_8 ] = true;		
	}
	if( SUCCEEDED( pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, TRUE, D3DMULTISAMPLE_6_SAMPLES, NULL ) ) )
	{
		m_mapMultisample[ NiDX9Renderer::FBMODE_MULTISAMPLES_6 ] = true;
	}
	if( SUCCEEDED( pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, TRUE, D3DMULTISAMPLE_4_SAMPLES, NULL ) ) )
	{
		m_mapMultisample[ NiDX9Renderer::FBMODE_MULTISAMPLES_4 ] = true;
	}
	if( SUCCEEDED( pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, TRUE, D3DMULTISAMPLE_2_SAMPLES, NULL ) ) )
	{
		m_mapMultisample[ NiDX9Renderer::FBMODE_MULTISAMPLES_2 ] = true;
	}
	if( SUCCEEDED( pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, TRUE, D3DMULTISAMPLE_NONE, NULL ) ) )
	{
		m_mapMultisample[ NiDX9Renderer::FBMODE_DEFAULT ] = true;		
	}
	// 설정된 멀티 샘플 단계 체크 ( 설정된 값이 없다면 2 )
	if( IsMultisample( (NiDX9Renderer::FramebufferMode)g_pResist->m_Global.s_nAntialicing ) == false )
	{
		g_pResist->m_Global.s_nAntialicing = NiDX9Renderer::FBMODE_DEFAULT;
		if( m_mapMultisample.empty() == false )
		{
			std::map< NiDX9Renderer::FramebufferMode, bool >::iterator it = m_mapMultisample.begin();
			std::map< NiDX9Renderer::FramebufferMode, bool >::iterator itEnd = m_mapMultisample.end();
			for( ; it!=itEnd; ++it )
			{
				if( g_pResist->m_Global.s_nAntialicing < (DWORD)it->first )
					g_pResist->m_Global.s_nAntialicing = (DWORD)it->first;

				if( g_pResist->m_Global.s_nAntialicing >= NiDX9Renderer::FBMODE_MULTISAMPLES_2 )
				{
					break;
				}
			}
		}
	}

	pD3D->Release();
}

void DxResolutionInfo::Delete()
{
	std::list< D3DDISPLAYMODE* >::iterator it = m_list.begin();
	std::list< D3DDISPLAYMODE* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
		SAFE_DELETE( *it );
	m_list.clear();
}

D3DDISPLAYMODE* DxResolutionInfo::GetResolution( int nIndex )
{
	assert_cs( nIndex >= 0 );
	std::list< D3DDISPLAYMODE* >::iterator it = m_list.begin();
	std::list< D3DDISPLAYMODE* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		if( (--nIndex) < 0 )
			return (*it);
	}

	assert_cs( false );
	return NULL;
}

D3DDISPLAYMODE* DxResolutionInfo::GetResolution( int nWidth, int nHeight )
{
	std::list< D3DDISPLAYMODE* >::iterator it = m_list.begin();
	std::list< D3DDISPLAYMODE* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		if( ( (*it)->Width == nWidth )&&( (*it)->Height == nHeight ) )
			return (*it);
	}
	return NULL;
}

int DxResolutionInfo::FindResolutionIndex( int nWidth, int nHeight )
{
	int nIndex = 0;
	std::list< D3DDISPLAYMODE* >::iterator it = m_list.begin();
	std::list< D3DDISPLAYMODE* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		if( ( (*it)->Width == nWidth )&&( (*it)->Height == nHeight ) )
			return nIndex;
		++nIndex;
	}
	return -1;
}