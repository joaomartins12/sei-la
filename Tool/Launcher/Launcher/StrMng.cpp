#include "StdAfx.h"
#include "StrMng.h"

cStrMng::cStrMng(void)
{
}

cStrMng::~cStrMng(void)
{
	Delete();	
}

cStrMng*	g_pStrMng = NULL;

void sStrInfo::ChangeStr( TCHAR* sz )
{	
	if( _tcslen( sz ) > 55 )
	{
		_tcsncpy_s( s_sz, 128, sz, 55 ); 
		_tcscat_s( s_sz, 128, L"..." ); 
	}
	else
		_tcscpy_s( s_sz, 128, sz ); 
} 


void cStrMng::Delete()
{
	std::list< sStrInfo* >::iterator it = m_strList.begin();
	std::list< sStrInfo* >::iterator itEnd = m_strList.end();

	for( ; it != itEnd; ++it )
	{
		SAFE_DELETE( *it );
	}
	m_strList.clear();

	//RemoveFontResource( _T( "NanumGothicBold.ttf" ) );
}

void cStrMng::Init()
{	
	//AddFontResource( _T( "NanumGothicBold.ttf" ) );	
}

void cStrMng::Update()
{

}

void cStrMng::Render( CDC* pDC )
{	
	if( m_strList.size() == 0 )
		return;

	std::list< sStrInfo* >::iterator it = m_strList.begin();
	std::list< sStrInfo* >::iterator itEnd = m_strList.end();

	for( ; it != itEnd; ++it )
	{
		CFont font;					

		if( (*it)->s_bBold == true )
		{
			font.CreateFont( (*it)->s_nFontSize, 0, 0, 0, FW_BOLD, 0, (*it)->s_bUnderline, 0, ANSI_CHARSET, 0, 0, 0, 0, _T("°íµñÃ¼") ); 			
		}
		else
		{
			font.CreateFont( (*it)->s_nFontSize, 0, 0, 0, FW_NORMAL, 0, (*it)->s_bUnderline, 0, ANSI_CHARSET, 0, 0, 0, 0, _T("°íµñÃ¼") ); 
		}

		CFont* pFont = pDC->SelectObject( &font );
		pDC->SetBkMode( TRANSPARENT );				

		if( (*it)->s_bOutline == true )
		{
			pDC->SetTextColor( RGB( 0, 0, 0 ) );		
			pDC->TextOut( (*it)->s_ptPos.x + 1, (*it)->s_ptPos.y + 1, (*it)->s_sz );		
		}		

		pDC->SetTextColor( (*it)->s_nColor );		
		pDC->TextOut( (*it)->s_ptPos.x, (*it)->s_ptPos.y, (*it)->s_sz );				

		pDC->SelectObject( pFont );	
		font.DeleteObject();
	}
}


