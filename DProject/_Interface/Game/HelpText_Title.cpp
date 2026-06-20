


#include "stdafx.h"
#include "HelpText_Title.h"

#define TITLE_DELTA_Y		20

void cHelpText_Title::sINFO::Delete()
{
	SAFE_NIDELETE( s_pText );
}

void cHelpText_Title::sINFO::Init( cText::sTEXTINFO* ti, CsHelp* pHelpFT )
{
	assert_cs( s_pText == NULL );

	s_pFT = pHelpFT;
	ti->SetText( pHelpFT->GetTitle()->s_szTitle );
	s_pText = NiNew cText;
	s_pText->Init( NULL, CsPoint::ZERO, ti, false );	
}

void cHelpText_Title::sINFO::Render( CsPoint pos )
{
	s_pText->Render( pos );
}







cHelpText_Title::cHelpText_Title()
{
	m_pSelectMask = NULL;
	m_pSelectTitle = NULL;
}

void cHelpText_Title::Delete()
{
	SAFE_NIDELETE( m_pSelectMask );
	m_pSelectTitle = NULL;

	int nCount = m_vpInfo.Size();
	for( int i=0; i<nCount; ++i )
	{
		m_vpInfo[ i ]->Delete();
		NiDelete m_vpInfo[ i ];
	}
	m_vpInfo.Destroy();
}

void cHelpText_Title::Init()
{
	m_pSelectMask = NiNew cImage;
	m_pSelectMask->Init( NULL, CsPoint::ZERO, CsPoint( 86, 22 ), "Help\\Title_Sel.tga", false );
	m_pSelectMask->SetTexToken( CsPoint( 0, 24 ) );

	cText::sTEXTINFO ti;
	ti.Init();

	CsVectorPB< CsHelp* >* pvpData = nsCsFileTable::g_pHelpMng->GetData();
	int nCount = pvpData->Size();
	for( int i=0; i<nCount; ++i )
	{
		sINFO* pInfo = NiNew sINFO;
		pInfo->Init( &ti, ( *pvpData )[ i ] );
		m_vpInfo.PushBack( pInfo );
	}
}

void cHelpText_Title::Update(float const& fDeltaTime)
{

}

bool cHelpText_Title::Update_ForMouse( CsPoint ptLocal )
{
	if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_DOWN )
		return false;

	CsRect rc( cHelpText::GetTitleStartPos(), cHelpText::GetTitleEndPos() );
	if( rc.PtInRect( ptLocal ) == FALSE )
	{
		return false;
	}	

	CsPoint ptTitleLocal = ptLocal - cHelpText::GetTitleStartPos();
	sINFO* pPick = Pick( ptTitleLocal );
	if( pPick != NULL )
	{
		SetSelectTitle( pPick );
		return true;
	}
	return true;
}

void cHelpText_Title::Render( CsPoint ptParent )
{
	CsPoint ptSelect( ptParent.x - 3, ptParent.y - 4 );

	int nCount = m_vpInfo.Size();
	for( int i=0; i<nCount; ++i )
	{
		ptSelect.y = ptParent.y - 4;
		m_pSelectMask->Render( ptSelect, m_pSelectTitle == m_vpInfo[ i ] ? 0 : 1 );

		m_vpInfo[ i ]->Render( ptParent + CsPoint( 12, 0 ) );
		ptParent.y += TITLE_DELTA_Y;
	}
}

void cHelpText_Title::ResetDevice()
{
	m_pSelectMask->ResetDevice();
}

cHelpText_Title::sINFO* cHelpText_Title::Pick( CsPoint ptLocal )
{
	int nCount = m_vpInfo.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( ptLocal.y < TITLE_DELTA_Y )
		{
			return m_vpInfo[ i ];
		}
		ptLocal.y -= TITLE_DELTA_Y;
	}
	return NULL;
}

void cHelpText_Title::SetSelectTitle( sINFO* pSelect )
{
	m_pSelectTitle = pSelect;
	g_pGameIF->GetHelpText()->GetBody()->ApplyContainer( m_pSelectTitle->s_pFT->GetBody() );
}

void cHelpText_Title::SetSelectTitle( int nUniqID )
{
	int nCount = m_vpInfo.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpInfo[ i ]->s_pFT->GetTitle()->s_nUniqID == nUniqID )
		{
			SetSelectTitle( m_vpInfo[ i ] );
			return;
		}
	}
	assert_cs( false );
}