
#include "stdafx.h"
#include "HpBar.h"

#define IF_HPBAR_MAX_SIZE			51
#define IF_HPBAR_BG_X				53
#define IF_HPBAR_BG_Y				7
#define IF_HPBAR_LIMIT_LENGTH		2000.0f

std::queue< cHpBar::sBAR* >		cHpBar::sBAR::g_MemPool[ cHpBar::MAX_TYPE ];

void cHpBar::sBAR::GlobalShotDown()
{
	sBAR* pBar;
	for( int i=0; i<cHpBar::MAX_TYPE; ++i )
	{
		while( g_MemPool[ i ].empty() == false )
		{
			pBar = g_MemPool[ i ].front();
			SAFE_NIDELETE( pBar->s_pBarBG );
			SAFE_NIDELETE( pBar->s_pBarHP );
			SAFE_NIDELETE( pBar->s_pBarHP2 );
			SAFE_NIDELETE( pBar->s_pBarDS );
			SAFE_NIDELETE( pBar->s_pBarFP );
			SAFE_NIDELETE( pBar );
			g_MemPool[ i ].pop();
		}
	}		
}

cHpBar::sBAR* cHpBar::sBAR::NewInstance( cHpBar::eTYPE type )
{
	sBAR* pReturn = NULL;
	if( g_MemPool[ type ].empty() == true )
	{
		switch( type )
		{
		case cHpBar::HP:
			pReturn = NiNew sBAR;
			pReturn->s_eType = type;
			pReturn->s_pBarBG = NiNew cSprite;
			pReturn->s_pBarBG->Init( NULL, CsPoint::ZERO, CsPoint( IF_HPBAR_BG_X, IF_HPBAR_BG_Y ), "HpBar\\Hp_Bar.tga", true );

			pReturn->s_pBarHP = NiNew cSprite;
			pReturn->s_pBarHP->Init( NULL, CsPoint::ZERO, CsPoint( IF_HPBAR_MAX_SIZE, 5 ), "TargetWindow\\MonsterBar.tga", true );

			pReturn->s_pBarDS = NULL;
			pReturn->s_pBarHP2 = NULL;
			pReturn->s_pBarFP = NULL;
			break;
		case cHpBar::HPDS:
			pReturn = NiNew sBAR;
			pReturn->s_eType = type;
			pReturn->s_pBarBG = NiNew cSprite;
			pReturn->s_pBarBG->Init( NULL, CsPoint::ZERO, CsPoint( IF_HPBAR_BG_X, IF_HPBAR_BG_Y ), "HpBar\\Hp_Bar.tga", true );

			pReturn->s_pBarHP = NiNew cSprite;
			pReturn->s_pBarHP->Init( NULL, CsPoint::ZERO, CsPoint( IF_HPBAR_MAX_SIZE, 5 ), "TargetWindow\\Hpbar2.tga", true );

			pReturn->s_pBarDS = NiNew cSprite;
			pReturn->s_pBarDS->Init( NULL, CsPoint::ZERO, CsPoint( IF_HPBAR_MAX_SIZE, 5 ), "TargetWindow\\Dsbar2.tga", true );

			pReturn->s_pBarHP2 = NULL;
			pReturn->s_pBarFP = NULL;
			break;
		case cHpBar::DS:
			pReturn = NiNew sBAR;
			pReturn->s_eType = type;
			pReturn->s_pBarBG = NiNew cSprite;
			pReturn->s_pBarBG->Init( NULL, CsPoint::ZERO, CsPoint( IF_HPBAR_BG_X, IF_HPBAR_BG_Y ), "HpBar\\Hp_Bar.tga", true );

			pReturn->s_pBarHP = NULL;

			pReturn->s_pBarDS = NiNew cSprite;
			pReturn->s_pBarDS->Init( NULL, CsPoint::ZERO, CsPoint( IF_HPBAR_MAX_SIZE, 5 ), "TargetWindow\\Dsbar2.tga", true );

			pReturn->s_pBarHP2 = NULL;
			pReturn->s_pBarFP = NULL;
			break;
		case cHpBar::HPDSFP:
			pReturn = NiNew sBAR;
			pReturn->s_eType = type;
			pReturn->s_pBarBG = NiNew cSprite;
			pReturn->s_pBarBG->Init( NULL, CsPoint::ZERO, CsPoint( IF_HPBAR_BG_X, IF_HPBAR_BG_Y ), "HpBar\\Hp_Bar.tga", true );

			pReturn->s_pBarHP = NiNew cSprite;
			pReturn->s_pBarHP->Init( NULL, CsPoint::ZERO, CsPoint( IF_HPBAR_MAX_SIZE, 5 ), "TargetWindow\\Hpbar2.tga", true );
			pReturn->s_pBarHP2 = NiNew cSprite;
			pReturn->s_pBarHP2->Init( NULL, CsPoint::ZERO, CsPoint( IF_HPBAR_MAX_SIZE, 5 ), "TargetWindow\\Hp3bar2.tga", true );

			pReturn->s_pBarDS = NiNew cSprite;
			pReturn->s_pBarDS->Init( NULL, CsPoint::ZERO, CsPoint( IF_HPBAR_MAX_SIZE, 5 ), "TargetWindow\\Dsbar2.tga", true );

			pReturn->s_pBarFP = NiNew cSprite;
			pReturn->s_pBarFP->Init( NULL, CsPoint::ZERO, CsPoint( IF_HPBAR_MAX_SIZE, 5 ), "TargetWindow\\Hp2bar2.tga", true );
			break;
		default:
			assert_cs( false );
		}		
	}
	else
	{
		pReturn = g_MemPool[ type ].front();
		g_MemPool[ type ].pop();
	}

	return pReturn;
}

