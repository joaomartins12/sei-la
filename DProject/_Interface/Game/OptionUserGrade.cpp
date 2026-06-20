#include "stdafx.h"
#include "OptionUserGrade.h"

void cOptionUserGrade::Destroy()
{
	cOptionBase::Delete();
}

void cOptionUserGrade::DeleteResource()
{
	cOptionBase::DeleteResource();

	SAFE_NIDELETE(m_pLogo1);
	SAFE_NIDELETE(m_pLogo2);
}

void cOptionUserGrade::Create( int nValue /* = 0 */ )
{
	if( cOptionBase::CreateBase( UISTRING_TEXT( "OPTION_USERGRADE_USER_RATING" ).c_str(), nValue ) == false )
		return;
 
	m_bMoveWindow = false; 

	CsPoint size( 120.f, 140.f);

	CsPoint pos( 210.f, 90.f );

	CsPoint pt1 =m_pRoot->GetClient();
	CsPoint pt2 =m_pRoot->GetDefaultPos();

	m_pLogo1 = NiNew cSprite;
	m_pLogo1->Init( m_pRoot,  pos, size, "CommonUI\\NotViolence.tga", false );
	pos.x -= ( size.x + 1 );
	m_pLogo1->SetAlpha( 1.0f );

	m_pLogo2 = NiNew cSprite;
	m_pLogo2->Init( m_pRoot, pos, size, "CommonUI\\TotalityLogo.tga", false );
	m_pLogo2->SetAlpha( 1.0f );

	for(int i = 0 ; i < 9 ; ++i)
		_AddControl_Title( _T( "" ), 1 );
	
	_AddControl_Title( UISTRING_TEXT( "OPTION_USERGRADE_TITLE_FIRST" ).c_str() );
	_AddControl_Title( UISTRING_TEXT( "OPTION_USERGRADE_TITLE_SECOND" ).c_str() );
	_AddControl_Title( UISTRING_TEXT( "OPTION_USERGRADE_TITLE_THIRD" ).c_str() );
	_AddControl_Title( UISTRING_TEXT( "OPTION_USERGRADE_TITLE_FOURTH" ).c_str() );

}

void cOptionUserGrade::Update(float const& fDeltaTime)
{
	cOptionBase::Update(fDeltaTime);
}

cBaseWindow::eMU_TYPE
cOptionUserGrade::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	return cOptionBase::Update_ForMouse();
}
void cOptionUserGrade::Render()
{
	cOptionBase::Render();

	if(m_pLogo1)
		m_pLogo1->Render();

	if(m_pLogo2)
		m_pLogo2->Render();
}

void cOptionUserGrade::ResetDevice()
{	
	cOptionBase::ResetDevice();
} 