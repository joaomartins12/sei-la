#include "StdAfx.h"

#include "DataTradeContents.h"
#include "DataTradeBase.h"
#include "DataTradeDigimonEvolution.h"

#define		IF_DIGIMON_EVOL_ICONSIZE			CsPoint( 24, 24 )
#define		IF_DIGIMON_EVOL_LINE_DELTAPOS		13

#define		MAIN_SIZE		CsPoint( 384, 303 )
#define		EVOL_SIZE		CsPoint( GetPopup(1)->GetSize().x, GetPopup(1)->GetSize().y/3 )

#define		EVOL_POS_LEFT		CsPoint(GetPopup(1)->GetPos() + m_VarPos)
#define		EVOL_POS_RIGHT		CsPoint(GetPopup(2)->GetPos() + m_VarPos)

//////////////////////////////////////////////////////////////////////////
//////////////////// DataTradeContents::ST_EVOL_INFO  //////////////////////
//////////////////////////////////////////////////////////////////////////
DataTradeDigimonEvolution::ST_EVOL_INFO::ST_EVOL_INFO() : s_pImage(NULL), s_eSlotState(CDigimonUser::eNone)
{
}
DataTradeDigimonEvolution::ST_EVOL_INFO::~ST_EVOL_INFO()
{
	Delete();
}
void DataTradeDigimonEvolution::ST_EVOL_INFO::Delete()
{
	SAFE_NIDELETE( s_pImage );
}
void DataTradeDigimonEvolution::ST_EVOL_INFO::ResetDevice()
{
	if( s_pImage )
		s_pImage->ResetDevice();
}

//////////////////////////////////////////////////////////////////////////
////////////////////	DataTradeDigimonEvolution	//////////////////////
//////////////////////////////////////////////////////////////////////////
DataTradeDigimonEvolution::DataTradeDigimonEvolution(void) : m_pEvolBG(NULL), m_pLineLR(NULL),
m_pLineTB(NULL),	m_pIconBG(NULL),	m_pEvolBlock(NULL),	m_pRideMask(NULL)//,	m_pEvolLockBtn(NULL)
{
}

DataTradeDigimonEvolution::~DataTradeDigimonEvolution(void)
{
	Destroy(); 

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}
void DataTradeDigimonEvolution::Destroy()
{	
	DataTradeBase::Destroy();
}

void DataTradeDigimonEvolution::DeleteResource()
{
	DataTradeBase::DeleteResource();

	DeleteEvolutionInfoLeft();
	DeleteEvolutionInfoRight();

	SAFE_NIDELETE( m_pEvolBG );
	SAFE_NIDELETE( m_pLineLR );
	SAFE_NIDELETE( m_pLineTB );
	SAFE_NIDELETE( m_pIconBG );
	SAFE_NIDELETE( m_pRideMask );
	SAFE_NIDELETE( m_pEvolBlock );
	//SAFE_NIDELETE( m_pEvolLockBtn );
}

void DataTradeDigimonEvolution::ResetDevice()
{
	DataTradeBase::ResetDevice();

	if( m_pEvolBG )
		m_pEvolBG->ResetDevice();
	if( m_pIconBG )
		m_pIconBG->ResetDevice();
	if( m_pEvolBlock )
		m_pEvolBlock->ResetDevice();
	if( m_pLineLR )
		m_pLineLR->ResetDevice();
	if( m_pLineTB )
		m_pLineTB->ResetDevice();
	if( m_pRideMask )
		m_pRideMask->ResetDevice();
	//if( m_pEvolLockBtn )
	//	m_pEvolLockBtn->ResetDevice();
}

bool DataTradeDigimonEvolution::Close( bool bSound /* = true  */ )
{
	DataTradeBase::Close( bSound );
	return true;
}

