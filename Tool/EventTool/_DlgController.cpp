// _DlgController.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EventTool.h"
#include "_DlgController.h"



#define TypeANI			L"ANI"
#define TypeMOVE		L"MOVE"
#define TypeTALK		L"TALK"
#define TypeSOUND		L"Sound"
#define	TypeMUSIC		L"Music"
#define TypeEFFECT		L"EFFECT"


// _DlgController

IMPLEMENT_DYNCREATE(_DlgController, CFormView)

_DlgController::_DlgController()
	: CFormView(_DlgController::IDD)
{

}

_DlgController::~_DlgController()
{
}

void _DlgController::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ANIMATION, m_List_Animation);
	DDX_Control(pDX, IDC_SLIDER_EVENTTIME, m_SldEventtime);
	DDX_Control(pDX, IDC_CHECK_PLAY, m_Check_Play);
	DDX_Control(pDX, IDC_EDIT_TIME, m_EditTime);	
	DDX_Control(pDX, IDC_LIST_EVENTPLAY, m_ListEventPlay);
	DDX_Control(pDX, IDC_EDIT_STARTTIME, m_EditStartTime);	
	DDX_Control(pDX, IDC_EDIT_EVENTNUM, m_EditEventNum);
	DDX_Control(pDX, IDC_EDIT_EVENTTITLE, m_EditEventTitle);
	DDX_Control(pDX, IDC_CHECK_SETAREA, m_Check_AreaSet);
	DDX_Control(pDX, IDC_SPIN_AREARAD, m_SpinAreaRad);
	DDX_Control(pDX, IDC_EDIT_AREARAD, m_EditAreaRad);
	DDX_Control(pDX, IDC_BUTTON_SETAREA, m_btArea);
	DDX_Control(pDX, IDC_LIST_PLAYER, m_List_Player);
	DDX_Control(pDX, IDC_EDIT_EVENTTIME, m_EditSceneNum);
	DDX_Control(pDX, IDC_CHECK_LISTPLAY, m_CheckChrCamera);	
	DDX_Control(pDX, IDC_EDIT_EVENTCOUNT, m_EditEventCount);
	DDX_Control(pDX, IDC_CHECK_USECON, m_CheckOnlyArea);
	DDX_Control(pDX, IDC_CHECK_USEMANY, m_CheckConLoop);
	DDX_Control(pDX, IDC_EDIT_EVENTCHRNAME, m_EditCharName);
	DDX_Control(pDX, IDC_EDIT_EVENTNUM2, m_EditEventNumUse);
	DDX_Control(pDX, IDC_EDIT_SCALE, m_EditScale);
	DDX_Control(pDX, IDC_SPIN_SCALE, m_SpinScale);
	DDX_Control(pDX, IDC_LIST_SCENE, m_List_Scene);
	DDX_Control(pDX, IDC_EDIT_POS, m_Edit_Pos);
	DDX_Control(pDX, IDC_CHECK_EVENTCAMERA, m_Check_EventCamera);
	DDX_Control(pDX, IDC_EDIT_POS2, m_Edit_TargetPos);
	DDX_Control(pDX, IDC_CHECK_DIRECTPLAY, m_CheckPlay_Direct);
	DDX_Control(pDX, IDC_EDIT_ZDELTA, m_Edit_ZOffset);
	DDX_Control(pDX, IDC_SPIN_ZDELTA, m_Spin_ZOffset);
}

BEGIN_MESSAGE_MAP(_DlgController, CFormView)
	ON_BN_CLICKED(IDC_MAPLOAD, &_DlgController::OnBnClickedMapload)
	ON_BN_CLICKED(IDC_INSERT_EVENT, &_DlgController::OnBnClickedInsertEvent)
	ON_BN_CLICKED(IDC_INSERT_EVENT2, &_DlgController::OnBnClickedInsertEvent2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ANIMATION, &_DlgController::OnLvnItemchangedListAnimation)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ANIMATION, &_DlgController::OnNMDblclkListAnimation)
	ON_BN_CLICKED(IDC_SETTIME, &_DlgController::OnBnClickedSettime)
	ON_BN_CLICKED(IDC_LISTDELETE, &_DlgController::OnBnClickedListdelete)
	ON_BN_CLICKED(IDC_RESET, &_DlgController::OnBnClickedReset)
	ON_BN_CLICKED(IDC_SETEVENT, &_DlgController::OnBnClickedSetevent)
	ON_BN_CLICKED(IDC_CHECK_PLAY, &_DlgController::OnBnClickedCheckPlay)	
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_EVENTTIME, &_DlgController::OnNMCustomdrawSliderEventtime)
	ON_BN_CLICKED(IDC_MOVEEVENT, &_DlgController::OnBnClickedMoveevent)
	ON_BN_CLICKED(IDC_EVENTTALK, &_DlgController::OnBnClickedEventtalk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_EVENTPLAY, &_DlgController::OnLvnItemchangedListEventplay)
	ON_BN_CLICKED(IDC_BUTTON_SETAREA, &_DlgController::OnBnClickedButtonSetarea)	
	ON_BN_CLICKED(IDC_EVENTSAVE, &_DlgController::OnBnClickedEventsave)
	ON_BN_CLICKED(IDC_EVENTLOAD, &_DlgController::OnBnClickedEventload)
	ON_BN_CLICKED(IDC_CHECK_LISTPLAY, &_DlgController::OnBnClickedCheckListplay)
	ON_BN_CLICKED(IDC_EVENTPLAYER, &_DlgController::OnBnClickedEventplayer)
	ON_BN_CLICKED(IDC_EVENTTALKLOAD, &_DlgController::OnBnClickedEventtalkload)	
	ON_BN_CLICKED(IDC_EVENTPASTE, &_DlgController::OnBnClickedEventpaste)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_EVENTPLAY, &_DlgController::OnNMRclickListEventplay)
	ON_EN_CHANGE(IDC_EDIT_EVENTSCENENUM, &_DlgController::OnEnChangeEditEventscenenum)
	ON_BN_CLICKED(IDC_CHECK_USECON, &_DlgController::OnBnClickedCheckUsecon)
	ON_EN_CHANGE(IDC_EDIT_EVENTNUM, &_DlgController::OnEnChangeEditEventnum)
	ON_BN_CLICKED(IDC_EVENTSAVETEMP, &_DlgController::OnBnClickedEventsavetemp)
	ON_BN_CLICKED(IDC_EVENTSOUND, &_DlgController::OnBnClickedEventsound)
	ON_BN_CLICKED(IDC_EVENTEFFECT, &_DlgController::OnBnClickedEventeffect)
	ON_BN_CLICKED(IDC_CHECK_FREEMOVE, &_DlgController::OnBnClickedCheckFreemove)
	ON_BN_CLICKED(IDC_GETPOS, &_DlgController::OnBnClickedGetpos)
	ON_BN_CLICKED(IDC_SCENE_DEL, &_DlgController::OnBnClickedSceneDel)	
	ON_BN_CLICKED(IDC_CHECK_DIRECTPLAY, &_DlgController::OnBnClickedCheckDirectplay)
	ON_LBN_SELCHANGE(IDC_LIST_PLAYER, &_DlgController::OnLbnSelchangeListPlayer)
	ON_EN_CHANGE(IDC_EDIT_ZDELTA, &_DlgController::OnEnChangeEditZdelta)
	ON_BN_CLICKED(IDC_SETCAMERA_BUTTON, &_DlgController::OnBnClickedSetcameraButton)
	ON_EN_CHANGE(IDC_EDIT_SCALE, &_DlgController::OnEnChangeEditScale)
END_MESSAGE_MAP()


// _DlgController 진단입니다.

#ifdef _DEBUG
void _DlgController::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void _DlgController::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void _DlgController::_Delete()
{	
	if( m_pTarget != NULL && m_nType == CEventData::EO_New )
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = m_pTarget;	

		g_pThread->DeleteChar( pUnit );
		m_pTarget = NULL;
	}
}

void _DlgController::_ResetAll()
{
	_SetPlayListReset();
	_Delete();
	m_List_Animation.DeleteAllItems();
	m_EditEventTitle.SetWindowText( NULL );
	m_EditCharName.SetWindowText( NULL );	
	_SetEventTime( 0.0f, 0.0f );	
}

