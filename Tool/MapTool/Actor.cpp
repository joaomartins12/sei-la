 
#include "stdafx.h"
#include "Actor.h"

cActor g_Actor;

#define CHAR_CAMERA_HEIGHT		120.0f

cActor::cActor()
{
}

void cActor::Delete()
{
	m_Digimon.PreDelete();
	m_Digimon.Delete();

	m_Tamer.PreDelete();
	m_Tamer.Delete();
}

void cActor::Init()
{
	/*
	sCharProp Prop;
	Prop.Init();
	Prop.s_bABVWire = true;
	Prop.s_bCharImg = false;
	Prop.s_bEffectProp = true;
	Prop.s_bEnableAttack = false;
	Prop.s_bFog = true;
	Prop.s_bLight = true;
	Prop.s_bShadow = true;
	Prop.s_eAniType = ANI_KFM;
	Prop.s_eInsertGeomType = CGeometry::Normal;
	Prop.s_ePathEngine = PATH_NORMAL;
	Prop.s_eStatType = STAT_BASE;

	CsC_PartObject::Init( 0, 80001, NiPoint3::ZERO, 0.0f, &Prop );
	UINT nItemCode[ nsPART::MAX_ATTACH_COUNT ] = {0, };
	this->ThreadLoad_Part( nItemCode );*/

	{
		// 로드시 필요한 초기화
		sTCUnit Uint;		
		// 초기화 - Prop 초기화 포함
		Uint.s_Prop.s_dwPropPlag = PR_EFFECT_ENABLE | PR_ANI_ENABLE | PR_PATH_USER;
		Uint.s_Prop.s_dwOptionPlag = _OP_ALL;
		Uint.s_Prop.s_eInsertGeomType = CGeometry::CharOutLine;

		m_Tamer.Init( 1, 80001, NiPoint3::ZERO, 0, &Uint.s_Prop );

		CsC_PartObject::sCHANGE_PART_INFO cp[ nsPART::MAX_TOTAL_COUNT ];
		for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
		{
			cp[ i ].s_nPartIndex = i;
		}		
		m_Tamer.ThreadLoad_Part( cp );
	}

	{
		// 로드시 필요한 초기화
		sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File );

		pUint->s_Prop.s_dwPropPlag = PR_EFFECT_ENABLE | PR_ANI_ENABLE | PR_PATH_USER;
		pUint->s_Prop.s_dwOptionPlag = _OP_ALL;
		pUint->s_Prop.s_eInsertGeomType = CGeometry::CharOutLine;

		// 객체 할당
		m_Digimon.Init( 0, 61011, NiPoint3::ZERO, 0, &pUint->s_Prop );

		// 로드 설정
		pUint->s_pLoadedObject = &m_Digimon;
		strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, "Data\\Digimon\\Shiegrey\\1061011_Shiegrey.kfm" );

		// 쓰레드 로드
		g_pThread->LoadChar( pUint );
	}	

	m_bEnableActor = false;
}