void DataTradeDigimonEvolution::Create( int nValue /*=0*/ )
{
	DataTradeBase::Create();

	m_VarPos	= CsPoint(0, 140);

	AddTitle( UISTRING_TEXT( "DATA_TRADE_TITLE_EVOLUTION" ).c_str(), CsPoint(0, m_TopPos.y + 2), CFont::FS_11);
	
	SetVariableName((TCHAR*)UISTRING_TEXT( "DATA_TRADE_EVOLUTION" ).c_str());

	// 공통 정보 생성
	//m_pEvolBG = NiNew cSprite;
	//m_pEvolBG->Init( NULL, CsPoint::ZERO, EVOL_SIZE, "DigimonStatus\\Evol\\BG.tga", false );

	m_pLineLR = NiNew cSprite;
	m_pLineLR->Init( NULL, CsPoint::ZERO, CsPoint( 5, 5 ), "DigimonStatus\\Evol\\LineLR.tga", false );

	m_pLineTB = NiNew cSprite;
	m_pLineTB->Init( NULL, CsPoint::ZERO, CsPoint( 5, 5 ), "DigimonStatus\\Evol\\LineTB.tga", false );

	m_pIconBG = NiNew cSprite;
	m_pIconBG->Init( NULL, CsPoint::ZERO, IF_DIGIMON_EVOL_ICONSIZE+CsPoint( 12, 12 ), "DigimonStatus\\Evol\\IconBG.tga", false );

	m_pEvolBlock = NiNew cSprite;
	m_pEvolBlock->Init( NULL, CsPoint::ZERO, IF_DIGIMON_EVOL_ICONSIZE, "DigimonStatus\\Evol\\Block.tga", false );

	//m_pEvolLockBtn = NiNew cButton;
	//m_pEvolLockBtn->Init( NULL, CsPoint::ZERO, IF_DIGIMON_EVOL_ICONSIZE, "DigimonStatus\\Evol\\BlockBtn.tga", false );
	//m_pEvolLockBtn->SetTexToken( CsPoint( 0, 32 ) );

	m_pRideMask = NiNew cSprite;
	m_pRideMask->Init( NULL, CsPoint::ZERO, CsPoint( 23, 23 ), "DigimonStatus\\Evol\\RideMask.tga", false );
}

void DataTradeDigimonEvolution::Update(float const& fDeltaTime)
{
	DataTradeBase::Update(fDeltaTime);

	_SlotTooltip(m_EvolInfoLeft, SLOT_LEFT, EVOL_POS_LEFT);

	_SlotTooltip(m_EvolInfoRight, SLOT_RIGHT, EVOL_POS_RIGHT);
}

cBaseWindow::eMU_TYPE DataTradeDigimonEvolution::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = DataTradeBase::Update_ForMouse();
	return muReturn;
}

void DataTradeDigimonEvolution::Render()
{
	DataTradeBase::Render();

	if(IsFlicker())	// 데이터 교환 성공후 플리커(깜박임) 기능 중일 때
		return;
	
	_RenderEvolLeft();
	_RenderEvolRight();
}

