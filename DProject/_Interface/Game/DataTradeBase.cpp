#include "StdAfx.h"

#include "DataTradeContents.h"
#include "DataTradeBase.h"

#define	SIZE_WIDTH_TOP		(260)//*g_nScreenWidth/DEFAULT_SCREEN_WIDTH)
#define	SIZE_HEIGHT_TOP		(100)//*g_nScreenHeight/DEFAULT_SCREEN_HEIGHT)

#define	SIZE_WIDTH_LEFT		(290)//*g_nScreenWidth/DEFAULT_SCREEN_WIDTH)
#define	SIZE_HEIGHT_LEFT	(298)//*g_nScreenHeight/DEFAULT_SCREEN_HEIGHT)

#define	SIZE_WIDTH_RIGHT	(290)//*g_nScreenWidth/DEFAULT_SCREEN_WIDTH)
#define	SIZE_HEIGHT_RIGHT	(298)//*g_nScreenHeight/DEFAULT_SCREEN_HEIGHT)

#define TRADE_BUTTON_SIZE	CsPoint(120, 54)

#define BLOCK_SIZE			CsPoint(32,32)	// 아이콘, 블록의 사이즈
#define TRADE_ITEM_SIZE		CsPoint(40,40)	// 데이터 교환 아이템 이미지의 사이즈
#define NUMBER_SIZE			CsPoint(12,12)	// 데이터 교환 아이템 이미지의 갯수 표시 해 주는 숫자의 사이즈

#define DIGIMON_TRANSCEND_HATCHLV 6

#define		BASE_POS_LEFT		CsPoint(GetPopup(1)->GetPos() + m_VarPos)
#define		BASE_POS_RIGHT		CsPoint(GetPopup(2)->GetPos() + m_VarPos)

DataTradeBase::DataTradeBase(void) : m_pDataTradeButton(NULL), m_OutWindow(false), m_bIconClick(false), m_TradeItemCount(0),
m_bFlicker(false), m_TradeItemNumbering(NULL)
{
	m_tempText.Init( &g_pEngine->m_FontSystem, CFont::FS_11, NiColor( 1.0f, 1.0f, 0.5f ) );
	m_tempText.s_Color = FONT_WHITE;
	m_tempText.s_eTextAlign = DT_CENTER;

	SetOutWindow(true);

	m_pPopup[0] = NULL;
	m_pPopup[1] = NULL;
	m_pPopup[2] = NULL;

	m_NickNamePos	= CsPoint(BLOCK_SIZE.x/2, 19);
	m_TypeNamePos	= CsPoint(BLOCK_SIZE.x/2, 47);
	m_VarNamePos	= CsPoint(BLOCK_SIZE.x/2, 90);
	m_VarPos		= CsPoint(BLOCK_SIZE.x/2, 140);

	m_IconSize = BLOCK_SIZE;
	m_BlockSize = BLOCK_SIZE;
}

DataTradeBase::~DataTradeBase(void)
{
	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}
void DataTradeBase::Destroy()
{	
	cBaseWindow::Destroy();
}

void DataTradeBase::DeleteResource()
{
	DeleteScript();

	for( int i=0; i<MAX_MON; ++i )
	{
		_DATA_INFO*		pInfo	=	&m_DigimonInfo[i];
		pInfo->Delete();
	}

	m_pPopup[0] = NULL;
	m_pPopup[1] = NULL;
	m_pPopup[2] = NULL;
	m_pPopupBG[0] = NULL;
	m_pPopupBG[1] = NULL;
	m_pPopupBG[2] = NULL;

	m_pDataTradeButton = NULL;
	m_pRetuonToSelectButton = NULL;
	m_TradeItemNumbering = NULL;
}

void DataTradeBase::ResetDevice()
{
	SetPosSize(CsPoint::ZERO, CsPoint(g_nScreenWidth, g_nScreenHeight));

	InitSize();

	ResetPosition();	
	
	ResetDeviceScript();
}

bool DataTradeBase::Close( bool bSound /* = true  */ )
{
	if( IsEnableWindow() == false )
		return false;
	if( bSound )
		PlayCloseSound();

	DeleteResource();
	SetShowWindow( false );
	return true;
}

void DataTradeBase::Create( int nValue /*=0*/ )
{
	if( IsLive() == false )	
	{
	}

	m_bIconClick = false;

	EndFlicker();

	InitSize();

	cBaseWindow::Init();

	CsPoint CenterPos = GetCenterPos();

	InitScript( NULL, CsPoint::ZERO, CsPoint(g_nScreenWidth, g_nScreenHeight), false, IFREGION_X::LEFT, IFREGION_Y::TOP, false);	

	// 바탕 화면
	m_pPopup[0] = AddPopup( m_TopPos, CsPoint(SIZE_WIDTH_TOP, SIZE_HEIGHT_TOP), CsPoint( 0, 0 ), "Control_G\\Popup\\NDlg", "tga" );
	m_pPopup[1] = AddPopup( m_LeftPos, CsPoint(SIZE_WIDTH_LEFT, SIZE_HEIGHT_LEFT), CsPoint( 0, 0 ), "Control_G\\Popup\\NDlg", "tga" );	
	m_pPopup[2] = AddPopup( m_RightPos, CsPoint(SIZE_WIDTH_RIGHT, SIZE_HEIGHT_RIGHT), CsPoint( 0, 0 ), "Control_G\\Popup\\NDlg", "tga" );

	m_pPopupBG[0] = AddSprite(m_TopPos, CsPoint(SIZE_WIDTH_TOP, SIZE_HEIGHT_TOP), "DataExchange\\Data_Exchange_DigimonList.tga" );

	string LR_INFO_IMAGE_PATH = "DataExchange\\Data_Exchange_Info_Evolution.tga";

	switch(GetSystem()->GetState())
	{
	case DTCON::eTRADE_SCALE:
		LR_INFO_IMAGE_PATH = "DataExchange\\Data_Exchange_Info_Size.tga";
		break;
	case DTCON::eTRADE_UPGRADE:
		LR_INFO_IMAGE_PATH = "DataExchange\\Data_Exchange_Info_Evolution.tga";
		break;
	case DTCON::eTRADE_EVOLUTION:
		LR_INFO_IMAGE_PATH = "DataExchange\\Data_Exchange_Info_Evolution.tga";
		break;
	}
	m_pPopupBG[1] = AddSprite(m_LeftPos, CsPoint(SIZE_WIDTH_LEFT, SIZE_HEIGHT_LEFT), LR_INFO_IMAGE_PATH.c_str() );
	m_pPopupBG[2] = AddSprite(m_RightPos, CsPoint(SIZE_WIDTH_RIGHT, SIZE_HEIGHT_RIGHT), LR_INFO_IMAGE_PATH.c_str() );

	// 데이터 교환 버튼
	m_pDataTradeButton = AddButton( m_RightPos + CsPoint( SIZE_WIDTH_LEFT+20, SIZE_HEIGHT_LEFT-TRADE_BUTTON_SIZE.y ), TRADE_BUTTON_SIZE, TRADE_BUTTON_SIZE, "DataExchange\\Data_Exchange_Button.tga");
	m_pDataTradeButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &DataTradeBase::CreateDataTradeEffect );

	m_tempText.SetText((TCHAR*)UISTRING_TEXT( "DATA_TRADE_BUTTON_EXCHANGE" ).c_str());
	m_pDataTradeButton->SetText(&m_tempText);
	m_pDataTradeButton->SetPosText(m_pDataTradeButton->GetPos()+TRADE_BUTTON_SIZE/2);

	// 이전 데이터 교환 선택창으로 돌아가기.
	m_pRetuonToSelectButton = AddButton( m_LeftPos + CsPoint(-TRADE_BUTTON_SIZE.x-20, SIZE_HEIGHT_LEFT-TRADE_BUTTON_SIZE.y ), TRADE_BUTTON_SIZE, TRADE_BUTTON_SIZE, "DataExchange\\Data_Exchange_Button.tga");
	m_pRetuonToSelectButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &DataTradeBase::ReturnToSelect );
	m_pRetuonToSelectButton->SetTexToken(TRADE_BUTTON_SIZE);
	m_tempText.SetText((TCHAR*)UISTRING_TEXT( "DATA_TRADE_BUTTON_SELECT" ).c_str());
	m_pRetuonToSelectButton->SetText(&m_tempText);
	m_pRetuonToSelectButton->SetPosText(m_pRetuonToSelectButton->GetPos()+TRADE_BUTTON_SIZE/2);

	// 아이템 갯수 이미지의 위치 정보. RenderItem을 사용하기 때문에 생성시에는 위치값만 가지고 있는다.
	m_TradeItemImagePos = CsPoint(GetCenterPos().x-TRADE_ITEM_SIZE.x/2,	BASE_POS_LEFT.y);	

	// 데이터 교환 아이템의 인벤에 존재하는 갯수
	m_TradeItemCount = GetSystem()->GetInvenItemCount(GetSystem()->GetState());

	//
	GetNumberingText();

	// 위쪽 정보 + 좌, 우 정보
	for( int i = SLOT_TOP_MON1; i < SLOT_MOVE; ++i )
		CreateData( GetIntToEnum( i ), NULL );

	// 위치 해상도에 맞게 재설정.
	ResetPosition();
}

