#include "StdAfx.h"

#define XML_ENCODING_TYPE "UTF-8"
#define	OPTION_UI	"OPTION_UI"
COptionMng::COptionMng(void)
{
}
COptionMng::~COptionMng(void)
{
}

bool COptionMng::GetCharacterPath(TCHAR*	name)
{
	if(g_pResist)
	{
		int pathSizeID = lstrlen(g_pResist->m_szID);			//	ID
		int pathSizeServer = lstrlen(g_pResist->m_szServer);	//	서버
		int pathSizeChar = lstrlen(g_pResist->m_szChar);		//	케릭터명
		if(pathSizeID==0 || pathSizeServer==0 || pathSizeChar==0)
			return false;

		size_t nHashCode = CsFPS::GetHashCode( g_pResist->m_szChar );

		_stprintf_s( name, MAX_PATH, _T( "UserData\\%s\\%s\\%u" ), g_pResist->m_szID, g_pResist->m_szServer, nHashCode  );

		nsCSFILE::CreateDirectory( name );
		return true;
	}
	return false;
}

void COptionMng::LoadMachineOption()
{

}

void COptionMng::LoadCharOption(wstring	wStr)
{
	TCHAR szKey[ MAX_PATH ] = { 0, };
	if(GetCharacterPath(szKey)==false)
		return;
	_stprintf_s( szKey, 128, _T( "%s\\%s" ), szKey, wStr.c_str() );

	CHAR CHszKey[ MAX_PATH ];
	W2M(CHszKey, szKey, MAX_PATH);

	TiXmlDocument TiDoc;
	if( !TiDoc.LoadFile( CHszKey ) )	// Default 정보 로드.
	{
		m_PlayerUI.LoadToPlayerUIRecord(NULL, OPTION_UI);
	}
	else									// 정보 로드.
	{
		m_PlayerUI.LoadToPlayerUIRecord(&TiDoc, OPTION_UI);
	}
}

void COptionMng::SaveMachineOption()
{

}

void COptionMng::SaveCharOption(wstring	wStr)
{
	TCHAR szKey[ MAX_PATH ] = { 0, };	
	if(GetCharacterPath(szKey)==false)
		return;

	_stprintf_s( szKey, 128, _T( "%s\\%s" ), szKey, wStr.c_str());

	CHAR CHszKey[ MAX_PATH ];
	W2M(CHszKey, szKey, MAX_PATH);

	TiXmlDocument TiDoc( CHszKey );
	TiDoc.LinkEndChild( new TiXmlDeclaration( "1.0", XML_ENCODING_TYPE, "" ) );

	m_PlayerUI.SaveToPlayerUIRecord(&TiDoc, OPTION_UI);

	TiDoc.SaveFile();
}

COptionUIChat* COptionMng::LoadPlayerUIChatClass(int type, bool bLoad)
{
	COptionUIChat*	pChatClass = m_PlayerUI.GetUIChatClass(type, bLoad);

	return	pChatClass;
}

ST_CHAT_OPTION* COptionMng::LoadPlayerUIOption(int type, bool bLoad)
{
	ST_CHAT_OPTION*	pUiOpt = m_PlayerUI.GetUIOption(type, bLoad);

	return	pUiOpt;
}

void COptionMng::SavePlayerUIOption(int type, ST_CHAT_OPTION* pThis)
{
	SAFE_POINTER_RET(pThis);

	m_PlayerUI.SetUIOption(type, pThis);
}

INT COptionMng::GetPlayerUIOptionValue(int type, eOPTION_UI_VALUE_TYPE const OptID, bool bLoad)
{
	return m_PlayerUI.GetUIOptionValue(type, OptID, bLoad);	
}

ST_CHAT_OPTION*	COptionMng::GetPlayerUIOption(int type, bool bLoad)
{
	return m_PlayerUI.GetUIOption(type, bLoad);
}