void DataTradeDigimonEvolution::CreateEvolutionInfoLeft()
{
	_DATA_INFO*	pInfo = GetDataInfo(SLOT_LEFT);

	stLoadData*	pData = pInfo->GetLoadData();

	SAFE_POINTER_RET(pData);

	CDigimonEvolveInfo* pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( pData->s_dwBaseDigimonID );	// 현재 소환된 디지몬의 ID 값으로 정보를 불러온다.

	const CsPoint ptStartPos( 20, -15 );
	CsPoint ptIconPos = ptStartPos;
	CsPoint ptIconGab( 32, 64 );

	for( int i=0; i<nLimit::EvoUnit; ++i )	// 진화 정보 16개를 돌려서..
	{
		CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( i );
		cEvoUnit	EvoUnit = pData->s_EvoUnit[i];		

		if( ( pFTEvolObj == NULL )||( pFTEvolObj->m_nEnableSlot == 0 ) )
		{		
			m_EvolInfoLeft[ i ].s_eSlotState = CDigimonUser::eNone;
			continue;
		}

		m_EvolInfoLeft[ i ].s_IFIcon.Init( CsRect( ptIconPos, (CsSIZE)IF_DIGIMON_EVOL_ICONSIZE) );

		// 진화 이미지 이름 찾기
		CsDigimon::sINFO* pFTInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( pFTEvolObj->m_dwID )->GetInfo();
		std::string strPath = g_pModelDataMng->GetEvoModelIconFile( pFTInfo->s_dwModelID );
		if(strPath.empty())
			continue;

		SAFE_NIDELETE(m_EvolInfoLeft[ i ].s_pImage);

		assert_cs( m_EvolInfoLeft[ i ].s_pImage == NULL );

		// 진화 이미지 로드		
		m_EvolInfoLeft[ i ].s_pImage = NiNew cImage;
		m_EvolInfoLeft[ i ].s_pImage->Init( NULL, CsPoint::ZERO, IF_DIGIMON_EVOL_ICONSIZE, strPath.c_str(), false, false );
		m_EvolInfoLeft[ i ].s_pImage->SetTexToken( CsPoint( 32, 0 ) );

		ptIconPos.x += (IF_DIGIMON_EVOL_ICONSIZE.x + ptIconGab.x);
		if( ptIconPos.x + ptIconGab.x >= 290/*BG width*/ )
		{
			ptIconPos.y += ptIconGab.y;
			ptIconPos.x = ptStartPos.x;
		}
	}

	for( int i=0; i<nLimit::EvoUnit; ++i )	// 진화 정보 16개를 돌려서.. 이미지 정보 생성.
	{
		CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( i );

		if( ( pFTEvolObj == NULL )||( pFTEvolObj->m_nEnableSlot == 0 ) )
		{		
			m_EvolInfoLeft[ i ].s_eSlotState = CDigimonUser::eNone;
			continue;
		}

		cEvoUnit	EvoUnit = pData->s_EvoUnit[i];	

		if( pFTEvolObj->m_nOpenItemTypeS != 0 ) 
		{
			// 해당 슬롯이 활성됐냐?
			if( EvoUnit.m_nSlotState & nEvo::Opened )
			{
				m_EvolInfoLeft[ i ].s_eSlotState = CDigimonUser::eInActive;
			}
			else
			{
				m_EvolInfoLeft[ i ].s_eSlotState = CDigimonUser::eClosed;
				continue;
			}
		}

		// 해당 슬롯에 칩셋 조건이 있고 퀘스트 조건이 있으면(테이블값)
		if( pFTEvolObj->m_nChipsetType != 0 && pFTEvolObj->m_nOpenQuest != 0 ) 
		{
			if( ( EvoUnit.m_nSlotState & nEvo::Opened) && pData->s_nLevel >= pFTEvolObj->m_nOpenLevel)	//퀘스트 완료하고 슬롯이 활성화가 안되는 문제가 있다. 
			{
				m_EvolInfoLeft[ i ].s_eSlotState = CDigimonUser::eActive;
			}
			// 해당 슬롯이 활성됐냐?
			else if( EvoUnit.m_nSlotState & nEvo::Opened )
			{
				m_EvolInfoLeft[ i ].s_eSlotState = CDigimonUser::eInActive;		
			}
			else	
			{
				m_EvolInfoLeft[ i ].s_eSlotState = CDigimonUser::eClosed;
				continue;
			}
		}
		else if( ( pFTEvolObj->m_nEnableSlot == 1 )&&( pData->s_nLevel >= pFTEvolObj->m_nOpenLevel ) )
		{// 해당 슬롯이 사용가능하고 오픈래벨보다 이상이면.
			// 퀘스트 조건이있으면
			if( pFTEvolObj->m_nOpenQuest != 0 )
			{
				// 퀘스트 완료 했냐?
				if( g_pDataMng->GetQuest()->IsCompleate( pFTEvolObj->m_nOpenQuest ) == true )
					m_EvolInfoLeft[ i ].s_eSlotState = CDigimonUser::eActive;
				else
					m_EvolInfoLeft[ i ].s_eSlotState = CDigimonUser::eInActive;																
			}
			else
			{
				m_EvolInfoLeft[ i ].s_eSlotState = CDigimonUser::eActive;
			}
		}

		// 그외
		else
		{
			m_EvolInfoLeft[ i ].s_eSlotState = CDigimonUser::eInActive;	
		}

		// 라이드 가능인가
		m_EvolInfoLeft[ i ].s_bActiveRide =( EvoUnit.m_nSlotState & nEvo::RideMode) && nsCsFileTable::g_pDigimonMng->IsRide( pFTEvolObj->m_dwID );

		if(m_EvolInfoLeft[ i ].s_eSlotState == CDigimonUser::eInActive)
			m_EvolInfoLeft[ i ].s_pImage->SetState( cImage::NORMAL2 );
	}
}
void DataTradeDigimonEvolution::CreateEvolutionInfoRight()
{
	_DATA_INFO*	pInfo = GetDataInfo(SLOT_RIGHT);

		stLoadData*	pData = pInfo->GetLoadData();

	SAFE_POINTER_RET(pData);

	CDigimonEvolveInfo* pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( pData->s_dwBaseDigimonID );	// 현재 소환된 디지몬의 ID 값으로 정보를 불러온다.

	const CsPoint ptStartPos( 20, -15 );
	CsPoint ptIconPos = ptStartPos;
	CsPoint ptIconGab( 32, 64 );

	for( int i=0; i<nLimit::EvoUnit; ++i )	// 진화 정보 16개를 돌려서..	0번은 미사용.
	{
		CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( i );
		cEvoUnit	EvoUnit = pData->s_EvoUnit[i];		

		if( ( pFTEvolObj == NULL )||( pFTEvolObj->m_nEnableSlot == 0 ) )
		{		
			m_EvolInfoRight[ i ].s_eSlotState = CDigimonUser::eNone;
			continue;
		}

		m_EvolInfoRight[ i ].s_IFIcon.Init( CsRect( ptIconPos, (CsSIZE)IF_DIGIMON_EVOL_ICONSIZE) );

		// 진화 이미지 이름 찾기
		CsDigimon::sINFO* pFTInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( pFTEvolObj->m_dwID )->GetInfo();
		std::string strPath = g_pModelDataMng->GetEvoModelIconFile( pFTInfo->s_dwModelID );		
		if(strPath.empty())
			continue;

		SAFE_NIDELETE(m_EvolInfoRight[ i ].s_pImage);

		assert_cs( m_EvolInfoRight[ i ].s_pImage == NULL );

		// 진화 이미지 로드		
		m_EvolInfoRight[ i ].s_pImage = NiNew cImage;
		m_EvolInfoRight[ i ].s_pImage->Init( NULL, CsPoint::ZERO, IF_DIGIMON_EVOL_ICONSIZE, strPath.c_str(), false, false );
		m_EvolInfoRight[ i ].s_pImage->SetTexToken( CsPoint( 32, 0 ) );

		ptIconPos.x += (IF_DIGIMON_EVOL_ICONSIZE.x + ptIconGab.x);
		if( ptIconPos.x + ptIconGab.x >= 290/*BG width*/ )
		{
			ptIconPos.y += ptIconGab.y;
			ptIconPos.x = ptStartPos.x;
		}
	}

	for( int i=0; i<nLimit::EvoUnit; ++i )	// 진화 정보 16개를 돌려서.. 이미지 정보 생성.
	{
		CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( i );

		if( ( pFTEvolObj == NULL )||( pFTEvolObj->m_nEnableSlot == 0 ) )
		{		
			m_EvolInfoRight[ i ].s_eSlotState = CDigimonUser::eNone;
			continue;
		}

		cEvoUnit	EvoUnit = pData->s_EvoUnit[i];	

		if( pFTEvolObj->m_nOpenItemTypeS != 0 ) 
		{
			// 해당 슬롯이 활성됐냐?
			if( EvoUnit.m_nSlotState & nEvo::Opened )
			{
				m_EvolInfoRight[ i ].s_eSlotState = CDigimonUser::eInActive;
			}
			else
			{
				m_EvolInfoRight[ i ].s_eSlotState = CDigimonUser::eClosed;
				continue;
			}
		}

		// 해당 슬롯에 칩셋 조건이 있고 퀘스트 조건이 있으면(테이블값)
		if( pFTEvolObj->m_nChipsetType != 0 && pFTEvolObj->m_nOpenQuest != 0 ) 
		{
			if( ( EvoUnit.m_nSlotState & nEvo::Opened ) && pData->s_nLevel >= pFTEvolObj->m_nOpenLevel)	//퀘스트 완료하고 슬롯이 활성화가 안되는 문제가 있다. 
			{
				m_EvolInfoRight[ i ].s_eSlotState = CDigimonUser::eActive;

			}
			// 해당 슬롯이 활성됐냐?
			else if( EvoUnit.m_nSlotState & nEvo::Opened )
			{
				m_EvolInfoRight[ i ].s_eSlotState = CDigimonUser::eInActive;		
			}
			else	
			{
				m_EvolInfoRight[ i ].s_eSlotState = CDigimonUser::eClosed;
				continue;
			}
		}
		else if( ( pFTEvolObj->m_nEnableSlot == 1 )&&( pData->s_nLevel >= pFTEvolObj->m_nOpenLevel ) )
		{// 해당 슬롯이 사용가능하고 오픈래벨보다 이상이면.
			// 퀘스트 조건이있으면
			if( pFTEvolObj->m_nOpenQuest != 0 )
			{
				// 퀘스트 완료 했냐?
				if( g_pDataMng->GetQuest()->IsCompleate( pFTEvolObj->m_nOpenQuest ) == true )
					m_EvolInfoRight[ i ].s_eSlotState = CDigimonUser::eActive;
				else
					m_EvolInfoRight[ i ].s_eSlotState = CDigimonUser::eInActive;																
			}
			else
			{
				m_EvolInfoRight[ i ].s_eSlotState = CDigimonUser::eActive;
			}
		}

		// 그외
		else
		{
			m_EvolInfoRight[ i ].s_eSlotState = CDigimonUser::eInActive;	
		}

		// 라이드 가능인가
		m_EvolInfoRight[ i ].s_bActiveRide =( EvoUnit.m_nSlotState & nEvo::RideMode) && nsCsFileTable::g_pDigimonMng->IsRide( pFTEvolObj->m_dwID );

		if(m_EvolInfoRight[ i ].s_eSlotState == CDigimonUser::eInActive)
			m_EvolInfoRight[ i ].s_pImage->SetState( cImage::NORMAL2 );
	}
}

