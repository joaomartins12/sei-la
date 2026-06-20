

#include "stdafx.h"
#include "ViewViewer.h"
#include "NiMemTracker.h"
#include "NiStandardAllocator.h"
#include "ToolNiNode/UndoManager.h"
#include "Path/PathManager.h"
#include "CellObject/CellObjManager.h"

bool CViewViewer::CALLBACK_ThreadDelete( sTCUnit* pUnit )
{
	CsC_Object* pObject = pUnit->s_pLoadedObject;
	pObject->Delete();

	// 파트
	switch( pObject->GetLeafRTTI() )
	{
	case RTTI_MAPNPC:
		CNpc::DeleteInstance( (CNpc*)pObject );
		return true;
	}
	return false;
}


struct sMAPTOOL_DEBUG : public nsCSDEBUG::sDEBUG_FUNC{
	virtual void	AssertRelease(){ GET_MV_MAINTAB_DLG->_GetDlgMain()->OnBnClickedBtnSaveas(); }
};
sMAPTOOL_DEBUG			g_DebugFunc;

NiVisibleArray*			g_pVisible;
NiCullingProcessPtr		g_pCuller;
CsGBDX9RendererPtr		g_pRenderer;

float					g_fElapsedTime;

CViewViewer::CViewViewer()
:m_ResetCamHeightRate(0.3f)
{
	m_bEnableConfigKey = true;

	g_fElapsedTime = 0.0f;
}

CViewViewer::~CViewViewer()
{

}

//==========================================================================================
//
//		Delete
//
//==========================================================================================
void CViewViewer::_Delete()
{
	CViewBase::_Delete();

	g_TempRoot.DeleteTemp();

	g_Actor.Delete();
	m_SceneTool.Delete();
	m_ScenePreview.Delete();	
	g_ToolNiNodeMng.Delete();
	g_ToolObjMng.Delete();

	g_Sorting.Delete();

	CsGBPick::Delete();
	cRender::Delete();

	__DeleteCamera();
	_DeleteFont();
	__DeleteRenderer();

	nsCsFileTable::g_FileTableMng.Delete();

	CsC_Thread::GlobalShotDown();
	THREAD_MEMPOOL_DELETE( CNpc )

	g_MapMng.Delete();
}

void CViewViewer::__DeleteRenderer()
{
	NiDelete g_pVisible;
	g_pCuller = 0;
}

//==========================================================================================
//
//		Init
//
//==========================================================================================

void CViewViewer::_Init()
{
	nsCSDEBUG::g_pDebugFunc = &g_DebugFunc;

	CViewBase::_Init();
	GetClientRect( &m_rcView );

	__CreateRenderer();

	CsC_Thread::GlobalInit();
	g_pThread->UseThread( false );
	g_pThread->ResistCallBack_DeleteUnit( CALLBACK_ThreadDelete );

	// Shader	
	__InitShader();

	// Camera
	__CreateCamera();

	// Map Manager
	g_MapMng.Init(g_pRenderer, 8, true, true);

	// Pick	
	CsGBPick::Create(NiPick::FIND_ALL, NiPick::WORLD_COORDINATES, true, false);


	// FileTable
	nsCsFileTable::g_bUseBase = false;
	nsCsFileTable::g_bUseSkill = false;
	nsCsFileTable::g_bUseNature = false;
	nsCsMapTable::g_bUseWeather = true;
	nsCsFileTable::g_bUITextTBMng = false;
	nsCsFileTable::g_bUseItem = false;
	nsCsFileTable::g_bBuffMng = false;
	nsCsFileTable::g_bSceneDataMng = false;
	nsCsFileTable::g_bUseEvent = false;
	nsCsFileTable::g_bUseTotalExcel_None = false;
	nsCsFileTable::g_bUseMoveObject = false;
	nsCsFileTable::g_bUseQuest = false;
	nsCsFileTable::g_bReadInactiveQuest = false;

	nsCsFileTable::g_bUseEvoExtra = false;
	nsCsFileTable::g_bUseMessage = false;
	nsCsFileTable::g_bUseCuid = false;
	nsCsFileTable::g_bUseHelp = false;
	nsCsFileTable::g_bUseCashShop = false;
	nsCsFileTable::g_bAddExp = false;
	nsCsFileTable::g_bUseAchieve = false;
	nsCsFileTable::g_bUseGotcha = false;
	nsCsFileTable::g_bUseReward = false;
	nsCsFileTable::g_bUseMaster_CardMng = false;
	nsCsFileTable::g_bBattleTableMng = false;
	nsCsFileTable::g_bLoadServerTranferTBMng = false;
	nsCsFileTable::g_bDigimonParcelTBMng = false;
	nsCsFileTable::g_bInfiniteWarTBMng = false;
	nsCsFileTable::g_bExtraEvolutionTBMng = false;
	nsCsFileTable::g_bLoadCharCreateTBMng = false;

	Csini x( ".\\__ToolLocal.ini" );	
	nsCsFileTable::eLANGUAGE lan;

#ifdef VERSION_USA	//GSP 버전은 고정으로 영문버전
	lan = nsCsFileTable::ENGLISH;
	//CsMessageBox( MB_OK, L"이것은 영문 버젼이에요.~!" );

#elif VERSION_HK	// 홍콩 버전 _ 컨텐츠가 다르므로 별도 관리
	lan = nsCsFileTable::HONGKONG;
	//CsMessageBox( MB_OK, L"이것은 홍콩 버젼이에요.~!" );

#elif VERSION_TW	// 대만 버전 _ 컨텐츠가 다르므로 별도 관리
	lan = nsCsFileTable::TAIWAN;
	//CsMessageBox( MB_OK, L"이것은 대만 버젼이에요.~!" );

#elif VERSION_TH
	lan = nsCsFileTable::THAILAND;
	//CsMessageBox( MB_OK, L"สวัสดีครับ ยินดีที่ได้รู้จักครับ~!" );

#else
 	char* cLan = x.GetStr( "LANGUAGE", "tool" );
// 	if( cLan && ( strcmp( cLan, "ENGLISH" ) == 0 ) )
// 	{
// 		lan = nsCsFileTable::ENGLISH;	
// 		CsMessageBox( MB_OK, L"이것은 영문 버젼이에요.~!" );
// 	}
// 	else if( cLan && ( strcmp( cLan, "THAILAND" ) == 0 ) )
// 	{
// 		lan = nsCsFileTable::THAILAND;
// 		CsMessageBox( MB_OK, L"สวัสดีครับ ยินดีที่ได้รู้จักครับ~!" );		
// 	}
// 
// 	else
	{
		lan = nsCsFileTable::KOREA_TRANSLATION;
		//CsMessageBox( MB_OK, L"이것은 한국 버젼이에요.~!" );		
	}
#endif

/*#ifdef _DEBUG
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, lan );
#else
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, lan );
#endif*/ /*_DEBUG*/

	nsCsFileTable::g_FileTableMng.Init(nsCsFileTable::FT_FILEPACK, lan);

	cRender::Init();
	g_ToolObjMng.Init();
	m_SceneTool.Init();
	m_ScenePreview.Init();
	g_ToolNiNodeMng.Init();	
	GET_MV_MAINTAB_DLG->_ViewInit();

	_CreateFont();

	CsMFC::Init();
}

