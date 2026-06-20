#include "stdafx.h"
#include "Protect.h"

#define MAX_NUM 10
#define WAIT_TIME 15.0f
#define IF_QUESTION_WIDTH 300

cProtect::cProtect():m_pProgress(NULL)
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
,m_pQeustionImg(NULL),m_pInputEditBox(NULL),m_pOkButton(NULL), m_pQeustionMessage(NULL)
#else
,m_pSelectList(NULL),m_pCorrectAnswerImg(NULL),m_pMaskAnswerImg(NULL)
#endif
{ 
}

cProtect::~cProtect()
{	
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE cProtect::GetWindowType()
{ 
	return WT_PROTECT; 
}

void cProtect::PreResetMap()
{ 
	Close( false ); 
}

void cProtect::Destroy()
{	
	cBaseWindow::Delete();	
}

void cProtect::DeleteResource()
{
	DeleteScript();
}

void cProtect::Create(int nValue /* = 0  */)
{
	if( !cBaseWindow::Init() )
		return;
	
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	InitScript( "MacroProtect\\macroP_win.dds",  CsPoint( (g_nScreenWidth-396) / 20, (g_nScreenHeight-488)/2 ), CsPoint( 396, 488 ), true );	
	AddTitle( UISTRING_TEXT("MACROPROTECT_WINDOW_TITLE").c_str());

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.SetText(L" ");
		ti.SetLineGabHeight( 5 );
		cText* pMsg = AddText( &ti, CsPoint(20, 50));
		pMsg->SetText( UISTRING_TEXT("MACROPROTECT_WINDOW_EXPLAIN_MSG").c_str(), 350 );		
	}

	
	m_pQeustionImg = NiNew cSprite;
	if( m_pQeustionImg )
	{	
		m_pQeustionImg->Init( GetRoot(), CsPoint((396 - CAPTCHA_WIDTH)>>1, 230 ), CsPoint( CAPTCHA_WIDTH, CAPTCHA_HEIGHT ), " ", false );
		AddChildControl( m_pQeustionImg );
	}

#ifdef SDM_MACRO_PROTECT_QUESTION_20190524
	CsPoint editPos( 40, 395 );
#else
	CsPoint editPos( 40, 295 );
#endif
	AddSprite( editPos, CsPoint( 315, 68 ), "Lobby\\CharacterCreate\\input_field.tga", true );
	editPos.y += 20;

	m_pInputEditBox = NiNew cEditBox;
	if( m_pInputEditBox )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14, NiColor::WHITE );
		ti.SetText( _T( "" ) );
		ti.s_eTextAlign = DT_CENTER;
		ti.s_bOutLine = false;
		m_pInputEditBox->Init( GetRoot(), editPos, CsPoint( 315, 20 ), &ti, false );
		m_pInputEditBox->SetEmptyMsgText(UISTRING_TEXT("MACROPROTECT_WINDOW_INPUT_MSG").c_str(), NiColor(0.5f,0.5f,0.5f) );
		m_pInputEditBox->SetFontLength( 10 );
		m_pInputEditBox->EnableUnderline( ti.s_Color );
		m_pInputEditBox->SetEnableSound( true );
		AddChildControl( m_pInputEditBox );		
	}

	m_pProgress = NiNew cUI_TimerProgressBar;
	if( m_pProgress )
	{
		m_pProgress->Init( m_pRoot, CsPoint( 23, 365 ), CsPoint( 350, 24 ), false );
		m_pProgress->SetGuageImage( "TargetWindow\\Expbar.tga" );
		m_pProgress->SetViewTextType( cUI_ProgressBar::NONE );
		m_pProgress->SetDirType( cUI_ProgressBar::RIGHT_LEFT );	
		AddChildControl( m_pProgress );
	}

	#ifdef SDM_MACRO_PROTECT_QUESTION_20190524
		m_pOkButton = AddButton( CsPoint( 133, 450 ), CsPoint( 130, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT("MACROPROTECT_WINDOW_BTN_CONFIRM").c_str());
		if( m_pOkButton )
			m_pOkButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cProtect::_OnOkButtonClicked );

		{
			cText::sTEXTINFO ti;
			ti.Init( CFont::FS_14, NiColor::WHITE );
			ti.SetText(L" ");
			ti.s_eTextAlign = DT_CENTER;
			m_pQeustionMessage = AddText( &ti, CsPoint(396>>1, 310 ));
		}	
	#else
		m_pOkButton = AddButton( CsPoint( 133, 425 ), CsPoint( 130, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT("MACROPROTECT_WINDOW_BTN_CONFIRM").c_str());
		if( m_pOkButton )
			m_pOkButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cProtect::_OnOkButtonClicked );
	#endif

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.SetText(L" ");
		ti.s_eTextAlign = DT_RIGHT;
		m_pRemaincount = AddText( &ti, CsPoint(380, 465));
	}