void DataTradeDigimonEvolution::DeleteEvolutionInfoLeft()
{
	for( int i=0; i<nLimit::EvoUnit; ++i )	// 진화 정보 16개를 돌려서..
	{
		SAFE_NIDELETE(m_EvolInfoLeft[ i ].s_pImage);
	}
}

void DataTradeDigimonEvolution::DeleteEvolutionInfoRight()
{
	for( int i=0; i<nLimit::EvoUnit; ++i )	// 진화 정보 16개를 돌려서..
	{
		SAFE_NIDELETE(m_EvolInfoRight[ i ].s_pImage);
	}
}

void DataTradeDigimonEvolution::CreateTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo)
{
	SAFE_POINTER_RET(pInfo);

	DataTradeBase::CreateTradeSlotData(etype, pInfo);

	// 진화 정보 생성.
	switch(etype)
	{
	case SLOT_LEFT:
		CreateEvolutionInfoLeft();
		break;

	case SLOT_RIGHT:
		CreateEvolutionInfoRight();
		break;
	}

}
void DataTradeDigimonEvolution::DeleteTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo)
{
	DataTradeBase::DeleteTradeSlotData(etype, pInfo);
}

void DataTradeDigimonEvolution::ResetPosition()
{
	DataTradeBase::ResetPosition();
}

