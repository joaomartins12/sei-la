#include "StdAfx.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#include "ChatContents.h"
#include "ChatBase.h"

ChatBase::stExpandData::stExpandData()
{
	reset();
}

ChatBase::stExpandData::~stExpandData()
{

}

void ChatBase::stExpandData::reset(int level)
{
	nCurLevel	= level;		// 확대 레벨
}

void ChatBase::stExpandData::AddLevel()
{
	if( ++nCurLevel > MAX_LEVEL_SIZE )
		nCurLevel = 0;
}

int ChatBase::stExpandData::GetLevel() const
{
	return nCurLevel;
}

int ChatBase::stExpandData::GetHeight(int level) const
{
	// 임의의 값이 있으면
	if( level != -1)	return level*LINE_EXPAND*LINE_HEIGHT;
	// 0*3*17	// 1*3*17 ...
	// 높이레벨 * 늘어날 줄수 * 줄당 높이
	return nCurLevel*LINE_EXPAND*LINE_HEIGHT;
}

int ChatBase::stExpandData::GetDefaultHeight() const
{
	// 0*3*17	// 1*3*17 ...
	// 높이레벨 * 늘어날 줄수 * 줄당 높이
	
	return LINE_EXPAND*LINE_HEIGHT;
}

int ChatBase::stExpandData::GetLineCnt()
{
	// 0 * 3 + 6 // 1 * 3 + 6 ...
	// 높이레벨 * 늘어날 줄수 + 기본 줄수
	return nCurLevel * LINE_EXPAND + LEVEL_RENDER_COUNT;
}

ChatBase::ChatBase(void) :
m_pScrollBar(NULL), m_pChatBG(NULL), m_pLastScrollButton(NULL),
m_pMoveBtn(NULL), m_pChatBGSizeButton(NULL), m_nCurIdx(0), m_MovePosY(-31), m_OutWindow(false), m_ShowAddWindow(false)
{
	m_String.Delete();

	SetStringPos( CsPoint(8, -22));
}

ChatBase::~ChatBase(void)
{
	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void ChatBase::Destroy()
{	
	cBaseWindow::Delete();
	m_String.Delete();
}

void ChatBase::DeleteResource()
{
	SAFE_NIDELETE( m_pChatBG );

	DeleteScript();
}

void ChatBase::ResetDevice()
{
	ResetDeviceScript();
	if( m_pChatBG )
		m_pChatBG->ResetDevice();

	m_String.ResetDevice();	
}

void ChatBase::ResetMap()
{

}

bool ChatBase::Close( bool bSound /* = true  */ )
{
	if( IsEnableWindow() == false )
		return false;

	if( bSound )
	{
		PlayCloseSound();
	}

	DeleteResource();
	SetShowWindow( false );
	return true;
}

void ChatBase::Create( int nValue /*=0*/ )
{
	if( IsLive() == false )	
	{		
		m_String.Delete();
		m_nCurIdx = 0;
	}
	m_ClassValue = nValue;

	cBaseWindow::Init();

	CsPoint ptBGSize = CsPoint(CHAT_SIZE_WIDTH, SizelevelHeight(CHAT_SIZE_HEIGHT));

	InitScript( NULL, m_ptRootClient, ptBGSize, true, IFREGION_X::LEFT, IFREGION_Y::TOP, false);

	// 바탕 화면
	m_pChatBG = NiNew cPopUp;
	if(m_pChatBG)
		m_pChatBG->Init( NULL, CsPoint::ZERO, CsPoint(CHAT_SIZE_WIDTH, SizelevelHeight(CHAT_SIZE_HEIGHT)), CsPoint( 3, 3 ), "Control_G\\Popup\\NDlg", "tga", false );	

	// 스크롤바
	//m_pScrollBar =	AddScrollBar( cScrollBar::TYPE_1, CsPoint( 369, 3+15),	CsPoint( 16, SizelevelHeight(CHAT_SIZE_HEIGHT) - 31 - 3),cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, m_ptRootSize ), m_Expand.GetLineCnt());
	int sizeY = SizelevelHeight(CHAT_SIZE_HEIGHT) -31 - 3 -15;
	m_pScrollBar =	AddScrollBar( cScrollBar::TYPE_1, CsPoint( 369, 3+15),
		CsPoint( 16, sizeY),
		cScrollBar::GetDefaultBtnSize(),
		CsRect( CsPoint::ZERO, m_ptRootSize), m_Expand.GetLineCnt());
	
	if(m_pScrollBar)
	{
		m_String.SetBuddy(m_pScrollBar);
		// 스크롤바 조절
		m_pScrollBar->SetRenderCount( m_Expand.GetLineCnt() );
		m_pScrollBar->SetCurPosIndex( m_nCurIdx );
		m_pScrollBar->SetRange( CsPoint( 0, m_String.GetSize() ) );
		m_pScrollBar->ResetDevice();
	}

	// 마지막 줄 보기 버튼
	m_pLastScrollButton = AddButton( CsPoint( 369, SizelevelHeight(CHAT_SIZE_HEIGHT) - 31 - 6 + 3), CsPoint( 16, 15 ), CsPoint( 0, 15 ), "Chat\\ScrollLastButton.tga" );
	if(m_pLastScrollButton)
		m_pLastScrollButton->SetEnable( false );

	// 채팅창 확대 버튼
	m_pChatBGSizeButton = AddButton( CsPoint( 369, SizelevelHeight(CHAT_SIZE_HEIGHT) - 31 - 6 + 3 + 16 ), CsPoint( 16, 16 ), CsPoint( 0, 20 ), "Chat\\Ch_Size.tga" );
	if(m_pChatBGSizeButton)
		m_pChatBGSizeButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &ChatBase::_ChageBGSize );

	// 이동버튼
	m_pMoveBtn = AddButton( CsPoint( 369, 3 ), CsPoint( 16, 15 ), CsPoint( 0, 22 ), "System\\MoveWindow.tga" );
	if(m_pMoveBtn)
		m_pMoveBtn->AddEvent( cButton::BUTTON_LBDOWN_EVENT, this, &ChatBase::_MoveUi );
}

