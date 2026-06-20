
#include "stdafx.h"
#include "ToolTip.h"

void cTooltip::_MakeTooltip_BuffInfo()
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13 );

	cString* pString;
	TCHAR sz[ TOOLTIP_MAX_TEXT_LEN ];

	cBuffData::sBUFF_DATA*	pBuff;
	CsAchieve::sINFO* pAchieveInfo;
	if( m_nMainID == IF_BUFF_ACHIEVE )	// 업적창에서 띄운 툴팁일 경우
	{		
		pAchieveInfo = (CsAchieve::sINFO*)m_pData;	// 업적정보
		SAFE_POINTER_RET( pAchieveInfo );

		int nBuffID = pAchieveInfo->s_nBuffCode;	// 업적에 연결된 버프

		// 버프 정보 만들기
		cBuffData::sBUFF_DATA pBuffData;
		pBuffData.s_pBuffTable = nsCsFileTable::g_pBuffMng->GetBuff(nBuffID);
		pBuffData.s_nTargetUID = 0;
		pBuffData.s_nBuffCurLV = 0;
		pBuffData.s_dwSkillCode = 0;
		pBuffData.s_nLifeTime = 0;
		pBuff = &pBuffData;
	}
	else
		pBuff = (cBuffData::sBUFF_DATA*)m_pData;
	SAFE_POINTER_RET( pBuff );
	SAFE_POINTER_RET( pBuff->s_pBuffTable );
	CsBuff::sINFO* pBuffInfo = pBuff->s_pBuffTable->GetInfo();
	SAFE_POINTER_RET( pBuffInfo );

	//=================================================================================================================
	//
	//		타이틀
	//
	//=================================================================================================================
	// 이름
	ti.s_eFontSize = CFont::FS_13;

	switch( pBuffInfo->s_nBuffType )
	{
	case 1: //버프	
		ti.s_Color = NiColor( 0, 1, 0 );
		break;
	case 2: //디버프
		ti.s_Color = TOOLTIP_CANNOT_COLOR;
		break;
	case 3: //시스템 버프
		ti.s_Color = NiColor( 1, 1, 0 );
		break;
	default:
		assert_cs( false );
	}

	if( pBuffInfo->s_nBuffIcon >= 4000 )
		pString = _AddIcon( ICONITEM::SKILL4, pBuffInfo->s_nBuffIcon );
	else if( pBuffInfo->s_nBuffIcon >= 3000 )
		pString = _AddIcon( ICONITEM::SKILL3, pBuffInfo->s_nBuffIcon );
	else if( pBuffInfo->s_nBuffIcon >= 2000 )
		pString = _AddIcon( ICONITEM::SKILL2, pBuffInfo->s_nBuffIcon );
	else
		pString = _AddIcon( ICONITEM::SKILL1, pBuffInfo->s_nBuffIcon );

	if( m_nMainID == IF_BUFF_ACHIEVE )	// 업적창에서 띄운 툴팁일 경우
	{
		// 텍스트 사이즈/컬러 돌리고
		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
		ti.s_Color = FONT_WHITE;
		// 업적 이름으로 셋팅
		ti.SetText( pAchieveInfo->s_szName );	
		// 아이콘 빼기
		pString = NiNew cString;
	}
	else
		ti.SetText( pBuffInfo->s_szName );	

	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	_AddLine( false );

	//=================================================================================================================
	//
	//		내용
	//
	//=================================================================================================================	

	pString = NiNew cString;
	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;

	if( pBuffInfo->s_dwID != 40148 )	// 평화 선언일 경우 아래 내용을 표시하지 않는다.
	{
		ti.s_Color = FONT_GOLD;	
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_TYPE" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

		ti.s_Color = FONT_WHITE;

		switch( pBuffInfo->s_nBuffType )
		{
		case 1: //버프
			ti.SetText( UISTRING_TEXT( "TOOLTIP_BUFF_BUFF" ).c_str() );
			break;
		case 2: //디버프
			ti.SetText( UISTRING_TEXT( "TOOLTIP_BUFF_DEBUFF" ).c_str() );
			break;
		case 3: //시스템 버프
			ti.SetText( UISTRING_TEXT( "TOOLTIP_BUFF_SYSTEM_BUFF" ).c_str() );
			break;
		default:
			assert_cs( false );
		}	
		pString->AddText( &ti );
		m_StringList.AddTail( pString );
	}

	// 남은시간
	if( pBuff->s_nLifeTime > 0 && pBuff->s_nLifeTime != UINT_MAX )
	{
		USHORT nBuffClass = pBuffInfo->s_nBuffClass;
		// 버프클래스 999 (업적 적용 버프) 일 경우 남은 시간 표시 안함 chu8820 14.05.28
		// 패시브 스킬 일때는 남은시간 표시 안함.// 그지 같은 코드 ㅡ,.ㅡ;; 언젠가 바꾸자
		if( nBuffClass != IF_BUFF_ACHIEVE && (( nBuffClass < 300 ) || ( nBuffClass > 399 ) ) &&	nBuffClass != 900 && nBuffClass != 901)	
		{
#ifdef TARGETUI_TIMEMASK_DELETE
			if( m_nMainID != IF_BUFF_TARGET )	// 대상의 UI에는 버프를 보여줄 필요가 없다.
#endif
			{
				pString = NiNew cString;
				ti.s_Color = FONT_GOLD;	
				ti.SetText( UISTRING_TEXT( "TOOLTIP_TIME_REMAINING" ).c_str() );
				pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

				ti.s_Color = FONT_WHITE;
				ti.SetText( cTooltip::Time2Str( pBuff->s_nLifeTime, 0, TT_FULL ) );
				pString->AddText( &ti );			

				m_StringList.AddTail( pString );
			}

		}
	}	

