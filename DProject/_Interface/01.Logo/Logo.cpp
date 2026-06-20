
#include "stdafx.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#include "Logo.h"

#define	FAID_IN_DELAY	1.0f
#define	FAID_WAIT_DELAY	2.0f
#define	FAID_OUT_DELAY	1.0f

void CLogo::sRenderImg::Render( int nScreenWidth, int nScreenHeight, float fCurAlpha)
{
	if( stringList.GetSize() )
	{
		CsPoint pos;
		pos.x = nScreenWidth / 2;
		pos.y = (int)( 470 * nScreenHeight / 768.0f );
		stringList.SetAlpha( fCurAlpha );
		stringList.Render( pos, 35, DT_CENTER );
	}

	std::vector<cSprite*>::iterator it = imgs.begin();
	for( ; it != imgs.end(); ++it )
	{
		(*it)->SetAlpha( fCurAlpha );
		(*it)->Render();
	}
}

CLogo::CLogo():m_nImgIndex(0), m_nMaxImage(0), m_fCurAlpha(0), m_FaidState(eFAID_IN)
{
}

CLogo::~CLogo()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy();
}

void CLogo::Init()
{
	m_fCurAlpha = 0.0f;
	m_nImgIndex = 0;

	AddLogo( "CommonUI\\Digitalic_logo.tga", CsPoint( 125, 87 ) );
#ifdef VERSION_KOR
	AddGuideLogo();
#endif
	m_nMaxImage = m_imgVector.size();
	m_FaidState = eFAID_IN;
}

void CLogo::ResetDevice()
{
	std::vector<sRenderImg*>::iterator it = m_imgVector.begin();
	for( ; it != m_imgVector.end(); ++it )
	{
		(*it)->stringList.ResetDevice();
		std::vector<cSprite*>::iterator spit = (*it)->imgs.begin();
		for( ; spit != (*it)->imgs.end(); ++spit )
			(*spit)->ResetDevice();
	}
}

void CLogo::Destroy()
{
	std::vector<sRenderImg*>::iterator it = m_imgVector.begin();
	for( ; it != m_imgVector.end(); ++it )
	{
		(*it)->stringList.Delete();

		std::vector<cSprite*>::iterator spit = (*it)->imgs.begin();
		for( ; spit != (*it)->imgs.end(); ++spit )
			SAFE_NIDELETE( (*spit) );
		(*it)->imgs.clear();
	}
	m_imgVector.clear();
}

void CLogo::Update()
{
	FaidUpdate();
}

void CLogo::Render()
{
	if( m_nImgIndex >= m_nMaxImage )
		return;	

	m_imgVector[m_nImgIndex]->Render( g_nScreenWidth, g_nScreenHeight, m_fCurAlpha );

}

void CLogo::AddLogo( char* pFile, CsPoint csSize )
{
	CsPoint pos((g_nScreenWidth- csSize.x ) >> 1, (g_nScreenHeight- csSize.y) >> 1);
	sRenderImg *tmp1 = new sRenderImg;
	cSprite* pLogo = NiNew cSprite;
	pLogo->Init( NULL, pos, csSize, pFile, false );
	tmp1->imgs.push_back( pLogo );
	m_imgVector.push_back( tmp1 );
}

void CLogo::AddGuideLogo()
{
	sRenderImg * tmp2  = new sRenderImg;	
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_20, NiColor::BLACK );
	ti.s_bOutLine = false;
	//ti.s_eTextAlign = DT_CENTER;

	cString* pString = NiNew cString;
	ti.SetText( _T( "본 게임물은 12세 이용가 게임으로서" ) );
	pString->AddText( &ti );
	tmp2->stringList.AddTail( pString );

	pString = NiNew cString;
	ti.SetText( _T( "해당 연령 미만의 어린이가 이용하기에는 다소 폭력적인 내용을 포함하고 있으므로" ) );
	pString->AddText( &ti );
	tmp2->stringList.AddTail( pString );

	pString = NiNew cString;
	ti.SetText( _T( "보호자의 지도 감독이 필요한 게임입니다." ) );
	pString->AddText( &ti );
	tmp2->stringList.AddTail( pString );

	float fRate = 0.26f;

	CsPoint size;
	size.x = (int)( g_nScreenWidth*fRate*0.5f );
	size.y = 117 * size.x / 99;

	CsPoint pos2;
	pos2.x = g_nScreenWidth-2;
	pos2.y = 2;

	pos2.x -= size.x;
	cSprite* pLogo = NiNew cSprite;
	pLogo->Init( NULL, pos2, size, "CommonUI\\NotViolence.tga", false );
	tmp2->imgs.push_back( pLogo );

	pos2.x -= ( size.x + 1 );
	cSprite* pLogo2 = NiNew cSprite;
	pLogo2->Init( NULL, pos2, size, "CommonUI\\TotalityLogo.tga", false );
	tmp2->imgs.push_back( pLogo2 );
	m_imgVector.push_back( tmp2 );
}

void CLogo::FaidUpdate()
{
	if( m_nImgIndex >= m_nMaxImage )
		return;

	switch(m_FaidState)
	{
	case eFAID_NONE:
		break;
	case eFAID_IN:
		{
			m_Duration += g_fDeltaTime;
			m_fCurAlpha = abs(m_Duration/FAID_IN_DELAY);

			if(m_Duration > FAID_IN_DELAY)
			{
				m_Duration = 0.0f;
				m_fCurAlpha = 1.0f;
				m_FaidState = eFAID_WAIT;
			}
		}
		break;

	case eFAID_WAIT:
		m_Duration += abs(g_fDeltaTime);
		if(m_Duration > FAID_WAIT_DELAY)
		{
			m_Duration = 0.0f;;
			m_FaidState = eFAID_OUT;
		}
		break;

	case eFAID_OUT:
		{
			m_Duration += abs(g_fDeltaTime);

			m_fCurAlpha = 1.0f - m_Duration/FAID_OUT_DELAY;

			if(m_Duration > FAID_OUT_DELAY)
			{
				m_Duration = 0.0f;
				m_fCurAlpha = 0.0f;
				m_nImgIndex++;
				if(m_nImgIndex >= m_nMaxImage)
				{
					m_FaidState = eFAID_NONE;
					GetSystem()->EndLogo();
				}
				else
				{
					m_FaidState = eFAID_IN;
				}
			}
		}		
		break;
	}
}
bool CLogo::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void CLogo::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}