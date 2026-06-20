

#include "stdafx.h"
#include "Help.h"

void CsHelp::sTEXT::Delete()
{
	s_Text.clear();
}

void CsHelp::sTEXT::Save( FILE* fp )
{
	fwrite( &s_TextType, sizeof( USHORT ), 1, fp );
	fwrite( &s_TextTypeVal1, sizeof( UINT ), 1, fp );
	fwrite( &s_TextTypeVal2, sizeof( UINT ), 1, fp );
	fwrite( &s_rcRegion, sizeof( CsRect ), 1, fp );
	fwrite( s_Color, sizeof( BYTE ), 3, fp );
	fwrite( &s_nTextSize, sizeof( SHORT ), 1, fp );
	fwrite( &s_nBold, sizeof( SHORT ), 1, fp );
	int nLen = (int)_tcslen( s_Text.c_str() );
	fwrite( &nLen, sizeof( int ), 1, fp );
	fwrite( s_Text.c_str(), sizeof( TCHAR ), nLen, fp );
}

void CsHelp::sTEXT::Load( FILE* fp )
{
	fread( &s_TextType, sizeof( USHORT ), 1, fp );
	fread( &s_TextTypeVal1, sizeof( UINT ), 1, fp );
	fread( &s_TextTypeVal2, sizeof( UINT ), 1, fp );
	fread( &s_rcRegion, sizeof( CsRect ), 1, fp );
	fread( s_Color, sizeof( BYTE ), 3, fp );
	fread( &s_nTextSize, sizeof( SHORT ), 1, fp );
	fread( &s_nBold, sizeof( SHORT ), 1, fp );
	int nLen;
	fread( &nLen, sizeof( int ), 1, fp );
	s_Text.resize( nLen );	
	fread( (TCHAR*)s_Text.c_str(), sizeof( TCHAR ), nLen, fp );	
}

void CsHelp::sTEXT::LoadFilePack( int nHandle )
{
	_read( nHandle, &s_TextType, sizeof( USHORT ) );
	_read( nHandle, &s_TextTypeVal1, sizeof( UINT ) );
	_read( nHandle, &s_TextTypeVal2, sizeof( UINT ) );
	_read( nHandle, &s_rcRegion, sizeof( CsRect ) );
	_read( nHandle, s_Color, sizeof( BYTE )*3 );
	_read( nHandle, &s_nTextSize, sizeof( SHORT ) );
	_read( nHandle, &s_nBold, sizeof( SHORT ) );
	int nLen;
	_read( nHandle, &nLen, sizeof( int ) );
	s_Text.resize( nLen );	
	_read( nHandle, (TCHAR*)s_Text.c_str(), sizeof( TCHAR )*nLen );	
}

bool CsHelp::sTEXT::Check()
{
	TCHAR* sz = (TCHAR*)s_Text.c_str();
	int len = (int)_tcslen( sz );
	if( len == 0 )
		return true;

	for( int i=0; i<len; ++i )
	{
		if( ( sz[ i ] != L' ' )&&( sz[ i ] != L'\n' )&&( sz[ i ] != 0x0d ) )
			return false;
	}
	return true;
}

void CsHelp::sSTRING::Delete()
{
	int nSize =	s_vpText.Size();
	for( int i=0; i<nSize; ++i )
		delete s_vpText[ i ];
	s_vpText.Destroy();
}

void CsHelp::sSTRING::DeleteText( int nIndex )
{
	assert_cs( s_vpText.IsExistElement( nIndex ) );
	delete s_vpText[ nIndex ];
	s_vpText.DeleteElement( nIndex );
}

void CsHelp::sSTRING::DeleteText( CsHelp::sTEXT* pTextFT )
{
	int nSize =	s_vpText.Size();
	for( int i=0; i<nSize; ++i )
	{
		if( s_vpText[ i ] == pTextFT )
		{
			DeleteText( i );
			return;
		}
	}
	assert_cs( false );
}

void CsHelp::sSTRING::DeleteText_ExceptFirst()
{
	int nSize =	s_vpText.Size();
	for( int i=nSize-1; i>0; --i )
	{
		DeleteText( i );
	}
}

void CsHelp::sSTRING::Save( FILE* fp )
{
	int nSize =	s_vpText.Size();
	fwrite( &nSize, sizeof( int ), 1, fp );
	for( int i=0; i<nSize; ++i )
	{
		s_vpText[ i ]->Save( fp );
	}
	fwrite( &s_ptPos, sizeof( CsPoint ), 1, fp );
	fwrite( &s_nLineDeltaHeight, sizeof( int ), 1, fp );
}

