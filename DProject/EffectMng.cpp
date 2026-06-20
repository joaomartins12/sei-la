
#include "stdafx.h"
#include "EffectMng.h"

CEffectMng*		g_pEffectMng = NULL;

CEffectMng::CEffectMng()
{

}

CEffectMng::~CEffectMng()
{

}

void CEffectMng::GlobalInit()
{
	assert_cs( g_pEffectMng == NULL );
	g_pEffectMng = NiNew CEffectMng;

	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_EFFECT_ENABLE;
	prop.s_dwOptionPlag = 0;
	prop.s_eInsertGeomType = CGeometry::Normal;

	g_pEffectMng->Init( 0, 0, NiPoint3::ZERO, 0.0f, &prop );
	g_pEffectMng->Init_Marble();
}

void CEffectMng::GlobalShotDown()
{
	if( g_pEffectMng == NULL )
		return;

	g_pEffectMng->Destroy_Marble();
	g_pEffectMng->Delete();
	NiDelete g_pEffectMng;
	g_pEffectMng = NULL;
}

void CEffectMng::Reset()
{
	GetProp_Effect()->Reset();
	_ResetMarble();
}

CsC_EffectObject* CEffectMng::AddEffect( char* cEffectPath, NiPoint3 vPos, float fScale )
{
	return GetProp_Effect()->AddEffect( cEffectPath, vPos, fScale );
}

void CEffectMng::DeleteEffect( CsC_AvObject* pDelEffect )
{
	GetProp_Effect()->DeleteEffect( pDelEffect );
}

void CEffectMng::Update( float fDeltaTime )
{
	GetProp_Effect()->PreUpdate( fDeltaTime );
	_Update_Marble( fDeltaTime );
}

void CEffectMng::Render()
{
	GetProp_Effect()->Render();
	_Render_Marble();
}

//================================================================================================================
//
//		포인트 구슬
//
//================================================================================================================

void CEffectMng::Init_Marble()
{
#define MARBLE_EVOLUTION_OBJECT_PATH	"Data\\EtcObject\\Marble_Evo.nif"

	NiStream kStream;
	if( !kStream.Load( MARBLE_EVOLUTION_OBJECT_PATH ) )
	{
		//CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", MARBLE_EVOLUTION_OBJECT_PATH );
		return;
	}

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_MarbleObj[ MT_EVOLUTION ].SetNiObject( pNode, CGeometry::Normal );
}

void CEffectMng::Destroy_Marble()
{
	_ResetMarble();

	sMARBLE_INFO* pMarble = NULL;
	for( int i=0; i<MT_MAX_CNT; ++i )
	{
		while( m_mpMarble[ i ].empty() == false )
		{
			pMarble = m_mpMarble[ i ].front();
			pMarble->s_NodeObj.Delete();
			delete pMarble;
			m_mpMarble[ i ].pop();
		}
	}	

	for( int i=0; i<MT_MAX_CNT; ++i )
		m_MarbleObj[ i ].Delete();
}

void CEffectMng::AddEffect_Marble( eMARBLE_TYPE mt, NiPoint3 vSrc, CsC_AvObject* pOwner, int nCnt )
{
	for( int i=0; i<nCnt; ++i )
	{
		sMARBLE_INFO* pNew = _NewMarble( mt, vSrc, pOwner );
		m_listMarble.push_back( pNew );
	}
}

void CEffectMng::_Update_Marble( float fDeltaTime )
{
	sMARBLE_INFO* pMarble = NULL;
	std::list< sMARBLE_INFO* >::iterator it = m_listMarble.begin();
	std::list< sMARBLE_INFO* >::iterator itEnd = m_listMarble.end();
	while( it!=itEnd )
	{
		pMarble = *it;
		pMarble->s_fAniTime += fDeltaTime;

		switch( pMarble->s_eStep )
		{
		case sMARBLE_INFO::CREATE:
			{
				float fRate = pMarble->s_fAniTime*2.4f;
				pMarble->s_vCurPos = nsCSGBFUNC::CalMullRom(&pMarble->s_vSpline1,
															&pMarble->s_vInitPos,
															&pMarble->s_vTrPos,
															&pMarble->s_vSpline2,
															CsMin( 1.0f, fRate ) );

				if( fRate > 1.0f )
				{
					/*nsCSGBFUNC::StartAnimation( pMarble->s_NodeObj.m_pNiNode, pMarble->s_fAniTime );
					pMarble->s_eStep = sMARBLE_INFO::ON_TR;
					pMarble->s_fOnTrTime += pMarble->s_fAniTime;*/

					pMarble->s_eStep = sMARBLE_INFO::FALLOW;
					pMarble->s_fAniTime = 0.0f;
				}

				pMarble->s_NodeObj.m_pNiNode->SetTranslate( pMarble->s_vCurPos );
			}
			break;
		/*case sMARBLE_INFO::ON_TR:
			{
				if( pMarble->s_fAniTime > pMarble->s_fOnTrTime )
				{
					nsCSGBFUNC::StopAnimation( pMarble->s_NodeObj.m_pNiNode );
					pMarble->s_eStep = sMARBLE_INFO::FALLOW;
					pMarble->s_fAniTime = 0.0f;
				}
			}
			break;*/
		case sMARBLE_INFO::FALLOW:
			{
				bool bEnd = false;
				if( pMarble->s_pOwner->IsLoad() )
				{
					// Dir
					NiPoint3 vDest = pMarble->s_pOwner->GetPos();
					vDest.z += pMarble->s_pOwner->GetToolHeight()*0.5f;
					NiPoint3 vSrc = pMarble->s_vCurPos;
					NiPoint3 vDir = vDest - vSrc;
					float fDist = vDir.Unitize();
					// Velocity
					float fSpeed = pow( pMarble->s_fAniTime+1.0f, 6 )*60.0f*g_fDeltaTime + 50.0f;
					//float fSpeed = pow( pMarble->s_fAniTime+1.0f, 4 )*g_fDeltaTime;
					if( fDist < fSpeed )
					{
						fSpeed = fDist;
						bEnd = true;
					}
					pMarble->s_vCurPos = vSrc + vDir*fSpeed;
					pMarble->s_NodeObj.m_pNiNode->SetTranslate( pMarble->s_vCurPos );
				}
				else
				{
					bEnd = true;
				}

				if( bEnd == true )
				{
					pMarble->s_eStep		= sMARBLE_INFO::DIE;
					pMarble->s_fOnTrTime	= 0.2f;					
				}
			}
			break;
		case sMARBLE_INFO::DIE:
			{
				bool bEnd = false;
				if( pMarble->s_pOwner->IsLoad() )
				{
					if( ( pMarble->s_fOnTrTime -= g_fDeltaTime ) > 0 )
					{
						NiPoint3 vDest = pMarble->s_pOwner->GetPos();
						vDest.z += pMarble->s_pOwner->GetToolHeight()*0.5f;
						pMarble->s_NodeObj.m_pNiNode->SetTranslate( vDest );
					}
					else
					{
						bEnd = true;
					}
				}
				else
				{
					bEnd = true;
				}

				if( bEnd == true )
				{
					it = m_listMarble.erase( it );
					_DelMarble( pMarble );
					continue;
				}
			}
			break;
		default:
			assert_cs( false );
		}		
		pMarble->s_NodeObj.m_pNiNode->Update( pMarble->s_fAniTime );
		++it;
	}
}