void DataTradeBase::Update(float const& fDeltaTime)
{
	if(InFlickerOn())
	{
		m_FlickerTime += fDeltaTime;
		if( m_FlickerTime > m_FlickerChangeTime)
			OnFlicker();
	}
}
cBaseWindow::eMU_TYPE
DataTradeBase::Update_ForMouse()
{
	// 데이터 교환 이펙트 실행중이면 키 먹지 않음.
	if( GetSystem()->IsTradeDataEffect() )
	{
		return cBaseWindow::MUT_NONE;
	}
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	_UpdateIcon_ForMouse();

	if(m_pDataTradeButton)
		m_pDataTradeButton->Update_ForMouse();


	if(m_pRetuonToSelectButton)
		m_pRetuonToSelectButton->Update_ForMouse();

	//if( m_OutWindow ) return cBaseWindow::MUT_OUT_WINDOW;

	return muReturn;
}

void DataTradeBase::Render()
{
	RenderScript();

	// 이동하는 아이콘 위치 표시.
	_DATA_INFO*	MoveInfo=	&m_DigimonInfo[SLOT_MOVE];
	if(MoveInfo->GetSprite())
		MoveInfo->GetSprite()->SetPos(CURSOR_ST.GetPos() - GetRootClient());

	// 상단 디지몬 정보 툴팁
	for( int i = SLOT_TOP_MON1; i < nLimit::DigimonBaseSlot - 1; ++i )
	{
		_DATA_INFO*	pInfo	=	&m_DigimonInfo[i];
		sIFIcon*	IfIcon	=	pInfo->GetIcon();
		CsPoint pos = IfIcon->GetPos() + GetRootClient();

		if( IfIcon->s_eState != sIFIcon::NONE )	// 마우스가 선택된 위치에 있다면 마스크 랜더링
		{
			IfIcon->RenderMask( pos, BLOCK_SIZE );
		}
	}

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	// 필요 아이템 이미지의 번호 보관하기.
	int LsasCodeCount = GetSystem()->GetItemCodeCount( GetSystem()->GetState() );

	// 아이템 코드
	int Item_code = GetSystem()->GetItemCode( GetSystem()->GetState(), LsasCodeCount - 1 );	// LsasCodeCount는 최소1개 0번부터 시작한다.
	// 인벤 아이템 갯수
	int ItemCount = GetSystem()->GetInvenItemCount( GetSystem()->GetState() );

	// 표시될 아이콘
	g_pIconMng->RenderItem( Item_code, m_TradeItemImagePos, TRADE_ITEM_SIZE);

	// 수량 표시의 위치및 값 재조정
	if(m_TradeItemCount != ItemCount)
	{
		GetNumberingText();		
	}

	// 표시될 아이콘 정보
	cItemInfo	_Trade_Use_Item;
	_Trade_Use_Item.m_nType = Item_code;
	_Trade_Use_Item.m_nCount = ItemCount;

	CsPoint	Root = GetRootClient();
		
	// 아이템 갯수 표시 툴팁
	CsRect	_Rect(m_TradeItemImagePos, CsSIZE(TRADE_ITEM_SIZE));
	if( _Rect.PtInRect( ptMouseLocalPos ) == TRUE )
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( m_TradeItemImagePos+GetRootClient(), TRADE_ITEM_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, Item_code, cBaseWindow::WT_INVENTORY, 0, 0,  &_Trade_Use_Item);
}

void DataTradeBase::GetNumberingText()
{
	int ItemCount = GetSystem()->GetInvenItemCount( GetSystem()->GetState() );
	int UseCount = GetSystem()->GetRequireItemCount(GetSystem()->GetState());

	m_TradeItemCount = ItemCount;

	wstring	wStr;
	DmCS::StringFn::Format( wStr, L"%d/%d", ItemCount, UseCount );

	if(m_TradeItemNumbering == NULL)
	{
		m_tempText.SetText( wStr.c_str() );
		m_TradeItemNumbering = AddText( &m_tempText, m_TradeItemImagePos+CsPoint(TRADE_ITEM_SIZE.x/2, TRADE_ITEM_SIZE.y) );
	}
	else
	{
		m_TradeItemNumbering->SetText( wStr.c_str() );
	}

	// 색상 설정.
	m_TradeItemNumbering->SetColor(FONT_WHITE);	// 교환 가능한 경우의 색상(화이트)
	if(ItemCount < UseCount)					// 교환 불가능한 경우의 색상(레드)
		m_TradeItemNumbering->SetColor(FONT_RED);	
}
int DataTradeBase::TOP_UPDATE(CURSOR::eTYPE eCursor)
{
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	int	Result = 0;

	// 상단 아이콘 클릭시
	for( int i = SLOT_TOP_MON1; i < nLimit::DigimonBaseSlot - 1; ++i )
	{
		_DATA_INFO*		pInfo	=	&m_DigimonInfo[i];

		if(pInfo==NULL) continue;

		sIFIcon*	IfIcon	=	pInfo->GetIcon();

		if(IfIcon==NULL) continue;

		if(Result ==1)
			IfIcon->ReleaseState();
		else if( eCursor == CURSOR::LBUTTON_DOWN && IfIcon->MouseLBtnClick( ptMouseLocalPos ) == true )	
		{				
			if( !m_bIconClick && pInfo->GetSprite()->GetColorA().a == 1.0f )
				DragToImageStart(GetIntToEnum(i), pInfo);
			else if( m_bIconClick )
				DragToImageEnd(SLOT_MOVE, &m_DigimonInfo[SLOT_MOVE]);
			Result = 1;
		}
		else if( eCursor == CURSOR::RBUTTON_DOWN && IfIcon->MouseRBtnClick( ptMouseLocalPos ) == true )
		{
			if( !m_bIconClick && pInfo->GetSprite()->GetColorA().a == 1.0f )
				RbuttonClick(GetIntToEnum(i), pInfo);
			Result = 1;
		}
		else if( eCursor == CURSOR::LBUTTON_PRESS && IfIcon->s_eState != sIFIcon::MOUSE_LCLICK &&	IfIcon->MouseOn( ptMouseLocalPos ) == true )
			Result = 1;
		else if( eCursor == CURSOR::RBUTTON_PRESS && IfIcon->s_eState != sIFIcon::MOUSE_RCLICK &&	IfIcon->MouseOn( ptMouseLocalPos ) == true )
			Result = 1;
		else if( IfIcon->MouseOn( ptMouseLocalPos ) == true )
			Result = 1;
		else
			IfIcon->ReleaseState();

		// 툴립
		if( pInfo->GetSlotIndex() == NONE_SLOT ) continue;

		if( IfIcon->s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
		{
			CsPoint pos =IfIcon->GetPos() + GetRootClient();
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, BLOCK_SIZE, 140, cTooltip::TACTICS, pInfo->GetSlotIndex(), 0, 0, 0, 0 );
		}
	}

	//if( Result == 0 && CurState == CURSOR::LBUTTON_DOWN)	//	바깥을 클릭
	//{
	//	if( m_bIconClick )
	//		DragToImageEnd(SLOT_MOVE, &m_DigimonInfo[SLOT_MOVE]);
	//}

	return	Result;
}