// 	if( pBuff->s_nBuffCurLV > 0 )
// 	{
// 		pString = NiNew cString;
// 		ti.s_Color = FONT_GOLD;	
// 		ti.SetText( _LAN( "버프레벨" ) );
// 		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
// 
// 		ti.s_Color = FONT_WHITE;
// 		ti.SetText( pBuff->s_nBuffCurLV );
// 
// 		pString->AddText( &ti );
// 
// 		ti.SetText( L"Lv" );
// 		pString->AddText( &ti );
// 		m_StringList.AddTail( pString );
// 	}

	TCHAR szInt[ 32 ] = {0,};
	bool  bMinus;
	if( nsCsFileTable::g_pSkillMng->IsSkill( pBuffInfo->s_dwSkillCode ) == true )
	{		
		CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( pBuffInfo->s_dwSkillCode );
		SAFE_POINTER_RET( pSkill );
		CsSkill::sINFO* pSkillCode = pSkill->GetInfo();
		SAFE_POINTER_RET( pSkillCode );
		for( int i = 0; i < SKILL_APPLY_MAX_COUNT; i++ ) 
		{
			if( i == 0 &&								// Apply_1 이고
				pSkillCode->s_Apply[0].s_nID == nSkill::Me_002 &&	// Apply_1의 ID가 2(DM_Digimon_List.xls - Skill_Code) 이고
				pSkillCode->s_Apply[ 1 ].s_nA != 0 )	// Apply_2가 있을 때
				continue;

			if( pSkillCode->s_Apply[ i ].s_nA != 0 && !(pSkillCode->s_nMemorySkill) )
			{

				if( pSkillCode->s_Apply[ i ].s_nA == APPLY_ATDMG ||
					pSkillCode->s_Apply[ i ].s_nA == APPLY_EvoTypeDamageIncrease||
					pSkillCode->s_Apply[ i ].s_nA == APPLY_NatureTypeDamageIncrease)	// 속성 데미지 증가일 경우 버프 효과를 표시하지 않는다.
					continue;

				if( pSkillCode->s_Apply[ i ].s_nA == APPLY_WEAKNESS ||			// 쇠약 = 받는 피해량
					pSkillCode->s_Apply[ i ].s_nA == APPLY_ATTRIBUTEDAMAGE ||	// 속성 피해량	
					pSkillCode->s_Apply[ i ].s_nA == APPLY_ATTRIBUTEDR ||		// 속성 반사 = 속성 피해량 반사
					pSkillCode->s_Apply[ i ].s_nA == APPLY_ATTRIBUTEDS ||		// 속성 흡수 = 속성 피해량 흡수
					pSkillCode->s_Apply[ i ].s_nA == APPLY_SILENCE ||			// 침묵 = 스킬 사용 불가)	// 속성 데미지 증가일 경우 버프 효과를 표시하지 않는다.
					pSkillCode->s_Apply[ i ].s_nA == APPLY_NatureShield ||
					pSkillCode->s_Apply[ i ].s_nA == APPLY_RegionBuffDummy)			// 특정 속성의 데미소 감소)	// 속성 데미지 증가일 경우 버프 효과를 표시하지 않는다.
					continue;
				pString = NiNew cString;

				ti.s_Color = FONT_GOLD;	
				ti.SetText( UISTRING_TEXT( "TOOLTIP_BUFF_BUFF_EFFECT" ).c_str() );

				if( i == 0 )
					pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3 ;		
				else
				{
					// 스킬효과는 1번이 버프효과 시작 위치로 한다 ( 0번은 데미지 )
					if( /*pBuff->s_pBuffTable->GetInfo()->s_nBuffType == 2 &&*/ 
						i == 1 && pSkillCode->s_Apply[0].s_nID == nSkill::Me_002 )
						pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
					else
						pString->TailAddSizeX( TOOLTIP_TAB_SIZE * 3 - 5);
				}

				int nValue = 0;

				if( pBuff->s_dwSkillCode != 0 )
					nValue = FMCommon::GetSkillAtt( pBuff->s_dwSkillCode, pBuff->s_nBuffCurLV, i );
				else
					nValue = FMCommon::GetSkillAtt( pBuffInfo->s_dwSkillCode, pBuff->s_nBuffCurLV, i );
  				if(nValue != 0)
				{
					// 크리티컬 / 회피 증가 스킬 추가 14.05.28 chu8820
					if( pSkillCode->s_Apply[ i ].s_nA == APPLY_CA ||
						pSkillCode->s_Apply[ i ].s_nA == APPLY_EV )
					{
						if(pSkillCode->s_Apply[1].s_nID == 207/*nSkill::Me_207*/)
							_stprintf_s( szInt, 32, _T( " %.2f%% " ), ((float)nValue) );
						else
							_stprintf_s( szInt, 32, _T( " %.2f%% " ), ((float)nValue)*0.01f );
					}
					else if( pSkillCode->s_Apply[1].s_nID != 208/*nSkill::Me_208*/ )	// 208 연산은 툴팁 수치를 보여줄 필요 없음.
						_stprintf_s( szInt, 32, _T( " %d " ), nValue );
				}
  				else
  					_stprintf_s( szInt, 32, _T( " " ) );

				if( FMCommon::GetSkillAttStr( pSkillCode->s_Apply[ i ].s_nA, TOOLTIP_MAX_TEXT_LEN, sz, pSkillCode->s_Apply[ i ].s_nB ) == true )
				{
					_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szInt );

					switch( pBuff->s_pBuffTable->GetInfo()->s_nBuffType )
					{
					case 1:
					case 3:
						bMinus = false;
						break;
					case 2:				
						bMinus = true;
						break;
					}	
					// 데미지 반사/흡수 버프 스킬 추가_14.05.14		chu8820
					if( pSkillCode->s_Apply[ i ].s_nA != APPLY_DR || pSkillCode->s_Apply[ i ].s_nA != APPLY_AB )
						FMCommon::GetStrApply_ID( sz, TOOLTIP_MAX_TEXT_LEN, pSkillCode->s_Apply[ i ].s_nID, bMinus );
				}				

				ti.s_Color = FONT_WHITE;
				ti.SetText(	sz );
				pString->AddText( &ti );				

				m_StringList.AddTail( pString );	
			}
		}		
	}
	else
	{
		// 이곳은 버프 테이블에 스킬코드 값이 없는 별도 버프효과 기입하는 곳입니다 수동처리
		__BuffExecpt( pBuff->s_pBuffTable->GetInfo()->s_dwID );
	}

	//=================================================================================================================
	//
	//		설명
	//
	//=================================================================================================================		

	_tcscpy_s( sz, TOOLTIP_MAX_TEXT_LEN, pBuff->s_pBuffTable->GetInfo()->s_szComment );
	if( wcslen( sz ) > 0 )
	{
		_AddLine( true );
		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
		ti.s_Color = FONT_WHITE;
		g_pStringAnalysis->Cut_Parcing( &m_StringList, m_nMaxSizeX-20, sz, &ti );
	}
}

