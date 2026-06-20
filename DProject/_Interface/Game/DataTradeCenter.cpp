#include "StdAfx.h"

#include "DataTradeContents.h"
#include "DataTradeCenter.h"

#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"

#define CREATE_SHOW_TIME		1.0f
#define DELETE_SHOW_TIME		1.0f
#define DATA_EXCHANGE_TIME		1.0f

CDataTradeCenter::CDataTradeCenter(void) : m_pDigimonLeft(NULL), m_pDigimonRight(NULL), m_pTradeEffect(NULL), m_pEffectObject(NULL)
{		
	m_vPos = NiPoint3( 0.0f, 0.0f, -100000.0f );

	ResetDevice();

	m_prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
	m_prop.s_dwOptionPlag = OP_ABVWIRE | OP_LIGHT;
	m_prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();
}

CDataTradeCenter::~CDataTradeCenter(void)
{
	End();
	Delete();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void CDataTradeCenter::Start()
{
	m_pDigimonLeft = NULL;
	m_pDigimonRight = NULL;

	g_pMngCollector->SetSceneState( CMngCollector::eSCENE_DATA_TRADE );


	m_fBackupDistMin = CAMERA_ST.GetDistRange_Min();
	m_fBackupDistMax = CAMERA_ST.GetDistRange_Max();

	m_bIsSound = g_pResist->IsSound();
	g_pResist->SetEnableSound( false );
	g_pSoundMgr->Play_Stop_Sound();

	CsC_AvObject::g_bEnableVoice = false;
	
	PlayMusic( "Enchant_Room.mp3" );

	// 이동 중지	
	g_pCharMng->GetTamerUser()->ReleaseArriveTarget();
	g_pCharMng->GetTamerUser()->DeletePath();		
	g_pCharMng->GetDigimonUser( 0 )->DeletePath();	

	m_vPos = NiPoint3( 0.0f, 0.0f, -100000.0f );

	// 닷츠센터 불러오기.
#define Dats_Center		"Data/etcobject/DatsCenter/F_DATS_base.nif"
	NiStream kStream;
	if( !kStream.Load( Dats_Center ) )
	{
		CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", Dats_Center );
		return;
	}

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_DatsCenter.SetNiObject( pNode, CGeometry::Normal );	
	m_DatsCenter.m_pNiNode->SetTranslate( m_vPos );

	NiTimeController::StartAnimations( m_DatsCenter.m_pNiNode, (float)g_fAccumTime );
	//nsCSGBFUNC::Set_Emittance( m_DatsCenter.m_pNiNode, NiColor( 1.0f, 1.0f, 1.0f ) );
	m_DatsCenter.m_pNiNode->UpdateEffects();
	m_DatsCenter.m_pNiNode->Update(0.0f);

	// 좌측 구 선택 효과 불러오기.
#define LEFT_SELECT_EFFECT		"Data/etcobject/DatsCenter/E_Gradation_0001.nif"	
	if( !kStream.Load( LEFT_SELECT_EFFECT ) )
	{
		CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", LEFT_SELECT_EFFECT );
		return;
	}

	pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_PickLeftObj.SetNiObject( pNode, CGeometry::Normal );	
	m_PickLeftObj.m_pNiNode->SetTranslate( m_vPos );

	NiTimeController::StartAnimations( m_PickLeftObj.m_pNiNode, (float)g_fAccumTime );
	nsCSGBFUNC::Set_Emittance( m_PickLeftObj.m_pNiNode, NiColor( 1.0f, 1.0f, 1.0f ) );	


	// 우측 구 선택 효과 불러오기.
#define RIGHT_SELECT_EFFECT		"Data/etcobject/DatsCenter/E_Gradation_0002.nif"
	if( !kStream.Load( RIGHT_SELECT_EFFECT ) )
	{
		CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", RIGHT_SELECT_EFFECT );
		return;
	}

	pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_PickRightObj.SetNiObject( pNode, CGeometry::Normal );	
	m_PickRightObj.m_pNiNode->SetTranslate( m_vPos );

	NiTimeController::StartAnimations( m_PickRightObj.m_pNiNode, (float)g_fAccumTime );
	nsCSGBFUNC::Set_Emittance( m_PickRightObj.m_pNiNode, NiColor( 1.0f, 1.0f, 1.0f ) );	


	// EXIT 선택 효과 불러오기.
#define D_Gradation		"Data/etcobject/DatsCenter/D_Gradation_0001.nif"	
	if( !kStream.Load( D_Gradation ) )
	{
		CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", D_Gradation );
		return;
	}

	pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_PickExitObj.SetNiObject( pNode, CGeometry::Normal );	
	m_PickExitObj.m_pNiNode->SetTranslate( m_vPos );

	NiTimeController::StartAnimations( m_PickExitObj.m_pNiNode, (float)g_fAccumTime );
	nsCSGBFUNC::Set_Emittance( m_PickExitObj.m_pNiNode, NiColor( 1.0f, 1.0f, 1.0f ) );	

	// 카메라 설정			
	CAMERA_ST.SetDeltaHeight( 120.0f );
	CAMERA_ST.SetTranslate( NiPoint3( 0, -1400.0f, -99700.0f ) );			
	CAMERA_ST.SetRotation( 0.27f, -0.02f );
	CAMERA_ST.SetDistRange( 300.0f, 2000.01f );	
	CAMERA_ST.SetDist(2000.0f);
	
	_SetDefaultCamera();
					
	m_nPicked = NONE_PICK;
	m_bOpenUI = false;

	// 제거 해준 경험치 표시
	GAME_EVENT_ST.OnEvent( EVENT_CODE::HIDE_EXP_EVENT_UI, NULL );
}

void CDataTradeCenter::End()
{
	// 페이드 아웃	
//	SAFE_DELETE( g_pFade );
//	g_pFade = NiNew CFade( FADE_IN, 0.4f );

	g_pMngCollector->ReleaseScene();

	sCAMERAINFO ci;	
	ci.s_fDist = 1000.0f;
	ci.s_fInitRoll = NI_PI;
	ci.s_fInitPitch += NI_PI*0.4f;
	ci.s_fFarPlane = 200000.0f;
	CAMERA_ST.Reset( &ci );
	CAMERA_ST.SetDistRange( m_fBackupDistMin, m_fBackupDistMax );
	CAMERA_ST.SetRotationLimit( -CsD2R( 35 ), CsD2R( 70 ) );
	//CAMERA_ST.SetDeltaHeight( 120.0f );
	CAMERA_ST.SetDistAccel( 1.5f );

	g_pResist->SetEnableSound( m_bIsSound );
	CsC_AvObject::g_bEnableVoice = g_pResist->m_Global.s_bEnableVoice;
	g_pCharMng->GetTamerUser()->PlayBGM();	

	// 디지몬 이펙트 변경
	g_pCharMng->GetDigimonUser( 0 )->CheckEnchantEffect();

	_DeleteTradeEffect();
	_DeleteDigimonLeft();
	_DeleteDigimonRight();

	_DeleteEffectDigimonLeft();
	_DeleteEffectDigimonRight();

	// 제거 해준 경험치 표시
	GAME_EVENT_ST.OnEvent( EVENT_CODE::SHOW_EXP_EVENT_UI, NULL );

	PlayMusic( _LoadMusic() );

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SMALLMAP ) )	
	{
		if( g_pTerrainMng )
		{
			DWORD nMap = g_pTerrainMng->GetBackupMapID();
			g_pGameIF->GetSmallMap()->LoadSmallMap2( nMap );	// 데이터 교환 센터에서 나올때 스몰맵이 Active상태이면 재로드
		}
		else 
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SMALLMAP );
	}
}