void _DlgController::_Init()
{
	DWORD dwExStyle = m_List_Animation.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_List_Animation.SetExtendedStyle( dwExStyle );
	m_List_Animation.InsertColumn( 0, L"Name", LVCFMT_RIGHT, 115 );
	m_List_Animation.InsertColumn( 1, L"Evt", LVCFMT_CENTER, 35 );
	m_List_Animation.InsertColumn( 2, L"Loop", LVCFMT_CENTER, 38 );
	m_List_Animation.InsertColumn( 3, L"Time", LVCFMT_CENTER, 45 );
	m_List_Animation.InsertColumn( 4, L"SeqID", LVCFMT_LEFT, 55 );

	m_ListEventPlay.SetExtendedStyle( dwExStyle );
	m_ListEventPlay.InsertColumn( 0, L"시간", LVCFMT_CENTER, 45 );
	m_ListEventPlay.InsertColumn( 1, L"타입", LVCFMT_CENTER, 40 );
	m_ListEventPlay.InsertColumn( 2, L"내용", LVCFMT_CENTER, 205 );	
	m_ListEventPlay.InsertColumn( 3, L"완료시간", LVCFMT_CENTER, 45 );	

	m_EditEventNum.SetLimitText( 3 );
	m_EditEventTitle.SetLimitText( 32 );	

	m_fTime = 0.0f;
	m_fStartTime = 0.0f;
	m_fEndTime = 0.0f;
	m_nSceneNum = 0;
	m_pTarget = NULL;
	m_bNewObjectLoad = false;
	m_bMoveEvent = false;
	m_bArea = false;
	m_bSetArea = false;
	m_bCharCamera = false;
	m_bMouseMove = false;
	m_bEffectPos = false;
	m_nType = CEventData::EO_New;		

	m_SpinScale.SetCsBuddy( &m_EditScale, 0.01f, 3 );
	nsCsMfcFunc::Float2Control( &m_EditScale, 1.0f );

	m_Spin_ZOffset.SetCsBuddy( &m_Edit_ZOffset, 0.01f, 3 );
	nsCsMfcFunc::Float2Control( &m_Edit_ZOffset, 0.0f );

	m_Check_AreaSet.SetCheck( BST_CHECKED );	
	m_btArea.SetWindowText( L"영역설정" );	
}

void _DlgController::_Update()
{
	if( _IsPlay() )
	{
		if( m_fTime + g_fDeltaTime < m_fEndTime )
		{
			m_fTime += g_fDeltaTime;			
			int nPos = (int)( m_fTime*1000.0f );
			m_SldEventtime.SetPos( nPos );			
		}	
		/*else
		{			
			m_fTime = 0.0f;
			m_SldEventtime.SetPos( 0 );	
			_SetPlay( false );
			g_EventScene.EndEvent();
		}		*/
	}
	else
	{
		m_fTime = m_SldEventtime.GetPos()/ 1000.0f;				
	}

	TCHAR sz[ 128 ];		
	_stprintf_s( sz, 128, L"%.3f / %.3f", m_fTime,  m_fEndTime );
	m_EditTime.SetWindowText( sz );
	
	_stprintf_s( sz, 128, L"Event수 %d", g_pEventSceneMng->EventProgressCount() );
	m_EditEventCount.SetWindowText( sz );

	if( !_IsChrCamera() )
	{
		if( m_pTarget != NULL )
		{
			switch( m_pTarget->GetLeafRTTI() )
			{
			case RTTI_EVENTOBJECT:
				{
					((CsC_EventObject*)m_pTarget)->Update( g_fDeltaTime );
				}
				break;
			case RTTI_EVENTTAMEROBJECT:
				{
					((CsC_EventTamerObject*)m_pTarget)->Update( g_fDeltaTime );
				}
				break;
			case RTTI_NPC:			
				break;
			default:
				assert_cs(false);
				break;
			}

		}	
	}	

	if( nsCsMfcFunc::Control2Int(&m_EditAreaRad) <= 100 )
	{
		nsCsMfcFunc::Int2Control( &m_EditAreaRad, 100 );
	}		
	
	if( m_pTarget != NULL )
	{
		_stprintf_s( sz, 128, L"X: %.3f Y: %.3f Z: %.3f", 
			g_EventScene.m_vEffectPoint.x - m_pTarget->GetPos().x,
			g_EventScene.m_vEffectPoint.y - m_pTarget->GetPos().y,
			g_EventScene.m_vEffectPoint.z - m_pTarget->GetPos().z );

		m_Edit_Pos.SetWindowText( sz );	

		_stprintf_s( sz, 128, L"X: %.3f Y: %.3f Z: %.3f", 
			m_pTarget->GetPos().x,
			m_pTarget->GetPos().y,
			m_pTarget->GetPos().z );

		m_Edit_TargetPos.SetWindowText( sz );	
	}	
}

void _DlgController::_Render()
{
	if( m_pTarget != NULL && !_IsPlay() )
	{
		switch( m_pTarget->GetLeafRTTI() )
		{
		case RTTI_EVENTOBJECT:
			{
				if( nsCsMfcFunc::Control2Float(&m_EditScale) < 0.0f )
					m_EditScale.SetWindowText( L"0.00" );
				m_pTarget->SetScale( nsCsMfcFunc::Control2Float(&m_EditScale) );
				((CsC_EventObject*)m_pTarget)->Render();
			}
			break;
		case RTTI_EVENTTAMEROBJECT:
			{
				if( nsCsMfcFunc::Control2Float(&m_EditScale) < 0.0f )
					m_EditScale.SetWindowText( L"0.00" );
				m_pTarget->SetScale( nsCsMfcFunc::Control2Float(&m_EditScale) );
				((CsC_EventTamerObject*)m_pTarget)->Render();
			}
			break;
		case RTTI_NPC:			
			break;
		default:
			assert_cs(false);
			break;
		}		
	}
}

void _DlgController::_SetPlay( bool bPlay )
{
	if( bPlay == true )
		m_Check_Play.SetCheck( BST_CHECKED ); 
	else
	{
		m_fTime = 0.0f;
		m_SldEventtime.SetPos( 0 );	
		m_Check_Play.SetCheck( BST_UNCHECKED );				
	}
}

void _DlgController::_NewEventChar( DWORD dwModelID )
{
	assert_cs( m_nType >= CEventData::EO_New && m_nType < CEventData::EO_END );
	if( m_nType == CEventData::EO_New )
	{
		_Delete();
	}	

	m_bNewObjectLoad = true;		

	// 초기 위치값
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( g_Camera.GetWorldLocation(), g_Camera.GetWorldDirection() );			
	NiPoint3 vPos = pRecord->GetIntersection();
	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );	

	// 회전값
	float fRot = 0.0f;	

	if( dwModelID / 10000 == 8 )
	{			
		//신규캐릭터 생성
		CsC_EventTamerObject* pEvent = CsC_EventTamerObject::NewInstance();	
		pEvent->Init( 1, dwModelID, vPos, fRot, NULL );			
		pEvent->m_eObjectState = CsC_EventTamerObject::OBJECT_NORMAL;

		m_pTarget = pEvent;
	}
	else
	{
		// 로드시 필요한 초기화
		sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File );

		//신규캐릭터 생성
		CsC_EventObject* pTarget = CsC_EventObject::NewInstance();

		// 초기화 - Prop 초기화 포함				
		pUint->s_Prop.s_eInsertGeomType	= CGeometry::GetCharOutLineType();		
		pUint->s_Prop.s_dwPropPlag		= PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_PATH_NORMAL;
		pUint->s_Prop.s_dwOptionPlag	= _OP_ALL;

		strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, g_pModelDataMng->GetData( dwModelID )->GetInfo()->s_cKfmPath );

		pTarget->Init( 1, dwModelID, vPos, fRot, &pUint->s_Prop );		
		pTarget->m_eObjectState = CsC_EventObject::OBJECT_NORMAL;

		TCHAR strName[ 32 ];
		m_EditCharName.GetWindowText( strName, 32 );
		pTarget->SetName( strName );

		// 로드 설정
		pUint->s_pLoadedObject = (CsC_EventObject*)pTarget;	

		// 쓰레드 로드	
		g_pThread->LoadChar( pUint );

		m_pTarget = pTarget;
	}		

	m_nType = CEventData::EO_New;

	//애니메이션 리스트생성
	_SetAniList();
}

