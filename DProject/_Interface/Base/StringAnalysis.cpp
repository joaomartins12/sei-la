#include "stdafx.h"
#include "StringAnalysis.h"

namespace
{
	static const size_t STRING_ANALYSIS_INPUT_MAX = 8192;

	inline std::wstring NormalizeAnalysisInput( TCHAR const* src )
	{
		if( src == NULL )
			return std::wstring();

		std::wstring safe( src );
		if( safe.length() > STRING_ANALYSIS_INPUT_MAX )
			safe.resize( STRING_ANALYSIS_INPUT_MAX );
		return safe;
	}
}


cStringAnalysis*	g_pStringAnalysis = NULL;

CSGBMEMPOOL_CPP( cStringAnalysis::sPARCING );

void cStringAnalysis::GlobalInit()
{
	assert_cs( g_pStringAnalysis == NULL );
	g_pStringAnalysis = NiNew cStringAnalysis;
	g_pStringAnalysis->Init();

	CSGBMEMPOOL_INIT( cStringAnalysis::sPARCING, 8 );
}

void cStringAnalysis::GlobalShotDown()
{
	if( g_pStringAnalysis != NULL )
		g_pStringAnalysis->Delete();
	SAFE_NIDELETE( g_pStringAnalysis );

	CSGBMEMPOOL_DELETE( cStringAnalysis::sPARCING );
}

void cStringAnalysis::Init()
{
	// [4/14/2016 hyun] 추가
	m_ParcingColor = NiColor();
	m_bChangeColor = false;
	// @@ 여기까지
}

void cStringAnalysis::Delete()
{
	// All per-call working state now lives in stack-allocated sCutContext
	// instances inside each Cut*() invocation, so there's nothing held on the
	// singleton to clean up here.  The previous defensive-cleanup block was a
	// band-aid for the same shared-state bug that the context move fixes.
}

//============================================================================================
//
//		파싱
//
//============================================================================================
void cStringAnalysis::_Parcing( sCutContext& ctx, TCHAR const* str, int nStrLen, int& nIndex )
{
	int nOrgIndex = nIndex;
	nIndex += 2;

	// 종료어 인가
	if( str[ nIndex ] == L'/' )
	{
		// 종료어 완료를 할수 있다면
		if( nIndex + 2 < nStrLen )
		{
			nIndex += 1;
			nIndex += _ParcingTypeEnd( ctx, &str[ nIndex ] );
		}
		// 종료어 완료가 안된다면 리턴
		else
		{
			nIndex = nOrgIndex;
		}
	}
	// 시동어 인가
	else
	{
		// 시동어 완료를 할수 있다면
		if( nIndex + 2 < nStrLen )
		{
			ePARCING_TYPE pt;
			// 파싱 스타트
			nIndex += _ParcingTypeStart( &str[ nIndex ], pt );
			// 파싱값 가져 올수 있다면
			if( _ParcingValueEnableCheck( nStrLen, nIndex, pt ) == true )
			{
				nIndex += _ParcingValue( ctx, &str[ nIndex ], pt );
			}
			else
			{
				// 가져 올수 없다면 원상복귀
				nIndex = nOrgIndex;
			}
		}
		// 종료어 완료가 안된다면 원상복귀
		else
		{
			nIndex = nOrgIndex;
		}
	}
}

int cStringAnalysis::_ParcingTypeStart( TCHAR const* str, ePARCING_TYPE& pt )
{
	if( _tcsncmp( str, _T( "tc:" ), 3 ) == 0 )
	{
		pt = PT_TEXT_COLOR;
		return 3;
	}
	else if( _tcsncmp( str, _T( "tb:" ), 3 ) == 0 )
	{
		pt = PT_TEXT_BOLD;
		return 3;
	}

	// 파싱 실패
	pt = PT_NONE;
	return -1;
}

bool cStringAnalysis::_ParcingValueEnableCheck( int nStrLen, int nIndex, ePARCING_TYPE& pt )
{
	switch( pt )
	{
	case PT_TEXT_COLOR:
		// 0 0 0 -> 3자에 > 까지 4자
		return ( nIndex + 4 < nStrLen );
	case PT_TEXT_BOLD:
		// 0 -> 1자에 > 까지 2자
		return ( nIndex + 2 < nStrLen );
	}

	return false;
}

int cStringAnalysis::_ParcingValue( sCutContext& ctx, TCHAR const* str, ePARCING_TYPE& pt )
{
	switch( pt )
	{
	case PT_TEXT_COLOR:
		{
			_MakeToken_Parcing( ctx, false );

			sPARCING* pParcing = sPARCING::NewInstance();
			pParcing->s_eType = pt;

			if(m_bChangeColor) // [4/11/2016 hyun] 지정된 컬러로 변경해야 한다면
			{
				NiColor color = GetParcingColor();
				pParcing->s_Value = CS_XRGB(static_cast<int>(color.r*255),
											static_cast<int>(color.g*255),
											static_cast<int>(color.b*255));
			}
			else // [4/11/2016 hyun] 기존 코드
			{
				// 텍스트를 DWORD 형으로 변환
				TCHAR sz[ 3 ][ 2 ];
				for( int i=0; i<3; ++i ){
					sz[ i ][ 0 ] = str[ i ];
					sz[ i ][ 1 ] = NULL;
				}
				pParcing->s_Value = CS_XRGB(	(DWORD)(_ttoi( sz[ 0 ] )*255/9.0f),
												(DWORD)(_ttoi( sz[ 1 ] )*255/9.0f),
												(DWORD)(_ttoi( sz[ 2 ] )*255/9.0f) );
			}

			_RemoveParcing( ctx, PT_TEXT_COLOR );
			ctx.m_listParcing.push_back( pParcing );
		}
		return 4;		// _ParcingValueEnableCheck 값
	case PT_TEXT_BOLD:
		{
			_MakeToken_Parcing( ctx, false );

			sPARCING* pParcing = sPARCING::NewInstance();
			pParcing->s_eType = pt;

			// 텍스트를 DWORD 형으로 변환
			TCHAR sz[ 2 ];
			sz[ 0 ] = str[ 0 ];
			sz[ 1 ] = NULL;
			pParcing->s_Value = _ttoi( sz );
			_RemoveParcing( ctx, PT_TEXT_BOLD );
			ctx.m_listParcing.push_back( pParcing );
		}
		return 2;		// _ParcingValueEnableCheck 값
	}
	return 0;
}

int cStringAnalysis::_ParcingTypeEnd( sCutContext& ctx, TCHAR const* str )
{
	if( _tcsncmp( str, _T( "tc>" ), 3 ) == 0 )
	{
		_RemoveParcing( ctx, PT_TEXT_COLOR );
		return 3;
	}
	else if( _tcsncmp( str, _T( "tb>" ), 3 ) == 0 )
	{
		_RemoveParcing( ctx, PT_TEXT_BOLD );
		return 3;
	}

	// 파싱 실패
	return -1;
}

void cStringAnalysis::_RemoveParcing( sCutContext& ctx, ePARCING_TYPE pt )
{
	_MakeToken_Parcing( ctx, false );

	std::list< sPARCING* >::iterator it = ctx.m_listParcing.begin();
	std::list< sPARCING* >::iterator itEnd = ctx.m_listParcing.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_eType == pt )
		{
			switch( pt )
			{
			case PT_TEXT_COLOR:
				ctx.m_pTextInfo->s_Color = ctx.m_OrgColor;
				break;
			case PT_TEXT_BOLD:
				ctx.m_pTextInfo->SetBoldLevel( (cText::sTEXTINFO::eBOLD_LEVEL)ctx.m_nOrgBoldLevel );
				break;
			}

			sPARCING::DeleteInstance( *it );
			ctx.m_listParcing.erase( it );
			return;
		}
	}
}

bool cStringAnalysis::_MakeToken_Parcing_ForCardMaster( sCutContext& ctx, CsPoint pos )
{
	if( ctx.m_szToken[ 0 ] == NULL )
		return true;

	// 현재 토큰 생성 하자
	ctx.m_pTextInfo->SetText( ctx.m_szToken );
	ctx.m_pString->AddText( ctx.m_pTextInfo, pos );

	// 초기화
	memset( ctx.m_szToken, 0, sizeof( TCHAR )*MAX_ANAL_LEN );
	ctx.m_nTokenIndex = 0;
	ctx.m_nTotalWidth = 0;

	return true;
}


//============================================================================================
//
//		색깔 구분 해서 문자열 자르기
//
//============================================================================================
bool cStringAnalysis::_MakeToken_Parcing( sCutContext& ctx, bool bNewString )
{
	if( ctx.m_szToken[ 0 ] == NULL )
		return true;

	std::list< sPARCING* >::iterator it = ctx.m_listParcing.begin();
	std::list< sPARCING* >::iterator itEnd = ctx.m_listParcing.end();
	for( ; it!=itEnd; ++it )
	{
		switch( (*it)->s_eType )
		{
		case PT_TEXT_COLOR:
			ctx.m_pTextInfo->s_Color = CsCOLOR2NiColor( (*it)->s_Value );
			break;
		case PT_TEXT_BOLD:
			ctx.m_pTextInfo->SetBoldLevel( (cText::sTEXTINFO::eBOLD_LEVEL)(*it)->s_Value );
			break;
		}
	}

	// 현재 토큰 생성 하자
	ctx.m_pTextInfo->SetText( ctx.m_szToken );
	ctx.m_pString->AddText( ctx.m_pTextInfo );

	std::wstring strToken( ctx.m_szToken );
	ctx.m_strTempText += strToken;

	if( bNewString == true )
	{
		// Disown-then-allocate ordering — see _MakeToken for rationale.
		cString* toAdd = ctx.m_pString;
		ctx.m_pString = NULL;
		ctx.m_pStringList->AddTail( toAdd );
		ctx.m_pString = NiNew cString;
		ctx.m_nTotalWidth = 0;

		m_vecStringTotalText.push_back( ctx.m_strTempText );
		ctx.m_strTempText.clear();
	}
	ctx.m_bNewLine = bNewString;

	// 초기화
	memset( ctx.m_szToken, 0, sizeof( TCHAR )*MAX_ANAL_LEN );
	ctx.m_nTokenIndex = 0;

	return ( --ctx.m_nLineLimit == 0 ) ? false : true;
}