void CDataTradeCenter::Delete()
{	
	m_PickLeftObj.Delete();
	m_PickRightObj.Delete();
	m_PickExitObj.Delete();

	m_DatsCenter.Delete();
} 

void CDataTradeCenter::ResetDevice()
{	
	m_bLeftDelete = false;
	m_bRightDelete = false;

	m_fLeftDeleteTime = 0.0f;
	m_fRightDeleteTime = 0.0f;

	_DeleteTradeEffect();
	_DeleteDigimonLeft();
	_DeleteDigimonRight();


	_DeleteEffectDigimonLeft();
	_DeleteEffectDigimonRight();

	_SetDefaultCamera();
}

void CDataTradeCenter::Init()
{
	Start();
}


void CDataTradeCenter::_SetDefaultCamera()
{
	m_fCameraRot = 0.27f;
	m_fCameraPitch = 0.02f;
	m_fCameraDist = 2000.0f;
	CAMERA_ST.SetCameraDist( m_fCameraDist );
}

void CDataTradeCenter::_SetTradeCamera()
{
	m_fCameraRot = 0.27f;
	m_fCameraPitch = 0.02f;
	m_fCameraDist = 300.0f;	
	CAMERA_ST.SetCameraDist( m_fCameraDist );
}
void CDataTradeCenter::_UpdateDigimonViewCamera(float dTime)
{
	if( CAMERA_ST.GetDist() > m_fCameraDist )
	{
		float fRoll = CAMERA_ST.GetCurRoll() - ( m_fCameraRot * dTime * 1.8f );
		if( fRoll <= 0.00f )
			fRoll = 0.00f;

		float fPitch = CAMERA_ST.GetCurPitch() - ( m_fCameraPitch * dTime * 1.8f );
		if( fPitch <= 0.00f )
			fPitch = 0.00f;

		CAMERA_ST.SetRotation( fRoll, fPitch );
	}
}
void CDataTradeCenter::_UpdateObjectDelete(float dTime)
{
	if(m_bLeftDelete)
	{
		m_fLeftDeleteTime -= dTime;
		if(m_fLeftDeleteTime <= 0.0f)
		{
			_DeleteDigimonLeft();
		}
	}

	if(m_bRightDelete)
	{
		m_fRightDeleteTime -= dTime;
		if(m_fRightDeleteTime <= 0.0f)
		{
			_DeleteDigimonRight();
		}
	}
}