void cActor::ReleaseVisible()
{
	m_bEnableActor = false;

	CAMERA_ST.SetDAAConstant( CAMERA_DIST_ANI_ACCEL_CONSTANT_TOOL );
	CAMERA_ST.SetDeltaHeight( 0.0f );
	CAMERA_ST.SetDistRange( 250, 100000 );
	CAMERA_ST.ReleaseDistRange();
	CAMERA_ST.ReleaseRotationLimit();
	GET_MV_VIEW->_SetCameraAccel();	

	// Camera
	CAMERA_ST.SetAttachActor_ByTool( false );

	m_Digimon.GetCsNode()->m_pNiNode->DetachAllEffects();
	m_Digimon.GetCsNode()->m_pNiNode->UpdateEffects();
	m_Tamer.GetCsNode()->m_pNiNode->DetachAllEffects();
	m_Tamer.GetCsNode()->m_pNiNode->UpdateEffects();

	// Fog
	nsCsGBTerrain::g_pCurRoot->GetFogProperty()->SetFog( false );
}
void cActor::SetMoveRate(float fRate)
{
	m_Tamer.GetBaseStat()->SetMoveSpeed( (float)1500 * fRate);
}
void cActor::ResetVisible( NiPoint3 pos )
{
	pos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( pos.x, pos.y );

	m_Tamer.SetPos( pos );
	m_Tamer.GetBaseStat()->SetMoveSpeed( (float)1500 );
	m_Digimon.SetPos( pos );

	nsCsGBTerrain::g_pCurRoot->SetViewerPos_Tool( pos );

	CAMERA_ST.SetDAAConstant( CAMERA_DIST_ANI_ACCEL_CONSTANT );
	CAMERA_ST.SetDistAccel( 1.5f );
	CAMERA_ST.SetDistRange( 250.0f, CAMERA_ST.GetDistLevel( 5 ) );
	CAMERA_ST.SetRotationLimit( -CsD2R( 40 ), CsD2R( 70 ) );
	CAMERA_ST.SetTranslateAccel( 0.0f );	
	CAMERA_ST.SetDeltaHeight( 120.0f );

	CAMERA_ST.SetTranslate( pos );

	m_bEnableActor = true;

	// Camera
	CAMERA_ST.SetAttachActor_ByTool( true );

	// PathEngine
	m_Tamer.GetProp_Path()->ResetAgent();
	m_Tamer.GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
	nsCsGBTerrain::g_pCurRoot->GetLightMng()->ApplyChar( m_Tamer.GetCsNode()->m_pNiNode );
	m_Tamer.GetCsNode()->m_pNiNode->UpdateEffects();
	m_Tamer.GetCsNode()->ActiveShader();

	m_Digimon.GetProp_Path()->ResetAgent();
	m_Digimon.GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
	nsCsGBTerrain::g_pCurRoot->GetLightMng()->ApplyChar( m_Digimon.GetCsNode()->m_pNiNode );
	m_Digimon.GetCsNode()->m_pNiNode->UpdateEffects();
	m_Digimon.GetCsNode()->ActiveShader();

	// Fog
	nsCsGBTerrain::g_pCurRoot->GetFogProperty()->SetFog( true );
}


//==========================================================================================
//
//		Update
//
//==========================================================================================

void cActor::Update()
{
	if( m_bEnableActor == false )
		return;

	if( m_Tamer.GetProp_Path()->IsMove() )
	{
		m_Tamer.SetAnimation( ANI::MOVE_RUN );		
	}
	m_Tamer.Update( g_fElapsedTime );

	if( m_Digimon.GetProp_Path()->IsMove() )
	{
		m_Digimon.SetAnimation( ANI::MOVE_RUN );		
	}
	m_Digimon.Update( g_fElapsedTime );

	nsCsGBTerrain::g_pCurRoot->SetViewerPos_Tool( m_Tamer.GetPos() );
	CAMERA_ST.SetTranslate( m_Tamer.GetPos() );


	/*NiPoint3 n;
	nsCsGBTerrain::g_pCurRoot->GetHeight_Normal( GetPos().x, GetPos().y, n );
	NiMatrix3 mat;

	NiPoint3 a = n;
	a.x = 0;
	a.Unitize();

	NiPoint3 b = n;
	b.y = 0;
	b.Unitize();
	mat.FromEulerAnglesZXY( 0.0f, acos( a.Dot( NiPoint3( 0, 0, 1 ) ) ), acos( b.Dot( NiPoint3( 0, 0, 1 ) ) ) );

	GetCsNode()->m_pNiNode->SetRotate( mat );
	GetCsNode()->m_pNiNode->Update( 0.0f, false );*/
}
//==========================================================================================
//
//		Render
//
//==========================================================================================

void cActor::Render()
{
	if( m_bEnableActor == false )
		return;
	
	m_Tamer.Render();

	if( GET_MV_MAINTAB_DLG->_GetDlgPlayer()->_IsViewDigimon() )
		m_Digimon.Render();
}

//==========================================================================================
//
//		Move
//
//==========================================================================================

#define BLEND_TIME		0.2f

void cActor::SetMoveDest( NiPoint3 pos )
{
	if( g_pCsPathEngine->PathTest( m_Tamer.GetProp_Path()->GetPath(), pos.x, pos.y ) == false )
		return;

	m_Tamer.GetProp_Path()->SetPath( pos );
	m_Digimon.SetTargetPos();
}

