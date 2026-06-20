
#include "stdafx.h"
#include "Helper.h"

void cHelper::ResetDevice()
{
	if( m_pBG )
		m_pBG->ResetDevice();
}

void cHelper::Delete()
{
	SAFE_NIDELETE( m_pBG );
}

void cHelper::Init()
{
	m_pBG = NiNew cSprite;
	m_pBG->Init( NULL, CsPoint::ZERO, CsPoint( PROC_3D_WIN_WIDTH, PROC_3D_WIN_HEIGHT ), "Data\\Interface\\CharCreate\\CharSel.tga", false );
}

void cHelper::Update()
{

}

void cHelper::Render()
{
	m_pBG->Render();
}