bool	ResetDevice( bool bBeforeReset, void* pvData )
{
	if( bBeforeReset == true )
	{
		GET_MV_VIEW->_DeleteFont();
	}
	else
	{
		GET_MV_VIEW->_CreateFont();
	}
	return true;
}

void CViewViewer::__CreateRenderer()
{
	NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions(	NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(),	false) );	
	NiInit(pkInitOptions);

	NiCollisionSDM::Init();	

	NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);	
	NiTexture::SetMipmapByDefault(false);
	NiSourceTexture::SetUseMipmapping(false);
	NiMaterial::SetDefaultWorkingDirectory(".\\Data\\Shaders\\Generated");	
	NiSourceTexture::SetDestroyAppDataFlag( false );

	// 렌더러
	g_pRenderer = CsGBDX9Renderer::CsGBCreate( VIEWER_WIDTH, VIEWER_HEIGHT, NiDX9Renderer::USE_MULTITHREADED | NiDX9Renderer::USE_STENCIL, GetSafeHwnd(), NULL, 0,
		NiDX9Renderer::DEVDESC_HAL_HWVERTEX, NiDX9Renderer::FBFMT_UNKNOWN, NiDX9Renderer::DSFMT_UNKNOWN, NiDX9Renderer::PRESENT_INTERVAL_ONE,
		NiDX9Renderer::SWAPEFFECT_COPY,	NiDX9Renderer::FBMODE_MULTISAMPLES_4
		);

	NiAlphaAccumulator* pkAccum = NiNew NiAlphaAccumulator;
	g_pRenderer->SetSorter( pkAccum );

	g_pRenderer->AddResetNotificationFunc( ::ResetDevice, NULL );
	g_pRenderer->SetBackgroundColor( NiColor( 0.2f, 0.2f, 0.2f ) );


	g_pVisible = NiNew NiVisibleArray(512, 128);
	g_pCuller = NiNew NiCullingProcess( g_pVisible );	
}

bool CViewViewer::__InitShader()
{
	char cShader[ MAX_PATH ] = "Data\\Shaders";
	char* apcShaderDirectories[1];
	apcShaderDirectories[0] = cShader;
	
	return m_ShaderHelper.SetupShaderSystem( NULL, 0, apcShaderDirectories, 1 );
}

void CViewViewer::_PreDelete()
{
	g_Actor.Delete();

	g_ToolObjMng.PreViewDelete();
}

void CViewViewer::_PreReset()
{
	GET_MV_MAINTAB_DLG->_PreViewReset();	
	g_ToolObjMng.PreViewReset();	
}

void CViewViewer::_PostReset()
{
	g_Actor.Init();

	GET_MV_MAINTAB_DLG->_PostViewReset();
	GET_MV_OBJECTLIST_DLG->_PostViewReset();

	g_ToolObjMng.PostViewReset();

	m_bEnableConfigKey = true;
	g_fElapsedTime = 0.0f;

	_ResetCamera();

	switch( nsCsGBTerrain::g_eCreateType )
	{
	case nsCsGBTerrain::CT_MAKE:
	case nsCsGBTerrain::CT_FILELOAD_BYTOOL:
		_ResetAutoSave();
		m_SceneTool.Reset();		
		break;
	case nsCsGBTerrain::CT_FILELOAD:
		m_ScenePreview.Reset();
		break;
	}

	// Fog
	nsCsGBTerrain::g_pCurRoot->GetFogProperty()->SetFog( false );	
}