void _DlgController::_NewEventChar( DWORD dwModelID, NiPoint3 vPos, float vRot )
{
	assert_cs( m_nType >= CEventData::EO_New && m_nType < CEventData::EO_END );
	if( m_nType == CEventData::EO_New )
	{
		_Delete();
	}	

	m_bNewObjectLoad = false;			

	// 회전값
	float fRot = vRot;	

	if( dwModelID / 10000 == 8 )
	{			
		//신규캐릭터 생성
		CsC_EventTamerObject* pEvent = CsC_EventTamerObject::NewInstance();	
		pEvent->Init( 1, dwModelID, vPos, fRot, NULL );			
		pEvent->m_eObjectState = CsC_EventTamerObject::OBJECT_NORMAL;

		m_pTarget = pEvent;
	}
	else
	{
		// 로드시 필요한 초기화
		sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File );

		//신규캐릭터 생성
		CsC_EventObject* pTarget = CsC_EventObject::NewInstance();

		// 초기화 - Prop 초기화 포함
		pUint->s_Prop.s_eInsertGeomType	= CGeometry::GetCharOutLineType();		
		pUint->s_Prop.s_dwPropPlag		= PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_PATH_NORMAL;
		pUint->s_Prop.s_dwOptionPlag	= _OP_ALL;	
		strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, g_pModelDataMng->GetData( dwModelID )->GetInfo()->s_cKfmPath );

		pTarget->Init( 1, dwModelID, vPos, fRot, &pUint->s_Prop );		
		pTarget->m_eObjectState = CsC_EventObject::OBJECT_NORMAL;

		TCHAR strName[ 32 ];
		m_EditCharName.GetWindowText( strName, 32 );
		pTarget->SetName( strName );

		// 로드 설정
		pUint->s_pLoadedObject = (CsC_EventObject*)pTarget;	

		// 쓰레드 로드	
		g_pThread->LoadChar( pUint );

		m_pTarget = pTarget;
	}	

	m_nType = CEventData::EO_New;

	//애니메이션 리스트생성
	_SetAniList();
}


void _DlgController::_SetTarGet( CsC_AvObject* pTarget )
{		
	assert_cs( m_nType >= CEventData::EO_New && m_nType < CEventData::EO_END );
	if( m_nType == CEventData::EO_New )	
	{			
		//이전에 있던 타입이 NPC가 아니라면 delete해주자
		_Delete();
	}

	//npc매니져에서 받아온 avobject연결해준다
	m_pTarget = pTarget;
	m_nType = CEventData::EO_Npc;

	//애니메이션 리스트생성
	_SetAniList();	
}

void _DlgController::_SetAniList()
{
	assert_cs( m_pTarget != NULL )	

	POSITION pos = m_List_Animation.GetFirstSelectedItemPosition();
	int nSelect = m_List_Animation.GetNextSelectedItem( pos );

	m_List_Animation.DeleteAllItems();

	NiActorManager* pActorMng = m_pTarget->GetProp_Animation()->GetActorMng();
	CsC_AniProp_Sequence* m_pCsSequenceMng = m_pTarget->GetProp_Animation()->GetSequenceMng();

	std::map< DWORD, CsC_LoadSequence::sSEQUENCE_INFO* >* pMap = m_pCsSequenceMng->GetLoadSequence()->GetSequenceMap();
	std::map< DWORD, CsC_LoadSequence::sSEQUENCE_INFO* >::iterator it = pMap->begin();
	std::map< DWORD, CsC_LoadSequence::sSEQUENCE_INFO* >::iterator itEnd = pMap->end();
	
	TCHAR szName[ OBJECT_NAME_LEN ];
	TCHAR szInt[ 32 ];
	TCHAR* szShotName;
	for( ; it!=itEnd; ++it )
	{
		NiControllerSequence* pNiSequence = pActorMng->GetSequence( it->first );		

		// 리스트에 데이터 기입
		M2W( szName, pNiSequence->GetName(), OBJECT_NAME_LEN );
		size_t nLen = _tcslen( szName );
		szShotName = NULL;
		for( size_t i=0; i<nLen; ++i )
		{
			if( szName[ i ] == '_' )
			{
				szShotName = &szName[ i+1 ];
				break;
			}
		}
		int nIndex = m_List_Animation.GetItemCount();
		m_List_Animation.InsertItem( nIndex, szShotName != NULL ? szShotName : szName );

		_stprintf_s( szInt, 32, L"%d", it->second->s_vpTextKeyEvent.Size() );
		m_List_Animation.SetItem( nIndex, 1, LVIF_TEXT, szInt, 0, 0, 0, 0 );
		m_List_Animation.SetItem( nIndex, 2, LVIF_TEXT, it->second->s_eCyleType == NiTimeController::LOOP ? L"O" : L"X", 0, 0, 0, 0 );
		_stprintf_s( szInt, 32, L"%.2f", it->second->s_fEndTime );
		m_List_Animation.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );
		_stprintf_s( szInt, 32, L"%d", it->first );
		m_List_Animation.SetItem( nIndex, 4, LVIF_TEXT, szInt, 0, 0, 0, 0 );
		m_List_Animation.SetItemData( nIndex, it->first );		
	}
	
}

void _DlgController::_SetEventTime( float fStartTime, float fEndTime )
{ 
	m_SldEventtime.SetPos( 0 );
	m_SldEventtime.SetRange( 0, (int)( fEndTime*1000.0f ) ); 
	m_SldEventtime.SetTic( (int)( fEndTime*1000.0f ) );

	m_fEndTime = fEndTime;
	m_fStartTime = fStartTime;
	nsCsMfcFunc::Float2Control( &m_EditStartTime, fStartTime, 3 );
	
}

void _DlgController::_SetMoveEvent( NiPoint3 vPos )
{
	int nIndex = m_ListEventPlay.GetItemCount();
	TCHAR szInt[ 128 ];
	
	float fPlayTime = _GetDist( vPos ) / m_fMoveSpeed;	

	_stprintf_s( szInt, 128, L"%.2f", m_fTime  );
	m_ListEventPlay.InsertItem( nIndex, szInt );

	m_ListEventPlay.SetItem( nIndex, 1, LVIF_TEXT, TypeMOVE, 0, 0, 0, 0 );	

	_stprintf_s( szInt, 128, L"위치:%.3f,%.3f,%.3f 속도:%.3f", vPos.x, vPos.y, vPos.z, m_fMoveSpeed );
	m_ListEventPlay.SetItem( nIndex, 2, LVIF_TEXT, szInt, 0, 0, 0, 0 );	

	_stprintf_s( szInt, 128, L"%.3f", fPlayTime + m_fTime  );
	m_ListEventPlay.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );		
		
	_SetPlayListSort();
}

void _DlgController::_SetMoveEvent( NiPoint3 vPos, float fTime, float fMoveSpeed )
{
	int nIndex = m_ListEventPlay.GetItemCount();
	TCHAR szInt[ 128 ];

	float fPlayTime = _GetDist( vPos ) / fMoveSpeed;	

	_stprintf_s( szInt, 128, L"%.2f", fTime );
	m_ListEventPlay.InsertItem( nIndex, szInt );

	m_ListEventPlay.SetItem( nIndex, 1, LVIF_TEXT, TypeMOVE, 0, 0, 0, 0 );	

	_stprintf_s( szInt, 128, L"위치:%.3f,%.3f,%.3f 속도:%.3f", vPos.x, vPos.y, vPos.z, fMoveSpeed );
	m_ListEventPlay.SetItem( nIndex, 2, LVIF_TEXT, szInt, 0, 0, 0, 0 );	

	_stprintf_s( szInt, 128, L"%.3f", fPlayTime + fTime );
	m_ListEventPlay.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );		

	_SetPlayListSort();
}

void _DlgController::_SetSoundEvent( CString strFile, CEventData::sEventSound_Info::eEventSound eType )
{
	int nIndex = m_ListEventPlay.GetItemCount();

	TCHAR szInt[ 128 ];	

	switch( eType )
	{
	case CEventData::sEventSound_Info::ES_MUSIC:
		{
			_stprintf_s( szInt, 128, L"%.2f", m_fTime );
			m_ListEventPlay.InsertItem( nIndex, szInt );

			m_ListEventPlay.SetItem( nIndex, 1, LVIF_TEXT, TypeMUSIC, 0, 0, 0, 0 );	

			m_ListEventPlay.SetItem( nIndex, 2, LVIF_TEXT, strFile, 0, 0, 0, 0 );	

			_stprintf_s( szInt, 128, L"%.3f", 0.0f );
			m_ListEventPlay.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );		
		}
		break;
	case CEventData::sEventSound_Info::ES_SOUND:
		{
			_stprintf_s( szInt, 128, L"%.2f", m_fTime );
			m_ListEventPlay.InsertItem( nIndex, szInt );

			m_ListEventPlay.SetItem( nIndex, 1, LVIF_TEXT, TypeSOUND, 0, 0, 0, 0 );	

			m_ListEventPlay.SetItem( nIndex, 2, LVIF_TEXT, strFile, 0, 0, 0, 0 );	

			_stprintf_s( szInt, 128, L"%.3f", 0.0f );
			m_ListEventPlay.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );		
		}
		break;

	}

	_SetPlayListSort();
}

