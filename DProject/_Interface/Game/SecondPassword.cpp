#include "stdafx.h"
#include "SecondPassword.h"
#include "common/pPass2.h"

cSecondPassword*		g_pSecondPassword = NULL;

cSecondPassword::cSecondPassword()
{
	m_bShow = false;
	m_eType = eEnd;
	m_Str = NULL;
	m_Text = NULL;	
}

cSecondPassword::~cSecondPassword()
{
	
}

void cSecondPassword::GlobalInit()
{
	assert_cs( g_pSecondPassword == NULL );
	g_pSecondPassword = NiNew cSecondPassword;	

#ifdef USE_DMO_INI
	cProfile x("./dmo.ini");	
	strcpy(g_pSecondPassword->m_szIniPass, x.GetStr("NETWORK", "2pw"));
#endif	
}

void cSecondPassword::GlobalShotDown()
{
	if( g_pSecondPassword != NULL )
		g_pSecondPassword->Delete();
	SAFE_NIDELETE( g_pSecondPassword );
}

bool cSecondPassword::IsRegisted()
{ 
	if( g_pDataMng->GetAccount()->Get2ndPassInfo() == nPass2::Registered )
		return true;
	else
		return false;
}

bool cSecondPassword::IsCertified()
{ 
	if( g_pDataMng->GetAccount()->Get2ndPassInfo() == nPass2::Certified )
		return true;
	else
		return false;
}

void cSecondPassword::Delete()
{
	itLIST it = m_BtnInfo.begin();
	itLIST itEnd = m_BtnInfo.end();

	for( ; it != itEnd; ++it )
	{
		SAFE_NIDELETE( (*it)->s_Btn );		
		SAFE_NIDELETE( (*it)->s_Text );
		SAFE_NIDELETE( *it );
	}		
	m_BtnInfo.clear();
	SAFE_NIDELETE( m_Str );
	SAFE_NIDELETE( m_Text );
	m_StringList.Delete();

	DeleteScript();
}

void cSecondPassword::ResetDevice()
{
	if( !m_bShow )
		return;

	itLIST it = m_BtnInfo.begin();
	itLIST itEnd = m_BtnInfo.end();

	for( ; it != itEnd; ++it )
	{
		(*it)->s_Btn->ResetDevice();
		(*it)->s_Text->ResetDevice();
	}
	if( m_Str )
		m_Str->ResetDevice();
	if( m_Text )
		m_Text->ResetDevice();
	m_StringList.ResetDevice();
	ResetDeviceScript();
}