int DataTradeBase::LEFT_RIGHT_UPDATE(CURSOR::eTYPE eCursor)
{
	int	Result = 1;

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	_DATA_INFO*	pLeft_Info = &m_DigimonInfo[SLOT_LEFT];
	_DATA_INFO*	pRight_Info = &m_DigimonInfo[SLOT_RIGHT];

	int			L_Idx = pLeft_Info->GetSlotIndex();
	int			R_Idx = pRight_Info->GetSlotIndex();

	// 좌 우 아이콘 클릭시.
	if( GetSystem()->IsPickLeftSlot() )
	{
		if(eCursor == CURSOR::LBUTTON_DOWN)
		{
			if(!m_bIconClick)
				DragToImageStart(SLOT_LEFT, &m_DigimonInfo[SLOT_LEFT]);
			else if(m_bIconClick)
				DragToImageEnd(SLOT_LEFT, &m_DigimonInfo[SLOT_MOVE]);
		}
		else if(eCursor == CURSOR::RBUTTON_DOWN && !m_bIconClick)
		{
			if(pLeft_Info->GetLoadData())	// 데이터가 있으면.
				DeleteTradeSlotData(SLOT_LEFT, NULL);	// 데이터 삭제
		}
		// 툴립	//if( pLeft_Info->GetIcon()->s_Rect.PtInRect( ptMouseLocalPos ) == TRUE &&  L_Idx!= NONE_SLOT )
		if(L_Idx!= NONE_SLOT )
		{
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( ptMouseLocalPos, BLOCK_SIZE, 140, cTooltip::TACTICS, L_Idx, 0, 0, 0, 0 );
		}
	}
	else if( GetSystem()->IsPickRightSlot() )
	{
		if(eCursor == CURSOR::LBUTTON_DOWN )
		{
			if(!m_bIconClick)
				DragToImageStart(SLOT_RIGHT, &m_DigimonInfo[SLOT_RIGHT]);
			else if(m_bIconClick)
				DragToImageEnd(SLOT_RIGHT, &m_DigimonInfo[SLOT_MOVE]);
		}
		else if(eCursor == CURSOR::RBUTTON_DOWN && !m_bIconClick)
		{
			if(pRight_Info->GetLoadData())	// 데이터가 있으면.
				DeleteTradeSlotData(SLOT_RIGHT, NULL);	// 데이터 삭제
		}
		// 툴립	//if( pRight_Info->GetIcon()->s_Rect.PtInRect( ptMouseLocalPos ) == TRUE &&  R_Idx!= NONE_SLOT )
		if(R_Idx!= NONE_SLOT )
		{
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( ptMouseLocalPos, BLOCK_SIZE, 140, cTooltip::TACTICS, R_Idx, 0, 0, 0, 0 );
		}

	}
	else
	{
		if(eCursor == CURSOR::LBUTTON_DOWN && m_bIconClick)
			DragToImageEnd(SLOT_MOVE, &m_DigimonInfo[SLOT_MOVE]);
		Result = 0;
	}

	return	Result;
}


void	DataTradeBase::_UpdateIcon_ForMouse()
{
	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();
	
	int	TopClickOn = 0;

	TopClickOn = TOP_UPDATE(eCursor);

	if(TopClickOn == 1)	// 위쪽 창에서 선택된 내용이 있으면 좌우 창은 업데이트를 하지 않는다.
		return;
	
	LEFT_RIGHT_UPDATE(eCursor);	
}

DataTradeBase::eDIGIMONSLOT DataTradeBase::GetIntToEnum(int	i)
{
	eDIGIMONSLOT eDigiSlot = PARTNER_MON;
	switch(i)
	{
	case SLOT_TOP_MON1:
		eDigiSlot = SLOT_TOP_MON1;
		break;

	case SLOT_TOP_MON2:
		eDigiSlot = SLOT_TOP_MON2;
		break;

	case SLOT_TOP_MON3:
		eDigiSlot = SLOT_TOP_MON3;
		break;

#ifdef LJW_TACTICS_SLOT_EXTEND_190529
	case SLOT_TOP_MON4:
		eDigiSlot = SLOT_TOP_MON4;
		break;
#endif

	case SLOT_LEFT:
		eDigiSlot = SLOT_LEFT;
		break;

	case SLOT_RIGHT:
		eDigiSlot = SLOT_RIGHT;
		break;
	default:
		eDigiSlot = PARTNER_MON;
		break;
	}

	return eDigiSlot;
}
void DataTradeBase::DragToImageStart(eDIGIMONSLOT	eSlot, _DATA_INFO*	pInfo)
{
	SAFE_POINTER_RET(pInfo);

	//if( eSlot == SLOT_LEFT || eSlot == SLOT_RIGHT )	// 좌우는 시작안함.
	//	return;
	
	if( pInfo->GetLoadData() == NULL )		// 로드 데이터가 없는 경우.
		return;
	if(eSlot == PARTNER_MON)
		return;

	sIFIcon*	IfIcon	=	pInfo->GetIcon();

	CreateData(SLOT_MOVE, pInfo);
	pInfo->GetSprite()->SetAlpha(0.5f);
	//IfIcon->s_eState = sIFIcon::MOUSE_DRAG;
	m_bIconClick = true;	
}

