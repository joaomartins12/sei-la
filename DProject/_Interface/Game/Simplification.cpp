
#include "stdafx.h"
#include "Simplification.h"

#ifdef SIMPLEFICATION

#ifdef VERSION_USA
	#define IF_SIMPLE_BTN_SIZE	CsPoint( 77, 24 )
	#define IF_SIMPLE_BTN_POS	CsPoint( 0, 0 )
	#define IF_SIMPLE_BG_POS	CsPoint( 940, 614 )

#elif VERSION_KOR
	#define IF_SIMPLE_BTN_SIZE	CsPoint( 77, 24 )
	#define IF_SIMPLE_BTN_POS	CsPoint( 65, 6 )
	#define IF_SIMPLE_BG_POS	CsPoint( 810, 593 )

#else
	#define IF_SIMPLE_BTN_SIZE	CsPoint( 77, 24 )
	#define IF_SIMPLE_BTN_POS	CsPoint( 65, 6 )
	#define IF_SIMPLE_BG_POS	CsPoint( 397, 768 - 185 )
#endif

cSimplification::cSimplification()
{
	m_bLBtnDown = false;
}

void cSimplification::Destroy()
{
	cBaseWindow::Delete();	
}

void cSimplification::DeleteResource()
{
	DeleteScript();
	SAFE_NIDELETE(m_pSimpleChangeBtn);
	SAFE_NIDELETE(m_pSimpleCancleBtn);

#ifdef VERSION_USA
	SAFE_NIDELETE( m_pBG );
#endif

}

void cSimplification::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

#ifdef VERSION_KOR
	InitScript( "Simple\\simple_is_best.tga", IF_SIMPLE_BG_POS, CsPoint( 143, 36 ), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM );
#elif VERSION_USA
	InitScript( NULL, IF_SIMPLE_BG_POS, CsPoint( 143, 36 ), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM );

	//배경
	m_pBG = NiNew cSprite;
	m_pBG->Init( m_pRoot, CsPoint::ZERO, CsPoint( 143, 36 ), "Simple\\simple_is_best.tga", false );
#else
	InitScript( "Simple\\simple_is_best.tga", IF_SIMPLE_BG_POS, CsPoint( 143, 36 ), false, IFREGION_X::LEFT, IFREGION_Y::BOTTOM );
#endif
	//사용 버튼
	m_pSimpleChangeBtn = NiNew cButton;
	m_pSimpleChangeBtn->Init( m_pRoot, IF_SIMPLE_BTN_POS, IF_SIMPLE_BTN_SIZE, cButton::IMAGE_SIMPLE_OK, _T( " " ), false );

	//안함 버튼
	m_pSimpleCancleBtn = NiNew cButton;
	m_pSimpleCancleBtn->Init( m_pRoot, IF_SIMPLE_BTN_POS, IF_SIMPLE_BTN_SIZE, cButton::IMAGE_SIMPLE_CANCEL, _T( " " ), false );
	m_bLBtnDown = g_pResist->GetFigure();
}

void cSimplification::Update(float const& fDeltaTime)
{
	static CsPoint tempPos = m_pRoot->GetClient();

	if( m_bLBtnDown == true )
	{
#ifdef VERSION_USA
		m_pBG->SetAlpha( 0.5f );	//사용버튼 눌렀을때 버튼 알파 먹인다.
#else
		( (cSprite*)m_pRoot )->SetAlpha( 0.5f );	//사용버튼 눌렀을때 버튼 알파 먹인다.
#endif

		m_pSimpleChangeBtn->SetAlpha( 0.5f );
		m_pSimpleCancleBtn->SetAlpha( 0.5f );
	}

	if( tempPos == m_pRoot->GetClient() )	//현재 뿌려주고 있는 버튼 백그라운드 좌표 검사
		return;								//같으면 리턴
	
	tempPos = m_pRoot->GetClient();			//다르면 (해상도 바뀌었을 때) 다시 좌표 받아와서
	m_pSimpleChangeBtn->SetPos( CsPoint( 65, 6 ) );		//뿌려준다
	m_pSimpleCancleBtn->SetPos( CsPoint( 65, 6 ) );
}