void cSecondPassword::_Init()
{
	if( this->GetRoot() != NULL )
		Delete();

	if( m_eType == eSet )
	{
		if( m_eCaseType == ePassRegistCheck || m_eCaseType == ePassChangeCheck )
		{
			InitScript( "SecondPass\\2SecWin_1.tga", CsPoint( 325, 305 ), CsPoint( 355, 305 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );		
			AddTitle( _LAN( "2차비밀번호(설정확인)" ), CsPoint( 0, 10 ) );	
		}
		else
		{
			InitScript( "SecondPass\\2SecWin_1.tga", CsPoint( 325, 305 ), CsPoint( 355, 305 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );		
			AddTitle( _LAN( "2차비밀번호(설정)" ), CsPoint( 0, 10 ) );	
		}
	}
	else
	{
		cPrintMsg::PrintMsg( 20058 );
		InitScript( "SecondPass\\2SecWin.tga", CsPoint( 325, 305 ), CsPoint( 355, 305 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );
		AddTitle( _LAN( "2차비밀번호(확인)" ), CsPoint( 0, 10 ) );	
	}

	cText::sTEXTINFO ti;
	assert_cs( m_Text == NULL );
	m_Text = NiNew cString;		
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 247/255.0f, 211/255.0f, 153/255.0f ) );
	ti.SetText( _LAN( "마우스 입력" ) );	
	m_Text->AddText( &ti );
	
	cString* pStr = NiNew cString;			
	switch( m_eType )
	{
	case eSet:
		{
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 140/255.0f, 1.0f, 1.0f ) );
			ti.SetText( _LAN( "설정 :" ) );
			pStr->AddText( &ti, CsPoint( 30, 0 ) );
			m_StringList.AddTail( pStr );	
			m_StringList.AddTailEmpty( CFont::FS_10 );

			TCHAR sz[ 256 ];
			_tcscpy_s( sz, 256, _LAN( "-오른쪽 키 패드를 이용하여 입력해 주세요." ) );
			ti.s_Color = NiColor( 247/255.0f, 211/255.0f, 153/255.0f );	
			ti.s_eFontSize = CFont::FS_10;
			g_pStringAnalysis->Cut( &m_StringList, 165, sz, &ti );			
			_tcscpy_s( sz, 256, _LAN( "-최소 4자리 이상 최대 8자리까지 설정 가능합니다." ) );
			g_pStringAnalysis->Cut( &m_StringList, 165, sz, &ti );
			_tcscpy_s( sz, 256, _LAN( "-동일 숫자는 2차 비밀번호로 설정 하실 수 없습니다." ) );
			g_pStringAnalysis->Cut( &m_StringList, 165, sz, &ti );			
		}		
		break;
	case eQuestion:
		{
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 140/255.0f, 1.0f, 1.0f ) );
			ti.SetText( _LAN( "확인 :" ) );
			pStr->AddText( &ti, CsPoint( 30, 0 ) );
			m_StringList.AddTail( pStr );	
			m_StringList.AddTailEmpty( CFont::FS_10 );

			TCHAR sz[ 256 ];
			_tcscpy_s( sz, 256, _LAN( "-설정하신 2차 비밀번호를 확인합니다." ) );
			ti.s_Color = NiColor( 247/255.0f, 211/255.0f, 153/255.0f );	
			ti.s_eFontSize = CFont::FS_10;
			g_pStringAnalysis->Cut( &m_StringList, 165, sz, &ti );			
			_tcscpy_s( sz, 256, _LAN( "-설정하신 2차 비밀번호를 오른쪽 오른쪽 키 패드를 이용하여 입력해 주시기 바랍니다." ) );
			g_pStringAnalysis->Cut( &m_StringList, 165, sz, &ti );
		}		
		break;
	default:
		assert_cs( false );
		break;
	}		

	if( m_eType == eSet )
	{
		m_pOk = AddButton( CsPoint( 272, 243 ), CsPoint( 62, 30 ), cButton::IMAGE_NOR_1, _LAN( "등록" ), cWindow::SD_Ba2 );
	}
	else
	{
		m_pOk = AddButton( CsPoint( 272, 243 ), CsPoint( 62, 30 ), cButton::IMAGE_NOR_1, _LAN( "확인" ), cWindow::SD_Ba2 );
	}
	m_pDelete = AddButton( CsPoint( 195, 243 ), CsPoint( 35, 30 ), CsPoint( 35, 30 ), "SecondPass\\Back.tga", cWindow::SD_Ba1 );
	m_pClear = AddButton( CsPoint( 233, 243 ), CsPoint( 35, 30 ), CsPoint( 35, 30 ), "SecondPass\\AllBack.tga", cWindow::SD_Ba1 );
	m_pCancel = AddButton( CsPoint( 335, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );

	memset( m_szPassword, 0, sizeof( m_szPassword ) );
}

void cSecondPassword::Render()
{
	RenderScript();

	itLIST it = m_BtnInfo.begin();
	itLIST itEnd = m_BtnInfo.end();	
	int nIdx = 0;
	for( ; it != itEnd ; ++it, ++nIdx )
	{
		CsPoint vPos( 206 + (nIdx % 3) * 40, 81 + (nIdx / 3 ) * 40 );		
		vPos += GetRootClient();		
		
		(*it)->s_Btn->SetPos( vPos );
		(*it)->s_Btn->Render();
		(*it)->s_Text->Render( vPos + CsPoint( 11, 7 ) );		
	}	

	if( _tcslen( m_szPassword ) > 0 )
		m_Str->Render( GetRootClient() + CsPoint( 90, 78 ) );

	m_Text->Render( GetRootClient() + CsPoint( 206, 60 ) );
	m_StringList.Render( GetRootClient() + CsPoint( 13, 78 ), 5 );

	EndRenderScript();
}