void DataTradeBase::DragToImageEnd(eDIGIMONSLOT	eSlot, _DATA_INFO*	pInfo)
{
	SAFE_POINTER_RET(pInfo);

	_DATA_INFO*	LeftInfo	=	&m_DigimonInfo[SLOT_LEFT];
	_DATA_INFO*	RightInfo	=	&m_DigimonInfo[SLOT_RIGHT];

	int		BaseID =	pInfo->GetLoadData()->s_dwBaseDigimonID;
	int		LeftID =	(LeftInfo->GetLoadData()) ? LeftInfo->GetLoadData()->s_dwBaseDigimonID : NONE_MODEL;
	int		RightID =	(RightInfo->GetLoadData()) ? RightInfo->GetLoadData()->s_dwBaseDigimonID : NONE_MODEL;

	int		BaseHatchLv		= pInfo->GetLoadData()->s_HatchLevel;
	int		nLeftHatchLv	= (LeftInfo->GetLoadData()) ? LeftInfo->GetLoadData()->s_HatchLevel : NONE_MODEL;
	int		nRightHatchLv	= (RightInfo->GetLoadData()) ? RightInfo->GetLoadData()->s_HatchLevel : NONE_MODEL;

	//	참조중인 슬롯 이미지 클릭 표시.
	int	MoveIdx	=	pInfo->GetSlotIndex();
	int	LeftIdx	=	LeftInfo->GetSlotIndex();
	int	RightIdx=	RightInfo->GetSlotIndex();

	bool	bChange =false;

	if(	BaseID == CODE_AGUMON ||
		BaseID == CODE_LALAMON ||
		BaseID == CODE_GAOMON ||
		BaseID == CODE_FALCOMON )
	{
		// 파트너몬이라 등록 불가.
		GetSystem()->MessageToNotify(MESSAGE_PARTNERMON);
		eSlot = PARTNER_MON;
	}

	switch(eSlot)
	{
	case SLOT_TOP_MON1:
	case SLOT_TOP_MON2:
	case SLOT_TOP_MON3:
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
	case SLOT_TOP_MON4:
#endif
		break;
	case SLOT_LEFT:
		{	
			if( RightID == NONE_MODEL)		// 우측 데이터가 비어 있으면.
				CreateTradeSlotData(SLOT_LEFT, pInfo);
			else if(BaseID == LeftID )		// 이미 있는 데이터 인지 비교
			{
				if(MoveIdx == RightIdx)	// 좌우 교체
				{
					DeleteTradeSlotData(SLOT_RIGHT, NULL);	// 데이터 삭제
					CreateTradeSlotData(SLOT_RIGHT, LeftInfo);
					CreateTradeSlotData(SLOT_LEFT, pInfo);
				}
				else	// 좌측 다른 정보로 교체
				{
					if (nRightHatchLv >= DIGIMON_TRANSCEND_HATCHLV && BaseHatchLv != nRightHatchLv)
						GetSystem()->MessageToNotify(MESSAGE_MISMATCH);
					else if (BaseHatchLv >= DIGIMON_TRANSCEND_HATCHLV && BaseHatchLv != nRightHatchLv)
						GetSystem()->MessageToNotify(MESSAGE_MISMATCH);
					else
					{
						DeleteTradeSlotData(SLOT_LEFT, NULL);	// 데이터 삭제
						CreateTradeSlotData(SLOT_LEFT, pInfo);
					}
				}
				bChange = true;
			}
			else if(BaseID == RightID )		// 현재 타입과 우측 타입이 같은지 비교
			{
				if (nRightHatchLv >= DIGIMON_TRANSCEND_HATCHLV && BaseHatchLv != nRightHatchLv)
					GetSystem()->MessageToNotify(MESSAGE_MISMATCH_HATCHLV);
				else if (BaseHatchLv >= DIGIMON_TRANSCEND_HATCHLV && BaseHatchLv != nRightHatchLv)
					GetSystem()->MessageToNotify(MESSAGE_MISMATCH_HATCHLV);
				else
					CreateTradeSlotData(SLOT_LEFT, pInfo);
			}
			else
			{
				GetSystem()->MessageToNotify(MESSAGE_MISMATCH);
			}

			if(!bChange && pInfo->GetSlotIndex() == RightIdx)
				DeleteTradeSlotData(SLOT_RIGHT, NULL);	// 데이터 삭제
		}
		break;
	case SLOT_RIGHT:
		{
			if( LeftID == NONE_MODEL)		// 좌측 데이터가 비어 있으면.
				CreateTradeSlotData(SLOT_RIGHT, pInfo);
			else if(BaseID == RightID )		// 이미 있는 데이터 인지 비교
			{
				if(MoveIdx == LeftIdx)		// 좌우 정보 교체
				{
					DeleteTradeSlotData(SLOT_LEFT, NULL);	// 데이터 삭제
					CreateTradeSlotData(SLOT_LEFT, RightInfo);
					CreateTradeSlotData(SLOT_RIGHT, pInfo);
				}
				else	// 우측 다른 정보로 교체
				{
					if (nLeftHatchLv >= DIGIMON_TRANSCEND_HATCHLV && BaseHatchLv != nLeftHatchLv)
						GetSystem()->MessageToNotify(MESSAGE_MISMATCH_HATCHLV);
					else if (BaseHatchLv >= DIGIMON_TRANSCEND_HATCHLV && BaseHatchLv != nLeftHatchLv)
						GetSystem()->MessageToNotify(MESSAGE_MISMATCH_HATCHLV);
					else
					{
						DeleteTradeSlotData(SLOT_RIGHT, NULL);	// 데이터 삭제
						CreateTradeSlotData(SLOT_RIGHT, pInfo);
					}
				}
				bChange = true;
			}
			else if(BaseID == LeftID )		// 현재 타입과 좌측 타입이 같은지 비교
			{
				if (nLeftHatchLv >= DIGIMON_TRANSCEND_HATCHLV && BaseHatchLv != nLeftHatchLv)
					GetSystem()->MessageToNotify(MESSAGE_MISMATCH_HATCHLV);
				else if (BaseHatchLv >= DIGIMON_TRANSCEND_HATCHLV && BaseHatchLv != nLeftHatchLv)
					GetSystem()->MessageToNotify(MESSAGE_MISMATCH_HATCHLV);
				else
					CreateTradeSlotData(SLOT_RIGHT, pInfo);
			}
			else
			{
				GetSystem()->MessageToNotify(MESSAGE_MISMATCH);
			}

			if(!bChange && pInfo->GetSlotIndex() == LeftIdx)
				DeleteTradeSlotData(SLOT_LEFT, NULL);	// 데이터 삭제
		}
		break;
	case SLOT_MOVE:
		{
			if(MoveIdx == LeftIdx)
				DeleteTradeSlotData(SLOT_LEFT, NULL);	// 데이터 삭제
			else if(MoveIdx == RightIdx)
				DeleteTradeSlotData(SLOT_RIGHT, NULL);	// 데이터 삭제
		}
		break;
	}
	CreateData(SLOT_MOVE, NULL);
	m_bIconClick = false;

	//	참조중인 슬롯 이미지 클릭 표시.
	LeftIdx	=	LeftInfo->GetSlotIndex();
	RightIdx=	RightInfo->GetSlotIndex();

	if(LeftIdx != NONE_SLOT)
		m_DigimonInfo[LeftIdx].GetSprite()->SetAlpha(0.5f);

	if(RightIdx != NONE_SLOT)
		m_DigimonInfo[RightIdx].GetSprite()->SetAlpha(0.5f);	
}