#else
	InitScript( "MainOption\\Option_Win.tga", CsPoint::ZERO, CsPoint( 267, 321 ), true );	
	AddTitle( UISTRING_TEXT("MACROPROTECT_WINDOW_TITLE").c_str() );


	m_pCorrectAnswerImg = NiNew cSprite;
	if( m_pCorrectAnswerImg )
	{	
		m_pCorrectAnswerImg->Init( GetRoot(), CsPoint( 20, 40 ), CsPoint( 42, 42 ), "Icon\\Auto_Ban.bmp", false );
		
		AddChildControl( m_pCorrectAnswerImg );
	}

	m_pMaskAnswerImg = NiNew cSprite;
	if( m_pMaskAnswerImg )
	{	
		m_pMaskAnswerImg->Init( GetRoot(), CsPoint( 20, 40 ), CsPoint( 42, 42 ), false );
		AddChildControl( m_pMaskAnswerImg );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		cText* pText = AddText( &ti, CsPoint( 75, 50 ) );
		if( pText )
			pText->SetText( UISTRING_TEXT("MACROPROTECT_WINDOW_SELECT_MSG").c_str(), 170 );
	}

	m_pProgress = NiNew cUI_TimerProgressBar;
	if( m_pProgress )
	{
		m_pProgress->Init( m_pRoot, CsPoint( 12, 97 ), CsPoint( 242, 24 ), false );
		m_pProgress->SetGuageImage( "TargetWindow\\Expbar.tga" );
		m_pProgress->SetViewTextType( cUI_ProgressBar::NONE );
		m_pProgress->SetDirType( cUI_ProgressBar::RIGHT_LEFT );	
		m_pProgress->AddEvent(cUI_ProgressBar::eProgressbar_Done, this, &cProtect::_AnswerTimeOver);
		AddChildControl( m_pProgress );
	}

	m_pSelectList = NiNew cGridListBox;
	if( m_pSelectList )
	{
		m_pSelectList->Init( GetRoot(), CsPoint( 12, 135 ), CsPoint( 242, 100 ), CsPoint(8,8), CsPoint(42, 42), 
							 cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 5 );
		m_pSelectList->SetMouseOverImg( "Icon\\Mask_Over.tga" );
		m_pSelectList->SetSelectedImg( "Icon\\Mask_Select.tga" );
		m_pSelectList->SetBackOverAndSelectedImgRender(false);
		m_pSelectList->AddEvent( cGridListBox::GRID_SELECT_CHANGE_ITEM, this, &cProtect::_SelectAnswer );
		AddChildControl(m_pSelectList);
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		cText* pText = AddText( &ti, CsPoint( 12, 240 ) );
		if( pText )
			pText->SetText(UISTRING_TEXT("MACROPROTECT_WINDOW_REJECT_MSG").c_str(), 242 );
	}
#endif
}

void cProtect::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
	_UpdateMoveWindow();	
}	

#ifndef SDM_MACRO_PROTECT_RENEW_20181217
void cProtect::_AnswerTimeOver( void* pSender, void* pData )
{
	if( GetSystem() )
		GetSystem()->Send_MacroAnswer( -1 );
}
#endif