void cHpBar::sBAR::DeleteInstance( sBAR* pInst )
{
	pInst->s_nTargetUID = 0;
	g_MemPool[ pInst->s_eType ].push( pInst );
}

bool cHpBar::sBAR::Init( cHpBar::eTYPE type, uint nTargetUID )
{
	s_nTargetUID = nTargetUID;
	CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );
	if( pTarget == NULL )
		return false;

	switch( type )
	{
	case cHpBar::HP:
		s_nHPBarSize = (int)( pTarget->GetBaseStat()->GetHPRate()*IF_HPBAR_MAX_SIZE );
		break;
	case cHpBar::HPDS:
		s_nHPBarSize = (int)( pTarget->GetBaseStat()->GetHPRate()*IF_HPBAR_MAX_SIZE );
		s_nDSBarSize = (int)( pTarget->GetBaseStat()->GetDSRate()*IF_HPBAR_MAX_SIZE );
		break;
	case cHpBar::DS:
		s_nDSBarSize = (int)( pTarget->GetBaseStat()->GetDSRate()*IF_HPBAR_MAX_SIZE );
		break;
	case cHpBar::HPDSFP:
		s_nHPBarSize = (int)( pTarget->GetBaseStat()->GetHPRate()*IF_HPBAR_MAX_SIZE );
		s_nDSBarSize = (int)( pTarget->GetBaseStat()->GetDSRate()*IF_HPBAR_MAX_SIZE );
		s_nFPBarSize = (int)( pTarget->GetBaseStat()->GetFPRate()*IF_HPBAR_MAX_SIZE );
		break;
	default:
		assert_cs( false );
	}

	return true;
}

bool cHpBar::sBAR::Update( NiPoint3 vDestPos )
{
	CsC_AvObject* pTarget = g_pMngCollector->GetObject( s_nTargetUID );
	if( pTarget == NULL )
	{
		return false;
	}

	float nLength = IF_HPBAR_LIMIT_LENGTH;
	if( pTarget->GetLeafRTTI() == RTTI_MONSTER 
		&& nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_DUNGEON )
		nLength = 850.0f;

	if( ( pTarget->GetPos() - vDestPos ).Length() > nLength )
		return false;

	switch( s_eType )
	{
	case cHpBar::HP:
		s_nHPBarSize = (int)( CsMax( 0.0f, CsMin( 1.0f, pTarget->GetBaseStat()->GetHPRate() ) )*IF_HPBAR_MAX_SIZE );
		break;
	case cHpBar::HPDS:
		s_nHPBarSize = (int)( CsMax( 0.0f, CsMin( 1.0f, pTarget->GetBaseStat()->GetHPRate() ) )*IF_HPBAR_MAX_SIZE );
		s_nDSBarSize = (int)( CsMax( 0.0f, CsMin( 1.0f, pTarget->GetBaseStat()->GetDSRate() ) )*IF_HPBAR_MAX_SIZE );
		break;
	case cHpBar::DS:
		s_nDSBarSize = (int)( CsMax( 0.0f, CsMin( 1.0f, pTarget->GetBaseStat()->GetDSRate() ) )*IF_HPBAR_MAX_SIZE );
		break;
	case cHpBar::HPDSFP:
		s_nHPBarSize = (int)( pTarget->GetBaseStat()->GetHPRate()*IF_HPBAR_MAX_SIZE );
		s_nDSBarSize = (int)( pTarget->GetBaseStat()->GetDSRate()*IF_HPBAR_MAX_SIZE );
		s_nFPBarSize = (int)( pTarget->GetBaseStat()->GetFPRate()*IF_HPBAR_MAX_SIZE );
		break;
	default:
		assert_cs( false );
	}	
	return true;
}