void cSecondPassword::Update()
{
	if( !m_bShow )
		return;
	
	if( IsInWindow( g_Cursor.GetPos() ) == TRUE )
	{
		itLIST it = m_BtnInfo.begin();
		itLIST itEnd = m_BtnInfo.end();		
		for( ; it != itEnd ; ++it )
		{
			if( (*it)->s_Btn->Update_ForMouse() == cButton::ACTION_CLICK )
			{	
				int nLen = _tcslen( m_szPassword );
				if( nLen < MAX_SECONDPASS - 1 )
				{
					if( (*it)->s_nData < 10 )
					{
						TCHAR szNum[ 4 ];
						_stprintf_s( szNum, 4, _T( "%d" ), (*it)->s_nData );
						_tcscat( m_szPassword, szNum );	

						_SetPassword();
					}
				}
			}
		}

		if( m_pCancel->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			SetNextGameStep( GAME_EXIT );
		}

		if( m_pDelete->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			int nLen = _tcslen( m_szPassword );	
			if( nLen > 0 )
			{	
				memset( &m_szPassword[ nLen - 1 ], 0, sizeof( TCHAR ) );				
				_SetPassword();	
			}			
		}

		if( m_pClear->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			memset( m_szPassword, 0, sizeof( m_szPassword ) );
			_SetPassword();
		}			

		if( m_pOk->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			char sz2ndPass[ MAX_SECONDPASS + 1 ] = {0, };
			W2M( sz2ndPass, m_szPassword, MAX_SECONDPASS );

			bool bCheck = true;
			int nLen = strlen(sz2ndPass);			
			if( !(nLen>=4 && nLen<=8) )
				bCheck = false;

			int i = 0;
			for( i = 1; i < nLen; i++ )
			{
				if( sz2ndPass[ 0 ] != sz2ndPass[ i ] ) 
					break;
				else
					if( i == nLen - 1 )
						bCheck = false;
			}				

			if( bCheck )
			{
				SetEnableWindow( false );

				switch( m_eType )
				{
				case eSet: //설정일때 
					{
						switch( m_eCaseType )
						{
						case ePassRegist:
							{	
								cPrintMsg::PrintMsg( 20060 );
								Close();
								strcpy_s( m_szCheckPassword, MAX_SECONDPASS, sz2ndPass );
								Open( eSet, ePassRegistCheck );
								return;																	
							}						
							break;					
						case ePassRegistCheck:
							{
								if( stricmp( m_szCheckPassword, sz2ndPass ) == 0 )
								{
									if( net::account->SendRegister2ndPass( sz2ndPass ) )
									{										
										SetPassCheck( true );
										Close();
										return;
									}
									else
									{
										cPrintMsg::PrintMsg( 20054 );
									}
								}								
								else
								{
									// 비밀번호 일치 하지 않음..
									cPrintMsg::PrintMsg( 20052 );
								}																
							}
							break;
						case ePassChange:
							{
								//if( stricmp( m_szCheckPassword, sz2ndPass ) == 0 )
								//{
								//	if( net::gate->SendChange2ndPass( m_szPrePassword, sz2ndPass ) == false )
								//	{
								//		cPrintMsg::PrintMsg( 20054 );
								//	}	
								//}
								//else
								//{
								//	// 비밀번호 일치 하지 않음..
								//	cPrintMsg::PrintMsg( 20052 );
								//}								
								return;
							}
							break;	
						case ePassChangeCheck:
							{
								cPrintMsg::PrintMsg( 20060 );
								Close();	
								strcpy_s( m_szCheckPassword, MAX_SECONDPASS, sz2ndPass );
								Open( eSet, ePassChange );		
								return;
							}							
							break;
						default:
							assert_cs( false );
							break;
						}
					}
					break;
				case eQuestion: //확인일때 
					{
						switch( m_eCaseType )
						{								
						case ePassChange:
							{
								cPrintMsg::PrintMsg( 20059 );
								Close();
								strcpy_s( m_szPrePassword, MAX_SECONDPASS, sz2ndPass );
								Open( eSet, ePassChangeCheck );
								return;
							}							
							break;														
						case eNone:
							{	
								SendPassCertify();																
							}							
							break;							
						default:
							assert_cs( false );
							break;
						}	
					}
					break;
				default:
					assert_cs( false );
					break;
				}			
			}
			else
			{
				//비번 설정 규정에 맞지 않음
				cPrintMsg::PrintMsg( 20054 );
			}					
		}				
	}	
}