cBaseWindow::eMU_TYPE cProtect::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	if(m_pInputEditBox )
	{
		if(m_pInputEditBox->Update_ForMouse( CsPoint::ZERO ) == cEditBox::ACTION_CLICK)
			return muReturn;

		if( m_pInputEditBox->IsFocus() )
		{
			if( GLOBALINPUT_ST.IsEscape() )
			{
				m_pInputEditBox->ReleaseFocus(false);
				return muReturn;
			}
			else if( GLOBALINPUT_ST.IsEnter() )				
			{
				m_pInputEditBox->ReleaseFocus(false);
				_OnOkButtonClicked( NULL, NULL );
				return muReturn;
			}
		}
	}

	if( m_pOkButton && m_pOkButton->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 396, 23 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();
#else
	if(m_pSelectList&&m_pSelectList->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return muReturn;

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 255, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();
#endif

	return muReturn;
}

void cProtect::Render()
{
	RenderScript();
}

void cProtect::ResetDevice()
{
	ResetDeviceScript();
}

#ifdef SDM_MACRO_PROTECT_RENEW_20181217
void cProtect::_ResetQuestion(int const& nMaxTimeSec, int const& nRemainTimeSec, int const& nChangeCount, int const& nQuestionType)
{
	if( m_pProgress )
	{
		m_pProgress->Reset();
		m_pProgress->SetTimer(nRemainTimeSec, nMaxTimeSec);
		m_pProgress->Start();
	}

	if( m_pQeustionImg )
	{
		char const* pImgData = GetSystem()->GetQuestionImgData();
		if( pImgData )
		{
			NiBufferReader<NiBMPReader> reader;
			NiPixelData* pPixelData = reader.ReadBuffer( const_cast<char*>(pImgData), CAPTCHA_BMP_SIZE );
			if( pPixelData )
			{
				NiSourceTexture* pTexture = NiSourceTexture::Create( pPixelData );
				if( pTexture )
					m_pQeustionImg->ChangeTexture( CsPoint( CAPTCHA_WIDTH, CAPTCHA_HEIGHT ), pTexture );
			}
		}
	}

	if( m_pRemaincount )
	{
		std::wstring msg = UISTRING_TEXT("MACROPROTECT_WINDOW_REMAIN_COUNT");
		DmCS::StringFn::ReplaceAll( msg, L"#RemainCount#", nChangeCount );
		m_pRemaincount->SetText( msg.c_str() );
	}

#ifdef SDM_MACRO_PROTECT_QUESTION_20190524
	std::wstring wsMsg;
	switch( nQuestionType )
	{
	case nsMacroProtector::NONE:				// 모든 문자 입력
		wsMsg = UISTRING_TEXT("MACROPROTECT_WINDOW_QUESTION_INPUT_ALL");
		break;
	case nsMacroProtector::OnlyNumber:			// 숫자만 입력
		wsMsg = UISTRING_TEXT("MACROPROTECT_WINDOW_QUESTION_INPUT_NUMBER");
		break;
	case nsMacroProtector::OnlyCharactor:		// 문자만 입력
		wsMsg = UISTRING_TEXT("MACROPROTECT_WINDOW_QUESTION_INPUT_CHAR");
		break;
	case nsMacroProtector::SpecificPosision:	//특정 문자 갯수
		{
			wsMsg = UISTRING_TEXT("MACROPROTECT_WINDOW_QUESTION_INPUT_POSITION");
			std::list<int> const& numlist = GetSystem()->GetQuestionNumber();
			std::list<int>::const_iterator it = numlist.begin();
			std::wstring wsQuestNumber;
			for( int n = 0; it != numlist.end(); ++it, ++n )
			{
				std::wstring number;
				DmCS::StringFn::From( number, (*it) );
				DmCS::StringFn::Add( wsQuestNumber, number, (n != 0)? L", ": L"" );
			}
			DmCS::StringFn::Replace( wsMsg, L"#position#", wsQuestNumber );			
		}		
		break;
	case nsMacroProtector::SumAllNumber:		//모든 숫자 더하기
		wsMsg = UISTRING_TEXT("MACROPROTECT_WINDOW_QUESTION_INPUT_NUMBER_SUM");
		break;
	}
	if( m_pQeustionMessage )
		m_pQeustionMessage->SetText( wsMsg.c_str(), IF_QUESTION_WIDTH );
#endif
}

void cProtect::_OnOkButtonClicked(void* pSender, void* pData)
{
	SAFE_POINTER_RET( m_pInputEditBox );
	std::wstring wsAnswer = m_pInputEditBox->GetString();
	_SendAnswer( wsAnswer );
}

void cProtect::_SendAnswer( std::wstring const& wsAnswer )
{
	if( wsAnswer.empty() )
		return;

	GetSystem()->Send_MacroAnswer( wsAnswer );
}

#else
// 텍스처의 UV 좌표 가져 오는 함수
void cProtect::_GetTextureUV( int const& nCorrectValue, NiPoint2& sUV, NiPoint2& eUV )
{
	//  "Icon\\Auto_Ban.bmp" 256x256 이고 이미지의 갯수가 64개
	// 한 라인의 이미지 갯수 8개
	// UV 좌표는 0 ~ 1 사이 이므로
	// 1 / 8 = 0.125
	float fSize = 1.0f / MAX_QUESTION_IMG_ONELINE_COUNT;	

	int nLine = nCorrectValue / MAX_QUESTION_IMG_ONELINE_COUNT;
	int nCount = nCorrectValue % MAX_QUESTION_IMG_ONELINE_COUNT;

	sUV.x = nCount * fSize;
	sUV.y = nLine * fSize;

	eUV.x = sUV.x + fSize;
	eUV.y = sUV.y + fSize;
}

void cProtect::_SetTextureUV( int const& nValue, cSprite* pTexture )
{
	SAFE_POINTER_RET( pTexture );

	NiPoint2 sUV = NiPoint2::ZERO;
	NiPoint2 eUV = NiPoint2::ZERO;
	_GetTextureUV( nValue, sUV, eUV );
	pTexture->SetTextureUV( sUV, eUV );
}

void cProtect::_ResetQuestionImg( int const& nCorrectValue )
{	
	SAFE_POINTER_RET( m_pSelectList );

	m_pSelectList->RemoveAllItem();

	std::vector<int> vecResult;
	GetSystem()->RandomShuffle( MAX_QUESTION_IMG_COUNT, nCorrectValue, MAX_NUM, vecResult );

	for( int n = 0 ; n < MAX_NUM; ++n )
	{
		cString* pString = NiNew cString;
		SAFE_POINTER_RET(pString);

		cSprite* pModeImg = NiNew cSprite;
		if( pModeImg )
		{
			pModeImg->Init( NULL, CsPoint::ZERO, CsPoint( 42, 42 ), "Icon\\Auto_Ban.bmp", false );
			_SetTextureUV( vecResult[n], pModeImg );
			cString::sSPRITE* sSprite = pString->AddSprite( pModeImg );
			if( sSprite )
				sSprite->SetAutoPointerDelete(true);
		}

		cGridListBoxItem* addItem  = NiNew cGridListBoxItem(n, CsPoint( 42, 42 ));
		addItem->SetItem(pString);
		addItem->SetUserData( new sImgNumberData( vecResult[n] ) );
		m_pSelectList->AddItem( addItem );
	}
}

NiColor cProtect::_GetRandomColor()
{
	std::vector<int> temp;
	temp.resize(11);
	for( int n = 0; n < 11; ++n )
		temp[n] = n;

	std::random_shuffle( temp.begin(), temp.end() );

	NiColor color;
	color.r = temp[0] * 0.1f;
	color.g = temp[1] * 0.1f;
	color.b = temp[2] * 0.1f;
	return color;
}

void cProtect::_SelectAnswer(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pSelItem = static_cast<cGridListBoxItem*>(pData);
	sImgNumberData* pUserData = dynamic_cast<sImgNumberData*>(pSelItem->GetUserData());
	SAFE_POINTER_RET( pUserData );
	
	GetSystem()->Send_MacroAnswer( pUserData->m_nTypeIdx );
}

//////////////////////////////////////////////////////////////////////////

void cProtect::_ResetQuestion(int const& nValue)
{
	_SetTextureUV(nValue, m_pCorrectAnswerImg);
	_ResetQuestionImg( nValue );
	_RestPattern();

	if( m_pProgress )
	{
		m_pProgress->Reset();
		m_pProgress->SetTimer(WAIT_TIME, WAIT_TIME);
		m_pProgress->Start();
	}
}

void cProtect::_SetAnswerColorMask()
{
	SAFE_POINTER_RET( m_pMaskAnswerImg );
	
	NiColor color = _GetRandomColor();
	m_pMaskAnswerImg->SetColor( color );
	m_pMaskAnswerImg->SetAlpha( 0.4f );
	m_pMaskAnswerImg->SetVisible( true );
}
 
void cProtect::_SetRandomWindowPosition()
{
	int nRand = nBase::rand(0, 1);
	CsPoint nRandPos;
	nRandPos.x = nBase::rand(0, g_nScreenWidth - 267);
	nRandPos.y = nBase::rand(0, g_nScreenHeight - 321);

	if( nRand == 0 )
		nRandPos *= -1;

	SetMove( nRandPos );
	ResetDevice();
}

// 정답 이미지 랜덤 사이즈
void cProtect::_SetRandomAnswerSize()
{
	int nRand = nBase::rand(0, 20) - 10;
	CsPoint randomSize(42 + nRand, 42 + nRand );
	CsPoint defalutPos( 20 - (nRand / 2), 40 - (nRand / 2) );

	if( m_pCorrectAnswerImg )
	{
		m_pCorrectAnswerImg->SetPosSize( defalutPos, randomSize );
		m_pCorrectAnswerImg->ResetDevice();
	}
	if( m_pMaskAnswerImg )
	{
		m_pMaskAnswerImg->SetPosSize( defalutPos, randomSize );
		m_pMaskAnswerImg->ResetDevice();
	}
}

// 정답 이미지 랜덤 회전
void cProtect::_SetRandomAnswerRotat()
{
	float nRandRotat = nBase::rand(0, 3);
	if( m_pCorrectAnswerImg )
		m_pCorrectAnswerImg->SetRotation2( CsD2R(nRandRotat * 90) );
	if( m_pMaskAnswerImg )
		m_pMaskAnswerImg->SetRotation2( CsD2R(nRandRotat * 90) );
}

// 타이머 이미지의 위치로 창의 위치를 찾기 때문에
// 타이머 게이지 사이즈 및 위치 변경
void cProtect::_ProgressbarRandomHieght()
{
	SAFE_POINTER_RET( m_pProgress );
	//CsPoint( 12, 97 ), CsPoint( 242, 24 )
	int nWidth = nBase::rand(0, 100);
	int nHeight = nBase::rand(15, 24);
	m_pProgress->SetPos( CsPoint( 12 + (nWidth / 2), 97 + (24 - nHeight) ) );
	m_pProgress->SetSize( CsPoint( 242 - nWidth, nHeight ) );
	m_pProgress->SetProgressRect( CsRect( 0, 0, 242 - nWidth, nHeight ) );
	m_pProgress->ResetDevice();
}

void cProtect::_RestPattern()
{
	_SetRandomWindowPosition();
	_SetAnswerColorMask();
	//_ProgressbarRandomHieght();
	//_SetRandomAnswerSize();		// 정답 이미지 사이즈 랜덤 변경
	//_SetRandomAnswerRotat();// 텍스처 회전
}
#endif

bool cProtect::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eResetAnswer, this );
	return true;
}

void cProtect::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{	
	switch( iNotifiedEvt )
	{
	case SystemType::eResetAnswer:
		{
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
			int nMaxTimeSec = 0;
			int nRemainTimeSec = 0;
			int nChanceCount = 0;
			int nQuestionType = nsMacroProtector::NONE;
			kStream >> nMaxTimeSec >> nRemainTimeSec >> nChanceCount >> nQuestionType;
			_ResetQuestion(nMaxTimeSec,nRemainTimeSec,nChanceCount, nQuestionType);
#else
			int nAnswer=0;
			kStream >> nAnswer;
			_ResetQuestion(nAnswer);
#endif
		}
		break;
	}
}