int	CDataTradeCenter::_EndTradeEffect()
{
	if(m_pTradeEffect)
	{
		int nCount = m_pTradeEffect->GetProp_Effect()->GetEffectCount();
		return nCount;
	}
	return NONE_TRADE_EFFECT;
}

void CDataTradeCenter::_CreateTradeEffect()
{
	_DeleteTradeEffect();

	NiPoint3 vCharPos;
	NiAVObject* pBox1Node = m_DatsCenter.m_pNiNode->GetObjectByName( "Digimon_Box01" );
	float fBoxHeight = 330.0f;
	if( pBox1Node )
		fBoxHeight = pBox1Node->GetWorldBound().GetRadius() * 2.0f;

	vCharPos = NiPoint3( m_vPos + pBox1Node->GetTranslate() ); 	
	vCharPos.z -= 600.0f;

	NiPoint3 vCharPos2;
	NiAVObject* pBox1Node2 = m_DatsCenter.m_pNiNode->GetObjectByName( "Digimon_Box02" );
	vCharPos2 = NiPoint3( m_vPos + pBox1Node2->GetTranslate() ) - vCharPos;

	vCharPos.x = vCharPos.x + vCharPos2.x/2;
	vCharPos.y = vCharPos.y-900.0f;

	m_pTradeEffect = g_pCharMng->AddTempDigimon( &m_prop, CODE_AGUMON, NULL, vCharPos, 0.0f, NULL );

	float digimonScale = fBoxHeight / m_pTradeEffect->GetToolHeight();

	m_pTradeEffect->SetScale( digimonScale );
	m_pTradeEffect->SetAnimation( ANI::IDLE_NORMAL);
	switch( GetSystem()->GetState() ) 
	{		
	case DataTradeContents::eTRADE_SCALE:	// 데이터 교환 크기
		m_pEffectObject = m_pTradeEffect->GetProp_Effect()->AddEffect( "System\\Data_Exchange_1.nif", 1.0f, nsEFFECT::POS_CHARPOS );
		break;

	case DataTradeContents::eTRADE_UPGRADE:	// 데이터 교환 강화
		m_pEffectObject = m_pTradeEffect->GetProp_Effect()->AddEffect( "System\\Data_Exchange_2.nif", 1.0f, nsEFFECT::POS_CHARPOS );
		break;

	case DataTradeContents::eTRADE_EVOLUTION:// 데이터 교환 진화
		m_pEffectObject = m_pTradeEffect->GetProp_Effect()->AddEffect( "System\\Data_Exchange_3.nif", 1.0f, nsEFFECT::POS_CHARPOS );
		break;
	}	
	m_pTradeEffect->GetProp_Effect()->GetEffect(0)->SetAnimation( ANI::IDLE_NORMAL, DATA_EXCHANGE_TIME );

	PlaySystemSound( "game\\data_exchange_01.wav" );
}