void DataTradeDigimonEvolution::ResetData()
{
	CreateEvolutionInfoLeft();
	CreateEvolutionInfoRight();

	StartFlicker();
}
void DataTradeDigimonEvolution::_RenderEvolLeft()
{
	_DATA_INFO*	pLeftInfo	= GetDataInfo(SLOT_LEFT);

	SAFE_POINTER_RET(pLeftInfo);

	stLoadData*		pData = pLeftInfo->GetLoadData();

	SAFE_POINTER_RET(pData);

	// 진화 이미지
	CsPoint pos;	
	for( int i=0; i<nLimit::EvoUnit; ++i )
	{
		ST_EVOL_INFO*	pEvoInfo = &m_EvolInfoLeft[i];

		if( pEvoInfo->s_eSlotState == CDigimonUser::eNone )
			continue;

		pos = EVOL_POS_LEFT + pEvoInfo->s_IFIcon.GetPos();
		m_pIconBG->Render( pos - CsPoint( 6, 6 ) );
		switch( pEvoInfo->s_eSlotState )
		{
		case CDigimonUser::eActive:
		case CDigimonUser::eInActive:
			{
				pEvoInfo->s_pImage->Render( pos );
				if( pEvoInfo->s_IFIcon.s_eState != sIFIcon::NONE )
					pEvoInfo->s_IFIcon.RenderMask( pos, IF_DIGIMON_EVOL_ICONSIZE );
				if( pEvoInfo->s_bActiveRide )	// 라이드 마스크
					m_pRideMask->Render( pos + CsPoint( IF_DIGIMON_EVOL_LINE_DELTAPOS +2 , IF_DIGIMON_EVOL_LINE_DELTAPOS + 2 ) );
			}
			break;
		case CDigimonUser::eClosed:
			m_pEvolBlock->Render( pos );	// 벽돌 슬롯랜더
			break;
		//case ST_EVOL_INFO::SS_LOCK:
		//	m_pEvolLockBtn->SetPos(pos);
		//	m_pEvolLockBtn->Render();	// 좌물쇠 슬롯 랜더
		//	break;
		}
	}
}
void DataTradeDigimonEvolution::_RenderLineLeft()
{
	_DATA_INFO*	pLeftInfo	= GetDataInfo(SLOT_LEFT);


	stLoadData*		pData = pLeftInfo->GetLoadData();
	//stModelInfo*	pMInfo	= pLeftInfo->GetModelInfo();

	CDigimonEvolveInfo* pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( pData->s_dwBaseDigimonID );	// 현재 소환된 디지몬의 ID 값으로 정보를 불러온다.	// 베이스 디지몬의 진화 정보를 얻고

	CDigimonEvolveObj* pFTEvolObj;	// 테이블 정보

	int	nCurTree = g_pCharMng->GetDigimonUser( 0 )->GetEvoTree();
	// 라인
	std::map< int, bool > ExistMap;
	for( int i=0; i < nLimit::EvoUnit; ++i )	// 진화 사이즈만큼 돌려서
	{
		ST_EVOL_INFO*	pEvoInfo = &m_EvolInfoLeft[i];

		if( pEvoInfo->s_eSlotState == CDigimonUser::eNone )	// 슬롯 상태가 NONE 이면 넘기고
			continue;

		ExistMap.clear();	// 데이터 클리어

		pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( i );	// n 번째 진화 정보
		for( int o=0; o<MAX_EVOLUTION-1; ++o )	// 6개
		{
			int nSlot = pFTEvolObj->m_nEvolutionList[ o ].nSlot;
			if( nSlot == 0 )
				continue;
			
			ST_EVOL_INFO*	pSlotnfo = &m_EvolInfoLeft[nSlot];

			if( pSlotnfo->s_eSlotState == CDigimonUser::eNone )
				continue;

			CDigimonEvolveObj* pFTDestEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( nSlot );		
			for( int d=0; d<MAX_EVOLUTION-1; ++d ) // 6개
			{
				if( pFTDestEvolObj->m_nEvolutionList[ d ].nSlot != 0 )
				{
					ExistMap[ pFTDestEvolObj->m_nEvolutionList[ d ].nSlot ] = true;
				}
			}
			if( ExistMap.find( nSlot ) != ExistMap.end() )	// 데이터가 있으면
				continue;

			/* 테이블에 정의된 현재슬롯 트리 != 현재 찍은 트리*/
			if( pFTDestEvolObj->m_nEvolutionTree != nCurTree && (CDigimonUser::TREE_COMMON != nCurTree ) )
			{
				if( g_pCharMng->GetDigimonUser( 0 )->IsJogressTree( pFTDestEvolObj->m_nEvolutionTree ) )
					continue;
			}

			CsPoint ptSrc = pEvoInfo->s_IFIcon.GetPos();
			CsPoint ptDest = pSlotnfo->s_IFIcon.GetPos();
			CsPoint ptIconSize = IF_DIGIMON_EVOL_ICONSIZE;
			if( ptSrc.y == ptDest.y )
			{
				CsPoint pos = ptSrc + CsPoint( ptIconSize.x, IF_DIGIMON_EVOL_LINE_DELTAPOS ) + EVOL_POS_LEFT;
				CsPoint size;
				size.x = ptDest.x - ptSrc.x - ptIconSize.x;
				size.y = 5;
				m_pLineLR->Render( pos, size );
			}
			else if( ptSrc.x == ptDest.x )
			{
				CsPoint pos = ptSrc + CsPoint( IF_DIGIMON_EVOL_LINE_DELTAPOS, ptIconSize.y ) + EVOL_POS_LEFT;
				CsPoint size;
				size.x = 5;
				size.y = ptDest.y - ptSrc.y - ptIconSize.y;
				m_pLineTB->Render( pos, size );
			}
			else if( ptSrc.y < ptDest.y )
			{
				CsPoint pos = ptSrc + CsPoint( ptIconSize.x, IF_DIGIMON_EVOL_LINE_DELTAPOS ) + EVOL_POS_LEFT;
				CsPoint size;
				size.x = 23;
				size.y = 5;
				m_pLineLR->Render( pos, size );

				CsPoint pos2 = pos + size - CsPoint( 5, 1 );
				CsPoint size2;
				size2.x = 5;
				size2.y = ptDest.y - ptSrc.y - 3;				

				pos = pos2 + size2 - CsPoint( 5, 1 );
				size.x = (ptDest.x - ptSrc.x - ptIconSize.x ) - 23;
				size.y = 5;
				m_pLineLR->Render( pos, size );

				m_pLineTB->Render( pos2, size2 );
			}
			else
			{
				CsPoint pos = ptSrc + CsPoint( ptIconSize.x, IF_DIGIMON_EVOL_LINE_DELTAPOS ) + EVOL_POS_LEFT;
				CsPoint size;
				size.x = 23;
				size.y = 5;
				m_pLineLR->Render( pos, size );

				CsPoint pos2 = pos + size - CsPoint( 5, 1 );
				CsPoint size2;
				size2.x = 5;
				size2.y = ptDest.y - ptSrc.y - 3;				

				pos = pos2 + size2 - CsPoint( 5, 1 );
				size.x = (ptDest.x - ptSrc.x - ptIconSize.x ) - 23;
				size.y = 5;
				m_pLineLR->Render( pos, size );

				size2.y = -size2.y;
				pos2.y -= size2.y;
				m_pLineTB->Render( pos2, size2 );
			}
		}
	}
}