bool cStringAnalysis::_MakeToken( sCutContext& ctx, bool bNewString )
{
	if( ctx.m_szToken[ 0 ] == NULL )
	{
		if( bNewString == false )
			return true;

		_tcscpy_s( ctx.m_szToken, MAX_ANAL_LEN, L" " );
	}

	// 현재 토큰 생성 하자
	ctx.m_pTextInfo->SetText( ctx.m_szToken );

	assert_cs( ctx.m_pStrInfo != NULL );
	switch( ctx.m_pStrInfo->GetBasePlag() )
	{
	case sSTR_INFO::SF_NORMAL:
		_AddText( ctx, CsPoint(0,4) );
		break;
	case sSTR_INFO::SF_NONE:
		_AddText( ctx, CsPoint::ZERO );
		break;
	case sSTR_INFO::SF_BASE_HEIGHT:
		_AddText( ctx, CsPoint( -4*ctx.m_pString->GetListSize(), -ctx.m_pTextInfo->GetHBase() ) );
		break;
	default:
		assert_cs( false );
	}

	if( bNewString == true )
	{
		// Disown-then-allocate ordering: take a local copy, null ctx.m_pString,
		// then AddTail and allocate the next cString. If NiNew throws or hits
		// a downstream FT crash inside cString's construction path,
		// ctx.m_pString stays NULL — the sCutContext destructor won't
		// double-free the cString already owned by the stringList. Prior to
		// the per-call context refactor this slot was on the singleton and a
		// concurrent Cut from another thread would see and free it; that was
		// the root cause of the `cString::AddText -> m_listText.push_back`
		// access violations and the ChatMainWindow → cStringAnalysis::Cut
		// crash where the freed cString memory got reused by the NiAllocator
		// for an unrelated type.
		cString* toAdd = ctx.m_pString;
		ctx.m_pString = NULL;
		ctx.m_pStringList->AddTail( toAdd );
		ctx.m_pString = NiNew cString;
		ctx.m_nTotalWidth = 0;
	}
	ctx.m_bNewLine = bNewString;

	// 초기화
	memset( ctx.m_szToken, 0, sizeof( TCHAR )*MAX_ANAL_LEN );
	ctx.m_nTokenIndex = 0;

	return ( --ctx.m_nLineLimit == 0 ) ? false : true;
}

bool cStringAnalysis::_MakeToken_OnlyNewLine( sCutContext& ctx, bool bNewString )
{
	ctx.m_pTextInfo->SetText( _T( "" ) );
	switch( ctx.m_pStrInfo->GetBasePlag() )
	{
	case sSTR_INFO::SF_NONE:
		_AddText( ctx, CsPoint::ZERO );
		break;
	case sSTR_INFO::SF_BASE_HEIGHT:
		_AddText( ctx, CsPoint( -4*ctx.m_pString->GetListSize(), -ctx.m_pTextInfo->GetHBase() ) );
		break;
	default:
		assert_cs( false );
	}

	if( bNewString == true )
	{
		// Disown-then-allocate; see _MakeToken comment for rationale.
		cString* toAdd = ctx.m_pString;
		ctx.m_pString = NULL;
		ctx.m_pStringList->AddTail( toAdd );
		ctx.m_pString = NiNew cString;
		ctx.m_nTotalWidth = 0;
	}
	ctx.m_bNewLine = bNewString;

	// 초기화
	memset( ctx.m_szToken, 0, sizeof( TCHAR )*MAX_ANAL_LEN );
	ctx.m_nTokenIndex = 0;

	return ( --ctx.m_nLineLimit == 0 ) ? false : true;
}

void cStringAnalysis::_AddText( sCutContext& ctx, CsPoint ptDeltaPos )
{
	assert_cs( ctx.m_pStrInfo != NULL );
	switch( ctx.m_pStrInfo->GetStencilPlag() )
	{
	case sSTR_INFO::SF_NORMAL:
	case sSTR_INFO::SF_NONE:
		ctx.m_pString->AddText( ctx.m_pTextInfo, ptDeltaPos );
		break;
	case sSTR_INFO::SF_STENCIL_USE_1:
		{
			NiStencilProperty* propStencil = NiNew NiStencilProperty;
			propStencil->SetStencilReference( 1 );
			propStencil->SetStencilFunction( NiStencilProperty::TEST_EQUAL );
			propStencil->SetStencilPassAction( NiStencilProperty::ACTION_KEEP );
			propStencil->SetStencilOn( true );

			ctx.m_pString->AddText( propStencil, ctx.m_pTextInfo, ptDeltaPos );
		}
		break;
	default:
		assert_cs( false );
	}
}


void cStringAnalysis::Cut_LinkParcing( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo, cButton::eIMAGE_TYPE Type )
{
	SAFE_POINTER_RET( pStringList );
	SAFE_POINTER_RET( pTextInfo );
	SAFE_POINTER_RET( pTextInfo->s_pFont );
	if( pTextInfo->s_pFont->IsInitialize() == false )
	{
		return;
	}
	std::wstring safeInput = NormalizeAnalysisInput( str );
	if( safeInput.empty() )
	{
		return;
	}
	str = safeInput.c_str();

	sCutContext ctx;
	ctx.m_pTextInfo = pTextInfo;
	ctx.m_pStrInfo = pStringList->GetStrInfo();
	ctx.m_OrgColor = ctx.m_pTextInfo->s_Color;
	ctx.m_nOrgBoldLevel = (int)ctx.m_pTextInfo->GetBoldLevel();
	ctx.m_nLineLimit = -1;
	ctx.m_bNewLine = false;
	ctx.m_pString = NiNew cString;
	ctx.m_pStringList = pStringList;
	ctx.m_nTotalWidth = 0;

	// 총 길이
	int nStrLen = _tcslen( str );
	assert_cs( nStrLen > 0 );

	// 프리타입폰트
	FT_Face face = ctx.m_pTextInfo->s_pFont->GetFace( ctx.m_pTextInfo->s_eFontSize );

	int nCharSize;
	for( int i=0; i<nStrLen; ++i )
	{
		//=================================================================================================
		//	뉴라인
		//=================================================================================================
		if( str[ i ] == L'\n' )
		{
			_MakeToken_Parcing( ctx, true );
			continue;
		}

		//=================================================================================================
		//	파싱
		//=================================================================================================
		// 파싱하는데 최소 5개의 문자가 소요
		// 파싱 시작 - @<시동어:값>
		// 피상 종료 - @</종료어>
		// @@두개는 설명단어
		if( i < nStrLen - 5 )
		{
PARCING:
			int nCurIndex = i;

			// 파싱해야 하는 문자인가
			if( str[ i ] == L'@' )
			{
				if( str[ i + 1 ] == L'<' )
				{
					_Parcing( ctx, str, nStrLen, i );
					if( i == nStrLen )
						break;
				}
				else if( i < nStrLen - 9 )
				{
					if( ( str[ i + 1 ] == L'@' )&&( str[ i + 8 ] == L'@' )&&( str[ i + 9 ] == L'@' ) )
					{
						//_ParcingWord( str, nStrLen, i, width, Type );
						if( i == nStrLen )
							break;
					}
				}
			}

			// 인덱스 변경이 이루어진 경우에만
			if( nCurIndex != i )
			{
				if( str[ i ] == L'@' )
				{
					if( str[ i + 1 ] == L'<' )
						goto PARCING;
					else if( i < nStrLen - 9 )
					{
						if( ( str[ i + 1 ] == L'@' )&&( str[ i + 8 ] == L'@' )&&( str[ i + 9 ] == L'@' ) )
							goto PARCING;
					}
				}
			}
		}

		// 문자의 길이 가져온다.
		nCharSize = cText::GetCharWidth( face, str[ i ] );

		// 문자열 길이가 넘어 갔는가
		if( ( ctx.m_nTotalWidth + nCharSize ) > width )
		{
			_MakeToken_Parcing( ctx, true );
		}

		if( ( ctx.m_nTokenIndex != 0 )||( str[ i ] != L' ' )||( ctx.m_bNewLine == false ) )
		{
			ctx.m_szToken[ ctx.m_nTokenIndex ] = str[ i ];
			++ctx.m_nTokenIndex;
			ctx.m_nTotalWidth += nCharSize;
		}
	}

	if( ctx.m_nTokenIndex != 0 )
	{
		_MakeToken_Parcing( ctx, false );
	}
	if( ctx.m_pString->GetListSize() == 0 )
	{
		SAFE_NIDELETE( ctx.m_pString );
	}
	if( ctx.m_pString != NULL )
	{
		ctx.m_pStringList->AddTail( ctx.m_pString );
	}

	// 포인터 초기화 — ownership transferred to ctx.m_pStringList; sCutContext
	// destructor will free m_listParcing if anything is left.
	ctx.m_pString = NULL;
}

void cStringAnalysis::Cut_Parcing_ForCardMaster( cString* pString, int width, CsPoint	Pos, TCHAR const* str, cText::sTEXTINFO* pTextInfo )
{
	if( pTextInfo->s_pFont->IsInitialize() == false )
	{
		return;
	}

	sCutContext ctx;
	ctx.m_pTextInfo = pTextInfo;
	ctx.m_OrgColor = ctx.m_pTextInfo->s_Color;
	// Caller-owned cString — must be nulled in ctx before sCutContext dtor
	// runs, otherwise the dtor will free it and leave the caller with a
	// dangling pointer.  The whole function is a fall-through write into
	// pString; we never transfer ownership.
	ctx.m_pString = pString;
	ctx.m_bNewLine = false;
	ctx.m_nTotalWidth = 0;

	// 총 길이
	int nStrLen = _tcslen( str );
	assert_cs( nStrLen > 0 );

	// 프리타입폰트
	FT_Face face = ctx.m_pTextInfo->s_pFont->GetFace( ctx.m_pTextInfo->s_eFontSize );
	int height = 0;
	int nCharSize = 0;
	for( int i=0; i<nStrLen +1; ++i )
	{
		// 문자의 길이 가져온다.
		nCharSize = cText::GetCharWidth( face, str[ i ] );

		if( str[ i ] == NULL )
		{
			_MakeToken_Parcing_ForCardMaster( ctx, Pos + CsPoint(0, height));
			height += g_pEngine->m_FontSystem.GetHeight( ctx.m_pTextInfo->s_eFontSize );;
			continue;
		}

		// 문자열 길이가 넘어 갔는가
		if( ( ctx.m_nTotalWidth + nCharSize ) > width )
		{

			_MakeToken_Parcing_ForCardMaster( ctx, Pos + CsPoint(0, height));
			height += g_pEngine->m_FontSystem.GetHeight( ctx.m_pTextInfo->s_eFontSize );;
		}

		if( ( ctx.m_nTokenIndex != 0 )||( str[ i ] != L' ' ) )
		{
			ctx.m_szToken[ ctx.m_nTokenIndex ] = str[ i ];
			++ctx.m_nTokenIndex;
			ctx.m_nTotalWidth += nCharSize;
		}
	}

	// 포인터 초기화 — null before dtor so caller-owned cString isn't deleted.
	ctx.m_pString = NULL;
}