void CDataTradeCenter::_CreateDigimonLeft(DWORD dwID)
{
	_DeleteDigimonLeft();

	NiPoint3 vCharPos;
	NiAVObject* pBox1Node = m_DatsCenter.m_pNiNode->GetObjectByName( "Digimon_Box01" );
	float fBoxHeight = 330.0f;
	if( pBox1Node )
		fBoxHeight = pBox1Node->GetWorldBound().GetRadius() * 2.0f;

	vCharPos = NiPoint3( m_vPos + pBox1Node->GetTranslate() ); 	
	vCharPos.x -= 20.0f;
	vCharPos.z -= 150.0f;

	m_pDigimonLeft = g_pCharMng->AddTempDigimon( &m_prop, dwID, NULL, vCharPos, 0.0f, NULL );

	float digimonScale = fBoxHeight / m_pDigimonLeft->CsC_AvObject::GetToolHeight();

	m_pDigimonLeft->SetScale( digimonScale );	
	m_pDigimonLeft->SetAnimation( ANI::IDLE_NORMAL);

	m_pDigimonLeft->GetProp_Effect()->AddEffect( "System\\Tactics_success.nif", 1.0f, nsEFFECT::POS_CHARPOS );
	m_pDigimonLeft->GetProp_Effect()->GetEffect(0)->SetAnimation( ANI::IDLE_NORMAL, CREATE_SHOW_TIME );

	PlaySystemSound( "game\\Enchant_Success.mp3" );
}
void CDataTradeCenter::_CreateDigimonRight(DWORD dwID)
{
	_DeleteDigimonRight();

	NiPoint3 vCharPos;
	NiAVObject* pBox1Node = m_DatsCenter.m_pNiNode->GetObjectByName( "Digimon_Box02" );
	float fBoxHeight = 330.0f;
	if( pBox1Node )
		fBoxHeight = pBox1Node->GetWorldBound().GetRadius() * 2.0f;

	vCharPos = NiPoint3( m_vPos + pBox1Node->GetTranslate() );
	vCharPos.x += 20.0f;
	vCharPos.z -= 150.0f;

	m_pDigimonRight = g_pCharMng->AddTempDigimon( &m_prop, dwID, NULL, vCharPos, 0.0f, NULL );

	float digimonScale = fBoxHeight / m_pDigimonRight->CsC_AvObject::GetToolHeight();

	m_pDigimonRight->SetScale( digimonScale );
	m_pDigimonRight->SetAnimation( ANI::IDLE_NORMAL);

	m_pDigimonRight->GetProp_Effect()->AddEffect( "System\\Tactics_success.nif", 1.0f, nsEFFECT::POS_CHARPOS );
	m_pDigimonRight->GetProp_Effect()->GetEffect(0)->SetAnimation( ANI::IDLE_NORMAL, CREATE_SHOW_TIME );
	PlaySystemSound( "game\\Enchant_Success.mp3" );
}