void CsHelp::sSTRING::Load( FILE* fp )
{
	int nSize;
	fread( &nSize, sizeof( int ), 1, fp );
	for( int i=0; i<nSize; ++i )
	{
		AddText()->Load( fp );
	}
	fread( &s_ptPos, sizeof( CsPoint ), 1, fp );
	fread( &s_nLineDeltaHeight, sizeof( int ), 1, fp );
}

void CsHelp::sSTRING::LoadFilePack( int nHandle )
{
	int nSize;
	_read( nHandle, &nSize, sizeof( int ) );
	for( int i=0; i<nSize; ++i )
	{
		AddText()->LoadFilePack( nHandle );
	}
	_read( nHandle, &s_ptPos, sizeof( CsPoint ) );
	_read( nHandle, &s_nLineDeltaHeight, sizeof( int ) );
}

void CsHelp::sSTRING::Check()
{
	for( int i=0; i<s_vpText.Size(); ++i )
	{
		if( s_vpText[ i ]->Check() == true )
		{
			delete s_vpText[ i ];
			s_vpText.DeleteElement( i );

			--i;
		}
	}		
}

void CsHelp::sSTRING::CheckCombine()
{
	int nCount = s_vpText.Size();
	for( int i=0; i<nCount; ++i )
	{
		// 만약 뒷놈이랑같은 형태라면 합쳐주자
		CsHelp::sTEXT* pNext = GetNextText( s_vpText[ i ] );		
		if( pNext == NULL )
			continue;

		if( ( s_vpText[ i ]->s_nTextSize == pNext->s_nTextSize )&&
			( s_vpText[ i ]->s_Color[ 0 ] == pNext->s_Color[ 0 ] )&&
			( s_vpText[ i ]->s_Color[ 1 ] == pNext->s_Color[ 1 ] )&&
			( s_vpText[ i ]->s_Color[ 2 ] == pNext->s_Color[ 2 ] )&&
			( s_vpText[ i ]->s_nBold == pNext->s_nBold )	)
		{			
			s_vpText[ i ]->s_Text.append( pNext->s_Text );
			pNext->s_Text.clear();
			DeleteText( pNext );
			CheckCombine();
			return;
		}
	}
}

CsHelp::sTEXT* CsHelp::sSTRING::CheckText( CsHelp::sTEXT* pTextFT )
{
	if( pTextFT == NULL )
		return pTextFT;

	if( IsText( pTextFT ) == false )
		return pTextFT;

	if( pTextFT->s_Text.c_str()[ 0 ] != NULL )
		return pTextFT;

	// 만약 뒷놈이랑 앞놈이랑 같은 형태라면 합쳐주자
	CsHelp::sTEXT* pPrev = GetPrevText( pTextFT );
	CsHelp::sTEXT* pNext = GetNextText( pTextFT );
	DeleteText( pTextFT );
	if( pPrev && pNext )
	{
		if( ( pPrev->s_nTextSize == pNext->s_nTextSize )&&
			( pPrev->s_Color[ 0 ] == pNext->s_Color[ 0 ] )&&
			( pPrev->s_Color[ 1 ] == pNext->s_Color[ 1 ] )&&
			( pPrev->s_Color[ 2 ] == pNext->s_Color[ 2 ] )&&
			( pPrev->s_nBold == pNext->s_nBold )	)
		{			
			pPrev->s_Text.append( pNext->s_Text );
			pNext->s_Text.clear();
			return CheckText( pNext );
		}
	}

	return pNext != NULL ? pNext : pPrev;
}

bool CsHelp::sSTRING::IsText( CsHelp::sTEXT* pTextFT )
{
	int nCount = s_vpText.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( s_vpText[ i ] == pTextFT )
			return true;
	}
	return false;
}

CsHelp::sTEXT* CsHelp::sSTRING::AddText()
{
	CsHelp::sTEXT* pNew = csnew CsHelp::sTEXT;
	s_vpText.PushBack( pNew );
	return pNew;
}

CsHelp::sTEXT* CsHelp::sSTRING::AddTextAfter( CsHelp::sTEXT* pTextFT )
{
	int nCount = s_vpText.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( s_vpText[ i ] == pTextFT )
		{
			CsHelp::sTEXT* pNew = csnew CsHelp::sTEXT;	
			s_vpText.PushInsert( i, pNew );
			return pNew;
		}
	}
	assert_cs( false );
	return NULL;
}