void _DlgController::_SetSoundEvent( CString strFile, float fTime, CEventData::sEventSound_Info::eEventSound eType )
{
	int nIndex = m_ListEventPlay.GetItemCount();

	TCHAR szInt[ 128 ];		

	switch( eType )
	{
	case CEventData::sEventSound_Info::ES_MUSIC:
		{
			_stprintf_s( szInt, 128, L"%.2f", fTime  );
			m_ListEventPlay.InsertItem( nIndex, szInt );

			m_ListEventPlay.SetItem( nIndex, 1, LVIF_TEXT, TypeMUSIC, 0, 0, 0, 0 );	

			m_ListEventPlay.SetItem( nIndex, 2, LVIF_TEXT, strFile, 0, 0, 0, 0 );	

			_stprintf_s( szInt, 128, L"%.3f", 0.0f );
			m_ListEventPlay.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );	
		}
		break;
	case CEventData::sEventSound_Info::ES_SOUND:
		{
			_stprintf_s( szInt, 128, L"%.2f", fTime  );
			m_ListEventPlay.InsertItem( nIndex, szInt );

			m_ListEventPlay.SetItem( nIndex, 1, LVIF_TEXT, TypeSOUND, 0, 0, 0, 0 );	

			m_ListEventPlay.SetItem( nIndex, 2, LVIF_TEXT, strFile, 0, 0, 0, 0 );	

			_stprintf_s( szInt, 128, L"%.3f", 0.0f );
			m_ListEventPlay.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );	
		}
		break;

	}

	_SetPlayListSort();
}

void _DlgController::_SetEffectEvent( DWORD dwEffectNum )
{
	int nIndex = m_ListEventPlay.GetItemCount();

	TCHAR szInt[ 128 ];	

	_stprintf_s( szInt, 128, L"%.2f", m_fTime  );
	m_ListEventPlay.InsertItem( nIndex, szInt );

	m_ListEventPlay.SetItem( nIndex, 1, LVIF_TEXT, TypeEFFECT, 0, 0, 0, 0 );	

	_stprintf_s( szInt, 128, L"%u", dwEffectNum );
	m_ListEventPlay.SetItem( nIndex, 2, LVIF_TEXT, szInt, 0, 0, 0, 0 );	

	_stprintf_s( szInt, 128, L"%d", 0 );
	m_ListEventPlay.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );		

	_SetPlayListSort();
}

void _DlgController::_SetEffectEvent( DWORD dwEffectNum, float fTime )
{
	int nIndex = m_ListEventPlay.GetItemCount();

	TCHAR szInt[ 128 ];	

	_stprintf_s( szInt, 128, L"%.2f", fTime  );
	m_ListEventPlay.InsertItem( nIndex, szInt );

	m_ListEventPlay.SetItem( nIndex, 1, LVIF_TEXT, TypeEFFECT, 0, 0, 0, 0 );	

	_stprintf_s( szInt, 128, L"%u", dwEffectNum );
	m_ListEventPlay.SetItem( nIndex, 2, LVIF_TEXT, szInt, 0, 0, 0, 0 );	

	_stprintf_s( szInt, 128, L"%d", 0 );
	m_ListEventPlay.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );		

	_SetPlayListSort();
}

float _DlgController::_GetDist( NiPoint3 vPos )
{	
	NiPoint3 vDist;

	switch( m_nType )
	{
	case CEventData::EO_New:
		{
			vDist = m_vPos;
		}
		break;
	case CEventData::EO_Npc:
		{
			CsMapNpc*		m_pFT_MapNpc;		
			m_pFT_MapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( m_pTarget->GetModelID() );

			// 위치값
			vDist = NiPoint3( (float)m_pFT_MapNpc->GetInfo()->s_nInitPosX, (float)m_pFT_MapNpc->GetInfo()->s_nInitPosY, 0.0f );
			vDist.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );
		}
		break;
	}
	
	for( int nIndx = m_ListEventPlay.GetItemCount() - 1; nIndx >= 0; nIndx-- )
	{	
		CString TypeStr =  m_ListEventPlay.GetItemText( nIndx, 1 );
		if( TypeStr.Compare( TypeMOVE ) != 0 )
			continue;

		CString str;
		str = m_ListEventPlay.GetItemText( nIndx, 2 );
		
		int nPos = str.Find( _T(':') );
		int nPos2 = str.Find( _T(',') );	

		CString strPos = str.Mid( nPos + 1, nPos2 - nPos - 1 );                      
		vDist.x = (float)_tstof(strPos.GetBuffer(strPos.GetLength()));
		str.Delete( 0, nPos2 + 1 );

		nPos = 0;
		nPos2 = str.Find( _T(',') );				

		strPos = str.Mid( nPos, nPos2 - nPos );                      
		vDist.y = (float)_tstof(strPos.GetBuffer(strPos.GetLength()));
		str.Delete( 0, nPos2 + 1 );

		nPos = 0;
		nPos2 = str.Find( _T(' ') );				

		strPos = str.Mid( nPos, nPos2 - nPos );                      
		vDist.z = (float)_tstof(strPos.GetBuffer(strPos.GetLength()));
		str.Delete( 0, nPos2 + 1 );			
	}		

	vDist -= vPos;

	return vDist.Length();
}

void _DlgController::_SetTalkEvent( DWORD dwTalkNum )
{
	int nIndex = m_ListEventPlay.GetItemCount();
	TCHAR szInt[ 128 ];

	_stprintf_s( szInt, 128, L"%.2f", m_fTime  );
	m_ListEventPlay.InsertItem( nIndex, szInt );

	m_ListEventPlay.SetItem( nIndex, 1, LVIF_TEXT, TypeTALK, 0, 0, 0, 0 );	

	_stprintf_s( szInt, 128, L"%d", dwTalkNum );
	m_ListEventPlay.SetItem( nIndex, 2, LVIF_TEXT, szInt, 0, 0, 0, 0 );	

	float fPlayTime = 5.0f;
	_stprintf_s( szInt, 128, L"%.3f", fPlayTime + m_fTime );
	m_ListEventPlay.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );	

	_SetPlayListSort();
}

void _DlgController::_SetTalkEvent( DWORD dwTalkNum, float fTime )
{
	int nIndex = m_ListEventPlay.GetItemCount();
	TCHAR szInt[ 128 ];

	_stprintf_s( szInt, 128, L"%.2f", fTime  );
	m_ListEventPlay.InsertItem( nIndex, szInt );

	m_ListEventPlay.SetItem( nIndex, 1, LVIF_TEXT, TypeTALK, 0, 0, 0, 0 );	

	_stprintf_s( szInt, 128, L"%d", dwTalkNum );
	m_ListEventPlay.SetItem( nIndex, 2, LVIF_TEXT, szInt, 0, 0, 0, 0 );	

	float fPlayTime = 5.0f;
	_stprintf_s( szInt, 128, L"%.3f", fPlayTime + fTime  );
	m_ListEventPlay.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );	

	_SetPlayListSort();
}

void _DlgController::_SetAniEvent( DWORD dwAniId )
{
	int nIndex = m_ListEventPlay.GetItemCount();
	TCHAR szInt[ MAX_PATH ];

	_stprintf_s( szInt, MAX_PATH, L"%.2f", m_fTime );	
	
	m_ListEventPlay.InsertItem( nIndex, szInt );
	
	m_ListEventPlay.SetItem( nIndex, 1, LVIF_TEXT, TypeANI, 0, 0, 0, 0 );	

	TCHAR szName[ OBJECT_NAME_LEN ];	
	TCHAR* szShotName;

	NiActorManager* pActorMng = m_pTarget->GetProp_Animation()->GetActorMng();
	NiControllerSequence* pNiSequence = pActorMng->GetSequence( dwAniId );		

	if( pNiSequence == NULL )
	{
		CsMessageBox( MB_OK, L"애니메이션이 존재하지 않음 : %d", dwAniId );
	}

	// 리스트에 데이터 기입
	M2W( szName, pNiSequence->GetName(), OBJECT_NAME_LEN );
	size_t nLen = _tcslen( szName );
	szShotName = NULL;
	for( size_t i=0; i<nLen; ++i )
	{
		if( szName[ i ] == '_' )
		{
			szShotName = &szName[ i+1 ];
			break;
		}
	}
	_stprintf_s( szInt, MAX_PATH, L"%s(%d)", szName, dwAniId );
	m_ListEventPlay.SetItem( nIndex, 2, LVIF_TEXT, szInt, 0, 0, 0, 0 );	
	
	_stprintf_s( szInt, MAX_PATH, L"%.2f", m_pTarget->GetProp_Animation()->GetSequenceMng()->GetSequence( dwAniId )->s_fEndTime + m_fTime );
	m_ListEventPlay.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );	
}