void DataTradeBase::RbuttonClick(eDIGIMONSLOT	eSlot, _DATA_INFO*	pInfo)
{
	_DATA_INFO*	LeftInfo	=	&m_DigimonInfo[SLOT_LEFT];
	_DATA_INFO*	RightInfo	=	&m_DigimonInfo[SLOT_RIGHT];

	SAFE_POINTER_RET(pInfo->GetLoadData());

	int		BaseID =	pInfo->GetLoadData()->s_dwBaseDigimonID;
	int		LeftID =	(LeftInfo->GetLoadData()) ? LeftInfo->GetLoadData()->s_dwBaseDigimonID : NONE_MODEL;
	int		RightID =	(RightInfo->GetLoadData()) ? RightInfo->GetLoadData()->s_dwBaseDigimonID : NONE_MODEL;

	//int		BaseID =	pInfo->GetModelID();
	//int		LeftID =	LeftInfo->GetModelID();
	//int		RightID =	RightInfo->GetModelID();

	INT		bLeftEmpty	= 0;
	INT		bRightEmpty	= 0;

	if(	BaseID == CODE_AGUMON ||
		BaseID == CODE_LALAMON ||
		BaseID == CODE_GAOMON ||
		BaseID == CODE_FALCOMON )
	{
		// 파트너몬이라 등록 불가.
		GetSystem()->MessageToNotify(MESSAGE_PARTNERMON);
		return;
	}

	if( LeftID == NONE_MODEL)		// 좌측 데이터가 비어 있으면.
		bLeftEmpty = 0;
	else if(BaseID == LeftID )		// 현재 타입과 좌측 타입이 같은지 비교
		bLeftEmpty = 1;
	else
		bLeftEmpty = -1;

	if( RightID == NONE_MODEL)		// 우측 데이터가 비어 있으면.
		bRightEmpty = 0;
	else if(BaseID == RightID )		// 현재 타입과 우측 타입이 같은지 비교
		bRightEmpty = 1;
	else
		bRightEmpty = -1;

	if(bLeftEmpty == -1 && bRightEmpty == -1)	// 둘다 불가능하면 안만듬.
	{
		GetSystem()->MessageToNotify(MESSAGE_MISMATCH);
	}
	else if(bLeftEmpty == -1 && bRightEmpty == 0)	// 좌측만 차있고 정보가 다르면 교체
	{
		DragToImageStart(eSlot, pInfo);

		DragToImageEnd(SLOT_LEFT, &m_DigimonInfo[SLOT_MOVE]);
		return;
	}
	else if(bLeftEmpty == 0 && bRightEmpty == -1)	// 우측만 차있고 정보가 다르면 교체
	{
		DragToImageStart(eSlot, pInfo);

		DragToImageEnd(SLOT_RIGHT, &m_DigimonInfo[SLOT_MOVE]);
		return;
	}
	else if(bLeftEmpty == 0 && bRightEmpty == 0)	// 둘다 비었으면 무조건 만듬.
	{
		DragToImageStart(eSlot, pInfo);

		DragToImageEnd(SLOT_LEFT, &m_DigimonInfo[SLOT_MOVE]);
	}
	else if(bLeftEmpty == 1 && bRightEmpty == 1)	// 둘다 타입이 같으면. 우측으로만 정보 교체 생성 해준다.
	{
		DragToImageStart(eSlot, pInfo);

		DragToImageEnd(SLOT_RIGHT, &m_DigimonInfo[SLOT_MOVE]);
	}
	else if(bLeftEmpty == 0)	// 좌측만 비고 타입이 같으면.
	{
		DragToImageStart(eSlot, pInfo);

		DragToImageEnd(SLOT_LEFT, &m_DigimonInfo[SLOT_MOVE]);
	}
	else if(bRightEmpty == 0)	// 우측만 비고 타입이 같으면.
	{
		DragToImageStart(eSlot, pInfo);

		DragToImageEnd(SLOT_RIGHT, &m_DigimonInfo[SLOT_MOVE]);
	}
}

void DataTradeBase::ClearPrevTopSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo)
{
	// 교체 슬롯과 현재 슬롯의 정보가 같으면 참조 중이므로 해제 하지 않는다.
	if(pInfo && m_DigimonInfo[etype].GetSlotIndex() == pInfo->GetSlotIndex() )
		return;

	// 이전에 존재하던 슬롯정보가 있으면.
	if( m_DigimonInfo[etype].GetSlotIndex() != NONE_SLOT )
	{
		int	SlotTopIndex = m_DigimonInfo[etype].GetSlotIndex();
		switch(SlotTopIndex)
		{
		case	SLOT_TOP_MON1:
		case	SLOT_TOP_MON2:
		case	SLOT_TOP_MON3:
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
		case  SLOT_TOP_MON4:
#endif
			break;
		default:
			SlotTopIndex = -1;
			break;
		}
		
		if(SlotTopIndex != NONE_SLOT && m_DigimonInfo[SlotTopIndex].GetSprite())
		{
			// 참조가 끝난 슬롯의 이미지를 정상으로 돌려준다. 이 상태로 참조여부 판단한다.
			m_DigimonInfo[SlotTopIndex].GetSprite()->SetAlpha(1.0f);
		}
	}
}
void DataTradeBase::SetTradeSlotUpdate(eDIGIMONSLOT etype, _DATA_INFO*	pInfo)
{
	// 이전 참조 슬롯 해제
	ClearPrevTopSlotData(etype, pInfo);

	if(pInfo)	// 참조
	{
		int modelID = pInfo->GetModelID();
		m_DigimonInfo[etype].SetModelID(modelID);
		m_DigimonInfo[etype].SetSlotIndex(pInfo->GetSlotIndex());
		m_DigimonInfo[etype].ReferenceLoadData(pInfo->GetLoadData());
	}
	else		// 참조 초기화
	{
		m_DigimonInfo[etype].SetModelID(NONE_MODEL);
		m_DigimonInfo[etype].SetSlotIndex(NONE_SLOT);
		m_DigimonInfo[etype].ReferenceLoadData(NULL);

		if( m_DigimonInfo[etype].GetSprite() )
			m_DigimonInfo[etype].GetSprite()->SetAlpha(1.0f);

	}
}


void DataTradeBase::CreateData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo)
{
	// 이미지 정보
	if(pInfo)
		AddSpriteImage(etype, (char*)pInfo->GetSprPath(), pInfo->GetUseWokrPath());
	else
		AddSpriteImage(etype);

	// 슬롯별 위치 정보. SLOT_TOP_MON1~3 슬롯과 이동 슬롯일 경우 이미지 정보만 받는다.
	CsPoint	UiPos = m_TopPos;

	switch(etype)
	{
	case SLOT_TOP_MON1:
	case SLOT_TOP_MON2:
	case SLOT_TOP_MON3:
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
	case SLOT_TOP_MON4:
#endif
		return;

	case SLOT_LEFT:
		{
			SetTradeSlotUpdate(etype, pInfo);
			UiPos = m_DigimonInfo[SLOT_LEFT].GetIconPos();			
		}
		break;
	case SLOT_RIGHT:
		{
			SetTradeSlotUpdate(etype, pInfo);
			UiPos = m_DigimonInfo[SLOT_RIGHT].GetIconPos();
		}
		break;
	case SLOT_MOVE:
		SetTradeSlotUpdate(etype, pInfo);
		return;
	}
	
	// 디지몬 닉네임
	if(pInfo && pInfo->GetNickNameText())
		CreateNickName(m_DigimonInfo[etype], pInfo->GetNickNameText(), UiPos);
	else
		CreateNickName(m_DigimonInfo[etype], (TCHAR*)UISTRING_TEXT( "DATA_TRADE_DIGIMON_NICKNAME" ).c_str(), UiPos);
	
	// 디지몬 계열체이름
	if(pInfo && pInfo->GetTypeNameText())
		CreateTypeName(m_DigimonInfo[etype], pInfo->GetTypeNameText(), UiPos);
	else
		CreateTypeName(m_DigimonInfo[etype], (TCHAR*)UISTRING_TEXT( "DATA_TRADE_DIGIMON_TYPENAME" ).c_str(), UiPos);


	// 디지몬 정보
	if(pInfo && pInfo->GetVariableNameText())
		CreateVariableName(m_DigimonInfo[etype], pInfo->GetVariableNameText(), UiPos);
	else
		CreateVariableName(m_DigimonInfo[etype], (TCHAR*)UISTRING_TEXT( "DATA_TRADE_DIGIMON_SCALEDATA" ).c_str(), UiPos);

	// 디지몬 가변 정보명 갱신
	GetVariableName();
}