void ChatBase::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();

	// 라스트 버튼 활성화
	if(m_pLastScrollButton)
		m_pLastScrollButton->SetEnable( m_pScrollBar->IsEnableScroll() );
}

cBaseWindow::eMU_TYPE ChatBase::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	if( muReturn == cBaseWindow::MUT_OUT_WINDOW )
		return muReturn;

	if( m_pMoveBtn && m_pMoveBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	m_pScrollBar->Update_ForMouse();
	if( m_pLastScrollButton && m_pLastScrollButton->Update_ForMouse() == cButton::ACTION_CLICK)
	{
		LastScrollClick();
		return muReturn;
	}

	if( m_pChatBGSizeButton && m_pChatBGSizeButton->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_OutWindow ) return cBaseWindow::MUT_OUT_WINDOW;
	return muReturn;
}

void ChatBase::Render()
{
	if(m_pChatBG)	m_pChatBG->Render(m_ptRootClient);
	RenderScript();
		
	StringListRender();	
}

void ChatBase::StringListRender()
{
	CsPoint pos = m_ptRootClient + CsPoint( m_StringPos.x, SizelevelHeight(CHAT_SIZE_HEIGHT)+m_StringPos.y );

	m_String.RenderEnd(pos, m_MovePosY);
}

void ChatBase::SetAlpha(float alpha)
{
	
	m_pChatBG->SetAlpha(alpha);
	m_pScrollBar->SetAlpha(alpha);

	if(m_pLastScrollButton)
		m_pLastScrollButton->SetAlpha(alpha);
	if(m_pChatBGSizeButton)
		m_pChatBGSizeButton->SetAlpha(alpha);
	if(m_pMoveBtn)
		m_pMoveBtn->SetAlpha(alpha);
	
	
	//m_String.SetAlpha(alpha);
}
void ChatBase::_MoveUi( void* pSender, void* pData )
{
	_UpdateMoveWindow_ForMouse();
}

void ChatBase::_ChageBGSize( void* pSender, void* pData )
{
	// 전체 UI 위치 및 사이즈 조절	
	m_Expand.AddLevel();

	ChatBase::_ChageBGUpdate(NULL, pData);
}

void ChatBase::_ChageBGUpdate( void* pSender, void* pData )
{
	int DownPos = -m_Expand.GetDefaultHeight();	// 전체 크기 증가에 따른 Y축 내려줄 높이

	// 풀 사이즈에서 최소 사이즈로 바뀔 경우 위치값 재지정
	if(m_Expand.nCurLevel==0)
	{
		stExpandData		FullExpand;
		FullExpand.nCurLevel= MAX_LEVEL_SIZE-1;
		DownPos = CHAT_SIZE_HEIGHT+FullExpand.GetHeight() + m_Expand.GetDefaultHeight()-SizelevelHeight(CHAT_SIZE_HEIGHT);	// 현재 크기
	}

	// 배경 위치
	//m_pChatBG->GetSprite( 6 )->SetPos( CsPoint( 0, m_ptLevelSize[ m_nCurLevel ].y -3 ) );
	//m_pChatBG->GetSprite( 7 )->SetPos( CsPoint( 3, m_ptLevelSize[ m_nCurLevel ].y -3 ) );
	//m_pChatBG->GetSprite( 8 )->SetPos( CsPoint( CHAT_SIZE_WIDTH-3, m_ptLevelSize[ m_nCurLevel ].y -3 ) );

	// baseWindowSize 조절
	CsPoint ptBGSize = CsPoint(CHAT_SIZE_WIDTH, SizelevelHeight(CHAT_SIZE_HEIGHT));	//ptBGSize.y += 27;	// 채팅입력창 27 사이즈 포함

	// cScriptUI 위치 조절
	SetPosSize( m_ptRootClient+ CsPoint(0, DownPos), ptBGSize );
	__SetRootSize(ptBGSize);
	ResetDevice();

	// 배경 사이즈
	m_pChatBG->ReSize( CsPoint(CHAT_SIZE_WIDTH, SizelevelHeight(CHAT_SIZE_HEIGHT)) );
	
	// 스크롤 아래로 내리는 버튼 사이즈 조절
	if(m_pLastScrollButton)
		m_pLastScrollButton->SetPos( CsPoint( 369, SizelevelHeight(CHAT_SIZE_HEIGHT) - 31 - 6 + 3) );
	
	// 채팅창 확대 버튼 사이즈 조절
	if(m_pChatBGSizeButton)
		m_pChatBGSizeButton->SetPos( CsPoint( 369, SizelevelHeight(CHAT_SIZE_HEIGHT) - 31 - 6 + 3 + 16 ) );
	
	// 스크롤바 조절
	m_pScrollBar->SetRenderCount( m_Expand.GetLineCnt() );
	m_pScrollBar->ReSize( CsPoint( 16, SizelevelHeight(CHAT_SIZE_HEIGHT) - 31 - 3 -15), CsRect( CsPoint::ZERO, m_ptRootSize ) );
	m_pScrollBar->SetCurPosIndex( m_pScrollBar->GetMaxPosIndex() );	
	m_pScrollBar->SetRange( CsPoint( 0, m_String.GetSize() ) );
	
}

