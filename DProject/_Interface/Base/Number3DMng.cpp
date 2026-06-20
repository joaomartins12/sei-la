#include "stdafx.h"
#include "Number3DMng.h"

CSGBMEMPOOL_CPP( CNumber3D );

sCONTROLLER_INFO::sCONTROLLER_INFO():s_pNiNode(NULL),s_pController(NULL)
{

}

sCONTROLLER_INFO::~sCONTROLLER_INFO()
{

}

void sCONTROLLER_INFO::Delete()
{ 
	s_pNiNode = NULL; 
	s_pController = NULL;
}

void sCONTROLLER_INFO::SetNiNode( NiNode* pNiNode )
{
	s_pNiNode = pNiNode;			
	s_pController = nsCSGBFUNC::GetTransformController( s_pNiNode );
	nsCSGBFUNC::EndAnimationTime( s_fAniEndTime, s_pNiNode );
}

//////////////////////////////////////////////////////////////////////////
sNUMBER_INFO::sNUMBER_INFO()
{
	for( int i=0; i<10; ++i )
		s_pNumber[ i ] = NULL;
}

sNUMBER_INFO::~sNUMBER_INFO()
{
	Delete();
}


void sNUMBER_INFO::Delete()
{ 
	for( int i=0; i<10; ++i )
		SAFE_NIDELETE( s_pNumber[ i ] );
}

void sNUMBER_INFO::SetTexture( char const* pFileName )
{
	for( int i=0; i<10; ++i )
	{
		s_pNumber[ i ] = NiNew CBillboard;
		if( i<6 )
			s_pNumber[ i ]->CreateFile( pFileName, 20.f, 30.f, i/6.0f, (i+1)/6.0f, 0, 0.5f);
		else
			s_pNumber[ i ]->CreateFile( pFileName, 20.f, 30.f, (i-6)/6.0f, (i-5)/6.0f, 0.5f, 1.0f );
	}
}

//////////////////////////////////////////////////////////////////////////

CNumber3D::CNumber3D():m_pControllerInfo(NULL),m_pNumberInfo(NULL),m_fAlpha(1.0f),m_vPos(NiPoint3::ZERO)
,m_fAniTime(0.0f)
{
}

CNumber3D::~CNumber3D()
{

}

void CNumber3D::Init( NiPoint3 vPos, float fRot, int nCount, sCONTROLLER_INFO* pControllerInfo, sNUMBER_INFO* pNumberInfo )
{
	m_pControllerInfo	= pControllerInfo;
	m_pNumberInfo		= pNumberInfo;

	m_fAlpha			= 1.5f;
	m_fAniTime			= 0.0f;
	m_vPos				= vPos;
	m_vPos.z			+= 100.0f;

	m_tr.MakeIdentity();
	m_tr.m_Translate = m_vPos;
	m_tr.m_Rotate.MakeZRotation( fRot );

	m_lNumber.clear();
	do 
	{
		m_lNumber.push_front( m_pNumberInfo->s_pNumber[ nCount % 10 ] );
		nCount /= 10;
	} while ( nCount != 0 );
}

bool CNumber3D::Update()
{
	m_fAlpha -= 1.5f * g_fDeltaTime / m_pControllerInfo->s_fAniEndTime;
	if( m_fAlpha < 0.0f ){
		return false;
	}
	if( m_fAniTime > m_pControllerInfo->s_fAniEndTime )
		return false;

	NiInterpolator* pInterPolator = m_pControllerInfo->s_pController->GetInterpolator();
	NiQuatTransform trQuat;	
	bool bSuccess = pInterPolator->Update( m_fAniTime, NULL, trQuat );
	assert_cs( bSuccess == true );

	NiTransform trNode = ( (NiNode*)m_pControllerInfo->s_pController->GetTarget() )->GetLocalTransform();
	trNode.m_Translate = trQuat.GetTranslate();
	trNode.m_fScale = trQuat.GetScale();
	m_tr2 = m_tr*trNode;
	m_tr2 = m_tr2*( (NiNode*)m_pControllerInfo->s_pNiNode->GetAt( 0 ) )->GetAt( 0 )->GetLocalTransform();

	//trResult = trResult*pkGeometry->GetLocalTransform();
	
	m_fAniTime += g_fDeltaTime;	
	return true;
}