void DataTradeDigimonEvolution::_RenderEvolRight()
{
	_DATA_INFO*	pInfo	= GetDataInfo(SLOT_RIGHT);

	SAFE_POINTER_RET(pInfo);

	stLoadData*		pData = pInfo->GetLoadData();

	SAFE_POINTER_RET(pData);

	// 진화 이미지
	CsPoint pos;	
	for( int i=0; i<nLimit::EvoUnit; ++i )
	{
		ST_EVOL_INFO*	pEvoInfo = &m_EvolInfoRight[i];

		if( pEvoInfo->s_eSlotState == CDigimonUser::eNone )
			continue;

		pos = EVOL_POS_RIGHT + pEvoInfo->s_IFIcon.GetPos();
		m_pIconBG->Render( pos - CsPoint( 6, 6 ) );
		switch( pEvoInfo->s_eSlotState )
		{
		case CDigimonUser::eActive:
		case CDigimonUser::eInActive:
			{
				pEvoInfo->s_pImage->Render( pos );
				if( pEvoInfo->s_IFIcon.s_eState != sIFIcon::NONE )
					pEvoInfo->s_IFIcon.RenderMask( pos, IF_DIGIMON_EVOL_ICONSIZE );
				if( pEvoInfo->s_bActiveRide )	// 라이드 마스크
					m_pRideMask->Render( pos + CsPoint( IF_DIGIMON_EVOL_LINE_DELTAPOS +2 , IF_DIGIMON_EVOL_LINE_DELTAPOS + 2 ) );
			}
			break;
		case CDigimonUser::eClosed:
			m_pEvolBlock->Render( pos );	// 벽돌 슬롯랜더
			break;
		//case ST_EVOL_INFO::SS_LOCK:
		//	m_pEvolLockBtn->SetPos(pos);
		//	m_pEvolLockBtn->Render();	// 좌물쇠 슬롯 랜더
		//	break;
		}
	}
}