void cStringAnalysis::Cut_Parcing( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo )
{
	m_vecStringTotalText.clear(); // [10/20/2016 hyun] 현욱 추가

	SAFE_POINTER_RET( pStringList );
	SAFE_POINTER_RET( pTextInfo );
	SAFE_POINTER_RET( pTextInfo->s_pFont );
	if( pTextInfo->s_pFont->IsInitialize() == false )
	{
		return;
	}
	std::wstring safeInput = NormalizeAnalysisInput( str );
	if( safeInput.empty() )
	{
		return;
	}
	str = safeInput.c_str();

	sCutContext ctx;
	ctx.m_nLineLimit = -1;
	ctx.m_pTextInfo = pTextInfo;
	ctx.m_OrgColor = ctx.m_pTextInfo->s_Color;
	ctx.m_pStrInfo = pStringList->GetStrInfo();
	ctx.m_nOrgBoldLevel = (int)ctx.m_pTextInfo->GetBoldLevel();
	ctx.m_bNewLine = false;
	ctx.m_pString = NiNew cString;
	ctx.m_pStringList = pStringList;
	ctx.m_nTotalWidth = 0;

	// 총 길이
	int nStrLen = _tcslen( str );
	assert_cs( nStrLen > 0 );

	// 프리타입폰트
	FT_Face face = ctx.m_pTextInfo->s_pFont->GetFace( ctx.m_pTextInfo->s_eFontSize );

	int nCharSize;
	for( int i=0; i<nStrLen; ++i )
	{
		//=================================================================================================
		//	뉴라인
		//=================================================================================================
		if( str[ i ] == L'\n' )
		{
			// 스트링에 개행문자 연속으로 올 경우 개행문자 만큼 라인 추가해주기 위해 수정 ( 141127 chu8820 )
			if( ctx.m_szToken[ 0 ] == NULL )
				ctx.m_szToken[ 0 ] = _T( ' ' );
			_MakeToken_Parcing( ctx, true );
			continue;
		}
		else if( (str[ i ] == L'\\' && (i+1 < nStrLen) && str[i+1] == L'n' ) )
		{
			++i;
			// 스트링에 개행문자 연속으로 올 경우 개행문자 만큼 라인 추가해주기 위해 수정 ( 141127 chu8820 )
			if( ctx.m_szToken[ 0 ] == NULL )
				ctx.m_szToken[ 0 ] = _T( ' ' );
			_MakeToken_Parcing( ctx, true );
			continue;
		}

		//=================================================================================================
		//	파싱
		//=================================================================================================
		// 파싱하는데 최소 5개의 문자가 소요
		// 파싱 시작 - @<시동어:값>
		// 피상 종료 - @</종료어>
		// @@두개는 설명단어
		if( i < nStrLen - 5 )
		{
PARCING:
			int nCurIndex = i;

			// 파싱해야 하는 문자인가
			if( str[ i ] == L'@' )
			{
				if( str[ i + 1 ] == L'<' )
				{
					_Parcing( ctx, str, nStrLen, i );
					if( i == nStrLen )
						break;
				}
			}

			// 인덱스 변경이 이루어진 경우에만
			if( nCurIndex != i )
			{
				if( str[ i ] == L'@' )
				{
					if( str[ i + 1 ] == L'<' )
						goto PARCING;
				}
			}
		}

		if( str[ i ] == L'\n' )
		{
			if( ctx.m_szToken[ 0 ] == NULL )
				ctx.m_szToken[ 0 ] = _T( ' ' );
			_MakeToken_Parcing( ctx, true );
			continue;
		}
		else if( (str[ i ] == L'\\' && (i+1 < nStrLen) && str[i+1] == L'n' ) )
		{
			++i;
			// 스트링에 개행문자 연속으로 올 경우 개행문자 만큼 라인 추가해주기 위해 수정 ( 141127 chu8820 )
			if( ctx.m_szToken[ 0 ] == NULL )
				ctx.m_szToken[ 0 ] = _T( ' ' );
			_MakeToken_Parcing( ctx, true );
			continue;
		}

		// 문자의 길이 가져온다.
		nCharSize = cText::GetCharWidth( face, str[ i ] );

		// 문자열 길이가 넘어 갔는가
		if( ( ctx.m_nTotalWidth + nCharSize ) > width )
		{
			_MakeToken_Parcing( ctx, true );
		}

		if( ( ctx.m_nTokenIndex != 0 )||( str[ i ] != L' ' )||( ctx.m_bNewLine == false ) )
		{
			ctx.m_szToken[ ctx.m_nTokenIndex ] = str[ i ];
			++ctx.m_nTokenIndex;
			ctx.m_nTotalWidth += nCharSize;
		}
	}

	if( ctx.m_nTokenIndex != 0 )
	{
		_MakeToken_Parcing( ctx, false );
	}
	if( ctx.m_pString->GetListSize() == 0 )
	{
		SAFE_NIDELETE( ctx.m_pString );
	}
	if( ctx.m_pString != NULL )
	{
		ctx.m_pStringList->AddTail( ctx.m_pString );
		m_vecStringTotalText.push_back( ctx.m_strTempText );
	}

	// 포인터 초기화 — ownership transferred to ctx.m_pStringList; sCutContext
	// destructor will free m_listParcing if anything is left.
	ctx.m_pString = NULL;
}

bool cStringAnalysis::CutLimitWigth( std::list<dm_string_t>& outPut, cText::sTEXTINFO const& pTextInfo, int const& nWidthLimit )
{
	dm_string_t text = pTextInfo.GetText();
	return CutLimitWigth( outPut, text, pTextInfo.s_pFont, pTextInfo.s_eFontSize, nWidthLimit );
}

bool cStringAnalysis::CutLimitWigth( std::list<dm_string_t>& outPut, dm_string_t const& surText, CFont const* pFont, CFont::eFACE_SIZE fontSize, int const& nWidthLimit )
{
	SAFE_POINTER_RETVAL(pFont, false);
	if( !pFont->IsInitialize() )
		return false;

	if( surText.empty() )
		return false;

	FT_Face face = pFont->GetFace( fontSize );
	int nSpaceSize = cText::GetStringWidth( face, _T(" ") );

	int nCurrentWidth = 0;
	dm_string_t wsCurrentText;

	dm_stringvec_t cutNewLine;
	DmCS::StringFn::Cut( surText, _T("\r\n"), cutNewLine);

	size_t lineCount = cutNewLine.size();
	for( size_t n = 0; n < lineCount; ++n )
	{
		dm_stringvec_t cutTokken;
		DmCS::StringFn::Cut( cutNewLine[n], _T(" "), cutTokken);

		size_t tokkenCount = cutTokken.size();
		for( size_t l = 0; l < tokkenCount; ++l )
		{
			int nTokkenWidth = cText::GetStringWidth( face, cutTokken[l] );
			if( nTokkenWidth > nWidthLimit ) // 하나의 문자열이 width제한에 걸릴 경우 개별사이즈로 나눈다.
			{
				if( !wsCurrentText.empty() )
				{	// 단어를 추가해야 하는데 공백을 넣을 공간이 없을 경우
					// 새로운 라인을 넣어 공백을 넣는다.
					if( nSpaceSize + nCurrentWidth > nWidthLimit )
					{
						outPut.push_back( wsCurrentText );
						wsCurrentText = _T(" ");
						nCurrentWidth = nSpaceSize;
					}
					else
					{
						wsCurrentText += _T(" ");
						nCurrentWidth += nSpaceSize;
					}
				}

				dm_string_t txt = cutTokken[l];
				size_t sCount = txt.size();
				for( size_t m=0; m < sCount; ++m )
				{
					int nCharWidth = cText::GetCharWidth( face, txt[m] );
					if( nCurrentWidth + nCharWidth > nWidthLimit )
					{
						outPut.push_back( wsCurrentText );
						wsCurrentText = txt[m];
						nCurrentWidth = nCharWidth;
					}
					else
					{
						wsCurrentText += txt[m];
						nCurrentWidth += nCharWidth;
					}
				}
			}
			else if( nTokkenWidth + nCurrentWidth > nWidthLimit )
			{
				outPut.push_back( wsCurrentText );

				wsCurrentText = cutTokken[l];
				nCurrentWidth = nTokkenWidth;
			}
			else
			{
				if( !wsCurrentText.empty() )
				{	// 단어를 추가해야 하는데 공백을 넣을 공간이 없을 경우
					// 새로운 라인을 넣어 공백을 넣는다.
					if( nSpaceSize + nCurrentWidth > nWidthLimit )
					{
						outPut.push_back( wsCurrentText );
						wsCurrentText = _T(" ");
						nCurrentWidth = nSpaceSize;
					}
					else
					{
						wsCurrentText += _T(" ");
						nCurrentWidth += nSpaceSize;
					}
				}
				wsCurrentText += cutTokken[l];
				nCurrentWidth += nTokkenWidth;
			}
		}
		if( !wsCurrentText.empty() )
			outPut.push_back( wsCurrentText );
		wsCurrentText.clear();
		nCurrentWidth = 0;
	}
	return true;
}