//==========================================================================================
//
//		Update
//
//==========================================================================================
void CViewViewer::_Update()
{
	Sleep( g_Config.GetEnvironment()->s_nSleepTick );

	static float fOldTimeInSec = NiGetCurrentTimeInSec();
	float fCurTimeInSec = NiGetCurrentTimeInSec();
	g_fElapsedTime = fCurTimeInSec - fOldTimeInSec;
	fOldTimeInSec = fCurTimeInSec;
	
	switch( nsCsGBTerrain::g_eCreateType )
	{
	case nsCsGBTerrain::CT_NONE:
		g_pRenderer->BeginFrame();
		g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );
		g_pRenderer->EndUsingRenderTargetGroup();
		g_pRenderer->EndFrame();
		g_pRenderer->DisplayFrame();
		break;
	case nsCsGBTerrain::CT_MAKE:
	case nsCsGBTerrain::CT_FILELOAD_BYTOOL:
		__Update_Tool();
		CAMERA_ST.Update_ByTool( g_fElapsedTime );
		__Render_Tool();
		break;
	case nsCsGBTerrain::CT_FILELOAD:		
		__Update_Preview();
		CAMERA_ST.Update_ByTool( g_fElapsedTime );
		__Render_Preview();
		break;
	}	
}



//==========================================================================================
//
//		Camera
//
//==========================================================================================

void CViewViewer::__DeleteCamera()
{
	CAMERA_ST.Delete();
	DESTROY_SINGLETON( CAMERA_STPTR );
	DESTROY_SINGLETON (UNDO::Manager::GetSingletonPtr());
	DESTROY_SINGLETON (PATH::Manager::GetSingletonPtr());
	DESTROY_SINGLETON (CELLOBJ::Manager::GetSingletonPtr());
}

void CViewViewer::__CreateCamera()
{
	CREATE_SINGLETON( CsGBCamera );
	CREATE_SINGLETON (UNDO::Manager);
	CREATE_SINGLETON (PATH::Manager);
	CREATE_SINGLETON (CELLOBJ::Manager);
	PATH::Manager::GetSingletonPtr()->Init();
	CELLOBJ::Manager::GetSingletonPtr()->Init();
	CAMERA_ST.Create( g_pRenderer );
	_SetCameraAccel();
}

void CViewViewer::_ResetCamera()
{
	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	sCAMERAINFO ci;

	NiPoint3 SelectedCenter = g_ObjectMng.GetSelectedCenter();
	float fHalfHeight = (pInfo->s_nYVertCount-1)*pInfo->s_nGridSize*0.5f;
	if( SelectedCenter != NiPoint3::ZERO)
	{
		ci.s_ptInitPos = SelectedCenter;
		ci.s_fDist = (fHalfHeight/tan( CsD2R( CAMERA_ST.GetCameraInfo()->s_fFOV*0.5f )) * m_ResetCamHeightRate);	
	}
	else
	{
		ci.s_ptInitPos = NiPoint3( (pInfo->s_nXVertCount-1)*pInfo->s_nGridSize*0.5f, fHalfHeight, 0.0f );
		ci.s_fDist = fHalfHeight/tan( CsD2R( CAMERA_ST.GetCameraInfo()->s_fFOV*0.5f ) );	
	}
	CAMERA_ST.Reset( &ci );
	CAMERA_ST.SetAttachActor_ByTool( false );
	CAMERA_ST.SetDAAConstant( CAMERA_DIST_ANI_ACCEL_CONSTANT_TOOL );
	CAMERA_ST.SetDistRange( 250, 100000 );
	
	_SetCameraAccel();
}

void CViewViewer::__SetCameraAccel( bool bSpeedUp )
{
	if( g_Actor.IsEnableActor() == true )
		return;

// 	if( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD )
// 	{
		CAMERA_ST.SetTranslateAccel( 5.0f );
		CAMERA_ST.SetDistAccel( 1.5f );
// 		return;
// 	}

// 	if( bSpeedUp == false )
// 	{
// 		float fMoveDist = CAMERA_ST.GetDist()*0.0007f;
// 		CAMERA_ST.SetTranslateAccel( fMoveDist );
// 		CAMERA_ST.SetDistAccel( fMoveDist );
// 	}
// 	else
// 	{
// 		float fMoveDist = CAMERA_ST.GetDist()*0.0007f;
// 		CAMERA_ST.SetTranslateAccel( fMoveDist*3.0f );
// 		CAMERA_ST.SetDistAccel( fMoveDist*3.0f );
// 	}
}

//==============================================================================
//
//
//
//	Tool
//
//
//
//==============================================================================