cBaseWindow::eMU_TYPE
cSimplification::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( g_pEngine->m_Frame.GetFrameRate() > 30.0f && !g_pResist->GetFigure() )	//프레임 30이상, 사용안함 일때 뿌리지 않으려고 리턴
		return muReturn;
	
	//=================================================================================
	// 사용버튼
	//=================================================================================
	if( m_bLBtnDown == false )
	{
		switch( m_pSimpleChangeBtn->Update_ForMouse() )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_DOWN:
			return muReturn;
		case cButton::ACTION_CLICK:
			m_pSimpleChangeBtn->SetEnable( false );		//사용버튼 클릭시 사용버튼 날려주고
			m_bLBtnDown = true;
			
			m_pSimpleCancleBtn->SetEnable( true );		//안함버튼 활성화

#ifdef VERSION_USA
			m_pBG->SetAlpha( 0.5f );	//사용버튼 눌렀을때 버튼 알파 먹인다.
#else
			( (cSprite*)m_pRoot )->SetAlpha( 0.5f );	//사용버튼 눌렀을때 버튼 알파 먹인다.
#endif

			m_pSimpleChangeBtn->SetAlpha( 0.5f );
			m_pSimpleCancleBtn->SetAlpha( 0.5f );

			g_pResist->SetFigureTamer( true );			//테이머, 디지몬, 개인상점 동기화(활성)
			g_pResist->SetFigureDigimon( true );
			g_pResist->SetFigurePStore( true );
			return muReturn;
		default:
			return muReturn;
		}
	}

	//=================================================================================
	// 사용 안함 버튼
	//=================================================================================
	else
	{
		switch( m_pSimpleCancleBtn->Update_ForMouse() )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_DOWN:	
			return muReturn;
		case cButton::ACTION_CLICK:

			m_pSimpleCancleBtn->SetEnable( false );		//안함버튼 클릭시 안함버튼 날려주고
			m_bLBtnDown = false;
			m_pSimpleChangeBtn->SetEnable( true );		//사용버튼 활성화

#ifdef VERSION_USA
			m_pBG->SetAlpha( 1.0f );	// 사용 안할때 버튼 원상복구
#else
			( (cSprite*)m_pRoot )->SetAlpha( 1.0f );	//사용 안할때 버튼 원상복구
#endif

			m_pSimpleChangeBtn->SetAlpha( 1.0f );
			m_pSimpleCancleBtn->SetAlpha( 1.0f );


			g_pResist->SetFigureTamer( false );			//테이머, 디지몬, 개인상점 동기화(비활성)
			g_pResist->SetFigureDigimon( false );
			g_pResist->SetFigurePStore( false );
			return muReturn;
		default:
			return muReturn;
		}
	}

	return muReturn;

}

void cSimplification::Render()
{
	if( g_pEngine->m_Frame.GetFrameRate() < 30.0f || g_pResist->GetFigure() )		//프레임 체크해서 30이하일 때 렌더한다
	{
		RenderScript();

		if(m_bLBtnDown)
		{
			if( m_pSimpleCancleBtn )
				m_pSimpleCancleBtn->Render();		//안함 버튼
		}
		else
		{
			if( m_pSimpleChangeBtn )
				m_pSimpleChangeBtn->Render();		//사용 버튼
		}
	}
}

void cSimplification::ResetDevice()
{	
	// 140311_kimhoshok UPdate에서 호출되는 ResetDevice와 분리, 대신 _UpdatePos 사용
#ifdef CRASHDUMP_0305
	
#else
	m_pSimpleChangeBtn->ResetDevice();
	m_pSimpleCancleBtn->ResetDevice();
#endif

#ifdef VERSION_USA
	if(m_pBG)
		m_pBG->ResetDevice();
#endif

	ResetDeviceScript();
}

#ifdef CRASHDUMP_0305
void cSimplification::_UpdatePos()
{
	if( m_pSimpleChangeBtn ) 
		m_pSimpleChangeBtn->ResetDevice();
	if( m_pSimpleCancleBtn )
		m_pSimpleCancleBtn->ResetDevice();

#ifdef VERSION_USA
		if(m_pBG)
			m_pBG->ResetDevice();
#endif

	ResetDeviceScript();
}
#endif

#endif	// SIMPLEFICATION