void DataTradeDigimonEvolution::_RenderLineRight()
{
	_DATA_INFO*		pInfo	= GetDataInfo(SLOT_RIGHT);

	stLoadData*		pData = pInfo->GetLoadData();
	//stModelInfo*	pMInfo	= pInfo->GetModelInfo();

	CDigimonEvolveInfo* pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( pData->s_dwBaseDigimonID );	// 현재 소환된 디지몬의 ID 값으로 정보를 불러온다.	// 베이스 디지몬의 진화 정보를 얻고

	CDigimonEvolveObj* pFTEvolObj;	// 테이블 정보

	int	nCurTree = g_pCharMng->GetDigimonUser( 0 )->GetEvoTree();

	// 라인
	std::map< int, bool > ExistMap;
	for( int i=0; i < nLimit::EvoUnit; ++i )	// 진화 사이즈만큼 돌려서
	{
		ST_EVOL_INFO*	pEvoInfo = &m_EvolInfoRight[i];

		if( pEvoInfo->s_eSlotState == CDigimonUser::eNone )	// 슬롯 상태가 NONE 이면 넘기고
			continue;

		ExistMap.clear();	// 데이터 클리어

		pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( i );	// n 번째 진화 정보
		for( int o=0; o<MAX_EVOLUTION-1; ++o )	// 6개
		{
			int nSlot = pFTEvolObj->m_nEvolutionList[ o ].nSlot;
			if( nSlot == 0 )
				continue;

			ST_EVOL_INFO*	pSlotnfo = &m_EvolInfoRight[nSlot];

			if( pSlotnfo->s_eSlotState == CDigimonUser::eNone )
				continue;

			CDigimonEvolveObj* pFTDestEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( nSlot );		
			for( int d=0; d<MAX_EVOLUTION-1; ++d ) // 6개
			{
				if( pFTDestEvolObj->m_nEvolutionList[ d ].nSlot != 0 )
				{
					ExistMap[ pFTDestEvolObj->m_nEvolutionList[ d ].nSlot ] = true;
				}
			}
			if( ExistMap.find( nSlot ) != ExistMap.end() )	// 데이터가 있으면
				continue;

			/* 테이블에 정의된 현재슬롯 트리 != 현재 찍은 트리*/
			if( pFTDestEvolObj->m_nEvolutionTree != nCurTree && (CDigimonUser::TREE_COMMON != nCurTree ) )
			{
				if( g_pCharMng->GetDigimonUser( 0 )->IsJogressTree( pFTDestEvolObj->m_nEvolutionTree ) )
					continue;
			}

			CsPoint ptSrc = pEvoInfo->s_IFIcon.GetPos();
			CsPoint ptDest = pSlotnfo->s_IFIcon.GetPos();
			CsPoint ptIconSize = IF_DIGIMON_EVOL_ICONSIZE;
			if( ptSrc.y == ptDest.y )
			{
				CsPoint pos = ptSrc + CsPoint( ptIconSize.x, IF_DIGIMON_EVOL_LINE_DELTAPOS ) + EVOL_POS_RIGHT;
				CsPoint size;
				size.x = ptDest.x - ptSrc.x - ptIconSize.x;
				size.y = 5;
				m_pLineLR->Render( pos, size );
			}
			else if( ptSrc.x == ptDest.x )
			{
				CsPoint pos = ptSrc + CsPoint( IF_DIGIMON_EVOL_LINE_DELTAPOS, ptIconSize.y ) + EVOL_POS_RIGHT;
				CsPoint size;
				size.x = 5;
				size.y = ptDest.y - ptSrc.y - ptIconSize.y;
				m_pLineTB->Render( pos, size );
			}
			else if( ptSrc.y < ptDest.y )
			{
				CsPoint pos = ptSrc + CsPoint( ptIconSize.x, IF_DIGIMON_EVOL_LINE_DELTAPOS ) + EVOL_POS_RIGHT;
				CsPoint size;
				size.x = 23;
				size.y = 5;
				m_pLineLR->Render( pos, size );

				CsPoint pos2 = pos + size - CsPoint( 5, 1 );
				CsPoint size2;
				size2.x = 5;
				size2.y = ptDest.y - ptSrc.y - 3;				

				pos = pos2 + size2 - CsPoint( 5, 1 );
				size.x = (ptDest.x - ptSrc.x - ptIconSize.x ) - 23;
				size.y = 5;
				m_pLineLR->Render( pos, size );

				m_pLineTB->Render( pos2, size2 );
			}
			else
			{
				CsPoint pos = ptSrc + CsPoint( ptIconSize.x, IF_DIGIMON_EVOL_LINE_DELTAPOS ) + EVOL_POS_RIGHT;
				CsPoint size;
				size.x = 23;
				size.y = 5;
				m_pLineLR->Render( pos, size );

				CsPoint pos2 = pos + size - CsPoint( 5, 1 );
				CsPoint size2;
				size2.x = 5;
				size2.y = ptDest.y - ptSrc.y - 3;				

				pos = pos2 + size2 - CsPoint( 5, 1 );
				size.x = (ptDest.x - ptSrc.x - ptIconSize.x ) - 23;
				size.y = 5;
				m_pLineLR->Render( pos, size );

				size2.y = -size2.y;
				pos2.y -= size2.y;
				m_pLineTB->Render( pos2, size2 );
			}
		}
	}
}
void DataTradeDigimonEvolution::_SlotTooltip(ST_EVOL_INFO*	pInfo, eDIGIMONSLOT	eSlot, CsPoint	pos)
{
	CsPoint ptMouseLocalPos = CURSOR_ST.GetPos() - pos;

	stLoadData*	pData = GetDataInfo(eSlot)->GetLoadData();

	if(pData)
	{
		CsPoint toolpos;

		for( int i=0; i<nLimit::EvoUnit; ++i )
		{
			//ST_EVOL_INFO*	pEvoInfo = &m_EvolInfoLeft[i];
			ST_EVOL_INFO*	pEvoInfo = &pInfo[i];

			if( pEvoInfo->s_eSlotState == CDigimonUser::eNone )
				continue;

			if( pEvoInfo->s_IFIcon.s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				toolpos = pEvoInfo->s_IFIcon.GetPos() + pos;
#ifdef VERSION_USA
				int nMaxTooltipX = 340;
#else
				int nMaxTooltipX = TOOLTIP_MAX_SIZE;
#endif
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( toolpos, IF_DIGIMON_EVOL_ICONSIZE, 
					nMaxTooltipX, cTooltip::DATA_TRADE, pData->s_dwBaseDigimonID, i, pEvoInfo->s_eSlotState );
				break;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool DataTradeDigimonEvolution::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eEvolution_Reset, this );
	GetSystem()->Register( SystemType::eEvolution_Trade, this );

	return true;
}

void DataTradeDigimonEvolution::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eEvolution_Reset:
		ResetData();
		break;
	case SystemType::eEvolution_Trade:
		SendToTrade();
		break;
	}
}