void CViewViewer::__Update_Tool()
{
	CViewBase::_Update();

	if( g_Actor.IsEnableActor() == false )
		g_MapMng.Update_ByTool( g_fElapsedTime );
	else
		g_MapMng.Update( g_fElapsedTime );

	g_Actor.Update();

	GET_MV_MAINTAB_DLG->_UpdateView();
	g_ToolObjMng.Update();	
	g_ToolNiNodeMng.Update();

	__UpdateAutoSave();
}
void CViewViewer::RenderObject(void)
{
	if( theApp.GetShowObj() == true && 
		GET_MV_MAINTAB_DLG->_GetDlgPath() != NULL)
	{
		NiNode* pkNode = GET_MV_MAINTAB_DLG->_GetDlgPath()->GetPathObjRoot();
		if(pkNode){
			NiDrawScene( CAMERA_ST.GetCameraObj(), pkNode, *g_pCuller );
		}
	}
}
void CViewViewer::RenderMonsterRegion(void)
{
	if( theApp.GetShowMonsterRegion() == true && 
		GET_MV_MAINTAB_DLG->_GetDlgMonster() != NULL ){
		GET_MV_MAINTAB_DLG->_GetDlgMonster()->__RenderResist(false);
	}
}

void CViewViewer::__Render_Tool()
{
	g_pRenderer->BeginFrame();
	g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );

	GET_MV_MAINTAB_DLG->_PreRenderView();

	m_SceneTool.Render();	

	// 일반 다른 랜더링은 여기서 그리자
	GET_MV_MAINTAB_DLG->_PostRenderView();
	RenderMonsterRegion();

	g_ToolObjMng.Render();
	RenderObject();

	g_ToolNiNodeMng.Render();	
	
	// Z버퍼 클리어
	g_pRenderer->EndUsingRenderTargetGroup();
	g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ZBUFFER );
	g_ToolNiNodeMng.RenderClearZBuffer();

	// Z버퍼 한번더 클리어
	g_pRenderer->EndUsingRenderTargetGroup();
	g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ZBUFFER );
	

	// 폰트 랜더링
	_RenderFont();

	g_pRenderer->EndUsingRenderTargetGroup();
	g_pRenderer->EndFrame();
	g_pRenderer->DisplayFrame();	
}

//==========================================================================================
//
//		AutoSave
//
//==========================================================================================
void CViewViewer::_ResetAutoSave()
{
	m_fAutoSaveTime = g_Config.GetAutoSave()->s_fAutoSaveTime;
}

void CViewViewer::__UpdateAutoSave()
{
	if( ( nsCsGBTerrain::g_pCurRoot == NULL )||( nsCsGBTerrain::g_pCurRoot->GetRootNode()->IsExistTerrain() == false ) )
		return;

	if( g_Config.GetAutoSave()->s_bAutoSave == false )
		return;

	if( ( m_fAutoSaveTime -= g_fElapsedTime ) < 0.0f )
	{
		char cSave[ MAX_PATH ] = {0, };
		strcpy_s( cSave, MAX_PATH, nsCsGBTerrain::g_pCurRoot->GetMapFilePath() );

		bool bAutoSave = true;
		TCHAR sz[ MAX_PATH ];
		_stprintf_s( sz, MAX_PATH, L".\\ToolData\\AutoBackup%d.nif", g_Config.GetAutoSave()->s_nSaveIndex );
		_DlgNotice dn( _DlgNotice::NT_SAVE, sz, &bAutoSave );
		dn.DoModal();
		nsCsGBTerrain::g_pCurRoot->SetMapFilePath( cSave );

		if( ( ++g_Config.GetAutoSave()->s_nSaveIndex ) > 10 )
		{
			g_Config.GetAutoSave()->s_nSaveIndex = 0;
		}
	}
}

//==========================================================================================
//
//		Config Key
//
//==========================================================================================
bool CViewViewer::__CheckConfigKey( DWORD vKeyCode, bool bDown )
{
	for( DWORD i=0; i<KS_COUNT; ++i )
	{
		if( g_Config.GetKeySet()->s_KeyInfo[ i ].s_btKey == vKeyCode )
		{
			switch( g_Config.GetKeySet()->s_KeyInfo[ i ].s_dwPlag )
			{
			case 0:
				if( ( _IsShift() == false )&&( _IsControl() == false )&&( _IsAlt() == false ) )
				{
					if( bDown == true )
						__DoConfigKeyDown( (eKEY_SET)i );
					else
						__DoConfigKeyUp( (eKEY_SET)i );
					return true;
				}
				break;
			case KP_SHIPT:
				if( ( _IsShift() == true )&&( _IsControl() == false )&&( _IsAlt() == false ) )
				{
					if( bDown == true )
						__DoConfigKeyDown( (eKEY_SET)i );
					else
						__DoConfigKeyUp( (eKEY_SET)i );
					return true;
				}
				break;
			case KP_CONTROL:
				if( ( _IsShift() == false )&&( _IsControl() == true )&&( _IsAlt() == false ) )
				{
					if( bDown == true )
						__DoConfigKeyDown( (eKEY_SET)i );
					else
						__DoConfigKeyUp( (eKEY_SET)i );
					return true;
				}
				break;
			case KP_ALT:
				if( ( _IsShift() == false )&&( _IsControl() == false )&&( _IsAlt() == true ) )
				{
					if( bDown == true )
						__DoConfigKeyDown( (eKEY_SET)i );
					else
						__DoConfigKeyUp( (eKEY_SET)i );
					return true;
				}
				break;
			}
		}
	}
	return false;
}