void cTooltip::__BuffExecpt( DWORD dwBuffID )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );

	cString* pString;
	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;	

	TCHAR sz[ TOOLTIP_MAX_TEXT_LEN ];
	TCHAR szInt[ 32 ];

	switch( dwBuffID )
	{
	case nsBuff::BK_DARKTOWER:
	case nsBuff::BK_CANDY_SIZE_CHANGE:
	case nsBuff::BK_EQUIP_FRIENDSHIP:	
	case nsBuff::BK_EQUIP_EXP:			
	case nsBuff::BK_EQUIP_PC:	
	case nsBuff::BK_PARTY_PC:
	case nsBuff::BK_PLAYNET:
	case nsBuff::BK_MACRO_DEF:
	case 40148:
	case 40160:
#ifdef MONSTER_SKILL_GROWTH
	case nsBuff::BK_MONSTER_GROWTH:	//	몬스터 성장 버프
#endif
		break;
	case nsBuff::BK_SPEED_UP:
		{				
			pString = NiNew cString;

			ti.s_Color = FONT_GOLD;	
			ti.SetText( UISTRING_TEXT( "TOOLTIP_BUFF_BUFF_EFFECT" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;		
			ti.s_Color = FONT_WHITE;		

			if( FMCommon::GetSkillAttStr( APPLY_MS, TOOLTIP_MAX_TEXT_LEN, sz ) == true )
			{
#ifdef VERSION_TH
				_stprintf_s( szInt, 32, _T( " %d" ), 20 );
#else
				_stprintf_s( szInt, 32, _T( " %d" ), 30 );
#endif
				_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szInt );

				FMCommon::GetStrApply_ID( sz, TOOLTIP_MAX_TEXT_LEN, 101, false );
			}			

			ti.SetText(	sz );
			pString->AddText( &ti );			
			m_StringList.AddTail( pString );		
		}
		break;		
	case nsBuff::BK_BARCODE_WHITE:	
		{				
			pString = NiNew cString;
			
			ti.s_Color = FONT_GOLD;	
			ti.SetText( UISTRING_TEXT( "TOOLTIP_BUFF_BUFF_EFFECT" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;		
			ti.s_Color = FONT_WHITE;		

			if( FMCommon::GetSkillAttStr( APPLY_EXP, TOOLTIP_MAX_TEXT_LEN, sz ) == true )
			{
				_stprintf_s( szInt, 32, _T( " %d" ), 50 );
				_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szInt );

				FMCommon::GetStrApply_ID( sz, TOOLTIP_MAX_TEXT_LEN, 106, false );
			}			

			ti.SetText(	sz );
			pString->AddText( &ti );			
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			
			ti.s_Color = FONT_GOLD;	
			ti.SetText( UISTRING_TEXT( "TOOLTIP_BUFF_BUFF_EFFECT" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;		
			ti.s_Color = FONT_WHITE;		

			if( FMCommon::GetSkillAttStr( APPLY_FP, TOOLTIP_MAX_TEXT_LEN, sz ) == true )
			{
				_stprintf_s( szInt, 32, _T( " %d" ), 50 );
				_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szInt );

				FMCommon::GetStrApply_ID( sz, TOOLTIP_MAX_TEXT_LEN, 106, true );			
			}
			
			ti.SetText(	sz );
			pString->AddText( &ti );				
			m_StringList.AddTail( pString );	
		}
		break;
	case nsBuff::BK_BARCODE_BLACK:	
		{	
			pString = NiNew cString;
			ti.s_Color = FONT_GOLD;	
			ti.SetText( UISTRING_TEXT( "TOOLTIP_BUFF_BUFF_EFFECT" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;		

			ti.s_Color = FONT_WHITE;			
			if( FMCommon::GetSkillAttStr( APPLY_EXP, TOOLTIP_MAX_TEXT_LEN, sz ) == true )
			{
				_stprintf_s( szInt, 32, _T( " %d" ), 100 );
				_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szInt );

				FMCommon::GetStrApply_ID( sz, TOOLTIP_MAX_TEXT_LEN, 106, false );
			}			

			ti.SetText(	sz );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;

			ti.s_Color = FONT_GOLD;	
			ti.SetText( UISTRING_TEXT( "TOOLTIP_BUFF_BUFF_EFFECT" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;		
			ti.s_Color = FONT_WHITE;		

			if( FMCommon::GetSkillAttStr( APPLY_FP, TOOLTIP_MAX_TEXT_LEN, sz ) == true )
			{
				_stprintf_s( szInt, 32, _T( " %d" ), 100 );
				_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szInt );

				FMCommon::GetStrApply_ID( sz, TOOLTIP_MAX_TEXT_LEN, 106, true );
			}			
			
			ti.SetText(	sz );
			pString->AddText( &ti );			
			m_StringList.AddTail( pString );	
		}
		break;
	case nsBuff::BK_MACRO_EXP:
		{	
			pString = NiNew cString;
			ti.s_Color = FONT_GOLD;	
			ti.SetText( UISTRING_TEXT( "TOOLTIP_BUFF_BUFF_EFFECT" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;		

			ti.s_Color = FONT_WHITE;					
		
			if( FMCommon::GetSkillAttStr( APPLY_EXP, TOOLTIP_MAX_TEXT_LEN, sz ) == true )
			{
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
				_stprintf_s( szInt, 32, _T( " %d" ), 500 ); //버프 500%로 수정.
#else
				_stprintf_s( szInt, 32, _T( " %d" ), 50 ); // 버프 50%로 수정.
#endif
				_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szInt );

				FMCommon::GetStrApply_ID( sz, TOOLTIP_MAX_TEXT_LEN, 105, false );			
			}			

			ti.SetText(	sz );
			pString->AddText( &ti );					

			m_StringList.AddTail( pString );	
		}
		break;	
	case nsBuff::BK_BUFF_SPEED:
		{	
			pString = NiNew cString;
			ti.s_Color = FONT_GOLD;	
			ti.SetText( UISTRING_TEXT( "TOOLTIP_BUFF_BUFF_EFFECT" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;		

			ti.s_Color = FONT_WHITE;					
		
			if( FMCommon::GetSkillAttStr( APPLY_MS, TOOLTIP_MAX_TEXT_LEN, sz ) == true )
			{
				_stprintf_s( szInt, 32, _T( " %d" ), 5 );
				_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szInt );

				FMCommon::GetStrApply_ID( sz, TOOLTIP_MAX_TEXT_LEN, 101, false );			
			}			

			ti.SetText(	sz );
			pString->AddText( &ti );					

			m_StringList.AddTail( pString );	
		}
		break;
	case nsBuff::BK_MONSTER_PLAN_A:
		{	
			pString = NiNew cString;
			ti.s_Color = FONT_GOLD;	
			ti.SetText( UISTRING_TEXT( "TOOLTIP_BUFF_BUFF_EFFECT" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;		

			ti.s_Color = FONT_WHITE;

			if( FMCommon::GetSkillAttStr( APPLY_MS, TOOLTIP_MAX_TEXT_LEN, sz ) == true )
			{
				_stprintf_s( szInt, 32, _T( " 50%%" ) );
				_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szInt );

				FMCommon::GetStrApply_ID( sz, TOOLTIP_MAX_TEXT_LEN, 101, false );			
			}			

			ti.SetText(	sz );
			pString->AddText( &ti );					

			m_StringList.AddTail( pString );	
		}
		break;
	default:
		return;
		//assert_csm( false, L"존재 하지않는 예외처리용 버프타입" );
		//break;

	}

}
