#include "stdafx.h"
#include "Tooltip_Mng.h"

CToolTipMng::CToolTipMng()
{

}

CToolTipMng::~CToolTipMng()
{
	NISAFE_DELETE( m_pTooltip );
	NISAFE_DELETE( m_pCompareTooltip );
	NISAFE_DELETE( m_pRightTooltip );
}	

void CToolTipMng::Init()
{
	m_pTooltip = NiNew cTooltip;
	m_pTooltip->Init();
	m_pCompareTooltip = NiNew cTooltip;
	m_pCompareTooltip->Init();
	m_pRightTooltip = NiNew cToolTip_Right;
	m_pRightTooltip->Init();
}

void CToolTipMng::Update(float const& fDeltaTime)
{
 	if( m_pTooltip )
 		m_pTooltip->Update(fDeltaTime);
 	if( m_pCompareTooltip )
 		m_pCompareTooltip->Update(fDeltaTime);
 	if( m_pRightTooltip )
 		m_pRightTooltip->Update(fDeltaTime);
}

cTooltip *	CToolTipMng::GetTooltip()
{
	return m_pTooltip;
}

cTooltip *	CToolTipMng::GetCompareTooltip()
{
	return m_pCompareTooltip;
}

cToolTip_Right * CToolTipMng::GetRightTooltip()
{ 
	return m_pRightTooltip; 
}

void CToolTipMng::Render()
{
	if( m_pTooltip )
	{
		m_pTooltip->Render();
		if( m_pCompareTooltip )
			m_pCompareTooltip->Render(m_pTooltip->GetRenderPoint());
	}
	if( m_pRightTooltip )
		m_pRightTooltip->Render();
}

void CToolTipMng::ResetDevice()
{
	if( m_pTooltip )
		m_pTooltip->ResetDevice();
	if( m_pCompareTooltip )
		m_pCompareTooltip->ResetDevice();
	if( m_pRightTooltip )
		m_pRightTooltip->ResetDevice();
}