void CNumber3D::Render()
{
	float left = - (( m_lNumber.size() - 1 ) * 0.5f * 35.0f);

	std::list<CBillboard*>::iterator it = m_lNumber.begin();
	for( ; it != m_lNumber.end(); ++it )
	{
		(*it)->RenderAbsolute( m_tr2.m_Translate, left, 0.0f, m_fAlpha, m_tr2.m_fScale );
		left += 35.0f;
	}
}


CNumber3DMng* g_pNumber3DMng = NULL;

void CNumber3DMng::GlobalInit()
{
	assert_cs( g_pNumber3DMng == NULL );
	g_pNumber3DMng = NiNew CNumber3DMng;
	g_pNumber3DMng->Init();

	CSGBMEMPOOL_INIT( CNumber3D, 16 );
}

void CNumber3DMng::GlobalShotDown()
{
	if( g_pNumber3DMng )
		g_pNumber3DMng->Destroy();
	SAFE_NIDELETE( g_pNumber3DMng );

	CSGBMEMPOOL_DELETE( CNumber3D );
}

void CNumber3DMng::Destroy()
{
	for( int i=0; i<NUMTYPE::MAX_TYPE; ++i )
	{
		m_Number[ i ].Delete();
	}
	for( int i=0; i<NUMTYPE::MAX_EFFECT; ++i )
		m_Controller[ i ].Delete();

	std::list< CNumber3D* >::iterator it = m_NumberList.begin();
	std::list< CNumber3D* >::iterator itEnd = m_NumberList.end();
	for( ; it!=itEnd; ++it )
	{
		CNumber3D::DeleteInstance(*it);
	}
	m_NumberList.clear();
}

void CNumber3DMng::Init()
{
	CsC_AttackProp::ResistCallBack_DamageNumber( CALLBACK_Damage );

	m_Number[ NUMTYPE::DIGIMON_ATTACK ].SetTexture( "Data/interface/Number0.tga" );
	m_Number[ NUMTYPE::MONSTER_ATTACK ].SetTexture( "Data/interface/Number6.tga" );	
	m_Number[ NUMTYPE::DIGIMON_RECOVER ].SetTexture( "Data/interface/Number3.tga" );	

	{
		NiStream stream;
		if( stream.Load( "Data\\EtcObject\\Damage.nif" ) == false )
		{
			CsMessageBox( MB_OK, _T( "파일이 손상 되었습니다. 언인스톨 후 다시 다운 받아 주세요" ) );
			return;
		}
		m_Controller[ NUMTYPE::ET_NORMAL ].SetNiNode( (NiNode*)stream.GetObjectAt( 0 ) );
	}
	{
		NiStream stream;
		if( stream.Load( "Data\\EtcObject\\DamageCri.nif" ) == false )
		{
			CsMessageBox( MB_OK, _T( "파일이 손상 되었습니다. 언인스톨 후 다시 다운 받아 주세요" ) );
			return;
		}
		m_Controller[ NUMTYPE::ET_CRITICAL ].SetNiNode( (NiNode*)stream.GetObjectAt( 0 ) );	
	}
}


void CNumber3DMng::Add( int nNum, NiPoint3 vPos, float fRot, NUMTYPE::eType etype, NUMTYPE::eEffectType eEffecttype )
{
	if( nNum < 0 ) // recover타입
	{
		etype = NUMTYPE::DIGIMON_RECOVER;	
		nNum *= -1;
	}

	CNumber3D* pNumper = CNumber3D::NewInstance();
	pNumper->Init( vPos, fRot, nNum, &m_Controller[ eEffecttype ], &m_Number[ etype ] );
	m_NumberList.push_back(pNumper);
}

void CNumber3DMng::Update()
{
	std::list< CNumber3D* >::iterator it = m_NumberList.begin();
	std::list< CNumber3D* >::iterator itEnd = m_NumberList.end();
	while( it!=itEnd )
	{
		if( (*it)->Update() == false )
		{
			CNumber3D::DeleteInstance(*it);
			it = m_NumberList.erase( it );
		}
		else
		{
			++it;
		}
	}	
}

void CNumber3DMng::Render()
{
	std::list< CNumber3D* >::iterator it = m_NumberList.begin();
	std::list< CNumber3D* >::iterator itEnd = m_NumberList.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Render();
	}
}


void CNumber3DMng::CALLBACK_Damage( int nEffect, int nType, int nDamage, NiPoint3 vPos, float fDamageRot )
{
	g_pNumber3DMng->Add( nDamage, vPos, fDamageRot, (NUMTYPE::eType)nType, (NUMTYPE::eEffectType)nEffect );
}