void cStringAnalysis::Cut( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo, int nLineLimit /*=-1*/ )
{
	SAFE_POINTER_RET( pStringList );
	SAFE_POINTER_RET( pTextInfo );
	SAFE_POINTER_RET( pTextInfo->s_pFont );
	if( pTextInfo->s_pFont->IsInitialize() == false )
	{
		return;
	}
	std::wstring safeInput = NormalizeAnalysisInput( str );
	if( safeInput.empty() )
	{
		return;
	}
	str = safeInput.c_str();

	sCutContext ctx;
	ctx.m_nLineLimit = nLineLimit;
	ctx.m_pTextInfo = pTextInfo;
	ctx.m_OrgColor = ctx.m_pTextInfo->s_Color;
	ctx.m_pStrInfo = pStringList->GetStrInfo();
	ctx.m_pString = NiNew cString;
	ctx.m_pStringList = pStringList;
	ctx.m_nTotalWidth = 0;
	ctx.m_bNewLine = false;

	std::wstring wstr(str);
	// 총 길이
	int nStrLen = wstr.length();
	assert_cs( nStrLen > 0 );

	// 프리타입폰트
	FT_Face face = ctx.m_pTextInfo->s_pFont->GetFace( ctx.m_pTextInfo->s_eFontSize );

	int nCharSize;
	for( int i=0; i<nStrLen; ++i )
	{
		//=================================================================================================
		//	뉴라인
		//=================================================================================================
		if( wstr[ i ] == _T( '\n' ) )
		{
			if( ctx.m_nTokenIndex != 0 )
			{
				if( _MakeToken( ctx, true ) == false )
				{
					wstr[ i+1 ] = NULL;
					break;
				}
			}
			else
			{
				if( _MakeToken_OnlyNewLine( ctx, true ) == false )
				{
					wstr[ i+1 ] = NULL;
					break;
				}
			}

			if( i == nStrLen - 1 )
			{
				if( _MakeToken_OnlyNewLine( ctx, false ) == false )
				{
					wstr[ i+1 ] = NULL;
					break;
				}
			}

			continue;
		}

		// 문자의 길이 가져온다.
		nCharSize = cText::GetCharWidth( face, wstr[ i ] );

		// 문자열 길이가 넘어 갔는가
		if( ( ctx.m_nTotalWidth + nCharSize ) > width )
		{
			if( _MakeToken( ctx, true ) == false )
			{
				wstr[ i+1 ] = NULL;
				break;
			}
		}

		if( ( ctx.m_nTokenIndex != 0 )||( wstr[ i ] != L' ' )||( ctx.m_bNewLine == false ) )
		{
			ctx.m_szToken[ ctx.m_nTokenIndex ] = wstr[ i ];
			++ctx.m_nTokenIndex;
			ctx.m_nTotalWidth += nCharSize;
		}
	}


	if( ctx.m_nTokenIndex != 0 )
	{
		_MakeToken( ctx, false );
	}

	if( ctx.m_pString->GetListSize() == 0 )
	{
		SAFE_NIDELETE( ctx.m_pString );
	}
	if( ctx.m_pString != NULL )
	{
		ctx.m_pStringList->AddTail( ctx.m_pString );
	}

	// 포인터 초기화 — ownership transferred to ctx.m_pStringList.
	ctx.m_pString = NULL;
}


void cStringAnalysis::Cut_NoMultyLine( cStringList* pStringList, TCHAR const* str, cText::sTEXTINFO* pTextInfo, int nLineLimit /*=-1*/ )
{
	SAFE_POINTER_RET( pStringList );
	SAFE_POINTER_RET( pTextInfo );
	SAFE_POINTER_RET( pTextInfo->s_pFont );
	if( pTextInfo->s_pFont->IsInitialize() == false )
	{
		return;
	}
	std::wstring safeInput = NormalizeAnalysisInput( str );
	if( safeInput.empty() )
	{
		return;
	}
	str = safeInput.c_str();

	sCutContext ctx;
	ctx.m_nLineLimit = nLineLimit;
	ctx.m_pTextInfo = pTextInfo;
	ctx.m_OrgColor = ctx.m_pTextInfo->s_Color;
	ctx.m_pStrInfo = pStringList->GetStrInfo();
	ctx.m_pString = NiNew cString;
	ctx.m_pStringList = pStringList;
	ctx.m_nTotalWidth = 0;
	ctx.m_bNewLine = false;

	// 총 길이
	int nStrLen = _tcslen( str );
	assert_cs( nStrLen > 0 );

	// 프리타입폰트
	FT_Face face = ctx.m_pTextInfo->s_pFont->GetFace( ctx.m_pTextInfo->s_eFontSize );

	int nCharSize;
	for( int i=0; i<nStrLen; ++i )
	{
		//=================================================================================================
		//	뉴라인
		//=================================================================================================
		if( str[ i ] == _T( '\n' ) )
		{
			continue;
		}

		nCharSize = cText::GetCharWidth( face, str[ i ] );
		if( ( ctx.m_nTokenIndex != 0 )||( str[ i ] != L' ' )||( ctx.m_bNewLine == false ) )
		{
			ctx.m_szToken[ ctx.m_nTokenIndex ] = str[ i ];
			++ctx.m_nTokenIndex;
			ctx.m_nTotalWidth += nCharSize;
		}
	}


	if( ctx.m_nTokenIndex != 0 )
	{
		_MakeToken( ctx, false );
	}

	if( ctx.m_pString->GetListSize() == 0 )
	{
		SAFE_NIDELETE( ctx.m_pString );
	}
	if( ctx.m_pString != NULL )
	{
		ctx.m_pStringList->AddTail( ctx.m_pString );
	}

	// 포인터 초기화 — ownership transferred to ctx.m_pStringList.
	ctx.m_pString = NULL;
}


void cStringAnalysis::AddCut( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo )
{
	SAFE_POINTER_RET( pStringList );
	SAFE_POINTER_RET( pTextInfo );
	SAFE_POINTER_RET( pTextInfo->s_pFont );
	if( pTextInfo->s_pFont->IsInitialize() == false )
	{
		return;
	}
	std::wstring safeInput = NormalizeAnalysisInput( str );
	if( safeInput.empty() )
	{
		return;
	}
	str = safeInput.c_str();

	sCutContext ctx;
	ctx.m_nLineLimit = -1;
	ctx.m_pTextInfo = pTextInfo;
	ctx.m_OrgColor = ctx.m_pTextInfo->s_Color;
	ctx.m_pStrInfo = pStringList->GetStrInfo();
	ctx.m_bNewLine = false;

	bool bNewString = false;
	if( pStringList->GetSize() == 0 )
	{
		bNewString = true;
	}
	else
	{
		cString* pTail = pStringList->GetTail();
		if( pTail->GetListSize() != 0 )
		{
			cString::sELEMENT* pElement = pTail->GetElement( pTail->GetListSize() - 1 );
			if( pElement->GetType() == cString::sELEMENT::TEXT )
			{
				TCHAR* szCheck = (TCHAR*)( (cString::sTEXT*)pElement )->s_Text.GetTextInfo()->GetText();
				if( ( szCheck[ _tcslen( szCheck ) - 1 ] == L'\n' )||( szCheck[ _tcslen( szCheck ) - 1 ] == 0x000d ) )
				{
					bNewString = true;
				}
			}
		}
	}

	if( bNewString )
	{
		ctx.m_pString = NiNew cString;
	}
	else
	{
		ctx.m_pString = pStringList->GetTail();

		std::list< cString* >::iterator it = pStringList->GetList()->end();
		--it;
		pStringList->GetList()->erase( it );
	}

	ctx.m_pStringList = pStringList;
	ctx.m_nTotalWidth = 0;

	// 총 길이
	int nStrLen = (int)_tcslen( str );
	assert_cs( nStrLen > 0 );

	// 프리타입폰트
	FT_Face face = ctx.m_pTextInfo->s_pFont->GetFace( ctx.m_pTextInfo->s_eFontSize );

	int nCharSize;
	for( int i=0; i<nStrLen; ++i )
	{
		//=================================================================================================
		//	뉴라인
		//=================================================================================================
		if( str[ i ] == L'\n' )
		{
			_MakeToken( ctx, true );
			continue;
		}

		// 문자의 길이 가져온다.
		nCharSize = cText::GetCharWidth( face, str[ i ] );

		// 문자열 길이가 넘어 갔는가
		if( ( ctx.m_nTotalWidth + nCharSize ) > width )
		{
			_MakeToken( ctx, true );
		}

		//if( ( ctx.m_nTokenIndex != 0 )||( str[ i ] != L' ' )||( ctx.m_bNewLine == false ) )
		{
			ctx.m_szToken[ ctx.m_nTokenIndex ] = str[ i ];
			++ctx.m_nTokenIndex;
			ctx.m_nTotalWidth += nCharSize;
		}
	}

	if( ctx.m_nTokenIndex != 0 )
	{
		_MakeToken( ctx, false );
	}
	if( ctx.m_pString->GetListSize() == 0 )
	{
		SAFE_NIDELETE( ctx.m_pString );
	}
	if( ctx.m_pString != NULL )
	{
		ctx.m_pStringList->AddTail( ctx.m_pString );
	}

	// 포인터 초기화 — ownership transferred to ctx.m_pStringList.
	ctx.m_pString = NULL;
}