void _DlgController::_SetAniEvent( DWORD dwAniId, float fTime )
{
	int nIndex = m_ListEventPlay.GetItemCount();
	TCHAR szInt[ MAX_PATH ];

	_stprintf_s( szInt, MAX_PATH, L"%.2f", fTime );	

	m_ListEventPlay.InsertItem( nIndex, szInt );

	m_ListEventPlay.SetItem( nIndex, 1, LVIF_TEXT, TypeANI, 0, 0, 0, 0 );	

	TCHAR szName[ OBJECT_NAME_LEN ];
	TCHAR* szShotName;

	NiActorManager* pActorMng = m_pTarget->GetProp_Animation()->GetActorMng();
	NiControllerSequence* pNiSequence = pActorMng->GetSequence( dwAniId );	

	if( pNiSequence == NULL )
	{
		CsMessageBox( MB_OK, L"애니메이션이 존재하지 않음 : %d", dwAniId );
	}

	// 리스트에 데이터 기입
	M2W( szName, pNiSequence->GetName(), OBJECT_NAME_LEN );
	size_t nLen = _tcslen( szName );
	szShotName = NULL;
	for( size_t i=0; i<nLen; ++i )
	{
		if( szName[ i ] == '_' )
		{
			szShotName = &szName[ i+1 ];
			break;
		}
	}
	
	_stprintf_s( szInt, MAX_PATH, L"%s(%d)", szName, dwAniId );
	m_ListEventPlay.SetItem( nIndex, 2, LVIF_TEXT, szInt, 0, 0, 0, 0 );	

	_stprintf_s( szInt, MAX_PATH, L"%.2f", m_pTarget->GetProp_Animation()->GetSequenceMng()->GetSequence( dwAniId )->s_fEndTime + fTime );
	m_ListEventPlay.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );	
}

static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
	CString* strItem1 = (CString*)lParam1;
	CString* strItem2 = (CString*)lParam2;		   	

	float fTime1 = (float)_tstof( strItem1->GetBuffer(strItem1->GetLength()) );
	float fTime2 = (float)_tstof( strItem2->GetBuffer(strItem2->GetLength()) );

	if( fTime1 > fTime2 )
	{
		return 1;
	}
	else
	{
		return -1;
	}

	return 0; 
} 

void _DlgController::_SetPlayListSort()
{	
	int i;
	int nitem = m_ListEventPlay.GetItemCount();
	CString** arStr =  new CString*[ nitem ];
	for( i = 0; i < nitem; i++ )
	{	
		arStr[ i ]= new CString( m_ListEventPlay.GetItemText( i, 0 ) );
		m_ListEventPlay.SetItemData( i, (LPARAM)arStr[ i ] );
	}

	m_ListEventPlay.SortItems( CompareFunc, true );

	for( i = 0; i < nitem; i++)
	{
		delete arStr[ i ];
	}
	delete []arStr;	
	
}

void _DlgController::_SetEventModify( DWORD dwEventID )
{
	std::map< DWORD, CEventScene::MAP_EvntData >::iterator it = g_EventScene.m_mapTempData.find( g_Terrain.m_dwMapId );
	assert_cs( it != g_EventScene.m_mapTempData.end() );

	CEventScene::MAP_EvntData_IT itData = it->second.find( dwEventID );
	CEventData* pEvent = itData->second;		

	_SetPlayListReset();	

	m_EditEventTitle.SetWindowText( pEvent->GetEventInfo()->s_strTitle );
	m_EditCharName.SetWindowText( pEvent->GetObjectInfo()->s_strName );
	TCHAR szFloat[ 32 ];	
	_stprintf_s( szFloat, 32, L"%.3f", pEvent->GetObjectInfo()->s_fScale );
	m_EditScale.SetWindowText( szFloat );
	
	// 이벤트 시간 설정	
	_SetEventTime( pEvent->GetEventInfo()->s_fStartTime, pEvent->GetEventInfo()->s_fEventTime - pEvent->GetEventInfo()->s_fStartTime );	
	

	// 이벤트 대상 오브젝트 설정	
	switch( pEvent->GetObjectInfo()->s_sType )
	{
	case CEventData::EO_New:
		{
			m_vPos = pEvent->GetObjectInfo()->s_vPos;
			m_fRot = pEvent->GetObjectInfo()->s_fRot;

			TCHAR sz[ 32 ];
			_stprintf_s( sz, 32, L"%f", pEvent->GetObjectInfo()->s_vOffset.z );
			m_Edit_ZOffset.SetWindowText( sz );	
			
			_NewEventChar( pEvent->GetObjectInfo()->s_dwEventObjectID, m_vPos, m_fRot );
		}
		break;
	case CEventData::EO_Npc:
		{
			TCHAR sz[ 32 ];
			_stprintf_s( sz, 32, L"%f", pEvent->GetObjectInfo()->s_vOffset.z );
			m_Edit_ZOffset.SetWindowText( sz );

			_SetTarGet( g_pNpcMng->GetNpc( pEvent->GetObjectInfo()->s_dwEventObjectID ) );
		}
		break;
	default:
		assert_cs( false );
		break;
	}	

	int nSize = pEvent->GetEventSize();
	for( int i = 0; i < nSize; i++ )
	{
		switch( pEvent->GetEventData( i )->GetType() )
		{
		case CEventData::ET_Ani:
			{
				CEventData::sAnimation_Info* pAni = (CEventData::sAnimation_Info*)pEvent->GetEventData( i );					
				_SetAniEvent( pAni->s_dwMotionID, pAni->GetEventStartTime() );
			}
			break;
		case CEventData::ET_Move:
			{
				CEventData::sMovePos_Info* pMove = (CEventData::sMovePos_Info*)pEvent->GetEventData( i );					
				_SetMoveEvent( pMove->s_vPos, pMove->GetEventStartTime(), pMove->s_fSpeed );
			}
			break;
		case CEventData::ET_Talk:
			{
				CEventData::sEventTalk_Info* pTalk = (CEventData::sEventTalk_Info*)pEvent->GetEventData( i );					
				_SetTalkEvent( pTalk->s_dwTalkID, pTalk->GetEventStartTime() );
			}
			break;
		case CEventData::ET_Sound:
			{
				CEventData::sEventSound_Info* pSound = (CEventData::sEventSound_Info*)pEvent->GetEventData( i );
				CString str( pSound->szFileName );
				int nPos = str.ReverseFind( '\\' );
				CString strFile = str.Mid( nPos + 1, str.GetLength() );
				_SetSoundEvent( strFile, pSound->GetEventStartTime(), pSound->nType );
			}
			break;
		case CEventData::ET_Effect:
			{
				CEventData::sEventEffect_Info* pEffect = (CEventData::sEventEffect_Info*)pEvent->GetEventData( i );					
				_SetEffectEvent( (DWORD)pEffect->s_EffectHashCode, pEffect->GetEventStartTime() );
			}
			break;
		default:
			break;
		}
	}

}

void _DlgController::_SetEffectPos( NiPoint3 vPos )
{	
	m_bMouseMove = false;
	m_bEffectPos = false;
}


// _DlgController 메시지 처리기입니다.

void _DlgController::OnBnClickedMapload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( _IsPlay() || _IsChrCamera() || _IsDirectPlay() )
	{
		CsMessageBox( MB_OK, L"진행중인이벤트를 끄고하세요" );
		return;
	}
	
	_DlgMapList ie;
	if( ie.DoModal() == IDCANCEL )
		return;
}

void _DlgController::OnBnClickedInsertEvent()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_bMouseMove )
		return;
	if( m_EditEventTitle.GetWindowTextLength() < 1 || m_EditCharName.GetWindowTextLength() < 1 )
	{
		CsMessageBox( MB_OK, L"타이틀과 캐릭터이름을 먼저 입력해주세요" );		
		return;
	}
	
	//_SetPlayListReset();
	//_SetEventTime( 0.0f, 0.0f );

	g_EventScene.ReleaseMovePoint();				

	m_bMouseMove = true;

	_DlgEventlist ie;
	if( ie.DoModal() == IDCANCEL )
	{
		m_bMouseMove = false;
	}
}