void DataTradeBase::CreateTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo)
{
	SAFE_POINTER_RET(pInfo);

	int BaseID = pInfo->GetLoadData()->s_dwBaseDigimonID;

	if(etype == SLOT_LEFT)
		GetSystem()->CreateTradeCenterModelLeft(BaseID);

	else if(etype == SLOT_RIGHT)
		GetSystem()->CreateTradeCenterModelRight(BaseID);
	

	stLoadData*		pLoadData = pInfo->GetLoadData();

	CreateNickName(*pInfo, pLoadData->s_szName, CsPoint::ZERO);
	CsTacticsExplain*	pExplain =	nsCsFileTable::g_pTacticsMng->GetTacticsExplain( BaseID );
	if(pExplain)
		CreateTypeName(*pInfo, pExplain->GetInfo()->s_szTacticsName, CsPoint::ZERO);	// 계열체명

	CreateData(etype, pInfo);

	RemoveChildText(pInfo->GetNickName());pInfo->CreateNickName(NULL);
	RemoveChildText(pInfo->GetTypeName());pInfo->CreateTypeName(NULL);
}
void DataTradeBase::DeleteTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo)
{
	SAFE_POINTER_RET(m_DigimonInfo[etype].GetLoadData());

	if(etype == SLOT_LEFT)
		GetSystem()->DeleteTradeCenterModelLeft();

	else if(etype == SLOT_RIGHT)
		GetSystem()->DeleteTradeCenterModelRight();

	CreateData(etype, pInfo);
}

void DataTradeBase::CreateNickName(_DATA_INFO &info, TCHAR* pName, CsPoint pos)
{
	m_tempText.SetText(pName);

	if( info.GetNickName() == NULL)		// 없는 경우 생성
		info.CreateNickName( AddText( &m_tempText, pos+m_NickNamePos ) );
	else
		info.SetNickName((TCHAR*)m_tempText.GetText());	// 있는 경우 이름만 변경.
}
void DataTradeBase::CreateTypeName(_DATA_INFO &info, TCHAR* pName, CsPoint pos)
{
	m_tempText.SetText(pName);

	if( info.GetTypeName() == NULL)
		info.CreateTypeName( AddText( &m_tempText, pos+m_TypeNamePos ) );
	else
		info.SetTypeName((TCHAR*)m_tempText.GetText());
}
void DataTradeBase::CreateVariableName(_DATA_INFO &info, TCHAR* pName, CsPoint pos)
{
	m_tempText.s_eFontSize=CFont::FS_12;
	m_tempText.SetText(pName);	

	if( info.GetVariableName() == NULL)
		info.CreateVariableName( AddText( &m_tempText, pos+m_VarNamePos ) );
	else
		info.SetVariableName((TCHAR*)m_tempText.GetText());

	m_tempText.s_eFontSize=CFont::FS_11;
}

void DataTradeBase::SetVariableName(TCHAR* VarName)
{
	m_ValiableName = VarName;
	if(m_DigimonInfo[SLOT_LEFT].GetVariableName())
		m_DigimonInfo[SLOT_LEFT].SetVariableName((TCHAR*)m_ValiableName.c_str());
	if(m_DigimonInfo[SLOT_RIGHT].GetVariableName())
		m_DigimonInfo[SLOT_RIGHT].SetVariableName((TCHAR*)m_ValiableName.c_str());
}

TCHAR* DataTradeBase::GetVariableName()
{
	if(m_ValiableName.size())
	{
		if(m_DigimonInfo[SLOT_LEFT].GetVariableName())
			m_DigimonInfo[SLOT_LEFT].SetVariableName((TCHAR*)m_ValiableName.c_str());
		if(m_DigimonInfo[SLOT_RIGHT].GetVariableName())
			m_DigimonInfo[SLOT_RIGHT].SetVariableName((TCHAR*)m_ValiableName.c_str());

		return (TCHAR*)m_ValiableName.c_str();
	}

	return NULL;
}


int DataTradeBase::GetImagePath(int slotNum, string	&RefStr)
{
	int	DigimonID = NONE_MODEL;
	RefStr = "EI_Item\\Outbox_1.tga";
	if( slotNum >= nLimit::DigimonBaseSlot - 1 )	// 슬롯 이미지 정보는 0~2번 슬롯만 있기 때문에 나머지는 따로 처리한다.
		return	DigimonID;

	DigimonID = g_pDataMng->GetTactics()->GetTactics(slotNum)->s_dwBaseDigimonID;

	if(DigimonID != 0)	// 디지몬이 있는 경우 데이터 폴더 경로가 포함 되있는 Path를 받는다
	{
		
		DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimon( DigimonID )->GetInfo()->s_dwModelID;		
		RefStr = g_pModelDataMng->GetSmallModelIconFile( dwModelID );

		// 보유 슬롯 지정
		m_DigimonInfo[slotNum].SetSlotIndex(slotNum);	//	slotNum == SLOT_TOP_MON1(0) ~	SLOT_TOP_MON3(2)

		// 보유 슬롯정보 참조.
		m_DigimonInfo[slotNum].ReferenceLoadData( g_pDataMng->GetTactics()->GetTactics(slotNum) );

		// 디지몬 ID를 모델 ID 로 변경 SetMoDelID()함수에 넣어준다.
		DigimonID = dwModelID;

	}
	else
	{
		DigimonID = NONE_MODEL;
		RefStr = "EI_Item\\Outbox_1.tga";
	}

	return DigimonID;
}
void	DataTradeBase::AddSpriteImage(int slotNum, char* ImgPath, bool UseWokrPath)
{
	if(slotNum <	0	 ) return;
	if(slotNum >= MAX_MON) return;

	bool	bUseWorkingFolder = true;	// 작업 폴더 경로 사용 여부

	cSprite*	pSprReference = NULL;
	string		sprPath;

	int	DigimonID	= GetImagePath(slotNum, sprPath);	// 이미지 경로 정보 얻기. 슬롯0~2번만 얻을 수 있다.

	if(DigimonID!=NONE_MODEL)		// 디지몬이 있는 경우 데이터 폴더 경로가 포함 되있는 Path를 받는다
		bUseWorkingFolder = false;	// 때문에 작업 폴더는 미사용

	if(ImgPath)
	{
		sprPath = ImgPath;
		bUseWorkingFolder = UseWokrPath;
	}

	switch(slotNum)
	{
	case SLOT_TOP_MON1:
	case SLOT_TOP_MON2:
	case SLOT_TOP_MON3:
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
	case SLOT_TOP_MON4:
#endif
		pSprReference = AddSprite( m_DigimonInfo[slotNum].GetIconPos(), BLOCK_SIZE, sprPath.c_str(), true, FONT_WHITE, bUseWorkingFolder );
		break;
	case SLOT_LEFT:
	case SLOT_RIGHT:
		pSprReference = AddSprite( m_DigimonInfo[slotNum].GetIconPos(), BLOCK_SIZE, sprPath.c_str(), true, FONT_WHITE, bUseWorkingFolder );
		pSprReference->SetVisible(false);	// 좌우 이미지는 표시 하지 않는다.
		break;
	case SLOT_MOVE:
		if(!bUseWorkingFolder)	// 이미지 있을 경우에만 생성
		{
			pSprReference = AddSprite( CsPoint::ZERO, BLOCK_SIZE, sprPath.c_str(), true, FONT_WHITE, bUseWorkingFolder );
			pSprReference->SetPos(CURSOR_ST.GetPos()- GetRootClient());
		}
		break;
	default:
		return;
	}

	m_DigimonInfo[slotNum].SetModelID(DigimonID);
	SetSpriteInfo(slotNum, pSprReference);//	m_DigimonInfo[slotNum].SetSprite(pSprReference);
	m_DigimonInfo[slotNum].SetImagePath(sprPath.c_str());
	m_DigimonInfo[slotNum].SetUseWorkPath(bUseWorkingFolder);

	return;
}