void cStringAnalysis::Cut_MailParcing( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo )
{
	SAFE_POINTER_RET( pStringList );
	SAFE_POINTER_RET( pTextInfo );
	SAFE_POINTER_RET( pTextInfo->s_pFont );
	if( pTextInfo->s_pFont->IsInitialize() == false )
	{
		return;
	}
	std::wstring safeInput = NormalizeAnalysisInput( str );
	if( safeInput.empty() )
	{
		return;
	}
	str = safeInput.c_str();

	sCutContext ctx;
	ctx.m_nLineLimit = -1;
	ctx.m_pTextInfo = pTextInfo;
	ctx.m_OrgColor = ctx.m_pTextInfo->s_Color;
	ctx.m_pStrInfo = pStringList->GetStrInfo();
	ctx.m_pString = NiNew cString;
	ctx.m_pStringList = pStringList;
	ctx.m_nTotalWidth = 0;
	ctx.m_bNewLine = false;

	// 총 길이
	int nStrLen = _tcslen( str );
	assert_cs( nStrLen > 0 );

	// 프리타입폰트
	FT_Face face = ctx.m_pTextInfo->s_pFont->GetFace( ctx.m_pTextInfo->s_eFontSize );

	int nCharSize;
	for( int i=0; i<nStrLen; ++i )
	{
		//=================================================================================================
		//	들여쓰기
		//=================================================================================================
		if( str[ i ] == L'&' && str[ i + 1 ] == L'<' && str[ i + 5 ] == L'>' )
		{
			int nSize = _ttoi( &str[ i + 2 ] );
			if( nSize < 0 || nSize > 999 )
				continue;

			ctx.m_pString->HeadAddSizeX( nSize );
			i = i+6;
			continue;
		}

		//=================================================================================================
		//	탭문자
		//=================================================================================================
		if( str[ i ] == L'\t' )
		{
			ctx.m_pString->TailAddSizeX( 40 );
			continue;
		}

		//=================================================================================================
		//	뉴라인
		//=================================================================================================
		if( str[ i ] == L'\n' )
		{
			_MakeToken( ctx, true );
			continue;
		}

		//=================================================================================================
		//	아이템 문자 해독
		//=================================================================================================
		if( str[ i ] == L'`' && str[ i + 9 ] == L'`' ) // 캐쉬샵 선물메일인지
		{
			i = i+9;
			continue;
		}
		else if( str[ i ] == L'`' ) // 이벤트 메일인지
		{
			i = i+4;
			continue;
		}

		// 문자의 길이 가져온다.
		nCharSize = cText::GetCharWidth( face, str[ i ] );

		// 문자열 길이가 넘어 갔는가
		if( ( ctx.m_nTotalWidth + nCharSize ) > width )
		{
			_MakeToken( ctx, true );
		}

		if( ( ctx.m_nTokenIndex != 0 )||( str[ i ] != L' ' )||( ctx.m_bNewLine == false ) )
		{
			ctx.m_szToken[ ctx.m_nTokenIndex ] = str[ i ];
			++ctx.m_nTokenIndex;
			ctx.m_nTotalWidth += nCharSize;
		}
	}

	if( ctx.m_nTokenIndex != 0 )
	{
		_MakeToken( ctx, false );
	}
	if( ctx.m_pString->GetListSize() == 0 )
	{
		SAFE_NIDELETE( ctx.m_pString );
	}
	if( ctx.m_pString != NULL )
	{
		ctx.m_pStringList->AddTail( ctx.m_pString );
	}

	// 포인터 초기화 — ownership transferred to ctx.m_pStringList.
	ctx.m_pString = NULL;
}
//일정 길이 이상 넘어가면 끝을 자르면서 ...으로 표시
//UI리뉴얼 하면서 만든건데 디파인은 안걸어둠 다른데에 쓰일 수도 있으니..
void cStringAnalysis::Cut_LimitWigth( cStringList* pStringList, int width, TCHAR * str, cText::sTEXTINFO* pTextInfo, int nLineLimit /*=-1*/ )
{
	SAFE_POINTER_RET( pStringList );
	SAFE_POINTER_RET( pTextInfo );
	SAFE_POINTER_RET( pTextInfo->s_pFont );
	if( pTextInfo->s_pFont->IsInitialize() == false )
	{
		return;
	}
	std::wstring safeInput = NormalizeAnalysisInput( str );
	if( safeInput.empty() )
	{
		return;
	}
	std::vector<TCHAR> mutableInput( safeInput.begin(), safeInput.end() );
	mutableInput.push_back( 0 );
	str = &mutableInput[0];

	sCutContext ctx;
	ctx.m_nLineLimit = nLineLimit;
	ctx.m_pTextInfo = pTextInfo;
	ctx.m_OrgColor = ctx.m_pTextInfo->s_Color;
	ctx.m_pStrInfo = pStringList->GetStrInfo();
	ctx.m_pString = NiNew cString;
	ctx.m_pStringList = pStringList;
	ctx.m_nTotalWidth = 0;
	ctx.m_bNewLine = false;

	// 총 길이
	int nStrLen = _tcslen( str );
	assert_cs( nStrLen > 0 );

	// 프리타입폰트
	FT_Face face = ctx.m_pTextInfo->s_pFont->GetFace( ctx.m_pTextInfo->s_eFontSize );

	int nCharSize;
	for( int i=0; i<nStrLen; ++i )
	{
		//=================================================================================================
		//	뉴라인
		//=================================================================================================
		if( str[ i ] == _T( '\n' ) )
		{
			if( ctx.m_nTokenIndex != 0 )
			{
				if( _MakeToken( ctx, true ) == false )
				{
					str[ i+1 ] = NULL;
					break;
				}
			}
			else
			{
				if( _MakeToken_OnlyNewLine( ctx, true ) == false )
				{
					str[ i+1 ] = NULL;
					break;
				}
			}

			if( i == nStrLen - 1 )
			{
				if( _MakeToken_OnlyNewLine( ctx, false ) == false )
				{
					str[ i+1 ] = NULL;
					break;
				}
			}

			continue;
		}

		// 문자의 길이 가져온다.
		nCharSize = cText::GetCharWidth( face, str[ i ] );

		// 문자열 길이가 넘어 갔는가
		if( ( ctx.m_nTotalWidth + nCharSize ) > width )
		{
			for( int n = i; n< i+3; n++)
			{
				ctx.m_szToken[ n ] =  L'.';
			}
			ctx.m_szToken[ i+3 ] = NULL;
			break;
		}

		if( ( ctx.m_nTokenIndex != 0 )||( str[ i ] != L' ' )||( ctx.m_bNewLine == false ) )
		{
			ctx.m_szToken[ ctx.m_nTokenIndex ] = str[ i ];
			++ctx.m_nTokenIndex;
			ctx.m_nTotalWidth += nCharSize;
		}
	}


	if( ctx.m_nTokenIndex != 0 )
	{
		_MakeToken( ctx, false );
	}

	if( ctx.m_pString->GetListSize() == 0 )
	{
		SAFE_NIDELETE( ctx.m_pString );
	}
	if( ctx.m_pString != NULL )
	{
		ctx.m_pStringList->AddTail( ctx.m_pString );
	}

	// 포인터 초기화 — ownership transferred to ctx.m_pStringList.
	ctx.m_pString = NULL;
}

TCHAR* cStringAnalysis::StringTransfer( TCHAR* szOut, int nOutSize, TCHAR const* szIn )
{
	int nLen = (int)_tcslen( szIn );
	int nDestIndex = 0;
	memset( szOut, 0, sizeof( TCHAR )*nOutSize );

	for( int i=0; i<nLen; ++i )
	{
		if( i < (int)( nLen - 3 ) )
		{
			if( ( szIn[ i ] == L'#' )&&( szIn[ i + 1 ] == L'<' ) )
			{
				TCHAR* szInsert = NULL;
				int nJump = 0;
				if( _tcsncmp( &szIn[ i + 2 ], _T( "my_tname>" ), 9 ) == 0 )
				{
					szInsert = g_pCharMng->GetTamerUser()->GetName();
					nJump = 9;
				}
				else if( _tcsncmp( &szIn[ i + 2 ], _T( "my_dname>" ), 9 ) == 0 )
				{
					szInsert = g_pCharMng->GetDigimonUser( 0 )->GetName();
					nJump = 9;
				}

				if( szInsert )
				{
					if( ( nDestIndex + (int)_tcslen( szInsert ) ) < ( nOutSize - 1 ) )
					{
						szOut[ nDestIndex ] = NULL;
						_tcscat_s( szOut, nOutSize, szInsert );
						nDestIndex += (int)_tcslen( szInsert );
						i+= ( nJump + 1 );
						continue;
					}
					else
					{
						break;
					}					
				}				
			}
		}
		szOut[ nDestIndex ] = szIn[ i ];
		++nDestIndex;

		if( nDestIndex == ( nOutSize - 1 ) )
			break;
	}
	szOut[ nDestIndex ] = NULL;
	return szOut;
}