void _DlgController::OnBnClickedInsertEvent2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	_SetPlayListReset();

	g_EventScene.ReleaseMovePoint();		
	
	_SetEventTime( 0.0f, 0.0f );

	_DlgNpcList ie;
	if( ie.DoModal() == IDCANCEL )
		return;
}

void _DlgController::OnBnClickedEventtalk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_pTarget == NULL )
	{
		CsMessageBox( MB_OK, L"대상이 없네요.." );
		return;
	}

	_DlgEventTalkList ie;
	if( ie.DoModal() == IDCANCEL )
		return;
}

void _DlgController::OnBnClickedSettime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_pTarget == NULL )
	{
		CsMessageBox( MB_OK, L"이벤트대상이 없네요.." );
		return;
	}

	_DlgEventTime ie;	
	if( ie.DoModal() == IDCANCEL )
		return;
}

void _DlgController::OnLvnItemchangedListAnimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_List_Animation.SetFocus();

	POSITION pos = m_List_Animation.GetFirstSelectedItemPosition();
	int nSelect = m_List_Animation.GetNextSelectedItem( pos );
	if( nSelect < 0 )
		return;

	DWORD dwAniID = (DWORD)m_List_Animation.GetItemData( nSelect );
	m_pTarget->SetAnimation( dwAniID );

	*pResult = 0;
}

void _DlgController::OnNMDblclkListAnimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_List_Animation.SetFocus();

	POSITION pos = m_List_Animation.GetFirstSelectedItemPosition();
	int nSelect = m_List_Animation.GetNextSelectedItem( pos );
	if( nSelect < 0 )
		return;

	DWORD dwAniID = (DWORD)m_List_Animation.GetItemData( nSelect );		

	int nIndex = m_ListEventPlay.GetItemCount();
	TCHAR szInt[ 128 ];

	_stprintf_s( szInt, 32, L"%.2f", m_fTime + m_fStartTime );
	m_ListEventPlay.InsertItem( nIndex, szInt );

	m_ListEventPlay.SetItem( nIndex, 1, LVIF_TEXT, TypeANI, 0, 0, 0, 0 );	

	TCHAR szName[ OBJECT_NAME_LEN ];
	TCHAR* szShotName;

	NiActorManager* pActorMng = m_pTarget->GetProp_Animation()->GetActorMng();
	NiControllerSequence* pNiSequence = pActorMng->GetSequence( dwAniID );		

	// 리스트에 데이터 기입
	M2W( szName, pNiSequence->GetName(), OBJECT_NAME_LEN );
	size_t nLen = _tcslen( szName );
	szShotName = NULL;
	for( size_t i=0; i<nLen; ++i )
	{
		if( szName[ i ] == '_' )
		{
			szShotName = &szName[ i+1 ];
			break;
		}
	}
	_stprintf_s( szInt, 128, L"%s(%d)", szName, dwAniID );
	m_ListEventPlay.SetItem( nIndex, 2, LVIF_TEXT, szInt, 0, 0, 0, 0 );	

	CString str = m_List_Animation.GetItemText( nSelect, 3 );

	float PlayTime = (float)_tstof(str.GetBuffer(str.GetLength()));
	_stprintf_s( szInt, 128, L"%.2f", PlayTime + m_fTime + m_fStartTime );
	m_ListEventPlay.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );

	_SetPlayListSort();

	*pResult = 0;
}



void _DlgController::OnBnClickedListdelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ListEventPlay.SetFocus();

	std::stack< int > stackDel;
	POSITION pos = m_ListEventPlay.GetFirstSelectedItemPosition();
	while( pos )
	{
		stackDel.push( m_ListEventPlay.GetNextSelectedItem( pos ) );
	}

	while( stackDel.size() != 0 )
	{
		int nDelIndex = stackDel.top();
		stackDel.pop();
		m_ListEventPlay.DeleteItem( nDelIndex );					
	}

}

void _DlgController::OnBnClickedReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	_ResetAll();
}

void _DlgController::OnBnClickedSetevent()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	_DlgEventLoadList ie;
	ie.Init( 1 );
	if( ie.DoModal() == IDCANCEL )
		return;		
}

void _DlgController::OnBnClickedCheckPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( _IsChrCamera() )
	{
		g_pTalkBallone->Delete();
		_SetPlay( false );		
		return;
	}

	if( _IsPlay() )
	{	
		if( m_ListEventPlay.GetItemCount() < 1 || m_pTarget == NULL )
		{
			CsMessageBox( MB_OK, L"적용할 이벤트가 없네요." );	
			_SetPlay( false );
			return;
		}	

		if( m_EditEventTitle.GetWindowTextLength() <= 0 )
		{
			CsMessageBox( MB_OK, L"타이틀을 확인해주세요" );
			_SetPlay( false );
			return;
		}		

		g_EventScene.ReleaseMovePoint();
		g_EventScene.SetDeltaTime( m_fTime );
		CreateEventData();		
		g_EventScene.SetEventTarget();
	}
	else
	{
		if( m_nType == CEventData::EO_New )
		{
			m_pTarget->SetPos( m_vPos );
			m_pTarget->SetRotation( m_fRot, true );			
			m_pTarget->SetDefault_IdleAnimation();
		}		
		else if( m_nType == CEventData::EO_Npc )
		{
			CsMapNpc*		m_pFT_MapNpc;		
			m_pFT_MapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( m_pTarget->GetModelID() );

			// 위치값
			NiPoint3 vPos = NiPoint3( (float)m_pFT_MapNpc->GetInfo()->s_nInitPosX, (float)m_pFT_MapNpc->GetInfo()->s_nInitPosY, 0.0f );
			vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

			// 회전값
			m_pTarget->SetPos( vPos );
			m_pTarget->SetRotation( m_pFT_MapNpc->GetInfo()->s_fRotation, true );				
			m_pTarget->SetDefault_IdleAnimation();						
		}

		m_fTime = 0.0f;
		m_SldEventtime.SetPos( 0 );
		g_EventScene.EndEvent();
	}	
}