void CViewViewer::__DoConfigKeyDown( eKEY_SET eKey )
{
	switch( eKey )
	{
	case KS_SLIDE_B_LEFT:
		GET_MV_MAINTAB_DLG->_OnSlide_BLeft();
		break;
	case KS_SLIDE_B_RIGHT:
		GET_MV_MAINTAB_DLG->_OnSlide_BRight();
		break;	
	case KS_SLIDE_S_LEFT:
		GET_MV_MAINTAB_DLG->_OnSlide_SLeft();
		break;
	case KS_SLIDE_S_RIGHT:
		GET_MV_MAINTAB_DLG->_OnSlide_SRight();
		break;
	case KS_TER_NODE_CHILD:
		m_SceneTool.SetEnableZoom( true );
		break;
	case KS_OBJ_P_ROT:
		switch( GET_MV_MAINTAB_DLG->_GetSelTabIndex() )
		{
		case _DlgMainTab::TT_OBJECT:
			g_ObjectMng.RotationModelOnKey( true );
			break;
		case _DlgMainTab::TT_MODEL:
			g_ModelMng.RotationModelNpcOnKey( true );
			break;
		case _DlgMainTab::TT_PATH:
			_DlgMTPath::MoveCallBack_ERot( 45.0f );
			break;
		case _DlgMainTab::TT_CELLOBJECT:
			GET_MV_MAINTAB_DLG->_GetDlgCellObject()->_Rot( 45.0f );
			break;
		}
		break;
	case KS_OBJ_M_ROT:
		switch( GET_MV_MAINTAB_DLG->_GetSelTabIndex() )
		{
		case _DlgMainTab::TT_OBJECT:
			g_ObjectMng.RotationModelOnKey( false );
			break;
		case _DlgMainTab::TT_MODEL:
			g_ModelMng.RotationModelNpcOnKey( false );
			break;
		case _DlgMainTab::TT_PATH:
			_DlgMTPath::MoveCallBack_ERot( -45.0f );
			break;
		case _DlgMainTab::TT_CELLOBJECT:
			GET_MV_MAINTAB_DLG->_GetDlgCellObject()->_Rot( -45.0f );
			break;
		}
		break;
	case KS_RB_UNDO:
		//g_RollBack.UnDo();
		break;
	case KS_RB_REDO:
		//g_RollBack.ReDo();
		break;
	case KS_FILE_SAVEAS:
		GET_MV_MAINTAB_DLG->_GetDlgMain()->OnBnClickedBtnSave();
		break;
	case KS_FILE_LOADAS:
		GET_MV_MAINTAB_DLG->_GetDlgMain()->OnBnClickedBtnLoad();
		break;
	}
}