CsHelp::sTEXT* CsHelp::sSTRING::AddTextBefore( CsHelp::sTEXT* pTextFT )
{
	int nCount = s_vpText.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( s_vpText[ i ] == pTextFT )
		{
			CsHelp::sTEXT* pNew = csnew CsHelp::sTEXT;	
			// 첫번째 라면
			if( i == 0 )
			{
				s_vpText.PushFront( pNew );
			}
			else
			{
				s_vpText.PushInsert( i-1, pNew );
			}
			return pNew;
		}
	}
	assert_cs( false );
	return NULL;
}

CsHelp::sTEXT* CsHelp::sSTRING::GetFirstText()
{
	if( s_vpText.Size() == 0 )
	{
		return NULL;
	}
	return s_vpText[ 0 ];
}

CsHelp::sTEXT* CsHelp::sSTRING::GetLastText()
{
	int nSize =	s_vpText.Size();
	if( nSize == 0 )
	{
		return NULL;
	}

	CsHelp::sTEXT* pReturn = s_vpText[ nSize - 1 ];	
	return pReturn;
}

CsHelp::sTEXT* CsHelp::sSTRING::GetPrevText( CsHelp::sTEXT* pTextFT )
{
	int nSize =	s_vpText.Size();
	for( int i=1; i<nSize; ++i )
	{
		if( s_vpText[ i ] == pTextFT )
			return s_vpText[ i - 1 ];
	}
	return NULL;
}

CsHelp::sTEXT* CsHelp::sSTRING::GetNextText( CsHelp::sTEXT* pTextFT )
{
	int nSize =	s_vpText.Size();
	for( int i=0; i<nSize-1; ++i )
	{
		if( s_vpText[ i ] == pTextFT )
			return s_vpText[ i + 1 ];
	}
	return NULL;
}

void CsHelp::sIMAGE::Save( FILE* fp )
{
	fwrite( &s_nImageType, sizeof( int ), 1, fp );
	fwrite( &s_ptPos, sizeof( CsPoint ), 1, fp );
	fwrite( &s_ptSize, sizeof( CsPoint ), 1, fp );
	int nLen = (int)strlen( s_ImagePath.c_str() );
	fwrite( &nLen, sizeof( int ), 1, fp );
	fwrite( s_ImagePath.c_str(), sizeof( char ), nLen, fp );
}

void CsHelp::sIMAGE::Load( FILE* fp )
{
	fread( &s_nImageType, sizeof( int ), 1, fp );
	fread( &s_ptPos, sizeof( CsPoint ), 1, fp );
	fread( &s_ptSize, sizeof( CsPoint ), 1, fp );
	int nLen;
	fread( &nLen, sizeof( int ), 1, fp );
	s_ImagePath.resize( nLen );	
	fread( (TCHAR*)s_ImagePath.c_str(), sizeof( char ), nLen, fp );
}

void CsHelp::sIMAGE::LoadFilePack( int nHandle )
{
	_read( nHandle, &s_nImageType, sizeof( int ) );
	_read( nHandle, &s_ptPos, sizeof( CsPoint ) );
	_read( nHandle, &s_ptSize, sizeof( CsPoint ) );
	int nLen;
	_read( nHandle, &nLen, sizeof( int ) );
	s_ImagePath.resize( nLen );	
	_read( nHandle, (TCHAR*)s_ImagePath.c_str(), sizeof( char )*nLen );
}

void CsHelp::sCONTAINER::Delete()
{
	int nSize =	s_vpString.Size();
	for( int i=0; i<nSize; ++i )
		delete s_vpString[ i ];
	s_vpString.Destroy();

	nSize = s_vpImage.Size();
	for( int i=0; i<nSize; ++i )
		delete s_vpImage[ i ];
	s_vpImage.Destroy();
}

void CsHelp::sCONTAINER::DeleteString( CsHelp::sSTRING* pString )
{
	int nSize = s_vpString.Size();
	for( int i=0; i<nSize; ++i )
	{
		if( pString == s_vpString[ i ] )
		{
			delete pString;
			s_vpString.DeleteElement( i );
			return;
		}
	}
}

void CsHelp::sCONTAINER::DeleteImage( CsHelp::sIMAGE* pImage )
{
	int nSize = s_vpImage.Size();
	for( int i=0; i<nSize; ++i )
	{
		if( pImage == s_vpImage[ i ] )
		{
			delete pImage;
			s_vpImage.DeleteElement( i );
			return;
		}
	}
	assert_cs( false );
}

void CsHelp::sCONTAINER::Save( FILE* fp )
{
	fwrite( &s_nLimitScroll, sizeof( int ), 1, fp );

	int nSize =	s_vpString.Size();
	fwrite( &nSize, sizeof( int ), 1, fp );
	for( int i=0; i<nSize; ++i )
		s_vpString[ i ]->Save( fp );

	nSize =	s_vpImage.Size();
	fwrite( &nSize, sizeof( int ), 1, fp );
	for( int i=0; i<nSize; ++i )
		s_vpImage[ i ]->Save( fp );
}