void CDataTradeCenter::_DeleteTradeEffect()
{
	if(m_pTradeEffect)
	{
		m_pTradeEffect->GetProp_Effect()->DeleteEventEffect();
		g_pCharMng->DeleteTempChar( m_pTradeEffect );
		PlaySystemSound( "game\\data_exchange_02.wav" );
	}
	m_pTradeEffect = NULL;
	m_pEffectObject = NULL;
}
void CDataTradeCenter::_DeleteDigimonLeft()
{
	if(m_pDigimonLeft)
	{
		m_bLeftDelete = false;
		m_fLeftDeleteTime = 0.0f;
		m_pDigimonLeft->GetProp_Effect()->DeleteEventEffect();
		g_pCharMng->DeleteTempChar( m_pDigimonLeft );
	}
	m_pDigimonLeft = NULL;
}
void CDataTradeCenter::_DeleteDigimonRight()
{
	if(m_pDigimonRight)
	{
		m_bRightDelete = false;
		m_fRightDeleteTime = 0.0f;

		m_pDigimonRight->GetProp_Effect()->DeleteEventEffect();
		g_pCharMng->DeleteTempChar( m_pDigimonRight );
	}
	m_pDigimonRight = NULL;
}

void CDataTradeCenter::_DeleteEffectDigimonLeft()
{
	if(m_pDigimonLeft)
	{
		//PlaySystemSound( "game\\Enchant_Fail.mp3" );
		m_pEffectObject = NULL;
		m_bLeftDelete = true;
		m_fLeftDeleteTime = DELETE_SHOW_TIME;
		m_pDigimonLeft->GetProp_Effect()->AddEffect( "System\\Tactics_enchant_fail.nif", 1.0f, nsEFFECT::POS_CHARPOS );
		m_pDigimonLeft->GetProp_Effect()->GetEffect(0)->SetAnimation( ANI::IDLE_NORMAL, DELETE_SHOW_TIME );
	}
}
void CDataTradeCenter::_DeleteEffectDigimonRight()
{
	if(m_pDigimonRight)
	{
		//PlaySystemSound( "game\\Enchant_Fail.mp3" );
		m_pEffectObject = NULL;
		m_bRightDelete = true;
		m_fRightDeleteTime = DELETE_SHOW_TIME;
		m_pDigimonRight->SetAnimation( ANI::IDLE_NORMAL, DELETE_SHOW_TIME);
		m_pDigimonRight->GetProp_Effect()->AddEffect( "System\\Tactics_enchant_fail.nif", 1.0f, nsEFFECT::POS_CHARPOS );
		m_pDigimonRight->GetProp_Effect()->GetEffect(0)->SetAnimation( ANI::IDLE_NORMAL, DELETE_SHOW_TIME );
	}
}

char const* CDataTradeCenter::_LoadMusic()
{
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	if( nsCsMapTable::g_pMapListMng->IsList( dwMapID ) == false )
		return NULL;

	CsMapRegion::sINFO* pInfo = NULL;
	float fLength;
	NiPoint2 ptSrc = NiPoint2( g_pCharMng->GetTamerUser()->GetPos().x, g_pCharMng->GetTamerUser()->GetPos().y );
	CsMapRegionGroup* pGroup = nsCsMapTable::g_pMapRegionMng->GetRegionGroup( dwMapID );
	CsMapRegion::LIST* pList = pGroup->GetRegionList();
	CsMapRegion::LIST_IT it = pList->begin();
	for( ; it!=pList->end(); ++it )
	{
		pInfo = (*it)->GetInfo();
		fLength = ( ptSrc - NiPoint2( (float)pInfo->s_nCenterX, (float)pInfo->s_nCenterY ) ).Length();
		if( fLength < pInfo->s_nRadius )
		{
			if( pInfo->s_cBGSound[ 0 ] != NULL )
				return pInfo->s_cBGSound;
			break;
		}
	}

	CsMapList::sINFO* pListInfo = nsCsMapTable::g_pMapListMng->GetList( dwMapID )->GetInfo();	
	if( pListInfo->s_cBGSound[ 0 ] != NULL )
		return pListInfo->s_cBGSound.c_str();
	return NULL;
}