void CViewViewer::__DoConfigKeyUp( eKEY_SET eKey )
{
	static bool bFullScreen = false;

	switch( eKey )
	{
	case KS_RB_UNDO:
		UNDO::Manager::GetSingletonPtr()->ExcuteUndo();
		break;
	case KS_RB_REDO:
		UNDO::Manager::GetSingletonPtr()->ExcuteRedo();
		break;

	case KS_OBJ_DEL:
		switch( GET_MV_MAINTAB_DLG->_GetSelTabIndex() )
		{
		case _DlgMainTab::TT_OBJECT:
			g_ObjectMng.DeleteSelectObject();
			break;
		case _DlgMainTab::TT_MODEL:
			if( g_ModelMng.GetSelectNpc() != NULL )
			{
				GET_MV_MAINTAB_DLG->_GetDlgModel()->_UnresistNpcList( g_ModelMng.GetSelectNpc()->GetFTID() );
				g_ModelMng.DeleteSelectNpc();
			}			
			break;
		case _DlgMainTab::TT_CELLOBJECT:
			GET_MV_MAINTAB_DLG->_GetDlgCellObject()->_Del();
			break;
		}
		break;
	case KS_OBJ_DETACH:
		switch( GET_MV_MAINTAB_DLG->_GetSelTabIndex() )
		{
		case _DlgMainTab::TT_OBJECT:
			g_ObjectMng.DetachSelectObject();
			break;
		case _DlgMainTab::TT_MODEL:
			g_ModelMng.SelectNpcToModelNpc();
			break;
		case _DlgMainTab::TT_CELLOBJECT:
			GET_MV_MAINTAB_DLG->_GetDlgCellObject()->_Cut();
			break;
		}		
		break;
	case KS_OBJ_INIT_ROT:
		switch( GET_MV_MAINTAB_DLG->_GetSelTabIndex() )
		{
		case _DlgMainTab::TT_OBJECT:
			g_ObjectMng.ResetTransformSelectObject();
			break;
		}		
		break;
	case KS_TER_NODE_CHILD:
		m_SceneTool.SetEnableZoom( false );
		break;
	case KS_TER_NODE_PARENT:
		GET_MV_NODE->_SelectParentNode();
		break;
	case KS_OBJ_COPY:
		switch( GET_MV_MAINTAB_DLG->_GetSelTabIndex() )
		{
		case _DlgMainTab::TT_OBJECT:
			g_ObjectMng.SelectModelCopyResistModel();
			break;
		case _DlgMainTab::TT_CELLOBJECT:
			GET_MV_MAINTAB_DLG->_GetDlgCellObject()->_Copy();
			break;
		}
		break;
	case KS_OBJ_RENDERING:
		GET_MV_MAINTAB_DLG->_GetDlgObejct()->_ToggleRenderObject();
		break;
	case KS_TAB_SEL1:
		GET_MV_MAINTAB_DLG->_GetTabCtrl()->SetCurSel( _DlgMainTab::TT_MAIN );
		GET_MV_MAINTAB_DLG->OnTcnSelchangeTabMain( NULL, NULL );
		break;	
	case KS_OBJ_SAVE1:	g_ObjectMng.SelectModelCopySaveModel( 0 );		break;
	case KS_OBJ_SAVE2:	g_ObjectMng.SelectModelCopySaveModel( 1 );		break;
	case KS_OBJ_SAVE3:	g_ObjectMng.SelectModelCopySaveModel( 2 );		break;
	case KS_OBJ_SAVE4:	g_ObjectMng.SelectModelCopySaveModel( 3 );		break;
	case KS_OBJ_SAVE5:	g_ObjectMng.SelectModelCopySaveModel( 4 );		break;
	case KS_OBJ_SAVE6:	g_ObjectMng.SelectModelCopySaveModel( 5 );		break;
	case KS_OBJ_SAVE7:	g_ObjectMng.SelectModelCopySaveModel( 6 );		break;
	case KS_OBJ_SAVE8:	g_ObjectMng.SelectModelCopySaveModel( 7 );		break;
	case KS_OBJ_SAVE9:	g_ObjectMng.SelectModelCopySaveModel( 8 );		break;
	case KS_OBJ_SAVE10:	g_ObjectMng.SelectModelCopySaveModel( 9 );		break;
	case KS_OBJ_LOAD1:	g_ObjectMng.SaveModelCopySelectModel( 0 );		break;
	case KS_OBJ_LOAD2:	g_ObjectMng.SaveModelCopySelectModel( 1 );		break;
	case KS_OBJ_LOAD3:	g_ObjectMng.SaveModelCopySelectModel( 2 );		break;
	case KS_OBJ_LOAD4:	g_ObjectMng.SaveModelCopySelectModel( 3 );		break;
	case KS_OBJ_LOAD5:	g_ObjectMng.SaveModelCopySelectModel( 4 );		break;
	case KS_OBJ_LOAD6:	g_ObjectMng.SaveModelCopySelectModel( 5 );		break;
	case KS_OBJ_LOAD7:	g_ObjectMng.SaveModelCopySelectModel( 6 );		break;
	case KS_OBJ_LOAD8:	g_ObjectMng.SaveModelCopySelectModel( 7 );		break;
	case KS_OBJ_LOAD9:	g_ObjectMng.SaveModelCopySelectModel( 8 );		break;
	case KS_OBJ_LOAD10:	g_ObjectMng.SaveModelCopySelectModel( 9 );		break;
	case KS_OBJ_SELECTALL:	g_ObjectMng.SelectObject_SelectAll();		break;
	case KS_OBJ_TERRAIN_ATTACH:
		if( GET_MV_MAINTAB_DLG->_GetSelTabIndex() == _DlgMainTab::TT_OBJECT )
			g_ObjectMng.SelectObject_AttachTerrain();
		break;
	case KS_FULLSCREEN:
		if( bFullScreen == false )
		{
			g_pRenderer->Recreate( 1280, 1024 );
			CAMERA_ST.ChangeStandardFov( 1280, 1024 );
			GET_MV_VIEWFRAME->SetWindowPos( &wndTop, 0, 0, 1280, 1024, 0 );
// 			MAP_CHILDFRM_IT it = theApp._GetMapFrame()->begin();
// 			for( ; it!=theApp._GetMapFrame()->end(); ++it )
// 			{
// 				if( it->second != GET_MV_VIEWFRAME )
// 				{
// 					it->second->ShowWindow( SW_HIDE );
// 				}
// 			}
			GetClientRect( &m_rcView );
			bFullScreen = true;
		}
		else
		{
			g_pRenderer->Recreate( 800, 600 );
			CAMERA_ST.ChangeStandardFov( 800, 600 );
			GET_MV_VIEWFRAME->SetWindowPos( &wndTop, 0, 0, 800, 600, 0 );
// 			MAP_CHILDFRM_IT it = theApp._GetMapFrame()->begin();
// 			for( ; it!=theApp._GetMapFrame()->end(); ++it )
// 			{
// 				if( it->second != GET_MV_VIEWFRAME )
// 				{
// 					it->second->ShowWindow( SW_SHOW );
// 				}
// 			}
			GetClientRect( &m_rcView );
			bFullScreen = false;
		}
		break;
	}
}

//==============================================================================
//
//
//
//	Preview
//
//
//
//==============================================================================

void CViewViewer::__Update_Preview()
{
	g_MapMng.Update( g_fElapsedTime );
	m_ScenePreview.Update();
}

void CViewViewer::__Render_Preview()
{
	g_pRenderer->BeginFrame();
	g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );

	m_ScenePreview.Render();
	// 폰트 랜더링
	_RenderFont();

	g_pRenderer->EndUsingRenderTargetGroup();
	g_pRenderer->EndFrame();
	g_pRenderer->DisplayFrame();
}