void CsHelp::sCONTAINER::Load( FILE* fp )
{
	fread( &s_nLimitScroll, sizeof( int ), 1, fp );

	int nSize;
	fread( &nSize, sizeof( int ), 1, fp );
	for( int i=0; i<nSize; ++i )
	{
		AddString()->Load( fp );
	}

	fread( &nSize, sizeof( int ), 1, fp );
	for( int i=0; i<nSize; ++i )
	{
		AddImage()->Load( fp );
	}
}

void CsHelp::sCONTAINER::ToEng( CsHelp* pParent )
{
	assert_cs( nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH_A ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::THAILAND || nsCsFileTable::g_eLanguage == nsCsFileTable::TAIWAN || nsCsFileTable::g_eLanguage == nsCsFileTable::HONGKONG );

	int nSize = s_vpString.Size();
	for( int i=0; i<nSize; ++i )
	{
		DWORD tID = pParent->GetTitle()->s_nUniqID*1000 + i;
		if( nsCsFileTable::g_pTotalExcelMng->IsData( CsTotalExcelMng::HELPTEXT, tID ) )
		{
			pParent->GetBody()->s_vpString[ i ]->s_vpText[ 0 ]->s_Text = nsCsFileTable::g_pTotalExcelMng->GetData( CsTotalExcelMng::HELPTEXT, tID )->GetInfo()->s_szEng;
			pParent->GetBody()->s_vpString[ i ]->DeleteText_ExceptFirst();
		}
	}
}

void CsHelp::sCONTAINER::LoadFilePack( int nHandle )
{
	_read( nHandle, &s_nLimitScroll, sizeof( int ) );

	int nSize;
	_read( nHandle, &nSize, sizeof( int ) );
	for( int i=0; i<nSize; ++i )
	{
		AddString()->LoadFilePack( nHandle );
	}

	_read( nHandle, &nSize, sizeof( int ) );
	for( int i=0; i<nSize; ++i )
	{
		AddImage()->LoadFilePack( nHandle );
	}
}

void CsHelp::sCONTAINER::Check()
{
	for( int i=0; i<s_vpString.Size(); ++i )
	{
		s_vpString[ i ]->Check();
		s_vpString[ i ]->CheckCombine();
		if( s_vpString[ i ]->s_vpText.Size() == 0 )
		{
			delete s_vpString[ i ];
			s_vpString.DeleteElement( i );

			--i;
		}
	}
}

CsHelp::sSTRING* CsHelp::sCONTAINER::AddString()
{
	CsHelp::sSTRING* pNew = csnew CsHelp::sSTRING;
	s_vpString.PushBack( pNew );	
	return pNew;
}

CsHelp::sIMAGE* CsHelp::sCONTAINER::AddImage( sIMAGE::eIMAGE_TYPE it )
{
	CsHelp::sIMAGE* pNew = csnew CsHelp::sIMAGE;
	pNew->s_nImageType = it;
	s_vpImage.PushBack( pNew );	
	return pNew;
}

CsHelp::CsHelp()
{
	m_Body.Delete();
}

void CsHelp::Save( FILE* fp )
{
	fwrite( &m_Title, sizeof( sTITLE ), 1, fp );

	m_Body.Check();
	m_Body.Save( fp );
}

void CsHelp::Load( FILE* fp )
{
	fread( &m_Title, sizeof( sTITLE ), 1, fp );
	m_Body.Load( fp );
}

void CsHelp::LoadFilePack( int nHandle )
{
	_read( nHandle, &m_Title, sizeof( sTITLE ) );

	m_Body.LoadFilePack( nHandle );
}

void CsHelp::ToEng()
{
	assert_cs( nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH_A ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::THAILAND || nsCsFileTable::g_eLanguage == nsCsFileTable::TAIWAN || nsCsFileTable::g_eLanguage == nsCsFileTable::HONGKONG );
	DWORD tID = m_Title.s_nUniqID*1000 - 1;
	if( nsCsFileTable::g_pTotalExcelMng->IsData( CsTotalExcelMng::HELPTEXT, tID ) )
		_tcscpy_s( m_Title.s_szTitle, 64, nsCsFileTable::g_pTotalExcelMng->GetData( CsTotalExcelMng::HELPTEXT, tID )->GetInfo()->s_szEng );

	m_Body.ToEng( this );
}