void CEffectMng::_Render_Marble()
{
	sMARBLE_INFO* pMarble = NULL;
	std::list< sMARBLE_INFO* >::iterator it = m_listMarble.begin();
	std::list< sMARBLE_INFO* >::iterator itEnd = m_listMarble.end();
	for( ; it!=itEnd; ++it )
	{
		pMarble = *it;
		if( CAMERA_ST.UserCulling( pMarble->s_NodeObj.m_pNiNode ) == true )
		{
			pMarble->s_NodeObj.RenderAbsolute();
		}
	}
}

CEffectMng::sMARBLE_INFO* CEffectMng::_NewMarble( eMARBLE_TYPE mt, NiPoint3 vSrc, CsC_AvObject* pOwner )
{
	sMARBLE_INFO* pMarble = NULL;
	if( m_mpMarble[ mt ].empty() == true )
	{
		pMarble = csnew sMARBLE_INFO;
		pMarble->s_NodeObj.SetNiObject( (NiNode*)m_MarbleObj[ mt ].m_pNiNode->Clone(), CGeometry::Normal );
	}
	else
	{
		pMarble = m_mpMarble[ mt ].front();
		m_mpMarble[ mt ].pop();
	}

	pMarble->s_fAniTime = 0;
	pMarble->s_eType = mt;
	pMarble->s_eStep = sMARBLE_INFO::CREATE;
	pMarble->s_pOwner = pOwner;
	pMarble->s_vInitPos = vSrc;

	// 각
	NiPoint3 v = pOwner->GetPos() - vSrc;
	v.z = 0.0f;
	v.Unitize();
	float fRot = NiACos( NiPoint3( 0.0f, -1.0f, 0.0f ).Dot( v ) );
	if( NiPoint3( 0.0f, -1.0f, 0.0f ).Cross( v ).z > 0 )
		fRot = -fRot;	

	NiMatrix3 mat;
	int nRes = nBase::rand()%120 - 60;
	mat.MakeZRotation( fRot + CsD2R( nRes ) );
	NiPoint3 vDelta = mat * NiPoint3( 0, 1, 0 );

	// 거리
	vDelta *= (float)nBase::rand( 400, 600 );
	// 랜덤 위치
	pMarble->s_vTrPos.x = vSrc.x + vDelta.x;
	pMarble->s_vTrPos.y = vSrc.y + vDelta.y;
	pMarble->s_vTrPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( pMarble->s_vTrPos ) + (float)nBase::rand( 50, 210 );

	// 스플라인 위치
	pMarble->s_vSpline1 = vSrc + NiPoint3( 0, 0, -100 );
	pMarble->s_vSpline2 = pMarble->s_vTrPos + NiPoint3( 0, 0, -(float)nBase::rand( 1000, 3000 ) );

	// 지형 존재 시간
	pMarble->s_fOnTrTime = nBase::rand( 500, 1000 )*0.001f;

	return pMarble;
}

void CEffectMng::_ResetMarble()
{
	std::list< sMARBLE_INFO* >::iterator it = m_listMarble.begin();
	std::list< sMARBLE_INFO* >::iterator itEnd = m_listMarble.end();
	for( ; it!=itEnd; ++it )
	{
		_DelMarble( *it );
	}
	m_listMarble.clear();
}

void CEffectMng::_DelMarble( sMARBLE_INFO* pInfo )
{
	m_mpMarble[ pInfo->s_eType ].push( pInfo );
}