cHpBar*	g_pHpBar = NULL;

void cHpBar::GlobalInit()
{
	assert_cs( g_pHpBar == NULL );
	g_pHpBar = NiNew cHpBar;
	g_pHpBar->Init();
}

void cHpBar::GlobalShotDown()
{
	if( g_pHpBar )
	{
		g_pHpBar->Delete();
		SAFE_NIDELETE( g_pHpBar );
	}
	sBAR::GlobalShotDown();
}

void cHpBar::Init()
{

}

void cHpBar::PreResetMap()
{
	Delete();
}

void cHpBar::Delete()
{
	std::list< sBAR* >::iterator it = m_BarList.begin();
	std::list< sBAR* >::iterator itEnd = m_BarList.end();
	for( ; it!=itEnd; ++it )
	{
		sBAR::DeleteInstance( *it );
	}
	m_BarList.clear();

	m_vpSort.Destroy();
	m_vpRender.Destroy();
}

void cHpBar::SetBuddy( eTYPE type, uint nTargetUID )
{
	if( g_pResist->m_Global.s_bViewHpBar == false )
		return;

	CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );
	if( pTarget == NULL )
		return;

	if( ( pTarget->GetPos() - g_pCharMng->GetTamerUser()->GetPos() ).Length() > IF_HPBAR_LIMIT_LENGTH )
		return;

	// 같은 타겟이 있는지 살펴 본다.
	// 같은 타겟이 있다면 리턴
	std::list< sBAR* >::iterator it = m_BarList.begin();
	std::list< sBAR* >::iterator itEnd = m_BarList.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_nTargetUID == nTargetUID )
		{
			return;
		}
	}

	sBAR* pBar = sBAR::NewInstance( type );
	if( pBar->Init( type, nTargetUID ) == true )
	{
		m_BarList.push_back( pBar );
	}
	else
	{
		sBAR::DeleteInstance( pBar );
	}
}

void cHpBar::DeleteBuddy( uint nTargetUID )
{
	std::list< sBAR* >::iterator it = m_BarList.begin();
	std::list< sBAR* >::iterator itEnd = m_BarList.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_nTargetUID == nTargetUID )
		{
			sBAR::DeleteInstance( *it );
			m_BarList.erase( it );
			return;
		}
	}
}

void cHpBar::_Update()
{
	NiPoint3 vDestPos = g_pCharMng->GetTamerUser()->GetPos();
	std::list< sBAR* >::iterator it = m_BarList.begin();
	std::list< sBAR* >::iterator itEnd = m_BarList.end();
	while( it != itEnd )
	{
		if( (*it)->Update( vDestPos ) == false )
		{
			sBAR::DeleteInstance( *it );
			it = m_BarList.erase( it );
		}
		else
		{
			++it;
		}
	}
}