//==============================================================================
//
//
//
//	Etc
//
//
//
//==============================================================================

//==========================================================================================
//
//		Font
//
//==========================================================================================

void CViewViewer::_CreateFont()
{
	D3DXCreateFont( g_pRenderer->GetD3DDevice(), 15, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_pFont );
}

void CViewViewer::_RenderFont()
{
	static D3DCOLOR fontColor = D3DCOLOR_ARGB(255,255,255,0);
	static TCHAR szMsg[MAX_PATH] = TEXT("");
	static float fFrame = 0.0f;
	static float fLastTime = 0.0f;
	static DWORD dwFrames  = 0;
	static CRect rct = CRect( 10, 20, 100, 200 );

	float fTime = NiGetCurrentTimeInSec();

	++dwFrames;
	// Update the scene stats once per second
	if( fTime - fLastTime > 1.0f )
	{
		fFrame = dwFrames / (fTime - fLastTime);
		fLastTime = fTime;
		dwFrames  = 0;
	}	
	_stprintf_s( szMsg, MAX_PATH, L"%.1f", fFrame );
	m_pFont->DrawText( NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor );
}

void CViewViewer::_DeleteFont()
{
	SAFE_RELEASE( m_pFont );
}


//==========================================================================================
//
//		Mouse
//
//==========================================================================================
void CViewViewer::_OnLButtonDown( CPoint pt )
{
	SetCapture();

	if( g_TempRoot.IsLoad() == true )
	{
		g_TempRoot.OnLButtonDown( pt );
	}
	else
	{
		GET_MV_MAINTAB_DLG->_OnLButtonDown( pt );

		if( PtInRect( &m_rcView, pt ) == TRUE )
		{
			// Camera
			CAMERA_ST.OnLButtonDown( pt );

			switch( nsCsGBTerrain::g_eCreateType )
			{
			case nsCsGBTerrain::CT_MAKE:
			case nsCsGBTerrain::CT_FILELOAD_BYTOOL:
				__OnLButtonDown_Tool( pt );
				break;
			case nsCsGBTerrain::CT_FILELOAD:
				__OnLButtonDown_Preview( pt );
				break;
			}
		}

		g_ToolObjMng.LButtonDown( pt );
	}	
}

void CViewViewer::_OnLButtonDBCLick(CPoint pt)
{
	if( !PtInRect( &m_rcView, pt ) )
		return;

	GET_MV_MAINTAB_DLG->_OnLButtonDbClick( pt );
}

void CViewViewer::_OnLButtonUp( CPoint pt )
{
	ReleaseCapture();

	// Camera
	CAMERA_ST.OnLButtonUp( pt );
	GET_MV_MAINTAB_DLG->_OnLButtonUp( pt );

	switch( nsCsGBTerrain::g_eCreateType )
	{
	case nsCsGBTerrain::CT_MAKE:
	case nsCsGBTerrain::CT_FILELOAD_BYTOOL:
		__OnLButtonUp_Tool( pt );
		break;
	case nsCsGBTerrain::CT_FILELOAD:
		__OnLButtonUp_Preview( pt );
		break;
	}
	
}

void CViewViewer::_OnMButtonDown( CPoint pt )
{
	SetCapture();

	if( PtInRect( &m_rcView, pt ) == TRUE )
	{
		switch( nsCsGBTerrain::g_eCreateType )
		{
		case nsCsGBTerrain::CT_MAKE:
		case nsCsGBTerrain::CT_FILELOAD_BYTOOL:
			__OnMButtonDown_Tool( pt );
			// Camera
			CAMERA_ST.OnMButtonDown( pt );
			break;
		}
	}
}

void CViewViewer::_OnMButtonUp( CPoint pt )
{
	ReleaseCapture();

	// Camera
	CAMERA_ST.OnMButtonUp( pt );

	switch( nsCsGBTerrain::g_eCreateType )
	{
	case nsCsGBTerrain::CT_MAKE:
	case nsCsGBTerrain::CT_FILELOAD_BYTOOL:
		__OnMButtonUp_Tool( pt );
		break;
	}
}

void CViewViewer::_OnRButtonDown( CPoint pt )
{
	SetCapture();

	if( PtInRect( &m_rcView, pt ) == TRUE )
	{
		// Camera
		CAMERA_ST.OnRButtonDown( pt );

		switch( nsCsGBTerrain::g_eCreateType )
		{
		case nsCsGBTerrain::CT_MAKE:
		case nsCsGBTerrain::CT_FILELOAD_BYTOOL:
			__OnRButtonDown_Tool( pt );
			break;
		case nsCsGBTerrain::CT_FILELOAD:
			__OnRButtonDown_Preview( pt );
			break;
		}
	}
}

void CViewViewer::_OnRButtonUp( CPoint pt )
{
	ReleaseCapture();

	// Camera
	CAMERA_ST.OnRButtonUp( pt );

	switch( nsCsGBTerrain::g_eCreateType )
	{
	case nsCsGBTerrain::CT_MAKE:
	case nsCsGBTerrain::CT_FILELOAD_BYTOOL:
		__OnRButtonUp_Tool( pt );
		break;
	case nsCsGBTerrain::CT_FILELOAD:
		__OnRButtonUp_Preview( pt );
		break;
	}
}