bool _DlgController::CreateEventData()
{
	if( m_pTarget == NULL )
		return false;

	g_EventScene.CreateEvent();

	//이벤트 기본정보 입력
	CEventData::sEvent_Info pEventInfo;
	pEventInfo.s_fEventTime = m_fEndTime + m_fStartTime;	//이벤트 총시간	
	pEventInfo.s_fStartTime = m_fStartTime; //이벤트 시작시간		
	m_EditEventTitle.GetWindowText( pEventInfo.s_strTitle, 32 );			
	pEventInfo.s_nEventNum = (UINT)CsFileHash::GetHashCode( pEventInfo.s_strTitle );		

	//이벤트 대상 오브젝트 정보 입력
	CEventData::sEventObject_Info pEventObjectInfo;
	pEventObjectInfo.s_sType = m_nType;	// 새이벤트인지, NPC이벤트인지
	pEventObjectInfo.s_dwEventObjectID = m_pTarget->GetModelID(); //이벤트 주체의 모델번호	
	pEventObjectInfo.s_fScale = nsCsMfcFunc::Control2Float( &m_EditScale );
	m_EditCharName.GetWindowText( pEventObjectInfo.s_strName, 32 ); //캐릭터 이름

	switch( m_nType )
	{
	case CEventData::EO_New:
		{
			float fZoffset = nsCsMfcFunc::Control2Float( &m_Edit_ZOffset );
			pEventObjectInfo.s_vPos = m_vPos;
			pEventObjectInfo.s_vOffset = NiPoint3( 0.0f, 0.0f, fZoffset );
			pEventObjectInfo.s_fRot = m_pTarget->GetCurRot(); //m_fRot;
		}
		break;
	case CEventData::EO_Npc:
		{
			pEventObjectInfo.s_vPos = NiPoint3::ZERO;
			pEventObjectInfo.s_fRot = 0.0f;
		}
		break;
	default:
		{
			assert_cs( false );
		}
		break;
	}
	g_EventScene.SetEventInit( pEventInfo, pEventObjectInfo );


	for( int nIndx = 0 ; nIndx < m_ListEventPlay.GetItemCount(); nIndx++ )
	{	
		CString TypeStr =  m_ListEventPlay.GetItemText( nIndx, 1 );
		int		nType = 0;
		if( TypeStr.Compare( TypeANI ) == 0 )
		{
			nType = CEventData::ET_Ani;
		}
		else if( TypeStr.Compare( TypeMOVE ) == 0 )
		{
			nType = CEventData::ET_Move;
		}
		else if( TypeStr.Compare( TypeTALK ) == 0 )
		{
			nType = CEventData::ET_Talk;
		}
		else if( TypeStr.Compare( TypeSOUND ) == 0 )
		{
			nType = CEventData::ET_Sound;
		}
		else if( TypeStr.Compare( TypeMUSIC ) == 0 )
		{
			nType = CEventData::ET_Sound;
		}
		else if( TypeStr.Compare( TypeEFFECT ) == 0 )
		{
			nType = CEventData::ET_Effect;
		}

		switch( nType )
		{
		case CEventData::ET_Ani:
			{
				CEventData::sAnimation_Info* pEventAni = NiNew CEventData::sAnimation_Info;
				CString str;
				str = m_ListEventPlay.GetItemText( nIndx, 0 );
				float fTime = (float)_tstof(str.GetBuffer(str.GetLength()));
				pEventAni->SetEventStartTime( fTime ) ;

				str = m_ListEventPlay.GetItemText( nIndx, 2 );					
				int nPos = str.Find( _T('(') );
				int nPos2 = str.Find( _T(')') );	
				CString strPos = str.Mid( nPos + 1, nPos2 - nPos - 1 ); 

				pEventAni->s_dwMotionID = (DWORD)_tstof(strPos.GetBuffer(strPos.GetLength()));

				g_EventScene.SetEvent( pEventAni );					
			}
			break;
		case CEventData::ET_Move:
			{
				CEventData::sMovePos_Info* pEventMove = NiNew CEventData::sMovePos_Info;				
				CString str;
				str = m_ListEventPlay.GetItemText( nIndx, 0 );
				float fTime = (float)_tstof(str.GetBuffer(str.GetLength()));
				pEventMove->SetEventStartTime( fTime ) ;

				str = m_ListEventPlay.GetItemText( nIndx, 2 );

				NiPoint3 vPos;
				int nPos = str.Find( _T(':') );
				int nPos2 = str.Find( _T(',') );	

				CString strPos = str.Mid( nPos + 1, nPos2 - nPos - 1 );                      
				vPos.x = (float)_tstof(strPos.GetBuffer(strPos.GetLength()));
				str.Delete( 0, nPos2 + 1 );

				nPos = 0;
				nPos2 = str.Find( _T(',') );				

				strPos = str.Mid( nPos, nPos2 - nPos );                      
				vPos.y = (float)_tstof(strPos.GetBuffer(strPos.GetLength()));
				str.Delete( 0, nPos2 + 1 );

				nPos = 0;
				nPos2 = str.Find( _T(' ') );				

				strPos = str.Mid( nPos, nPos2 - nPos );                      
				vPos.z = (float)_tstof(strPos.GetBuffer(strPos.GetLength()));
				str.Delete( 0, nPos2 + 1 );

				pEventMove->s_vPos = vPos;

				nPos = str.Find( _T(':') );
				nPos2 = str.GetLength();	

				strPos = str.Mid( nPos + 1, nPos2 - nPos -1 );                      
				pEventMove->s_fSpeed = (float)_tstof(strPos.GetBuffer(strPos.GetLength()));

				g_EventScene.SetEvent( pEventMove );
			}
			break;
		case CEventData::ET_Talk:
			{
				CEventData::sEventTalk_Info* pEventTalk = NiNew CEventData::sEventTalk_Info;
				CString str;
				str = m_ListEventPlay.GetItemText( nIndx, 0 );
				float fTime = (float)_tstof(str.GetBuffer(str.GetLength()));
				pEventTalk->SetEventStartTime( fTime );

				str = m_ListEventPlay.GetItemText( nIndx, 2 );
				pEventTalk->s_dwTalkID = (DWORD)_tstof(str.GetBuffer(str.GetLength()));

				g_EventScene.SetEvent( pEventTalk );	
			}
			break;
		case CEventData::ET_Sound:
			{
				CEventData::sEventSound_Info* pEventSound = NiNew CEventData::sEventSound_Info;
				CString str;
				str = m_ListEventPlay.GetItemText( nIndx, 0 );
				float fTime = (float)_tstof(str.GetBuffer(str.GetLength()));

				if( TypeStr.Compare( TypeSOUND ) == 0 )
				{
					pEventSound->nType = CEventData::sEventSound_Info::ES_SOUND;
				}
				else if( TypeStr.Compare( TypeMUSIC ) == 0 )
				{
					pEventSound->nType = CEventData::sEventSound_Info::ES_MUSIC;
				}					

				pEventSound->SetEventStartTime( fTime );

				str = "\\EventSound\\"; 
				str += m_ListEventPlay.GetItemText( nIndx, 2 );					
				WCHAR *strWC = str.GetBuffer(0);
				CHAR strMB[MAX_PATH];
				WideCharToMultiByte( CP_ACP, 0, strWC, -1, strMB, MAX_PATH, NULL, NULL );
				str.ReleaseBuffer();

				sprintf_s( pEventSound->szFileName, MAX_PATH, "%s", strMB );

				g_EventScene.SetEvent( pEventSound );		
			}
			break;
		case CEventData::ET_Effect:
			{
				CEventData::sEventEffect_Info* pEffect = NiNew CEventData::sEventEffect_Info;

				CString str;
				str = m_ListEventPlay.GetItemText( nIndx, 0 );
				float fTime = (float)_tstof(str.GetBuffer(str.GetLength()));
				pEffect->SetEventStartTime( fTime );

				str = m_ListEventPlay.GetItemText( nIndx, 2 );
				pEffect->s_EffectHashCode = (DWORD)_tstof(str.GetBuffer(str.GetLength()));					

				g_EventScene.SetEvent( pEffect);
			}
			break;
		default:
			assert_cs( false );
			break;
		}
	}	

	return true;
}


void _DlgController::OnNMCustomdrawSliderEventtime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	

	*pResult = 0;
}

void _DlgController::OnBnClickedMoveevent()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_bMouseMove )
		return;

	if( m_pTarget == NULL )
	{
		CsMessageBox( MB_OK, L"이동시킬 대상이 없네요.." );
		return;	
	}	
	
	m_bMouseMove = true;

	_DlgEventSpeed ie;	
	if( ie.DoModal() == IDOK )
	{
		m_bMoveEvent = true;
	}
	else
	{
		m_bMouseMove = false;
		m_bMoveEvent = false;
		_SetEventMoveSpeed( 0.0f );
	}	
}

void _DlgController::OnLvnItemchangedListEventplay(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ListEventPlay.SetFocus();
	POSITION pos = m_ListEventPlay.GetFirstSelectedItemPosition();
	int nSelect = m_ListEventPlay.GetNextSelectedItem( pos );	
	
	if( nSelect < 0 )
		return;

	CString TypeStr =  m_ListEventPlay.GetItemText( nSelect, 1 );	
	if( TypeStr.Compare( TypeMOVE ) != 0 )
		return;

	CString str;
	str = m_ListEventPlay.GetItemText( nSelect, 2 );

	NiPoint3 vPos;
	int nPos = str.Find( _T(':') );
	int nPos2 = str.Find( _T(',') );	

	CString strPos = str.Mid( nPos + 1, nPos2 - nPos - 1 );                      
	vPos.x = (float)_tstof(strPos.GetBuffer(strPos.GetLength()));
	str.Delete( 0, nPos2 + 1 );

	nPos = 0;
	nPos2 = str.Find( _T(',') );				

	strPos = str.Mid( nPos, nPos2 - nPos );                      
	vPos.y = (float)_tstof(strPos.GetBuffer(strPos.GetLength()));
	str.Delete( 0, nPos2 + 1 );

	nPos = 0;
	nPos2 = str.Find( _T(' ') );				

	strPos = str.Mid( nPos, nPos2 - nPos );                      
	vPos.z = (float)_tstof(strPos.GetBuffer(strPos.GetLength()));
	str.Delete( 0, nPos2 + 1 );
	
	g_EventScene.SetMovePoint( vPos );

	*pResult = 0;
}

void _DlgController::OnBnClickedButtonSetarea()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_bMouseMove )
		return;

	if( !m_bSetArea )
	{
		m_bArea = true;		
		m_SpinAreaRad.SetCsBuddy( &m_EditAreaRad, 1.0f, 1 );
		nsCsMfcFunc::Int2Control( &m_EditAreaRad, 100 );
		m_btArea.SetWindowText( L"영역제거" );	
		m_bSetArea = true;
		m_bMouseMove = true;
	}
	else
	{
		g_EventScene.ReleaseMovePoint();	
		m_btArea.SetWindowText( L"영역설정" );	
		m_bSetArea = false;
		_SetOnlyArea( false );
	}
	
}