void CDataTradeCenter::_PickExit()
{
	// 데이터 교환 이펙트 실행중이면 키 먹지 않음.
	if(GetSystem()->IsTradeDataEffect())
		return;
	POINT pt;
	GetCursorPos( &pt );
	ScreenToClient( GAMEAPP_ST.GetHWnd(), &pt );	
	CsGBPick pick( false, NiPick::NO_SORT );
	float fLenth = FLT_MAX;
	NiPoint3 origin, dir;
	CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );
	
	if( (pick.Pick( m_DatsCenter.m_pNiNode->GetObjectByName( "exit" ), origin, dir, false ) == true) )
	{
		m_nPicked = EXIT_PICK;
		m_PickExitObj.m_pNiNode->Update( (float)g_fAccumTime );
		if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_UP )
		{
			m_nPicked = NONE_PICK;
			GetSystem()->CloseSelect(true);
			return;
		}	
	}
	else
	{
		m_nPicked = NONE_PICK;
	}

}
void CDataTradeCenter::_PickUpdate()
{
	// 데이터 교환 이펙트 실행중이면 키 먹지 않음.
	if(GetSystem()->IsTradeDataEffect())
		return;
	//
	_IsPickLeftSlot();
	_IsPickRightSlot();

	POINT pt;
	GetCursorPos( &pt );
	ScreenToClient( GAMEAPP_ST.GetHWnd(), &pt );	
	CsGBPick pick( false, NiPick::NO_SORT );
	float fLenth = FLT_MAX;
	NiPoint3 origin, dir;
	CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );
	if( pick.Pick( m_DatsCenter.m_pNiNode->GetObjectByName( "Digimon_Box03" ), origin, dir, false ) == true )	
	{
		m_nPicked = LEFT_PICK;
		m_PickLeftObj.m_pNiNode->Update( (float)g_fAccumTime );
	}
	else if( pick.Pick( m_DatsCenter.m_pNiNode->GetObjectByName( "Digimon_Box04" ), origin, dir, false ) == true )	
	{
		m_nPicked = RIGHT_PICK;
		m_PickRightObj.m_pNiNode->Update( (float)g_fAccumTime );
	}
	else if( pick.Pick( m_DatsCenter.m_pNiNode->GetObjectByName( "exit" ), origin, dir, false ) == true )	
	{
		m_nPicked = EXIT_PICK;
		m_PickExitObj.m_pNiNode->Update( (float)g_fAccumTime );
		if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_UP )
		{
			m_nPicked = NONE_PICK;
			GetSystem()->CloseSelect(true);
			return;
		}	
	}
	else
	{
		m_nPicked = NONE_PICK;
	}
}
bool CDataTradeCenter::_IsPickLeftSlot()
{
	if(m_nPicked == LEFT_PICK)
		GetSystem()->SetPickLeftSlot(true);
	else
		GetSystem()->SetPickLeftSlot(false);
	return true;
}

bool CDataTradeCenter::_IsPickRightSlot()
{
	if(m_nPicked == RIGHT_PICK)
		GetSystem()->SetPickRightSlot(true);
	else
		GetSystem()->SetPickRightSlot(false);
	return true;
}

