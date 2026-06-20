#include "StdAfx.h"

#include "../../ContentsSystem/ContentsSystem.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

#include "ChatContents.h"
#include "ChatBase.h"
#include "ChatSystemWindow.h"

#define CHAT_SYSTEM_COLOR	NiColor( 0, 1, 0.53f )

ChatSystemWindow::ChatSystemWindow(void)
{
	memset(	m_szLastStr, 0, 512 );
}

ChatSystemWindow::~ChatSystemWindow(void)
{
	SaveUIOption(XML_CHAT_SYSTEM);

	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}
void ChatSystemWindow::Destroy()
{	
	ChatBase::Destroy();
}

void ChatSystemWindow::DeleteResource()
{
	ChatBase::DeleteResource();
}

void ChatSystemWindow::ResetDevice()
{
	ChatBase::ResetDevice();
}

void ChatSystemWindow::ResetMap()
{
	ChatBase::ResetMap();
}

bool ChatSystemWindow::Close( bool bSound /* = true  */ )
{
	ChatBase::Close( bSound );
	return true;
}

void ChatSystemWindow::Create( int nValue /*=0*/ )
{
	LoadUIOption(XML_CHAT_SYSTEM);

	ChatBase::Create( nValue );
}

void ChatSystemWindow::Update(float const& fDeltaTime)
{
	ChatBase::Update(fDeltaTime);
}

cBaseWindow::eMU_TYPE ChatSystemWindow::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = ChatBase::Update_ForMouse();

	return muReturn;
}

void ChatSystemWindow::Render()
{
	ChatBase::Render();
}

void ChatSystemWindow::StringListRender()
{
	ChatBase::StringListRender();
}

void ChatSystemWindow::SetAlpha(float alpha)
{
}

void ChatSystemWindow::ControlUpdate(int SclrollSize)
{
	while( m_String.GetSize() > MAX_CHAT_LINE )
	{
		m_String.RemoveTail();		
	}

	for( int i=0; i<SclrollSize; ++i )
	{
		m_pScrollBar->IncreaseMaxCount( MAX_CHAT_LINE );
	}
	if( m_pScrollBar->GetCurPosIndex() == ( m_pScrollBar->GetMaxPosIndex() - SclrollSize ) )
	{
		m_pScrollBar->SetCurPosIndex( m_pScrollBar->GetMaxPosIndex() );
	}
}
void ChatSystemWindow::_MoveUi( void * sender, void* pData )
{
	_UpdateMoveWindow_ForMouse();
}

void ChatSystemWindow::_ChageBGSize( void * sender,  void* pData )
{
	ChatBase::_ChageBGSize(NULL, pData);
}

void ChatSystemWindow::_ChageBGUpdate( void * sender, void* pData )
{

}

void ChatSystemWindow::_SetText( TCHAR* szText, bool bParcing, bool bCheckSameString )
{
	// 같은 문자열이 들어올시 리턴
	if( bCheckSameString )
	{
		if( _tcscmp( szText, m_szLastStr ) == 0 )
			return;		
	}
	_tcscpy_s( m_szLastStr, 512, szText );

	//====================================================================================================
	//
	//		문자열
	//
	//====================================================================================================
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10, CHAT_SYSTEM_COLOR );
	// -----------------------------------
	// 문자열을 길이이에 맞게 자름.
	cStringList sl;
	if( bParcing == false )		g_pStringAnalysis->Cut( &sl, 346, szText, &ti );
	else						g_pStringAnalysis->Cut_Parcing( &sl, 346, szText, &ti );

	int nIncreaseCount = sl.GetSize();
	// -----------------------------------

	LIST_STRING* pList = sl.GetList();	
	for(LIST_STRING_IT it = pList->begin(); it != pList->end(); ++it )
	{
		m_String.AddHead( *it );
	}
	pList->clear();	

	ControlUpdate(nIncreaseCount);
}

void ChatSystemWindow::_SetTextWithColor( ContentsStream const& kStream )
{
	kStream.SetRp(0);
	std::wstring str;
	bool bParcing = false;	bool bCheckSameString = false; NiColor itemStrColor;
	kStream >> str >> bParcing >> bCheckSameString >> itemStrColor;

	TCHAR* szText = const_cast<TCHAR*>(str.data());
	// 같은 문자열이 들어올시 리턴
	if( bCheckSameString )
	{
		if( _tcscmp(szText, m_szLastStr ) == 0 )
			return;		
	}
	_tcscpy_s( m_szLastStr, 512, szText );

	//====================================================================================================
	//
	//		문자열
	//
	//====================================================================================================
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10, CHAT_SYSTEM_COLOR ); // 기본 시스템 컬러를 전체 문자열에 설정한다
	
	g_pStringAnalysis->SetParcingColor(itemStrColor); // [4/11/2016 hyun] 분석기에 아이템 문자 색상 설정
	// -----------------------------------
	// 문자열을 길이이에 맞게 자름.
	cStringList sl;
	if( bParcing == false )		g_pStringAnalysis->Cut( &sl, 346, szText, &ti );
	else						g_pStringAnalysis->Cut_Parcing( &sl, 346, szText, &ti );
	
	g_pStringAnalysis->CompleteParcingColor();	// [4/11/2016 hyun] 설정 완료
	
	int nIncreaseCount = sl.GetSize();
	// -----------------------------------

	LIST_STRING* pList = sl.GetList();	
	for(LIST_STRING_IT it = pList->begin(); it != pList->end(); ++it )
	{
		m_String.AddHead( *it );
	}
	pList->clear();	

	ControlUpdate(nIncreaseCount);
}

void ChatSystemWindow::_AddSetText(ContentsStream const& kStream)
{
	kStream.SetRp(0);	// 혹시 모를 일을 대비해 처음부터 읽는다.

	wstring str;
	bool bParcing = false;	bool bCheckSameString = false;

	kStream >> str >> bParcing >> bCheckSameString;

	_SetText(const_cast<TCHAR*>(str.c_str()), bParcing, bCheckSameString);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ChatSystemWindow::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( NS_CHAT::SYSTEM_TEXT, this );
	GetSystem()->Register( NS_CHAT::SYSTEM_TEXT_ITEMCROP, this );

	return true;
}

void ChatSystemWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
		case NS_CHAT::SYSTEM_TEXT:	
			_AddSetText(kStream);		
			break;
		case NS_CHAT::SYSTEM_TEXT_ITEMCROP:
			_SetTextWithColor(kStream);
			break;
	}
}