void _DlgController::OnBnClickedEventsave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	 
	CEventSceneData::sEvent_Condition pCon;
	pCon.s_dwMapID = g_Terrain.m_dwMapId;
	pCon.s_bArea = m_bSetArea;
	pCon.s_bLoop = _IsConLoop();
	pCon.s_bOnlyArea = _IsOnlyArea();
	pCon.s_fRad = _GetAreaRad();
	pCon.s_ptPos = g_EventScene.GetAreaPoint();	
	pCon.s_bUseCamera = _IsUseCamera();

	std::stack< CEventData* > StackEventData; 
	int nCount = m_List_Player.GetCount();
	for( int i = 0; i < nCount; i++ )
	{
		UINT EventNum = (UINT)m_List_Player.GetItemData( i );
		CEventData* pData = g_EventScene.GetTempEventData( EventNum );			
		StackEventData.push( pData );
	}

	if( g_pEventDataMng->InsertData( (UINT)nsCsMfcFunc::Control2Int( &m_EditEventNum ), &pCon, &StackEventData ) )
	{
		g_pEventDataMng->SaveFile();	
	}
}

void _DlgController::OnBnClickedEventload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_DlgSceneLoadList ie;
	ie.Init( 0 );
	if( ie.DoModal() == IDCANCEL )
		return;	

	m_List_Scene.ResetContent();

	TCHAR sz[ 32 ];
	_stprintf_s( sz, 32, L"%d", m_nSceneNum );
	m_EditSceneNum.SetWindowText( sz );	

	CEventSceneData* pScene = g_pEventDataMng->LoadData( m_nSceneNum );
	
	for( int i = 0; i < (int)pScene->GetEventSize(); i++ )
	{
		CString str( pScene->GetEventDatabyIdx( i )->GetEventInfo()->s_strTitle );
		m_List_Scene.InsertString( i, str );
	}	
	assert_cs( pScene->GetEventSize() == m_List_Scene.GetCount() );
}

void _DlgController::OnBnClickedCheckListplay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	

	std::map< UINT, CEventSceneData* >::iterator it = g_pEventDataMng->GetMap()->find( m_nSceneNum );
	if( it == g_pEventDataMng->GetMap()->end() )
	{
		CsMessageBox( MB_OK, L"이벤트 실행하고 체크해주세요" );
		m_CheckPlay_Direct.SetCheck( BST_UNCHECKED );
		return;
	}

	g_pEventSceneMng->ResetScene();
	m_CheckPlay_Direct.SetCheck( BST_UNCHECKED );

	if( m_bMouseMove )
		return;

	if( g_pMainWnd->_GetDlgController()->_IsChrCamera() )
	{		
		m_bCharCamera = true;
	}
	else
	{
		g_EventScene.DeleteCameraTarget();
		g_EventScene.ResetFreeCamera( false );
	}	
}

void _DlgController::_SetStartCharEvent()
{
	// 해당이벤트 실행

	if( _IsChrCamera() )
	{
		std::map< UINT, CEventSceneData* >::iterator it = g_pEventDataMng->GetMap()->find( m_nSceneNum );
		assert_cs( it != g_pEventDataMng->GetMap()->end() );

		g_pEventSceneMng->InsertScene( m_nSceneNum );

		NiPoint3 vPos;
		vPos.x = it->second->GetCondition()->s_ptPos.x;
		vPos.y = it->second->GetCondition()->s_ptPos.y;			
		vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos.x,vPos.y );

		g_EventScene.SetPlayAreaPoint( vPos, it->second->GetCondition()->s_fRad );		
	}
	else
	{		
		m_EditSceneNum.SetWindowText( NULL );
		g_pEventSceneMng->ResetScene();
	}	
}

void _DlgController::OnBnClickedEventplayer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nIdx = m_List_Player.GetCurSel();	
	if( nIdx < 0 )
		return;
	
	m_List_Player.DeleteString( nIdx );	
}

void _DlgController::_SetInsertListPlayer( DWORD dwID, CString str )
{
	int nCount = m_List_Player.GetCount();
	for( int i = 0; i < nCount; i++ )
	{
		if( dwID == m_List_Player.GetItemData( i ) )
			return;
	}

	m_List_Player.InsertString( nCount, str );
	m_List_Player.SetItemData( nCount, dwID );
}


void _DlgController::OnBnClickedEventtalkload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	nsCsFileTable::g_pTalkMng->EventTalk_Reload();
	CsMessageBox( MB_OK, L"대사 로드 완료" );
}

void _DlgController::OnBnClickedEventpaste()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	_DlgEventLoadList ie;
	ie.Init( 0 );
	if( ie.DoModal() == IDCANCEL )
		return;
}

void _DlgController::OnNMRclickListEventplay(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	m_ListEventPlay.SetFocus();

	POSITION pos = m_ListEventPlay.GetFirstSelectedItemPosition();
	int nSelect = m_ListEventPlay.GetNextSelectedItem( pos );
	if( nSelect < 0 )
		return;

	_DlgTimeChange ie;	
	if( ie.DoModal() == IDCANCEL )
		return;

	CString str1, str2;
	str1 = m_ListEventPlay.GetItemText( nSelect, 0 );
	str2 = m_ListEventPlay.GetItemText( nSelect, 3 );
	float fOldTime = (float)_tstof(str2.GetBuffer(str2.GetLength())) - (float)_tstof(str1.GetBuffer(str1.GetLength()));

	float fTime = ie.GetTime();
	TCHAR szInt[ 32 ];	

	_stprintf_s( szInt, 32, L"%.2f", fTime );
	m_ListEventPlay.SetItemText( nSelect, 0, szInt );

	_stprintf_s( szInt, 32, L"%.3f", fTime + fOldTime );
	m_ListEventPlay.SetItem( nSelect, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );	

	_SetPlayListSort();

}

void _DlgController::OnEnChangeEditEventscenenum()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void _DlgController::OnBnClickedCheckUsecon()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( _IsOnlyArea() )
	{
		if( !m_bSetArea )
		{
			CsMessageBox( MB_OK, L"영역 설정이 안되있습니다" );
			_SetOnlyArea( false );
		}
	}

}

void _DlgController::OnEnChangeEditEventnum()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.
	

	if( g_pEventDataMng->IsData( (UINT)nsCsMfcFunc::Control2Int( &m_EditEventNum ) ) )
	{
		m_EditEventNumUse.SetWindowText( L"중복번호" );
	}
	else
	{	
		if( (UINT)nsCsMfcFunc::Control2Int( &m_EditEventNum ) < 1 || (UINT)nsCsMfcFunc::Control2Int( &m_EditEventNum ) > 160 )
		{
			m_EditEventNumUse.SetWindowText( L"사용불가" );
		}
		else
		{
			m_EditEventNumUse.SetWindowText( L"사용가능" );
		}
	}

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void _DlgController::OnBnClickedEventsavetemp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( CreateEventData() == true )
		g_EventScene.SaveTempEventData();
}

void _DlgController::OnBnClickedEventsound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_DlgSoundEvent ie;
	if( ie.DoModal() == IDCANCEL )
		return;
}

void _DlgController::OnBnClickedEventeffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_DlgEffectList ie;
	if( ie.DoModal() == IDCANCEL )
		return;
}

void _DlgController::OnBnClickedCheckFreemove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void _DlgController::OnBnClickedGetpos()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_bMouseMove )
		return;

	if( m_pTarget == NULL )
	{
		CsMessageBox( MB_OK, L"대상이 없네요.." );
		return;
	}

	m_bMouseMove = true;
	m_bEffectPos = true;
}



void _DlgController::OnBnClickedSceneDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_DlgSceneLoadList ie;
	ie.Init( 1 );
	if( ie.DoModal() == IDCANCEL )
		return;	
}

void _DlgController::OnBnClickedCheckDirectplay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( _IsDirectPlay() )	
	{
		std::map< UINT, CEventSceneData* >::iterator it = g_pEventDataMng->GetMap()->find( m_nSceneNum );
		if( it == g_pEventDataMng->GetMap()->end() )
		{
			CsMessageBox( MB_OK, L"이벤트 실행하고 체크해주세요" );
			m_CheckPlay_Direct.SetCheck( BST_UNCHECKED );
			return;
		}
		g_pEventSceneMng->DirectScenePlay( m_nSceneNum );
	}
	else
	{		
		m_EditSceneNum.SetWindowText( NULL );
		g_pEventSceneMng->ResetScene();
	}
}

void _DlgController::OnLbnSelchangeListPlayer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void _DlgController::OnEnChangeEditZdelta()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void _DlgController::OnBnClickedSetcameraButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void _DlgController::OnEnChangeEditScale()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