bool CDataTradeCenter::_IsModelDeleteTime()
{
	if(m_pEffectObject)
	{
		float anitime = m_pTradeEffect->GetProp_Effect()->GetEffect(0)->GetAniTime();
		float Endtime = 0.0f;

		if(m_pEffectObject)
			Endtime = m_pEffectObject->GetEndTime() - DELETE_SHOW_TIME;

		if(anitime >= Endtime)
			return true;
	}
	return false;
}
void CDataTradeCenter::Update( float fDeltaTime, cGlobalInput::eMOUSE_INPUT mi )
{	
	switch( GetSystem()->GetState() ) 
	{		
	case DataTradeContents::eTRADE_SELECT:	// 데이터 교환 선택
		if( mi == cGlobalInput::MOUSEINPUT_ENABLE )
			_PickExit();
		else
			m_nPicked = NONE_PICK;
		if( CAMERA_ST.GetDist() < m_fCameraDist )
		{
			float fRot = CAMERA_ST.GetCurRoll() + ( m_fCameraRot * fDeltaTime * 1.8f );
			if( fRot > 0.27f )
				fRot = 0.27f;

			float fPitch = CAMERA_ST.GetCurPitch() + ( m_fCameraPitch * fDeltaTime * 1.8f );
			if( fPitch > -0.02f )
				fPitch = -0.02f;

			CAMERA_ST.SetRotation( fRot, fPitch );
		}	
		break;
	case DataTradeContents::eTRADE_SCALE:	// 데이터 교환 크기
		_SetTradeCamera(); 
		_PickUpdate();
		_UpdateObjectDelete(fDeltaTime);
		_UpdateDigimonViewCamera(fDeltaTime);
		break;

	case DataTradeContents::eTRADE_UPGRADE:	// 데이터 교환 강화
		_SetTradeCamera(); 
		_PickUpdate();
		_UpdateObjectDelete(fDeltaTime);
		_UpdateDigimonViewCamera(fDeltaTime);
		break;

	case DataTradeContents::eTRADE_EVOLUTION:	// 데이터 교환 진화
		_SetTradeCamera(); 
		_PickUpdate();
		_UpdateObjectDelete(fDeltaTime);
		_UpdateDigimonViewCamera(fDeltaTime);
		break;
	}

	CAMERA_ST._UpdateCamera();
	CAMERA_ST.SetDefaultFarFrustumPlane();	

	m_DatsCenter.m_pNiNode->Update( (float)g_fAccumTime );

	if(m_pDigimonLeft)
		m_pDigimonLeft->CsC_AvObject::Update( g_fDeltaTime );

	if(m_pDigimonRight)
		m_pDigimonRight->CsC_AvObject::Update( g_fDeltaTime );

	if(m_pTradeEffect)
	{
		m_pTradeEffect->CsC_AvObject::Update( g_fDeltaTime );
		int effect = _EndTradeEffect();
		if(effect == END_TRADE_EFFECT)
		{
			GetSystem()->EndTradeData();
			_DeleteTradeEffect();	// 종료된 데이터 교환 이펙트 삭제
		}
	}
}

void CDataTradeCenter::Render()
{
	switch( m_nPicked )
	{
	case LEFT_PICK:
		m_PickLeftObj.Render();
		break;
	case RIGHT_PICK:
		m_PickRightObj.Render();
		break;	
	case EXIT_PICK:
		m_PickExitObj.Render();
		break;	
	}	
	
	//
	if( m_pDigimonLeft && m_pDigimonLeft->GetCsNode())
		m_pDigimonLeft->CsC_AvObject::Render();

	if( m_pDigimonRight && m_pDigimonRight->GetCsNode())
		m_pDigimonRight->CsC_AvObject::Render();

	if( m_pTradeEffect )
		m_pTradeEffect->CsC_AvObject::RenderEtc();

	m_DatsCenter.Render();		
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CDataTradeCenter::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eCenter_Reset, this );
	GetSystem()->Register( SystemType::eCenter_CreateEffect, this );
	GetSystem()->Register( SystemType::eCenter_CreateDigimonLeft, this );
	GetSystem()->Register( SystemType::eCenter_CreateDigimonRight, this );
	GetSystem()->Register( SystemType::eCenter_DeletDigimonLeft, this );
	GetSystem()->Register( SystemType::eCenter_DeletDigimonRight, this );

	return true;
}

void CDataTradeCenter::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	kStream.SetRp(0);
	int ID = 0;
	switch(iNotifiedEvt)
	{
	case SystemType::eCenter_Reset:
		ResetDevice();
		break;

	case SystemType::eCenter_CreateEffect:
		_CreateTradeEffect();
		break;

	case SystemType::eCenter_CreateDigimonLeft:
		kStream>>ID;
		_CreateDigimonLeft(ID);
		break;

	case SystemType::eCenter_CreateDigimonRight:
		kStream>>ID;
		_CreateDigimonRight(ID);
		break;

	case SystemType::eCenter_DeletDigimonLeft:
		_DeleteEffectDigimonLeft();
		break;
	case SystemType::eCenter_DeletDigimonRight:
		_DeleteEffectDigimonRight();
		break;
	}
}