int	ChatBase::SizelevelHeight(int sizeHegiht) const
{
	return sizeHegiht+m_Expand.GetHeight();
}

void ChatBase::SetMovePosY(int y)
{
	m_MovePosY = y;
}
void ChatBase::SetOutWindow(bool w)
{
	m_OutWindow = w;
}

void ChatBase::LastScrollClick()
{
	m_pScrollBar->SetCurPosIndex( m_pScrollBar->GetMaxPosIndex() );
	m_nCurIdx = m_pScrollBar->GetCurPosIndex();
}

void ChatBase::SetStringPos(CsPoint s)
{
	m_StringPos = s;
}
void ChatBase::SetExpandLevel(int	level)
{
	if(level < 0)
		m_Expand.nCurLevel = 0;
	else if(level > MAX_LEVEL_SIZE)
		m_Expand.nCurLevel = MAX_LEVEL_SIZE;
	else
		m_Expand.nCurLevel = level;
}
int	ChatBase::GetExpandLevel() const
{
	return m_Expand.nCurLevel;
}
CsPoint ChatBase::GetStringPos() const
{
	return m_StringPos;
}

bool ChatBase::GetAddShowOn() const
{
	return m_ShowAddWindow;
}
void ChatBase::SetAddShowOn(bool	bLogon)
{
	m_ShowAddWindow = bLogon;
}

void ChatBase::CheckExecption()
{
	// 예외 처리
	CsPoint BasePos = GetRootClient();

	// 확장 레벨
	if(GetExpandLevel() < 0)
	{
		SetExpandLevel(0);
	}
	else if(GetExpandLevel() > MAX_LEVEL_SIZE)
	{
		SetExpandLevel(MAX_LEVEL_SIZE);
	}

	// UI 위치
	if( BasePos.x >= g_nScreenWidth-CHAT_SIZE_WIDTH )
	{
		BasePos.x = g_nScreenWidth-CHAT_SIZE_WIDTH;
	}
	else if( BasePos.x <= 0 )
	{
		BasePos.x = 0;
	}
	if( BasePos.y >= g_nScreenHeight-CHAT_SIZE_HEIGHT)
	{
		BasePos.y = g_nScreenHeight -CHAT_SIZE_HEIGHT;
	}
	else if( BasePos.y <= 0 )
	{
		BasePos.y = 0;
	}

	// 창 확장 레벨에 따른 Y축 조절
	int DownPos = CHAT_SIZE_HEIGHT+ m_Expand.GetDefaultHeight()*GetExpandLevel();

	if(BasePos.y+DownPos >= g_nScreenHeight)
	{
		BasePos.y = g_nScreenHeight - DownPos;
	}

	SetRootClient(BasePos);
}
void ChatBase::LoadUIOption(int type)
{
	if( IsLive() == false )	
	{
		ST_CHAT_OPTION*	pOpt = GetSystem()->LoadUIOption(type);

		SAFE_POINTER_RET(pOpt);

		m_Chat_Option = *pOpt;

		SetRootClient(pOpt->Pos);
		SetExpandLevel(pOpt->ExpandLevel);
		SetAddShowOn(pOpt->AddShow);
		CheckExecption();
	}
}

void ChatBase::SaveUIOption(int type)
{
	m_Chat_Option.Pos		 = GetRootClient();
	m_Chat_Option.ExpandLevel= GetExpandLevel();
	m_Chat_Option.AddShow	 = GetAddShowOn();

	GetSystem()->SaveUIOption(type, &m_Chat_Option);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ChatBase::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;
	return true;
}

void ChatBase::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}