void cSecondPassword::Close()
{
	m_bShow = false;
	Delete();	
}

void cSecondPassword::SetPassCheck( bool bSuccess )
{
#ifdef USE_DMO_INI
	if( !bSuccess )
		cPrintMsg::PrintMsg( 20052 );
#else
	switch( m_eType )
	{
	case eSet:
		{
			if( !bSuccess )
				cPrintMsg::PrintMsg( 20052 );
			else
				cPrintMsg::PrintMsg( 20053 );
		}
		break;
	case eQuestion:
		{
			if( !bSuccess )
			{
				//확인 실패
				cPrintMsg::PrintMsg( 20052 );		
				memset( m_szPassword, 0, sizeof( m_szPassword ) );
				_SetPassword();
			}
			else
			{
				//확인 성공
				cPrintMsg::PrintMsg( 20055 );			
				Close();
			}				
		}
		break;
	default:
		if( !bSuccess )
			cPrintMsg::PrintMsg( 20052 );
		break;
	}
#endif
	SetEnableWindow( true );
}

void cSecondPassword::Open( ePassType eType, eCaseType eCase, int nValue )
{		
	SetEnableWindow( true );

	m_eType = eType;
	m_eCaseType = eCase;
	m_nValue = nValue;
	
	_Init();
	int nData[ 12 ] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ,11 };	
	for( int i = 0; i < 20; i++ )
	{
		swap( nData[ rand() % 12 ], nData[ rand() % 12 ] );
	}	

	int nIdx = 0;
	for( int i = 0; i < 12 ; i++, ++nIdx )
	{
		sPassword_Info* sInfo = NiNew sPassword_Info;

		//버튼 셋팅
		sInfo->s_Btn = NiNew cButton;
		sInfo->s_Btn->Init( NULL, CsPoint::ZERO, CsPoint( 35, 30 ), "System\\Btn_Nor7.tga", false );		
		sInfo->s_Btn->SetTexToken( CsPoint( 0, 30 ) );
		sInfo->s_Btn->SetSound( cWindow::SD_Ba1 );

		sInfo->s_nData = nData[ nIdx ];

		sInfo->s_Text = NiNew cText;
		cText::sTEXTINFO ti;		
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16, NiColor( 0.0f, 0.0f, 0.0f ) );					
		ti.s_bOutLine = false;
		if( sInfo->s_nData < 10 )
			ti.SetText( sInfo->s_nData );			
		else
			ti.SetText( L"*" );			
		sInfo->s_Text->Init( NULL, CsPoint::ZERO, &ti, false );

		m_BtnInfo.push_back( sInfo );
	}	

	m_bShow = true;
}

void cSecondPassword::_SetPassword()
{	
	SAFE_NIDELETE( m_Str );				
	m_Str = NiNew cText;	

	TCHAR szTemp[ MAX_SECONDPASS + 1 ];
	_tcscpy_s( szTemp, MAX_SECONDPASS, m_szPassword );

	int nLen = _tcslen( m_szPassword );
	if( nLen > 1 )
	{	
		_tcsncset(szTemp, L'*', nLen - 1);
	}	

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 140/255.0f, 1.0f, 1.0f ) );					
	ti.SetText( szTemp );	
	m_Str->Init( NULL, CsPoint::ZERO, &ti, false );	
}

void cSecondPassword::SendPassCertify()
{
	char sz2ndPass[ MAX_SECONDPASS + 1 ] = {0, };
	W2M( sz2ndPass, m_szPassword, MAX_SECONDPASS );
	net::account->SendSecondPassCertified( sz2ndPass );
}