DataTradeBase::_DATA_INFO*	DataTradeBase::GetDataInfo(eDIGIMONSLOT etype)
{
	return &m_DigimonInfo[etype];
}
void DataTradeBase::SetSpriteInfo(int type, cSprite* pSpr)
{
	if(m_DigimonInfo[type].GetSprite())
	{
		RemoveChildControl( m_DigimonInfo[type].GetSprite() );
		m_DigimonInfo[type].SetSprite(NULL);
	}
	m_DigimonInfo[type].SetSprite(pSpr);
}

void DataTradeBase::SetOutWindow(bool w)
{
	m_OutWindow = w;
}

void DataTradeBase::ResetPosition()
{
	SAFE_POINTER_RET(m_pWindowTitle);

	if(m_pWindowTitle)
		m_pWindowTitle->SetPos( CsPoint(GetCenterPos().x, 7));

	//SAFE_POINTER_RET(m_pPopup[0]);

	// 팝업창 위치 재설정
	if(m_pPopup[0])
		m_pPopup[0]->ReSize(m_TopPos, CsPoint(SIZE_WIDTH_TOP, SIZE_HEIGHT_TOP));

	if(m_pPopup[1])
		m_pPopup[1]->ReSize(m_LeftPos, CsPoint(SIZE_WIDTH_LEFT, SIZE_HEIGHT_LEFT));

	if(m_pPopup[2])
		m_pPopup[2]->ReSize(m_RightPos, CsPoint(SIZE_WIDTH_RIGHT, SIZE_HEIGHT_RIGHT));

	// 팝업 이미지 위치 재설정.
	if(m_pPopupBG[0])
		m_pPopupBG[0]->SetPosSize(m_TopPos, CsPoint(SIZE_WIDTH_TOP, SIZE_HEIGHT_TOP));

	if(m_pPopupBG[1])
		m_pPopupBG[1]->SetPosSize(m_LeftPos, CsPoint(SIZE_WIDTH_LEFT, SIZE_HEIGHT_LEFT));

	if(m_pPopupBG[2])
		m_pPopupBG[2]->SetPosSize(m_RightPos, CsPoint(SIZE_WIDTH_RIGHT, SIZE_HEIGHT_RIGHT));

	// 데이터 교환 버튼 위치 재설정
	if(m_pDataTradeButton)
		m_pDataTradeButton->SetPos( CsPoint(GetCenterPos().x-TRADE_BUTTON_SIZE.x/2, g_nScreenHeight-TRADE_BUTTON_SIZE.y) );

	// 뒤로 가기 버튼 위치 재설정
	if(m_pRetuonToSelectButton)
		m_pRetuonToSelectButton->SetPos(CsPoint( GetCenterPos().x-TRADE_BUTTON_SIZE.x/2, m_LeftPos.y ));

	//슬롯 탑1,2,3, 좌, 우까지 이미지 위치 재설정
	for(int i=SLOT_TOP_MON1; i<SLOT_MOVE; ++i)
	{
		CsPoint	pos = m_DigimonInfo[i].GetIconPos();
		m_DigimonInfo[i].SetSpritePos(pos+CsPoint(0, 0));
	}

	//슬롯 좌, 우 텍스트 정보 위치 재설정
	for(int i=SLOT_LEFT; i<=SLOT_RIGHT; ++i)
	{
		CsPoint	pos = m_DigimonInfo[i].GetIconPos();

		m_DigimonInfo[i].SetNickNamePos(pos+m_NickNamePos);
		m_DigimonInfo[i].SetTypeNamePos(pos+m_TypeNamePos);
		m_DigimonInfo[i].SetVariableNamePos(pos+m_VarNamePos);
	}
}
void DataTradeBase::CreateDataTradeEffect(void* pSender, void* pData)
{
	// 데이터 교환 이펙트 실행중이면 키 먹지 않음.
	if( GetSystem()->IsTradeDataEffect() )
		return;

	// 데이터 교환 가능한지 판단 후 가능하면 교환 알림창->이펙트 생성->데이터 교환순으로 시작한다.
	GetSystem()->DataTradeCheck(
		GetDataInfo(SLOT_LEFT)->GetLoadData(), 
		GetDataInfo(SLOT_RIGHT)->GetLoadData(), 
		GetDataInfo(SLOT_LEFT)->GetTypeNameText() );
}

void DataTradeBase::ResetData()
{
}

void DataTradeBase::SendToTrade()
{
	int L_Slot = GetDataInfo(SLOT_LEFT)->GetSlotIndex()+1;
	int R_Slot = GetDataInfo(SLOT_RIGHT)->GetSlotIndex()+1;

	wstring	wStr =	GetDataInfo(SLOT_LEFT)->GetTypeNameText();

	ContentsStream kStream;
	kStream<<L_Slot<<R_Slot<<wStr;

	GetSystem()->SendTradeData(&kStream);
}
CsPoint DataTradeBase::GetCenterPos() const
{
	CsPoint CenterPos = CsPoint( g_nScreenWidth/2, g_nScreenHeight/2 );
	return CenterPos;
}


void DataTradeBase::InitSize()
{
	CsPoint CenterPos = GetCenterPos();

	//SetPos(CenterPos);

	m_TopPos	= CsPoint(CenterPos.x-SIZE_WIDTH_TOP/2, 20);

	float	RatioX = (float)g_nScreenWidth/DEFAULT_SCREEN_WIDTH-1.0f;
	float	RatioY = (float)g_nScreenHeight/DEFAULT_SCREEN_HEIGHT-1.0f;
	
	float	MoveX =	200.0f*RatioY;
	float	MoveY =	320.0f*RatioY;

	m_LeftPos	= CsPoint((CenterPos.x-SIZE_WIDTH_LEFT-(int)TRADE_BUTTON_SIZE.x/2)-(int)MoveX, (g_nScreenHeight-SIZE_HEIGHT_LEFT-20)-MoveY);
	m_RightPos	= CsPoint((CenterPos.x+(int)TRADE_BUTTON_SIZE.x/2)+MoveX, (g_nScreenHeight-SIZE_HEIGHT_RIGHT-20)-MoveY);

	// 디지몬 보유 슬롯들의 위치
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
	m_DigimonInfo[SLOT_TOP_MON1].SetIconInit( CsRect( m_TopPos+CsPoint( 33, 44 ), (CsSIZE)BLOCK_SIZE ) );
	m_DigimonInfo[SLOT_TOP_MON2].SetIconInit( CsRect( m_TopPos+CsPoint( 87, 44), (CsSIZE)BLOCK_SIZE ) );
	m_DigimonInfo[SLOT_TOP_MON3].SetIconInit( CsRect( m_TopPos+CsPoint( 141, 44), (CsSIZE)BLOCK_SIZE ) );
	m_DigimonInfo[SLOT_TOP_MON4].SetIconInit( CsRect( m_TopPos+CsPoint( 195, 44), (CsSIZE)BLOCK_SIZE ) );
#else
	m_DigimonInfo[SLOT_TOP_MON1].SetIconInit( CsRect( m_TopPos+CsPoint( SIZE_WIDTH_TOP/2-BLOCK_SIZE.x-BLOCK_SIZE.x/2-49,44), (CsSIZE)BLOCK_SIZE ) );
	m_DigimonInfo[SLOT_TOP_MON2].SetIconInit( CsRect( m_TopPos+CsPoint( SIZE_WIDTH_TOP/2-BLOCK_SIZE.x/2, 44), (CsSIZE)BLOCK_SIZE ) );
	m_DigimonInfo[SLOT_TOP_MON3].SetIconInit( CsRect( m_TopPos+CsPoint( SIZE_WIDTH_TOP/2+BLOCK_SIZE.x-BLOCK_SIZE.x/2+49, 44), (CsSIZE)BLOCK_SIZE ) );
#endif

	m_DigimonInfo[SLOT_LEFT].SetIconInit( CsRect( m_LeftPos+CsPoint(SIZE_WIDTH_LEFT/2-BLOCK_SIZE.x/2, 0), (CsSIZE)BLOCK_SIZE ) );
	m_DigimonInfo[SLOT_RIGHT].SetIconInit( CsRect( m_RightPos+CsPoint(SIZE_WIDTH_RIGHT/2-BLOCK_SIZE.x/2, 0), (CsSIZE)BLOCK_SIZE ) );

}
cPopUp*	DataTradeBase::GetPopup(int idx)
{
	if(idx < 0 || idx > 3)
		return m_pPopup[0];

	return m_pPopup[idx];
}