void cStringAnalysis::MaxLine_Cut(TCHAR const* word, cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo, int nLineLimit)
{
	SAFE_POINTER_RET(pStringList);

	// 문자열을 길이이에 맞게 자름.
	g_pStringAnalysis->Cut( pStringList, width, str, pTextInfo, nLineLimit);
	LIST_STRING* pList = pStringList->GetList();
	int size = 0;
	for(LIST_STRING_IT it = pList->begin(); it != pList->end(); ++it )
	{
		cString*	cSTR = *it;
		cString::sTEXT* pText =	(cString::sTEXT*)cSTR->FindElement( cString::sELEMENT::TEXT );				

		cText::sTEXTINFO* textinfo = pText->s_Text.GetTextInfo();

		if(size == 0)
			pTextInfo->SetText(textinfo->GetText());
		else
		{
			pTextInfo->AddText(word);
			break;
		}
		++size;
	}
}
TCHAR* cStringAnalysis::Quest_Parcing( TCHAR* szDest, const int nDestSize, TCHAR const* szSrc, CsQuestCompRequireGroup* pApplyGroup )
{
#define QUEST_PARCING_LEN		32
	int nSrcLen = _tcslen( szSrc );
	assert_cs( nDestSize > nSrcLen );

	int nDestIndex = 0;
	int nSrcIndex = 0;
	TCHAR szTemp[ QUEST_PARCING_LEN ];	
	while( nSrcIndex < nSrcLen )
	{
		if( szSrc[ nSrcIndex ] == L'%' )
		{
			++nSrcIndex;
			if( _tcsncmp( &szSrc[ nSrcIndex ], _T( "cc" ), 2 ) == 0 )
			{
				nSrcIndex += 2;
				// 그룹 인덱스 - 오직 하나의 숫자만
				szTemp[ 0 ] = szSrc[ nSrcIndex ];
				++nSrcIndex;
				szTemp[ 1 ] = NULL;
				int nCompRequireIndex = _ttoi( szTemp );

				assert_csm1( nCompRequireIndex != 0, _T( "명령어 인자가 잘못 되었습니다. 명령어 %s" ), szSrc );

				CsQuestCompRequire* pFTComp = pApplyGroup->Get( nCompRequireIndex - 1 );
				int nR = 9 - (int)( (float)pFTComp->GetCurCount() / (float)pFTComp->GetTypeCount() * 9.0f );
				assert_cs( nR<10 );

				_stprintf_s( szTemp, QUEST_PARCING_LEN, _T( "@<tc:%d90>%d" ), nR, pApplyGroup->Get( nCompRequireIndex-1 )->GetCurCount() );
				_tcscpy_s( &szDest[ nDestIndex ], nDestSize - nDestIndex - 1, szTemp );
				nDestIndex += (int)_tcslen( szTemp );
			}
			else if( _tcsncmp( &szSrc[ nSrcIndex ], _T( "mc" ), 2 ) == 0 )
			{
				nSrcIndex += 2;
				// 그룹 인덱스 - 오직 하나의 숫자만
				szTemp[ 0 ] = szSrc[ nSrcIndex ];
				++nSrcIndex;
				szTemp[ 1 ] = NULL;
				int nCompRequireIndex = _ttoi( szTemp );

				assert_csm1( nCompRequireIndex != 0, _T( "명령어 인자가 잘못 되었습니다. 명령어 %s" ), szSrc );
				_stprintf_s( szTemp, QUEST_PARCING_LEN, _T( "%d@</tc>" ), pApplyGroup->Get( nCompRequireIndex-1 )->GetTypeCount() );
				_tcscpy_s( &szDest[ nDestIndex ], nDestSize - nDestIndex - 1, szTemp );
				nDestIndex += (int)_tcslen( szTemp );
			}
			else if( _tcsncmp( &szSrc[ nSrcIndex ], _T( "tcc" ), 3 ) == 0 )
			{
				nSrcIndex += 3;

				int nCompCount = 0;
				int nCount = pApplyGroup->GetListCount();
				for( int i=0; i<nCount; ++i )
				{
					if( pApplyGroup->Get( i )->IsCompleat() == true )
						++nCompCount;
				}

				int nR = 9 - (int)( (float)nCompCount / (float)pApplyGroup->GetListCount() * 9.0f );
				_stprintf_s( szTemp, QUEST_PARCING_LEN, _T( "@<tc:%d90>%d" ), nR, nCompCount );				
				_tcscpy_s( &szDest[ nDestIndex ], nDestSize - nDestIndex - 1, szTemp );
				nDestIndex += (int)_tcslen( szTemp );
			}
			else if( _tcsncmp( &szSrc[ nSrcIndex ], _T( "tmc" ), 3 ) == 0 )
			{
				nSrcIndex += 3;

				_stprintf_s( szTemp, QUEST_PARCING_LEN, _T( "%d@</tc>" ), pApplyGroup->GetListCount() );
				_tcscpy_s( &szDest[ nDestIndex ], nDestSize - nDestIndex - 1, szTemp );
				nDestIndex += (int)_tcslen( szTemp );
			}
			else if( _tcsncmp( &szSrc[ nSrcIndex ], _T( "qs" ), 2 ) == 0 )
			{
				//nSrcIndex += 3;

				nSrcIndex += 2;

				// 그룹 인덱스 - 오직 하나의 숫자만
				szTemp[ 0 ] = szSrc[ nSrcIndex ];
				++nSrcIndex;
				szTemp[ 1 ] = NULL;
				int nCompRequireIndex = _ttoi( szTemp );
// #ifdef CRASHDUMP_0305
// 				DUMPLOG("06.StrAnaly001] src : %s, rqidx : %d", szSrc, nCompRequireIndex);
// #endif
				assert_csm1( nCompRequireIndex > 0, _T( "명령어 인자가 잘못 되었습니다. 명령어 %s" ), szSrc );
				assert_csm2( pApplyGroup->Get( nCompRequireIndex - 1 ), _T( "qs%d 그룹이 존재 하지 않습니다. \n%s" ), nCompRequireIndex, szSrc );
				if( pApplyGroup->Get( nCompRequireIndex - 1 )->IsCompleat() == true )
				{
					assert_csm1( nCompRequireIndex != 0, _T( "명령어 인자가 잘못 되었습니다. 명령어 %s" ), szSrc );
					_tcscpy_s( szTemp, QUEST_PARCING_LEN, _T( "@<tc:090>" ) );
					_tcscpy_s( &szDest[ nDestIndex ], nDestSize - nDestIndex - 1, szTemp );
					nDestIndex += (int)_tcslen( szTemp );
				}
			}
			else if( _tcsncmp( &szSrc[ nSrcIndex ], _T( "qe" ), 2 ) == 0 )
			{
				nSrcIndex += 2;
				_tcscpy_s( szTemp, QUEST_PARCING_LEN, _T( "@</tc>" ) );
				_tcscpy_s( &szDest[ nDestIndex ], nDestSize - nDestIndex - 1, szTemp );
				nDestIndex += (int)_tcslen( szTemp );
			}
			else
			{
				assert_csm1( false, _T( "알 수 없는 명령어 입니다. 명령어 %s" ), szSrc );
			}
		}
		else
		{
#ifdef QUEST_STRING_PARCING
			if( _tcsncmp( &szSrc[ nSrcIndex ], _T( "/" ), 1 ) == 0 )
			{
				_tcscpy_s( szTemp, 2, _T( " " ) );
				szDest[ nDestIndex++ ] = szTemp[0];
				szDest[ nDestIndex++ ] = szSrc[ nSrcIndex ];
				szDest[ nDestIndex++ ] = szTemp[0];
				++nSrcIndex;
			}
			else
			{
				szDest[ nDestIndex ] = szSrc[ nSrcIndex ];
				++nDestIndex;
				++nSrcIndex;
			}
#else
			szDest[ nDestIndex ] = szSrc[ nSrcIndex ];
			++nDestIndex;
			++nSrcIndex;
#endif
		}		
	}
	szDest[ nDestIndex ] = NULL;
	return szDest;
}