void cHpBar::Render()
{
	_Update();

	if( g_pResist->m_Global.s_bViewHpBar == false )
		return;

	_Cal3D_to_2D();
	_Sort();

	sBAR* pBar;
	int nCount = m_vpRender.Size();
	for( int i=0; i<nCount; ++i )
	{
		pBar = m_vpRender[ i ];

		switch( pBar->s_eType )
		{
		case cHpBar::HP:
			pBar->s_pBarBG->Render( pBar->s_vPos );
			pBar->s_pBarHP->Render( pBar->s_vPos + CsPoint( 1, 1 ), CsPoint( pBar->s_nHPBarSize, 5 ) );
			break;
		case cHpBar::HPDS:
			pBar->s_pBarBG->Render( pBar->s_vPos + CsPoint( 0, -4 ) );
			pBar->s_pBarHP->Render( pBar->s_vPos + CsPoint( 1, -3 ), CsPoint( pBar->s_nHPBarSize, 5 ) );
			pBar->s_pBarBG->Render( pBar->s_vPos + CsPoint( 0, 2 ) );
			pBar->s_pBarDS->Render( pBar->s_vPos + CsPoint( 1, 3 ), CsPoint( pBar->s_nDSBarSize, 5 ) );
			break;
		case cHpBar::DS:
			pBar->s_pBarBG->Render( pBar->s_vPos );
			pBar->s_pBarDS->Render( pBar->s_vPos + CsPoint( 1, 1 ), CsPoint( pBar->s_nDSBarSize, 5 ) );
			break;
		case cHpBar::HPDSFP:
			{
				pBar->s_pBarBG->Render( pBar->s_vPos + CsPoint( 0, -4 ) );
				if( pBar->s_nFPBarSize < pBar->s_nHPBarSize )
				{
					pBar->s_pBarHP->Render( pBar->s_vPos + CsPoint( 1, -3 ), CsPoint( pBar->s_nHPBarSize, 5 ) );
					pBar->s_pBarFP->Render( pBar->s_vPos + CsPoint( 1, -3 ), CsPoint( pBar->s_nFPBarSize, 5 ) );
				}
				else
				{
					pBar->s_pBarFP->Render( pBar->s_vPos + CsPoint( 1, -3 ), CsPoint( pBar->s_nFPBarSize, 5 ) );
					pBar->s_pBarHP2->Render( pBar->s_vPos + CsPoint( 1, -3 ), CsPoint( pBar->s_nHPBarSize, 5 ) );					
				}
				pBar->s_pBarBG->Render( pBar->s_vPos + CsPoint( 0, 2 ) );
				pBar->s_pBarDS->Render( pBar->s_vPos + CsPoint( 1, 3 ), CsPoint( pBar->s_nDSBarSize, 5 ) );				
			}			
			break;
		default:
			assert_cs( false );
		}		
	}
	m_vpSort.ClearElement();
	m_vpRender.ClearElement();
}

void cHpBar::_Cal3D_to_2D()
{
	CsC_AvObject* pTarget;
	NiPoint3 pos, out;
	NiPoint3 vCameraPos = CAMERA_ST.GetWorldTranslate();
	sBAR* pBar;
	std::list< sBAR* >::iterator it = m_BarList.begin();
	std::list< sBAR* >::iterator itEnd = m_BarList.end();
	for( ; it!=itEnd; ++it )
	{
		pBar = (*it);

		pTarget = g_pMngCollector->GetObject( pBar->s_nTargetUID );
		if( pTarget == NULL )
			continue;

		if( pTarget->IsLoad() == false )
			continue;
		if( pTarget->GetCsNode() == NULL )
			continue;
		if( pTarget->IsEnableObject() == false )
			continue;

		pos = pTarget->GetPos();
		float fLength = ( vCameraPos - pos ).Length();
		pos.z += pTarget->GetToolHeight() + 23.0f;
		out = g_pEngine->Cal3D_To_2D( pos );
		out.x -= (int)( IF_HPBAR_BG_X*0.5f );

		// 근, 원 나갔다면
		if( ( out.z < 0.0f )||( out.z > 1.0f ) )
			continue;

		pBar->s_vPos = CsPoint( (int)out.x, (int)out.y );

		// 화면을 나갔다면
		if( ( pBar->s_vPos.x > g_nScreenWidth )||
			( pBar->s_vPos.y > g_nScreenHeight )||
			( ( pBar->s_vPos.x + IF_HPBAR_BG_X ) < 0 )||
			( ( pBar->s_vPos.y + IF_HPBAR_BG_Y ) < 0 ) )
		{
			continue;
		}

		pBar->s_fDepth = out.z;
		pBar->s_bFind = false;

		m_vpSort.PushBack( pBar );
	}
}

void cHpBar::_Sort()
{
	int nSortCount = m_vpSort.Size();
	sBAR* pBar;
	int nSrc = 0;
	while( nSrc < nSortCount )
	{
		pBar = m_vpSort[ nSrc ];
		if( pBar->s_bFind == true )
		{
			++nSrc;
			continue;
		}

		for( int d=nSrc+1; d<nSortCount; ++d )
		{
			if( m_vpSort[ d ]->s_bFind == true )
				continue;

			if( d == nSrc )
				continue;

			if( pBar->s_fDepth < m_vpSort[ d ]->s_fDepth )
			{
				pBar = m_vpSort[ d ];
			}
		}
		pBar->s_bFind = true;
		m_vpRender.PushBack( pBar );
	}
}