void	DataTradeBase::ReturnToSelect(void* pSender, void* pData)
{
	GAME_EVENT_ST.OnEvent(EVENT_CODE::OPEN_TRADE_SELECT, NULL );
}
bool DataTradeBase::InFlickerOn() const
{
	if(m_FlickerCnt>0)
		return true;
	return	false;
}

bool DataTradeBase::IsFlicker() const
{
	return	m_bFlicker;
}
void DataTradeBase::OnFlicker()
{
	if(m_FlickerCnt>0)
	{
		m_bFlicker = !m_bFlicker;
		m_FlickerCnt--;
		m_FlickerTime=0.0f;
		if(m_FlickerCnt <=0 )
			EndFlicker();
	}
}
void DataTradeBase::StartFlicker(int Cnt, float Ctime)
{
	m_FlickerTime = 0.0f;
	m_bFlicker = true;
	m_FlickerCnt = Cnt;	
	m_FlickerChangeTime = Ctime;
}
void DataTradeBase::EndFlicker()
{
	m_bFlicker = false;
	m_FlickerCnt = 0;
	m_FlickerTime = 0.0f;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool DataTradeBase::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void DataTradeBase::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}
//////////////////////////////////////////////////////////////////////////
////////////////////	stDataTradeInfo(_DATA_INFO)	//////////////////////
//////////////////////////////////////////////////////////////////////////

typedef					DataTradeBase::_DATA_INFO	DATA_INFO;	// 데이터 교환 정보

DATA_INFO::stDataTradeInfo(): s_pSprite(NULL), s_nModelID(NONE_MODEL), s_bUseWorkPath(false), s_pNickName(NULL), s_pTypeName(NULL), s_pVariableName(NULL), s_SlotIndex(NONE_SLOT), s_LoadData(NULL)
{
}
DATA_INFO::~stDataTradeInfo() 
{
	Delete();
}
void DATA_INFO::ReleaseIconState( bool bIncluseDrag)	{	s_IFIcon.ReleaseState(bIncluseDrag);	}
void DATA_INFO::ReferenceLoadData(stLoadData* pData)	{	s_LoadData =pData;	}
void DATA_INFO::Delete()	
{	
	s_pSprite = NULL; s_pNickName = NULL, s_pTypeName = NULL, s_pVariableName = NULL;
	s_ImgPath.clear();
	s_LoadData= NULL;
}
void DATA_INFO::SetModelID(int ID)					{	s_nModelID=ID;		}
void DATA_INFO::SetSlotIndex(int idx)				{	s_SlotIndex=idx;		}
void DATA_INFO::SetIconState(sIFIcon::eSTATE state)	{	s_IFIcon.s_eState=state;}		
void DATA_INFO::SetIconInit( CsRect rc )			{ 	s_IFIcon.Init( rc ); }
void DATA_INFO::SetImagePath(const char* path)		{	
	s_ImgPath.clear();
	s_ImgPath.append(path);
}
void DATA_INFO::SetUseWorkPath(bool bUse)			{	s_bUseWorkPath= bUse;}

void DATA_INFO::SetSprite(cSprite* pSpr)	
{
	s_pSprite=pSpr;
}

void DATA_INFO::SetNickName(TCHAR*	pName)
{	
	if(s_pNickName)
		s_pNickName->SetText(pName);
}
void DATA_INFO::SetTypeName(TCHAR*	pName)
{	
	if(s_pTypeName)
		s_pTypeName->SetText(pName);
}
void DATA_INFO::SetVariableName(TCHAR*	pName)
{
	if(s_pVariableName)
		s_pVariableName->SetText(pName);
}
void DATA_INFO::SetSpritePos(CsPoint pos)
{
	if(s_pSprite)
		s_pSprite->SetPos(CsPoint( pos.x, pos.y) );
}
void DATA_INFO::SetNickNamePos(CsPoint pos)
{
	if(s_pNickName)
		s_pNickName->SetPos(pos);
}
void DATA_INFO::SetTypeNamePos(CsPoint pos)
{
	if(s_pTypeName)
		s_pTypeName->SetPos(pos);
}
void DATA_INFO::SetVariableNamePos(CsPoint pos)
{
	if(s_pVariableName)
		s_pVariableName->SetPos(pos);
}

void DATA_INFO::CreateNickName(cText* pText)
{	
	s_pNickName=pText;
}
void DATA_INFO::CreateTypeName(cText* pText)
{	
	s_pTypeName=pText;
}
void DATA_INFO::CreateVariableName(cText* pText)
{
	s_pVariableName=pText;
}

stLoadData*	DATA_INFO::GetLoadData()		const	{	return s_LoadData;}
int			DATA_INFO::GetSlotIndex()	const	{	return s_SlotIndex;	}
const char*	DATA_INFO::GetSprPath()		const	
{
	if(s_ImgPath.size()==0)
		return NULL;

	return s_ImgPath.c_str();
}
cSprite*	DATA_INFO::GetSprite()		const	{	return s_pSprite;}
sIFIcon*	DATA_INFO::GetIcon()				{	return &s_IFIcon;	}
CsPoint		DATA_INFO::GetIconPos()				{	return s_IFIcon.GetPos();	}
CsSIZE		DATA_INFO::GetIconSize()			{	return s_IFIcon.GetSize();	}

CsPoint		DATA_INFO::GetVariableNamePos()
{
	if(s_pVariableName)
		return s_pVariableName->GetPos();

	return	CsPoint::ZERO;
}


cText*		DATA_INFO::GetNickName()		const	{	return s_pNickName;}
cText*		DATA_INFO::GetTypeName()		const	{	return s_pTypeName;}
cText*		DATA_INFO::GetVariableName()	const	{	return s_pVariableName;}

TCHAR*		DATA_INFO::GetNickNameText()		const	{	SAFE_POINTER_RETVAL(s_pNickName, NULL);	return (TCHAR*)s_pNickName->GetText();}
TCHAR*		DATA_INFO::GetTypeNameText()		const	{	SAFE_POINTER_RETVAL(s_pTypeName, NULL);	return (TCHAR*)s_pTypeName->GetText();}
TCHAR*		DATA_INFO::GetVariableNameText()	const	{	SAFE_POINTER_RETVAL(s_pVariableName, NULL);	return (TCHAR*)s_pVariableName->GetText();}

int			DATA_INFO::GetModelID()		const	{	return s_nModelID;	}
bool		DATA_INFO::GetUseWokrPath()	const	{	return s_bUseWorkPath;}
stModelInfo*	DATA_INFO::GetModelInfo()	const
{	
	if(s_nModelID==NONE_MODEL)	return NULL;

	SAFE_POINTER_RETVAL(g_pModelDataMng, NULL);
	SAFE_POINTER_RETVAL(g_pModelDataMng->GetData( s_nModelID ), NULL);

	return g_pModelDataMng->GetData( s_nModelID )->GetInfo();
}
