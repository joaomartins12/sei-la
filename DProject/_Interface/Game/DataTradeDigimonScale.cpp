#include "StdAfx.h"

#include "DataTradeContents.h"
#include "DataTradeBase.h"
#include "DataTradeDigimonScale.h"

DataTradeDigimonScale::DataTradeDigimonScale(void) : m_pDigimonScaleLeft(NULL), m_pDigimonScaleRight(NULL)
{
}

DataTradeDigimonScale::~DataTradeDigimonScale(void)
{
	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}
void DataTradeDigimonScale::Destroy()
{	
	DataTradeBase::Destroy();
}

void DataTradeDigimonScale::DeleteResource()
{
	DataTradeBase::DeleteResource();
	
	m_pDigimonScaleLeft = NULL;

	m_pDigimonScaleRight = NULL;

}

void DataTradeDigimonScale::ResetDevice()
{
	DataTradeBase::ResetDevice();
}

bool DataTradeDigimonScale::Close( bool bSound /* = true  */ )
{
	DataTradeBase::Close( bSound );
	return true;
}

void DataTradeDigimonScale::Create( int nValue /*=0*/ )
{
	DataTradeBase::Create();

	m_VarPos		= CsPoint(m_BlockSize.cx/2, 140);

	AddTitle( UISTRING_TEXT( "DATA_TRADE_TITLE_SCALE" ).c_str(), CsPoint(0, m_TopPos.y + 2), CFont::FS_11);

	SetVariableName((TCHAR*)UISTRING_TEXT( "DATA_TRADE_SCALE" ).c_str());

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, NiColor( 1.0f, 1.0f, 0.5f ) );

	ti.s_Color = FONT_WHITE;
	ti.s_eTextAlign = DT_CENTER;

	//m_pBGLeft	= AddSprite(CsPoint::ZERO, CsPoint( 384, 303 ), "DigimonStatus\\Digimon_Win.tga");
	//m_pBGRight	= AddSprite(CsPoint::ZERO, CsPoint( 384, 303 ), "DigimonStatus\\Digimon_Win.tga");

	CsPoint	pos = GetDataInfo(SLOT_LEFT)->GetIconPos();
 
	//m_pDigimonScaleLeft = AddText( &ti, CsPoint( GetPopup(1)->GetPos().x+GetPopup(1)->GetSize().x/2, m_LeftPos.y+230 ) );
	m_pDigimonScaleLeft = AddText( &ti, pos +m_VarPos );

	//m_pDigimonScaleRight = AddText( &ti, CsPoint( GetPopup(2)->GetPos().x+GetPopup(2)->GetSize().x/2, m_LeftPos.y+230 ) );

	pos = GetDataInfo(SLOT_RIGHT)->GetIconPos();
	m_pDigimonScaleRight = AddText( &ti, pos +m_VarPos );

	SetDigimonScaleLeft(0, 0, 0);

	SetDigimonScaleRight(0, 0, 0);
}

void DataTradeDigimonScale::Update(float const& fDeltaTime)
{
	DataTradeBase::Update(fDeltaTime);
}

cBaseWindow::eMU_TYPE DataTradeDigimonScale::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = DataTradeBase::Update_ForMouse();
	
	return muReturn;
}

void DataTradeDigimonScale::Render()
{
	if(IsFlicker())
	{
		m_pDigimonScaleLeft->SetAlpha(0.0f);
		m_pDigimonScaleRight->SetAlpha(0.0f);
	}
	else
	{
		m_pDigimonScaleLeft->SetAlpha(1.0f);
		m_pDigimonScaleRight->SetAlpha(1.0f);
	}

	DataTradeBase::Render();
}

void DataTradeDigimonScale::CreateTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo)
{
	SAFE_POINTER_RET(pInfo);
	DataTradeBase::CreateTradeSlotData(etype, pInfo);

	stLoadData*		pLoadData	= pInfo->GetLoadData();
	SAFE_POINTER_RET(pLoadData);

	//DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimon( DigimonID )->GetInfo()->s_dwModelID;

	stModelInfo*	pModelInfo	= pInfo->GetModelInfo();
	SAFE_POINTER_RET(pModelInfo);

	int	ScalePercent = CsFloat2Int(pLoadData->s_fScale * 100.0f);
	int	CmSize = GetPercentCmSize(pLoadData, pModelInfo);
	int HatchLV = pLoadData->s_HatchLevel;

	// 크기 정보 생성.
	switch(etype)
	{
	case SLOT_LEFT:
		SetDigimonScaleLeft(CmSize, ScalePercent, HatchLV);
		break;

	case SLOT_RIGHT:
		SetDigimonScaleRight(CmSize, ScalePercent, HatchLV);
		break;
	}
}