void cStringAnalysis::ItemComment_Parcing( cStringList* pList, cItemInfo* pEquipItem )
{
#define EC_PARCING_LEN		128

	if( pEquipItem->m_nRate == 0 )
	{
		ItemComment_Parcing( pList, pEquipItem->GetType() );
		return;
	}

	assert_cs( pEquipItem->m_nRate > 0 );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );

	cString* pString;
	TCHAR szTemp[ EC_PARCING_LEN ];
	TCHAR szInt[ 32 ];

	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pEquipItem->GetType() );
	SAFE_POINTER_RET( pFTItem );
	CsItem::sINFO* pFTItemInfo = pFTItem->GetInfo();
	SAFE_POINTER_RET( pFTItemInfo );

	if( pFTItemInfo->IsAccessory() )//악세서리이면 
	{
		CsAccessory_Option* pAccessory = nsCsFileTable::g_pItemMng->GetAccessoryOption( pFTItemInfo->s_dwSkill );
		SAFE_POINTER_RET( pAccessory );
		CsAccessory_Option::sINFO* pAccInfo = pAccessory->GetInfo();
		SAFE_POINTER_RET( pAccInfo );

		pString = NiNew cString;
		//실제 +적용값 ( 옵션 * 디지터리 )
// 		ti.SetText( _LAN("적용값 ( 옵션*디지터리 ) ") );
		pString->AddText( &ti );

		//옵션값
		for( int i = 0 ; i < nLimit::MAX_ACCESSORY_OPTION ; i++ )
		{
			int nOpt = 	pEquipItem->m_nAccOption[ i ];
			if( nOpt == 0 )//옵션값 없으면 나가자
				break;

			int nVal = pEquipItem->m_nAccValues[ i ];
			int nStrSize = 0;

			pString = NiNew cString;
			std::wstring wsOption;

			switch( nOpt )	// 1: 공격력 , 2: 방어력 , 3: HP , 4: DS , 5: 스킬데미지 , 6: 속성데미지 , 7: 크리티컬
			{
			case nItem::AP :// 1: 공격력
				{
					//실제 +공격력 ( 공격력 옵션 * 디지터리 )
					float nRealVal = (float)nVal*( ( (float)(pEquipItem->m_nRate) )*0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %d ", UISTRING_TEXT( "COMMON_TXT_ATTACK" ).c_str(), (int)nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					//옵션값( 공격력 옵션 )
					DmCS::StringFn::Format( wsOption, L"( %s + %d )", UISTRING_TEXT( "COMMON_TXT_ATTACK" ).c_str(), nVal );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::DP:// 2: 방어력
				{
					//실제 +방어력 ( 방어력 옵션 * 디지터리 )
					float nRealVal = (float)nVal*( ( (float)(pEquipItem->m_nRate) )*0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %d ", UISTRING_TEXT( "COMMON_TXT_DEFENSE" ).c_str(), (int)nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					//옵션값( 방어력 옵션 ) 붙이기
					DmCS::StringFn::Format( wsOption, L"( %s + %d )", UISTRING_TEXT( "COMMON_TXT_DEFENSE" ).c_str(), nVal );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::MAXHP:// 3: HP
				{
					//실제 +HP ( HP 옵션 * 디지터리 )
					float nRealVal = (float)nVal*( ( (float)(pEquipItem->m_nRate) )*0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %d ", UISTRING_TEXT( "COMMON_TXT_MAX_HP" ).c_str(), (int)nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					//옵션값( HP 옵션 ) 붙이기
					DmCS::StringFn::Format( wsOption, L"( %s + %d )", UISTRING_TEXT( "COMMON_TXT_MAX_HP" ).c_str(), nVal );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::MAXDS:// 4: DS
				{
					//실제 +DS ( DS 옵션 * 디지터리 )
					float nRealVal = (float)nVal*( ( (float)(pEquipItem->m_nRate) )*0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %d ", UISTRING_TEXT( "COMMON_TXT_MAX_DS" ).c_str(), (int)nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					//옵션값( DS 옵션 ) 붙이기
					DmCS::StringFn::Format( wsOption, L"( %s + %d )", UISTRING_TEXT( "COMMON_TXT_MAX_DS" ).c_str(), nVal );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::SkillAP:// 5: 스킬데미지
				{
					//실제 +스킬데미지 ( 스킬데미지 옵션 * 디지터리 )
					float nRealVal = (float)nVal*( ( (float)(pEquipItem->m_nRate) )*0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %d ", UISTRING_TEXT( "COMMON_TXT_SKILLAP" ).c_str(), (int)nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					//옵션값( 스킬데미지 옵션 ) 붙이기
					DmCS::StringFn::Format( wsOption, L"( %s + %d )", UISTRING_TEXT( "COMMON_TXT_SKILLAP" ).c_str(), nVal );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AttrAP:// 6: 속성데미지
				{
					//실제 +속성데미지 ( 속성데미지 옵션 * 디지터리 )
					float nRealVal = (float)nVal*( ( (float)(pEquipItem->m_nRate) )*0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %.2f%% ", UISTRING_TEXT( "COMMON_TXT_ATTRIBUTEAP" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					//옵션값( 속성데미지 옵션 ) 붙이기
					DmCS::StringFn::Format( wsOption, L"( %s + %d%% )", UISTRING_TEXT( "COMMON_TXT_ATTRIBUTEAP" ).c_str(), nVal );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::CA:// 7: 크리티컬
				{
					//실제 +크리티컬 ( 크리티컬 옵션 * 디지터리 )
					float nRealVal = (float)nVal*( ( (float)(pEquipItem->m_nRate) )*0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %.2f%% ", UISTRING_TEXT( "COMMON_TXT_CRITICAL" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					//옵션값( 크리티컬 옵션 ) 붙이기
					DmCS::StringFn::Format( wsOption, L"( %s + %d%% )", UISTRING_TEXT( "COMMON_TXT_CRITICAL" ).c_str(), nVal );
					ti.SetText( wsOption.c_str() );
				}
				break;
//#ifdef NECKLACE_ACCESSORY
			case nItem::CD:	// 8: 치명타 데미지 증가
				{
					// 치명타 피해 증가
					float nRealVal = ((float)nVal * 0.01f) * ( ( (float)(pEquipItem->m_nRate) )*0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_CRITICAL_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					//옵션값( 치명피해 옵션 ) 붙이기
					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_CRITICAL_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AS:	// 9: 공속 증가
				{
					// 일반 공격 속도 증가
					float nRealVal = ((float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) )*0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_ATTACK_SPEED" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					//옵션값( 공속증가 옵션 ) 붙이기
					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_ATTACK_SPEED" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
				//악세사리 옵션 추가
			case nItem::EV:	// 10: 회피율 증가	
				{
					// 회피 증가
					float nRealVal = (float)nVal*( ( (float)(pEquipItem->m_nRate) )*0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %.2f%% ", UISTRING_TEXT( "COMMON_TXT_AVOID" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					// 옵션값( 회피 옵션 ) 붙이기
					DmCS::StringFn::Format( wsOption, L"( %s + %d%% )", UISTRING_TEXT( "COMMON_TXT_AVOID" ).c_str(), nVal );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::BL:	// 11: 블럭 확률 증가
				{
					// 일반 공격 속도 증가
					float nRealVal = (float)nVal*( ( (float)(pEquipItem->m_nRate) )*0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %.2f%% ", UISTRING_TEXT( "COMMON_TXT_BLOCK" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					//옵션값( 블럭 옵션 ) 붙이기
					DmCS::StringFn::Format( wsOption, L"( %s + %d%% )", UISTRING_TEXT( "COMMON_TXT_BLOCK" ).c_str(), nVal );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::HT:// 12: 적중도
				{
					float nTempVal = (float)(nVal*pEquipItem->m_nRate) * 0.01f;
					int nRealVal = (int)nTempVal;
					DmCS::StringFn::Format( wsOption, L" %s + %d ", UISTRING_TEXT( "COMMON_TXT_HIT_RATE" ).c_str(), (int)nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					//옵션값( 적중도 옵션 ) 붙이기
					DmCS::StringFn::Format( wsOption, L"( %s + %d )", UISTRING_TEXT( "COMMON_TXT_HIT_RATE" ).c_str(), nVal );
					ti.SetText( wsOption.c_str() );
				}
				break;
				// ============================ 여기에 속성 데미지 관련 툴팁 파싱 추가 ============================
#if COMMON_LIB_FIXED
			case nItem::AP_ATTRIBUTE_DA:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_DATA_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_DATA_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AP_ATTRIBUTE_VA:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_VACCINE_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_VACCINE_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AP_ATTRIBUTE_VI:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_VIRUS_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_VIRUS_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AP_ATTRIBUTE_UN:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_UNKNOWN_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_UNKNOWN_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AP_ATTRIBUTE_IC:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_ICE_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_ICE_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AP_ATTRIBUTE_WA:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_WATER_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_WATER_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AP_ATTRIBUTE_FI:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_FIRE_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_FIRE_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AP_ATTRIBUTE_EA:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_EARTH_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_EARTH_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AP_ATTRIBUTE_WI:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_WIND_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_WIND_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AP_ATTRIBUTE_WO:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_WOOD_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_WOOD_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AP_ATTRIBUTE_LI:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_LIGHT_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_LIGHT_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AP_ATTRIBUTE_DK:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_DARKNESS_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_DARKNESS_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AP_ATTRIBUTE_TH:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_THUNDER_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_THUNDER_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
			case nItem::AP_ATTRIBUTE_ST:
				{
					float nRealVal = ( (float)nVal * 0.01f ) * ( ( (float)(pEquipItem->m_nRate) ) * 0.01f );
					DmCS::StringFn::Format( wsOption, L" %s + %05.2f%% ", UISTRING_TEXT( "COMMON_TXT_STEEL_ATTRIBUTE_DAMAGE" ).c_str(), nRealVal );
					ti.SetText( wsOption.c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s + %05.2f%% )", UISTRING_TEXT( "COMMON_TXT_STEEL_ATTRIBUTE_DAMAGE" ).c_str(), ((float)nVal * 0.01f) );
					ti.SetText( wsOption.c_str() );
				}
				break;
#endif
				// =====================================================================================================
			default:// etc
				{
					ti.SetText( UISTRING_TEXT( "STRING_ANALYSIS_WRONG_OPTION_NUMBER" ).c_str() );
					pString->AddText( &ti );

					DmCS::StringFn::Format( wsOption, L"( %s )", UISTRING_TEXT( "STRING_ANALYSIS_WRONG_OPTION_NUMBER" ).c_str() );
					ti.SetText( wsOption.c_str() );
				}
				break;
			}

			//띄워야할 폭 계산
#ifdef VERSION_USA
			int nX =  TOOLTIP_TAB_SIZE*2 - pString->GetMaxSize().x;
#else
			int nX =  TOOLTIP_TAB_SIZE*3 - pString->GetMaxSize().x;
#endif
			nStrSize = TOOLTIP_TAB_SIZE*3 + nX;

			pString->TailAddSizeX( nStrSize );
			pString->AddText( &ti );

			if( pAccInfo->s_nGainOpt == i + 1 )//마지막 옵션
				pString->AddSizeY( 13 );
			pList->AddTail( pString );
		}

		int nLevel = 0;
		if( pEquipItem->m_nLevel > 0 )
			nLevel = pEquipItem->m_nLevel;

		std::wstring wsText;
		DmCS::StringFn::Format( wsText, L"%s : %d %s", 
			UISTRING_TEXT( "STRING_ANALYSIS_DIGIABLE_POWER_RENEWAL" ).c_str(), 
			nLevel, UISTRING_TEXT( "STRING_ANALYSIS_COUNT" ).c_str() );
		ti.SetText( wsText.c_str() );

		pString = NiNew cString;
		pString->AddText( &ti );
		pList->AddTail( pString );
		return;
	}
	else if( pFTItemInfo->s_nType_L == nItem::AccStone )//강화아이템일 때
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );	
		cString* pString = NiNew cString;
		ti.SetText( UISTRING_TEXT( "STRING_ANALYSIS_ENCHANT_TYPE" ).c_str() );
		pString->AddText( &ti );

		//띄워야할 폭 계산
		int nStrSize = TOOLTIP_TAB_SIZE*3 - pString->GetMaxSize().x;
		pString->TailAddSizeX( nStrSize );

		CsAccessory_Enchant* pAccessory = nsCsFileTable::g_pItemMng->GetAccessoryEnchant( pFTItemInfo->s_dwSkill );
		int nOpt =  pAccessory->GetInfo()->s_nOpt;//추가옵션 수
		switch( nOpt )
		{
		case nItem::DigiPower://디지터리 파워 증가/감소
			ti.SetText( UISTRING_TEXT( "STRING_ANALYSIS_DIGITARY_POWER_INCREASE_DECREASE" ).c_str() );
			break;
		case nItem::Renewal://리뉴얼 횟수 증가/보존
			ti.SetText( UISTRING_TEXT( "STRING_ANALYSIS_RENEWAL_NUMBER_INCREASE_RESERVE" ).c_str() );
			break;
		case nItem::AccOption://악세서리 옵션 랜덤 변경
			ti.SetText( UISTRING_TEXT( "STRING_ANALYSIS_CHANGE_ACCESSORY_OPTION" ).c_str() );
			break;
		case nItem::OptionValue://해당 악세서리 옵션 값 증가/감소
			ti.SetText( UISTRING_TEXT( "STRING_ANALYSIS_CHANGE_ACCESSORY_VALUE" ).c_str() );
			break;
		}
		ti.s_Color = FONT_WHITE;

		pString->AddText( &ti );
		pList->AddTail( pString );
		return;
	}

	CsSkill* pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( pFTItemInfo->s_dwSkill );
	SAFE_POINTER_RET( pFTSkill );
	CsSkill::sINFO* pFTSkillInfo = pFTSkill->GetInfo();
	SAFE_POINTER_RET( pFTSkillInfo );

	if( pEquipItem->IsCashEndTime( pFTItemInfo ) )
		ti.s_Color = TOOLTIP_CANNOT_COLOR;

	for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
	{
		// 메모리 스킬일 경우 등록하지 않는다.
		if( pFTSkillInfo->s_Apply[ i ].s_nID == 0 || 
			pFTSkillInfo->s_Apply[ i ].s_nA == APPLY_EvoTypeDamageIncrease||
			pFTSkillInfo->s_Apply[ i ].s_nA == APPLY_NatureTypeDamageIncrease ||	// 속성 데미지 증가일 경우 버프 효과를 표시하지 않는다.
			pFTSkillInfo->s_nMemorySkill )
			continue;

		pString = NiNew cString;
		
		if( FMCommon::GetSkillAttStr( pFTSkillInfo->s_Apply[ i ].s_nA, EC_PARCING_LEN, szTemp ) == true )
		{
			int nLv = ( pFTItemInfo->s_nType_L == nItem::Buff ) ? pFTItemInfo->s_nTypeValue : pFTItemInfo->s_nTypeValue;
			float fPV =	FMCommon::GetSkillAtt( pFTItemInfo->s_dwSkill, nLv, i ) *  pEquipItem->m_nRate * 0.01f;
			bool bMinus = ( fPV < 0.0f );
			int nValue = CsFloat2Int( abs( fPV ) );
			if( bMinus )
				nValue = -nValue;
			if( nValue != 0)
			{
				_stprintf_s( szInt, 32, _T( " %d " ), nValue );
				_tcscat_s( szTemp, EC_PARCING_LEN, szInt );
			}
			
			FMCommon::GetStrApply_ID( szTemp, EC_PARCING_LEN, pFTSkillInfo->s_Apply[ i ].s_nID, bMinus );			
		}		

		ti.SetText( szTemp );
		pString->AddText( &ti );
		pList->AddTail( pString );
	}
}

void cStringAnalysis::ItemComment_Parcing( cStringList* pList, int nEquipType )
{
#define EC_PARCING_LEN		128

	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( nEquipType );
	SAFE_POINTER_RET( pItem );
	CsItem::sINFO* pFTItem = pItem->GetInfo();
	SAFE_POINTER_RET( pFTItem );

	CsSkill::sINFO* pFTSkill = NULL;
	CsTamerSkill* pTamerSkill = NULL;
	if( pFTItem->s_nType_L == 180 )
	{
		pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( pFTItem->s_dwSkill, -1 );
		SAFE_POINTER_RET( pTamerSkill );
		SAFE_POINTER_RET( pTamerSkill->Get_TSkill_Info() );
		CsSkill* pTamerSkillInfo = nsCsFileTable::g_pSkillMng->GetSkill( pTamerSkill->Get_TSkill_Info()->s_dwSkillCode );
		SAFE_POINTER_RET( pTamerSkillInfo );
		pFTSkill = pTamerSkillInfo->GetInfo();
		SAFE_POINTER_RET( pFTSkill );
	}
	else if( pFTItem->IsAccessory() )//악세서리일 때
	{
		CsAccessory_Option* pAccessory = nsCsFileTable::g_pItemMng->GetAccessoryOption( pFTItem->s_dwSkill );
		SAFE_POINTER_RET( pAccessory );
		SAFE_POINTER_RET( pAccessory->GetInfo() );
		int nOptCnt =  pAccessory->GetInfo()->s_nGainOpt;//추가옵션 수
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );	
		ti.SetText( UISTRING_TEXT( "STRING_ANALYSIS_IDENTIFY_ACCESSORY_EXPLAIN" ).c_str() );
		cString* pString = NiNew cString;	
		pString->AddText( &ti );
		pList->AddTail( pString );

		return;
	}
	else if( pFTItem->s_nType_L == nItem::AccStone )//강화아이템일 때
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );	
		cString* pString = NiNew cString;
		ti.SetText( UISTRING_TEXT( "STRING_ANALYSIS_ENCHANT_TYPE" ).c_str() );
		pString->AddText( &ti );

		//띄워야할 폭 계산
		int nStrSize = TOOLTIP_TAB_SIZE*3 - pString->GetMaxSize().x;
		pString->TailAddSizeX( nStrSize );

		CsAccessory_Enchant* pAccessory = nsCsFileTable::g_pItemMng->GetAccessoryEnchant( pFTItem->s_dwSkill );
		SAFE_POINTER_RET( pAccessory );
		SAFE_POINTER_RET( pAccessory->GetInfo() );
		int nOpt =  pAccessory->GetInfo()->s_nOpt;//추가옵션 수
		switch( nOpt )
		{
		case nItem::DigiPower://디지터리 파워 증가/감소
			ti.SetText( UISTRING_TEXT( "STRING_ANALYSIS_DIGITARY_POWER_INCREASE_DECREASE" ).c_str() );
			break;
		case nItem::Renewal://리뉴얼 횟수 증가/보존
			ti.SetText( UISTRING_TEXT( "STRING_ANALYSIS_RENEWAL_NUMBER_INCREASE_RESERVE" ).c_str() );
			break;
		case nItem::AccOption://악세서리 옵션 랜덤 변경
			ti.SetText( UISTRING_TEXT( "STRING_ANALYSIS_CHANGE_ACCESSORY_OPTION" ).c_str() );
			break;
		case nItem::OptionValue://해당 악세서리 옵션 값 증가/감소
			ti.SetText( UISTRING_TEXT( "STRING_ANALYSIS_CHANGE_ACCESSORY_VALUE" ).c_str() );
			break;
		}
		ti.s_Color = FONT_WHITE;
		pString->AddText( &ti );
		pList->AddTail( pString );

		return;
	}
	else
	{
		CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( pFTItem->s_dwSkill );
		SAFE_POINTER_RET( pSkill );
		pFTSkill = pSkill->GetInfo();
		SAFE_POINTER_RET( pFTSkill );
	}

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );	
	cString* pString;

	TCHAR szTemp[ EC_PARCING_LEN ] = {0,};
	TCHAR szInt[ 128 ] = {0,};
	for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
	{
		// 메모리 스킬일 경우 등록하지 않는다.
		if( pFTSkill->s_Apply[ i ].s_nID == 0 || 
			pFTSkill->s_Apply[ i ].s_nA == APPLY_EvoTypeDamageIncrease||
			pFTSkill->s_Apply[ i ].s_nA == APPLY_NatureTypeDamageIncrease ||	// 속성 데미지 증가일 경우 버프 효과를 표시하지 않는다.
			pFTSkill->s_nMemorySkill )
			continue;

		pString = NiNew cString;

		if( FMCommon::GetSkillAttStr( pFTSkill->s_Apply[ i ].s_nA, EC_PARCING_LEN, szTemp ) == true )
		{
			int nLv = ( pFTItem->s_nType_L == nItem::Buff ) ? pFTItem->s_nTypeValue : pFTItem->s_nTypeValue;
			int nValue = 0;
			if( pFTItem->s_nType_L == 180 )
				nValue = FMCommon::GetSkillAtt( pTamerSkill->Get_TSkill_Info()->s_dwSkillCode, nLv, i );
			else
				nValue = FMCommon::GetSkillAtt( pFTItem->s_dwSkill, nLv, i );
			
			bool bMinus = ( nValue < 0 );
			int nMin = CsFloat2Int( abs( nValue * pFTItem->s_btApplyRateMin * 0.01f ) );
			int nMax = CsFloat2Int( abs( nValue * pFTItem->s_btApplyRateMax * 0.01f ) );
			if( bMinus )
			{
				nMin = -nMin;
				nMax = -nMax;
			}

			// 둘다 값이 0이라면 비율은 1로 적용
			if( ( nMin || nMax ) == false )
				nMin = nMax = nValue;
			
			if( nMin != nMax )
				_stprintf_s( szInt, 32, _T( " %d~%d " ), nMin, nMax );
			else if( nValue != 0)
				_stprintf_s( szInt, 32, _T( " %d " ), nMin );
			_tcscat_s( szTemp, EC_PARCING_LEN, szInt );

			FMCommon::GetStrApply_ID( szTemp, EC_PARCING_LEN, pFTSkill->s_Apply[ i ].s_nID, bMinus );						
		}			

		ti.SetText( szTemp );
		pString->AddText( &ti );
		pList->AddTail( pString );
	}	
}

void cStringAnalysis::Equip_EItemComment_Parcing( cStringList* pList, cString* pBeginString, cItemInfo* pEquip, int nSocketIndex, bool bCashEndItem )
{
#define EC_PARCING_LEN		128
	SAFE_POINTER_RET( pEquip );
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 0, 1, 0 ) );
	if( bCashEndItem )
		ti.s_Color = TOOLTIP_CANNOT_COLOR;

	int	nSocketRate = pEquip->GetSocketRate();
	
	assert_cs( pEquip->m_nSockItemType[ nSocketIndex ] != 0 );
	CsItem* pSocketItem = nsCsFileTable::g_pItemMng->GetItem( pEquip->m_nSockItemType[ nSocketIndex ] );
	SAFE_POINTER_RET( pSocketItem );
	CsItem::sINFO* pFTItem = pSocketItem->GetInfo();
	SAFE_POINTER_RET( pFTItem );
	CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( pFTItem->s_dwSkill );
	SAFE_POINTER_RET( pSkill );
	CsSkill::sINFO* pFTSkill = pSkill->GetInfo();
	SAFE_POINTER_RET( pFTSkill );
	cString* pString;
	int nTabSize = 0;	
	if( pBeginString != NULL )
	{
		pBeginString->CalMaxSize();
		nTabSize = pBeginString->GetMaxSize().x;
	}

	TCHAR szTemp[ EC_PARCING_LEN ];
	TCHAR szInt[ 128 ];
	for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
	{
		if( pFTSkill->s_Apply[ i ].s_nID == 0 )
			continue;

		if( pBeginString != NULL )
		{
			pString = pBeginString;
			pBeginString = NULL;
		}
		else
		{
			pString = NiNew cString;
			if( nTabSize != 0 )
			{
				ti.SetText( _T( "" ) );
				pString->AddText( &ti )->s_ptSize.x = nTabSize;
			}
		}

		if( FMCommon::GetSkillAttStr( pFTSkill->s_Apply[ i ].s_nA, EC_PARCING_LEN, szTemp ) == true )
		{	
			int nLv = ( pFTItem->s_nType_L == nItem::Buff ) ? pFTItem->s_nTypeValue : pFTItem->s_nTypeValue;
			float fPV = FMCommon::GetSkillAtt( pFTItem->s_dwSkill, nLv, i ) * pEquip->m_nSockAppRate[ nSocketIndex ] * 0.01f;
			bool bMinus = ( fPV < 0.0f );
			int nOrgValue = CsFloat2Int( abs( fPV ) );
			if( bMinus )
				nOrgValue = -nOrgValue;

			int nValue = CsFloat2Int( nOrgValue*nSocketRate*0.01f );
			_stprintf_s( szInt, 128, _T( " %d " ), nValue );
			_tcscat_s( szTemp, EC_PARCING_LEN, szInt );

			FMCommon::GetStrApply_ID( szTemp, EC_PARCING_LEN, pFTSkill->s_Apply[ i ].s_nID, bMinus );

			_stprintf_s( szInt, 128, _T( "  (%d x %d%%)" ), nOrgValue, nSocketRate );
			_tcscat_s( szTemp, EC_PARCING_LEN, szInt );
		}		

		ti.SetText( szTemp );
		pString->AddText( &ti );
		pList->AddTail( pString );
	}
}

void cStringAnalysis::SetParcingColor( const NiColor color )
{
	m_bChangeColor = true;
	m_ParcingColor = color;
}

NiColor cStringAnalysis::GetParcingColor() const
{
	return m_ParcingColor;
}

void cStringAnalysis::CompleteParcingColor()
{
	m_bChangeColor = false;
	m_ParcingColor = NiColor();
}