void CViewViewer::_OnMouseMove( CPoint pt )
{
	// Camera
	if( _IsShift() )
		CAMERA_ST.OnMouseMove( pt, 10.0f );
	else
		CAMERA_ST.OnMouseMove( pt );


	GET_MV_MAINTAB_DLG->_OnMouseMove( pt );

	switch( nsCsGBTerrain::g_eCreateType )
	{
	case nsCsGBTerrain::CT_MAKE:
	case nsCsGBTerrain::CT_FILELOAD_BYTOOL:
		__OnMouseMove_Tool( pt );
		break;
	case nsCsGBTerrain::CT_FILELOAD:
		__OnMouseMove_Preview( pt );
		break;
	}

	g_ToolObjMng.MouseMove( pt );	
}

void CViewViewer::_OnMouseWheel( short zDelta )
{
	// Camera
	_SetCameraAccel();
	if( _IsShift() )
		CAMERA_ST.OnMouseWheel( zDelta * 3 );
	else
		CAMERA_ST.OnMouseWheel( zDelta );

	switch( nsCsGBTerrain::g_eCreateType )
	{
	case nsCsGBTerrain::CT_MAKE:
	case nsCsGBTerrain::CT_FILELOAD_BYTOOL:
		__OnMouseWheel_Tool( zDelta );
		break;
	case nsCsGBTerrain::CT_FILELOAD:
		__OnMouseWheel_Preview( zDelta );
		break;
	}	
}

void CViewViewer::ChangeCameraPos(NiTransform trTarget )
{
	CAMERA_ST.SetTranslate( trTarget.m_Translate );
	CAMERA_ST._UpdateCamera();
}

//==========================================================================================
//
//		Key
//
//==========================================================================================
bool CViewViewer::_KeyDown( DWORD vKeyCode )
{
	if( m_bEnableConfigKey == true )
	{
		if( GET_MV_MAINTAB_DLG->_PreCheckKey( vKeyCode, true ) == true )
			return true;

		if( __CheckConfigKey( vKeyCode, true ) == true )
			return true;
	}

	switch( vKeyCode )
	{
	case VK_RETURN:
		return true;

	case VK_ESCAPE:
		GET_MV_MAINTAB_DLG->_KeyDownEscape();
		switch( nsCsGBTerrain::g_eCreateType )
		{
		case nsCsGBTerrain::CT_FILELOAD:
			m_ScenePreview.KeyEscape();
			break;
		}
		return true;

	case VK_CONTROL:
		__SetCameraAccel( true );
		GET_MV_OBJECTCONTROL_DLG->_SetControl( true );		
		break;

	case VK_SHIFT:
		break;
	}

	return false;
}

bool CViewViewer::_KeyUp( DWORD vKeyCode )
{
	if( m_bEnableConfigKey == true )
	{
		GET_MV_MAINTAB_DLG->OnKeyUp(vKeyCode);
		if( GET_MV_MAINTAB_DLG->_PreCheckKey( vKeyCode, false ) == true )
			return true;

		if( __CheckConfigKey( vKeyCode, false ) == true )
			return true;
	}

	switch( vKeyCode )
	{
	case VK_ESCAPE:
		return true;

	case VK_RETURN:
		return true;

	case VK_SHIFT:
		break;

	case VK_CONTROL:
		__SetCameraAccel( false );
		GET_MV_OBJECTCONTROL_DLG->_SetControl( false );
		break;
	}

	return false;
}

void CViewViewer::_SysKeyDown( DWORD vKeyCode )
{
	if( m_bEnableConfigKey == true )
	{
		__CheckConfigKey( vKeyCode, true );
	}	
}

void CViewViewer::_SysKeyUp( DWORD vKeyCode )
{
	if( m_bEnableConfigKey == true )
	{
		__CheckConfigKey( vKeyCode, false );
	}
}

BEGIN_MESSAGE_MAP(CViewViewer, CViewBase)
	ON_WM_MOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CViewViewer::OnMove(int x, int y)
{
	CViewBase::OnMove(x, y);
}


void CViewViewer::OnSize(UINT nType, int cx, int cy)
{
	CViewBase::OnSize(nType, cx, cy);

	if( g_pRenderer )
	{
		g_pRenderer->Recreate( cx, cy );

		CAMERA_ST.ChangeStandardFov( (float)cx, (float)cy );
		g_Preview.OnMainViewerSizeChange(cx, cy);
		//GET_MV_VIEWFRAME->SetWindowPos( &wndTop, 0, 0, cx, cy, 0 );
		GetClientRect( &m_rcView );
	}
}

void CViewViewer::_SetControl( bool bControl )
{
	if( IS_ENABLEFRM( nsMAPTOOL::MV_VIEWER ) == false )
		return;

	__SetCameraAccel( bControl );
	GET_MV_OBJECTCONTROL_DLG->_SetControl( bControl );
}

void CViewViewer::UpdateResetCamHeightRate(float fRate)
{
	m_ResetCamHeightRate = fRate;
	if( ( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_MAKE )||
		( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD_BYTOOL ) )
	{
		_ResetCamera();
	}
}