void DataTradeDigimonScale::DeleteTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo)
{
	DataTradeBase::DeleteTradeSlotData(etype, pInfo);

	// 크기 정보 삭제.
	switch(etype)
	{
	case SLOT_LEFT:
		SetDigimonScaleLeft(0, 0, 0);
		break;

	case SLOT_RIGHT:
		SetDigimonScaleRight(0, 0, 0);
		break;
	}
}
void DataTradeDigimonScale::ResetPosition()
{
	DataTradeBase::ResetPosition();

	CsPoint	pos = GetDataInfo(SLOT_LEFT)->GetIconPos();

	if(m_pDigimonScaleLeft)
		m_pDigimonScaleLeft->SetPos(pos+m_VarPos);

	pos =  GetDataInfo(SLOT_RIGHT)->GetIconPos();

	if(m_pDigimonScaleRight)
		m_pDigimonScaleRight->SetPos(pos+m_VarPos);
}
void DataTradeDigimonScale::ResetData()
{
	// 좌측 정보 리셋
	stLoadData*		pLoadData	= GetDataInfo(SLOT_LEFT)->GetLoadData();
	SAFE_POINTER_RET(pLoadData);

	stModelInfo*	pModelInfo	= GetDataInfo(SLOT_LEFT)->GetModelInfo();
	SAFE_POINTER_RET(pModelInfo);

	int		ScalePercent = CsFloat2Int(pLoadData->s_fScale * 100.0f);
	int		CmSize = GetPercentCmSize(pLoadData, pModelInfo);
	int		HatchLV = pLoadData->s_HatchLevel;

	SetDigimonScaleLeft(CmSize, ScalePercent, HatchLV);

	// 우측 정보 리셋
	pLoadData	= GetDataInfo(SLOT_RIGHT)->GetLoadData();
	SAFE_POINTER_RET(pLoadData);

	pModelInfo	= GetDataInfo(SLOT_RIGHT)->GetModelInfo();
	SAFE_POINTER_RET(pModelInfo);

	ScalePercent = CsFloat2Int(pLoadData->s_fScale * 100.0f);
	CmSize = GetPercentCmSize(pLoadData, pModelInfo);
	HatchLV = pLoadData->s_HatchLevel;

	SetDigimonScaleRight(CmSize, ScalePercent, HatchLV);

	StartFlicker();
}
void DataTradeDigimonScale::SetDigimonScaleLeft(int cmSize, int percentSize, int hatchlv)
{
	SAFE_POINTER_RET(m_pDigimonScaleLeft);

	std::wstring text = UISTRING_TEXT( "DATA_TRADE_SCALE_INFO" );
	DmCS::StringFn::ReplaceAll( text, L"#DigimonHeight#", cmSize );
	DmCS::StringFn::ReplaceAll( text, L"#DigimonScale#", percentSize );
	DmCS::StringFn::ReplaceAll( text, L"#DigimonHatchLV#", hatchlv );
	m_pDigimonScaleLeft->SetText(text.c_str());
}

void DataTradeDigimonScale::SetDigimonScaleRight(int cmSize, int percentSize, int hatchlv)
{
	SAFE_POINTER_RET(m_pDigimonScaleRight);
	std::wstring text = UISTRING_TEXT( "DATA_TRADE_SCALE_INFO" );
	DmCS::StringFn::ReplaceAll( text, L"#DigimonHeight#", cmSize );
	DmCS::StringFn::ReplaceAll( text, L"#DigimonScale#", percentSize );
	DmCS::StringFn::ReplaceAll( text, L"#DigimonHatchLV#", hatchlv );
	m_pDigimonScaleRight->SetText(text.c_str());
	//TCHAR szTall[ 32 ] = { 0, };
	//_stprintf_s( szTall, 32, _T( "%d cm (%d%%)" ), cmSize, percentSize);
	//m_pDigimonScaleRight->SetText(szTall);
}
int DataTradeDigimonScale::GetPercentCmSize(stLoadData*		pLoadData, stModelInfo*	pModelInfo)
{
	float Scale = pLoadData->s_fScale;
	float SizeScale = Scale * pModelInfo->s_fScale;	// 모델의 스케일값이 일정하지 않아서 추가 계산
	int	  CmSize = CsFloat2Int(pModelInfo->s_fHeight * 100.0f * SizeScale);

	return	CmSize;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool DataTradeDigimonScale::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eScale_Reset, this );
	GetSystem()->Register( SystemType::eScale_Trade, this );

	return true;
}

void DataTradeDigimonScale::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eScale_Reset:
		ResetData();
		break;
	case SystemType::eScale_Trade:
		SendToTrade();
		break;
	}
}