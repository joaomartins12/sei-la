

#include "stdafx.h"

//
//void Check( char* cExcel )
//{	
//	char cBuffer[ CEXCLE_CELL_LEN ];
//
//
//	struct sEXCEL
//	{
//		DWORD	s_ID;
//		int		s_Protocol;
//		DWORD	s_dwSequence;
//		DWORD	s_dwTamerIDX;
//		int		s_nMoney;
//		int		s_nNpc;
//		DWORD	s_dwTime;
//		int		s_Mac1;
//		int		s_Mac2;
//
//		DWORD	s_TotalMoney;
//	};
//
//	struct sBAD_TAMER
//	{
//		sEXCEL*					s_pSelf;
//		std::list< sEXCEL* >	s_list;
//	};
//
//	struct sSAME_SEQ
//	{
//		sEXCEL*		s_pFirst;
//		sEXCEL*		s_pSecond;
//	};
//
//
//	// 맵 엑셀 저장
//	std::map< DWORD, sEXCEL* > mapExcel;
//	std::map< DWORD, sEXCEL* > mapFind;
//	std::map< DWORD, sBAD_TAMER* > mapBadTamer;
//	std::map< DWORD, sBAD_TAMER* > mapRecvTamer;
//	std::map< DWORD, sSAME_SEQ* > mapSameSeq;
//	std::map< DWORD, sSAME_SEQ* > mapSameSeq2;
//	std::map< DWORD, sSAME_SEQ* > mapSameSeq3;
//	{
//		cExcelReader excel;
//		bool bSuccess = excel.Open( cExcel, "Sheet1" );		
//
//		// 2줄은 버린다.
//		bSuccess = excel.MoveFirst();
//		assert_cs( bSuccess == true );	
//		bSuccess = excel.MoveNext();
//		assert_cs( bSuccess == true );		
//
//		sEXCEL excelInfo;
//		while( excel.MoveNext() == true )
//		{
//			excelInfo.s_ID = atol( excel.GetData( 0, cBuffer ) );
//			if( excelInfo.s_ID == 0 )
//				continue;
//
//			excelInfo.s_Protocol = atoi( excel.GetData( 1, cBuffer ) );
//			excelInfo.s_dwSequence = atol( excel.GetData( 2, cBuffer ) );
//			excelInfo.s_dwTamerIDX = atoi( excel.GetData( 4, cBuffer ) );
//			excelInfo.s_nMoney = atoi( excel.GetData( 9, cBuffer ) );
//			excelInfo.s_nNpc = atoi( excel.GetData( 15, cBuffer ) );
//			excelInfo.s_dwTime = atol( excel.GetData( 17, cBuffer ) );
//			excelInfo.s_Mac1 = atol( excel.GetData( 22, cBuffer ) );
//			excelInfo.s_Mac2 = atol( excel.GetData( 23, cBuffer ) );
//
//			assert_cs( mapExcel.find( excelInfo.s_ID ) == mapExcel.end() );
//			sEXCEL* pNew = new sEXCEL;
//			memcpy( pNew, &excelInfo, sizeof( sEXCEL ) );
//			mapExcel[ excelInfo.s_ID ] = pNew;
//		}
//		excel.Close();
//	}
//
//	// 파인드 저장
//	{
//		std::map< DWORD, sEXCEL* >::iterator itExcel = mapExcel.begin();
//		std::map< DWORD, sEXCEL* >::iterator itExcelEnd = mapExcel.end();
//		for( ; itExcel!=itExcelEnd; ++itExcel )
//		{
//			sEXCEL* excelInfo = itExcel->second;
//			if( excelInfo->s_nNpc != 0 )
//				continue;
//
//
//			bool bSameSequence = false;
//			std::map< DWORD, sEXCEL* >::iterator itExcel2 = mapExcel.begin();
//			std::map< DWORD, sEXCEL* >::iterator itExcelEnd2 = mapExcel.end();		
//			for( ; itExcel2!=itExcelEnd2; ++itExcel2 )
//			{
//				sEXCEL* excelInfo2 = itExcel2->second;
//				if( excelInfo->s_ID == excelInfo2->s_ID )
//					continue;				
//
//				if( excelInfo->s_dwSequence != excelInfo2->s_dwSequence )
//					continue;
//
//				if( excelInfo->s_nMoney == -excelInfo2->s_nMoney )
//				{
//					// 같은 짝
//					if( mapSameSeq.find( excelInfo->s_dwSequence ) != mapSameSeq.end() )
//					{
//						if( ( mapSameSeq[ excelInfo->s_dwSequence ]->s_pFirst != excelInfo )&&
//							( mapSameSeq[ excelInfo->s_dwSequence ]->s_pSecond != excelInfo )&&
//							( mapSameSeq[ excelInfo->s_dwSequence ]->s_pFirst != excelInfo2 )&&
//							( mapSameSeq[ excelInfo->s_dwSequence ]->s_pSecond != excelInfo2 ) )
//						{
//							if( mapSameSeq2.find( excelInfo->s_dwSequence ) != mapSameSeq2.end() )
//							{
//								if( ( mapSameSeq2[ excelInfo->s_dwSequence ]->s_pFirst != excelInfo )&&
//									( mapSameSeq2[ excelInfo->s_dwSequence ]->s_pSecond != excelInfo )&&
//									( mapSameSeq2[ excelInfo->s_dwSequence ]->s_pFirst != excelInfo2 )&&
//									( mapSameSeq2[ excelInfo->s_dwSequence ]->s_pSecond != excelInfo2 ) )
//								{
//									if( mapSameSeq3.find( excelInfo->s_dwSequence ) != mapSameSeq3.end() )
//									{
//										DWORD dwSeqID = excelInfo->s_dwSequence;										
//										assert_cs( (mapSameSeq3[ excelInfo->s_dwSequence ]->s_pFirst == excelInfo)||
//											(mapSameSeq3[ excelInfo->s_dwSequence ]->s_pSecond == excelInfo)||
//											(mapSameSeq3[ excelInfo->s_dwSequence ]->s_pFirst == excelInfo2)||
//											(mapSameSeq3[ excelInfo->s_dwSequence ]->s_pSecond == excelInfo2) );
//									}
//									else
//									{
//										sSAME_SEQ* pSS = new sSAME_SEQ;
//										pSS->s_pFirst = excelInfo;
//										pSS->s_pSecond = excelInfo2;
//										mapSameSeq3[ excelInfo->s_dwSequence ] = pSS;
//									}
//								}								
//							}
//							else
//							{
//								sSAME_SEQ* pSS = new sSAME_SEQ;
//								pSS->s_pFirst = excelInfo;
//								pSS->s_pSecond = excelInfo2;
//								mapSameSeq2[ excelInfo->s_dwSequence ] = pSS;
//							}
//						}
//					}
//					else
//					{
//						sSAME_SEQ* pSS = new sSAME_SEQ;
//						pSS->s_pFirst = excelInfo;
//						pSS->s_pSecond = excelInfo2;
//						mapSameSeq[ excelInfo->s_dwSequence ] = pSS;
//					}
//
//					bSameSequence = true;
//					break;
//				}
//			}
//
//			if( bSameSequence == true )
//				continue;
//
//			assert_cs( mapFind.find( excelInfo->s_ID ) == mapFind.end() );
//			mapFind[ excelInfo->s_ID ] = excelInfo;
//		}
//	}
//
//	// 배드 테이머 리스트화
//	{
//		std::map< DWORD, sEXCEL* >::iterator it = mapFind.begin();
//		std::map< DWORD, sEXCEL* >::iterator itEnd = mapFind.end();
//		for( ; it!=itEnd; ++it )
//		{
//			sEXCEL* pExcelInfo = it->second;
//
//			if( mapBadTamer.find( pExcelInfo->s_dwTamerIDX ) == mapBadTamer.end() )
//			{
//				sBAD_TAMER* pBadTamer = new sBAD_TAMER;
//				mapBadTamer[ pExcelInfo->s_dwTamerIDX ] = pBadTamer;
//			}			
//			mapBadTamer[ pExcelInfo->s_dwTamerIDX ]->s_pSelf = pExcelInfo;
//			mapBadTamer[ pExcelInfo->s_dwTamerIDX ]->s_list.push_back( pExcelInfo );			
//		}
//	}
//
//
//	// 배드 테이머 엑셀에 기입
//	{
//		YExcel::BasicExcel excel;
//		bool bLoad = excel.Load( cExcel );
//		assert_cs( bLoad == true );
//
//		YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( "Sheet2" );
//		assert_cs( pSheet != NULL );
//
//		pSheet->DeleteAllRows();
//		size_t siInputRow = 2;
//
//
//		pSheet->Cell( 1, 0 )->SetString( "TamerID" );
//		pSheet->Cell( 1, 1 )->SetString( "Tera" );
//		pSheet->Cell( 1, 2 )->SetString( "Mega" );
//		pSheet->Cell( 1, 3 )->SetString( "Bit" );
//		pSheet->Cell( 1, 4 )->SetString( "UID" );
//		pSheet->Cell( 1, 5 )->SetString( "Sequence" );
//		pSheet->Cell( 1, 6 )->SetString( "Protocol" );
//		pSheet->Cell( 1, 7 )->SetString( "Time" );
//		pSheet->Cell( 1, 8 )->SetString( "Mac1" );
//		pSheet->Cell( 1, 9 )->SetString( "Mac2" );
//
//		std::map< DWORD, sBAD_TAMER* >::iterator it = mapBadTamer.begin();
//		std::map< DWORD, sBAD_TAMER* >::iterator itEnd = mapBadTamer.end();
//		for( ; it!=itEnd; ++it )
//		{
//			sBAD_TAMER* bt = it->second;
//			pSheet->Cell( siInputRow, 0 )->SetInteger( bt->s_pSelf->s_dwTamerIDX );
//
//			std::list< sEXCEL* >::iterator itList = bt->s_list.begin();
//			std::list< sEXCEL* >::iterator itListEnd = bt->s_list.end();
//
//			int nTotalMoney = 0;
//			size_t nOldRow = siInputRow;
//			for( ; itList!=itListEnd; ++itList )
//			{
//				sEXCEL* pListExcelInfo = *itList;
//				int dwMoney = pListExcelInfo->s_nMoney;
//				nTotalMoney += dwMoney;
//				int Tera = dwMoney/1000000;
//				int Mega = dwMoney%1000000;
//				int Bit = Mega%1000;
//				Mega = Mega/1000;
//				if( Tera )
//					pSheet->Cell( siInputRow, 1 )->SetInteger( Tera );
//				if( ( Tera != 0 )||Mega )
//					pSheet->Cell( siInputRow, 2 )->SetInteger( Mega );
//				pSheet->Cell( siInputRow, 3 )->SetInteger( Bit );
//				pSheet->Cell( siInputRow, 4 )->SetInteger( pListExcelInfo->s_ID );
//				pSheet->Cell( siInputRow, 5 )->SetInteger( pListExcelInfo->s_dwSequence );
//				pSheet->Cell( siInputRow, 6 )->SetInteger( pListExcelInfo->s_Protocol );
//				pSheet->Cell( siInputRow, 7 )->SetInteger( pListExcelInfo->s_dwTime );				
//				pSheet->Cell( siInputRow, 8 )->SetInteger( pListExcelInfo->s_Mac1 );
//				pSheet->Cell( siInputRow, 9 )->SetInteger( pListExcelInfo->s_Mac2 );
//
//				++siInputRow;
//			}
//
//			// 합계
//			pSheet->Cell( nOldRow, 11 )->SetString( "TotalMoney" );	
//			int Tera = nTotalMoney/1000000;
//			int Mega = nTotalMoney%1000000;
//			int Bit = Mega%1000;
//			Mega = Mega/1000;
//			if( Tera )
//				pSheet->Cell( nOldRow, 12 )->SetInteger( Tera );
//			if( ( Tera != 0 )||Mega )
//				pSheet->Cell( nOldRow, 13 )->SetInteger( Mega );
//			pSheet->Cell( nOldRow, 14 )->SetInteger( Bit );
//			++siInputRow;
//		}
//		/*excel.Save();
//		excel.Close();*/
//	}
//
//	// 배드 테이머가 거래건넘들
//	{
//		std::map< DWORD, sEXCEL* >::iterator itExcel = mapExcel.begin();
//		std::map< DWORD, sEXCEL* >::iterator itExcelEnd = mapExcel.end();
//		for( ; itExcel!=itExcelEnd; ++itExcel )
//		{
//			sEXCEL* pExcel = itExcel->second;
//
//			// 이미 기존에 파인드 된 항목 제외
//			if( mapFind.find( pExcel->s_ID ) != mapFind.end() )
//				continue;
//
//			// 엔피씨와의 거래 제외
//			if( pExcel->s_nNpc != 0 )
//				continue;
//
//			sEXCEL* pBadTamer = NULL;
//			sEXCEL* pBadTamer2 = NULL;
//			// 같은 시퀀스를 사용하였는가 ( 정상 거래 )
//			if( mapSameSeq.find( pExcel->s_dwSequence ) != mapSameSeq.end() )
//			{
//				if( mapSameSeq[ pExcel->s_dwSequence ]->s_pFirst == pExcel )
//				{
//					// 거래 대상중에 배드테이머가 존재 하는가
//					if( mapBadTamer.find( mapSameSeq[ pExcel->s_dwSequence ]->s_pFirst->s_dwTamerIDX ) != mapBadTamer.end() )
//					{
//						pBadTamer = mapSameSeq[ pExcel->s_dwSequence ]->s_pFirst;
//						pBadTamer2 = mapSameSeq[ pExcel->s_dwSequence ]->s_pSecond;
//					}
//					else if( mapBadTamer.find( mapSameSeq[ pExcel->s_dwSequence ]->s_pSecond->s_dwTamerIDX ) != mapBadTamer.end() )
//					{
//						pBadTamer = mapSameSeq[ pExcel->s_dwSequence ]->s_pSecond;
//						pBadTamer2 = mapSameSeq[ pExcel->s_dwSequence ]->s_pFirst;
//					}
//				}				
//			}
//
//			if( mapSameSeq2.find( pExcel->s_dwSequence ) != mapSameSeq2.end() )
//			{
//				if( mapSameSeq2[ pExcel->s_dwSequence ]->s_pFirst == pExcel )
//				{
//					// 거래 대상중에 배드테이머가 존재 하는가
//					if( mapBadTamer.find( mapSameSeq2[ pExcel->s_dwSequence ]->s_pFirst->s_dwTamerIDX ) != mapBadTamer.end() )
//					{
//						pBadTamer = mapSameSeq2[ pExcel->s_dwSequence ]->s_pFirst;
//						pBadTamer2 = mapSameSeq2[ pExcel->s_dwSequence ]->s_pSecond;
//					}
//					else if( mapBadTamer.find( mapSameSeq2[ pExcel->s_dwSequence ]->s_pSecond->s_dwTamerIDX ) != mapBadTamer.end() )
//					{
//						pBadTamer = mapSameSeq2[ pExcel->s_dwSequence ]->s_pSecond;
//						pBadTamer2 = mapSameSeq2[ pExcel->s_dwSequence ]->s_pFirst;
//					}
//				}
//			}
//
//			if( mapSameSeq3.find( pExcel->s_dwSequence ) != mapSameSeq3.end() )
//			{
//				if( mapSameSeq3[ pExcel->s_dwSequence ]->s_pFirst == pExcel )
//				{
//					// 거래 대상중에 배드테이머가 존재 하는가
//					if( mapBadTamer.find( mapSameSeq3[ pExcel->s_dwSequence ]->s_pFirst->s_dwTamerIDX ) != mapBadTamer.end() )
//					{
//						pBadTamer = mapSameSeq3[ pExcel->s_dwSequence ]->s_pFirst;
//						pBadTamer2 = mapSameSeq3[ pExcel->s_dwSequence ]->s_pSecond;
//					}
//					else if( mapBadTamer.find( mapSameSeq3[ pExcel->s_dwSequence ]->s_pSecond->s_dwTamerIDX ) != mapBadTamer.end() )
//					{
//						pBadTamer = mapSameSeq3[ pExcel->s_dwSequence ]->s_pSecond;
//						pBadTamer2 = mapSameSeq3[ pExcel->s_dwSequence ]->s_pFirst;
//					}
//				}
//			}
//
//
//			if( pBadTamer == NULL )
//				continue;
//
//			if( mapBadTamer.find( pBadTamer2->s_dwTamerIDX ) != mapBadTamer.end() )
//				continue;
//
//			if( pBadTamer2->s_nMoney < 0 )
//				continue;
//
//			if( mapRecvTamer.find( pBadTamer2->s_dwTamerIDX ) == mapRecvTamer.end() )
//			{
//				sBAD_TAMER* pRecvTamer = new sBAD_TAMER;
//				pRecvTamer->s_pSelf = pBadTamer2;				
//				pRecvTamer->s_pSelf->s_TotalMoney = pBadTamer2->s_nMoney;
//				mapRecvTamer[ pBadTamer2->s_dwTamerIDX ] = pRecvTamer;
//			}
//			else
//			{
//				mapRecvTamer[ pBadTamer2->s_dwTamerIDX ]->s_pSelf->s_TotalMoney += pBadTamer2->s_nMoney;
//			}
//
//
//			/*if( mapRecvTamer.find( pBadTamer->s_dwTamerIDX ) == mapRecvTamer.end() )
//			{
//			sBAD_TAMER* pRecvTamer = new sBAD_TAMER;
//			pRecvTamer->s_pSelf = pBadTamer;				
//			mapRecvTamer[ pBadTamer->s_dwTamerIDX ] = pRecvTamer;
//			}
//
//			if( pBadTamer2->s_nMoney < 0 )
//			continue;
//
//			bool bFind = false;
//			std::list< sEXCEL* >::iterator itList = mapRecvTamer[ pBadTamer->s_dwTamerIDX ]->s_list.begin();
//			std::list< sEXCEL* >::iterator itListEnd = mapRecvTamer[ pBadTamer->s_dwTamerIDX ]->s_list.end();
//			for( ; itList!=itListEnd; ++itList )
//			{
//			if( ( *itList )->s_dwTamerIDX == pBadTamer2->s_dwTamerIDX )
//			{
//			( *itList )->s_TotalMoney += pBadTamer2->s_nMoney;
//			bFind = true;
//			break;
//			}
//			}
//
//			if( bFind == false )
//			{
//			pBadTamer2->s_TotalMoney = pBadTamer2->s_nMoney;
//			mapRecvTamer[ pBadTamer->s_dwTamerIDX ]->s_list.push_back( pBadTamer2 );
//			}*/
//		}
//
//		std::map< DWORD, sBAD_TAMER* >::iterator it = mapRecvTamer.begin();
//		std::map< DWORD, sBAD_TAMER* >::iterator itDelete = it;
//		std::map< DWORD, sBAD_TAMER* >::iterator itEnd = mapRecvTamer.end();
//		while( it!=itEnd )
//		{
//			itDelete = it;
//			++it;
//			if( itDelete->second->s_pSelf->s_TotalMoney < 150000 )
//			{
//				mapRecvTamer.erase( itDelete );
//			}
//		}
//
//	}
//
//	// 배드테이머가 거래건넘들 적자
//	{
//		YExcel::BasicExcel excel;
//		bool bLoad = excel.Load( cExcel );
//		assert_cs( bLoad == true );
//
//		YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( "Sheet3" );
//		assert_cs( pSheet != NULL );
//
//		pSheet->DeleteAllRows();
//		size_t siInputRow = 2;
//
//		pSheet->Cell( 1, 0 )->SetString( "BadTamerID" );
//		pSheet->Cell( 1, 1 )->SetString( "IsBad" );
//		pSheet->Cell( 1, 2 )->SetString( "DestTamerID" );
//		pSheet->Cell( 1, 3 )->SetString( "IsBad" );
//		pSheet->Cell( 1, 4 )->SetString( "Tera" );
//		pSheet->Cell( 1, 5 )->SetString( "Mega" );
//		pSheet->Cell( 1, 6 )->SetString( "Bit" );
//		pSheet->Cell( 1, 7 )->SetString( "UID" );
//		pSheet->Cell( 1, 8 )->SetString( "Sequence" );
//		pSheet->Cell( 1, 9 )->SetString( "Protocol" );
//		pSheet->Cell( 1, 10 )->SetString( "Time" );
//		pSheet->Cell( 1, 11 )->SetString( "Mac1" );
//		pSheet->Cell( 1, 12 )->SetString( "Mac2" );
//
//		std::map< DWORD, sBAD_TAMER* >::iterator it = mapRecvTamer.begin();
//		std::map< DWORD, sBAD_TAMER* >::iterator itEnd = mapRecvTamer.end();
//		for( ; it!=itEnd; ++it )
//		{
//			sBAD_TAMER* bt = it->second;
//			pSheet->Cell( siInputRow, 0 )->SetInteger( bt->s_pSelf->s_dwTamerIDX );			
//
//			int dwMoney = bt->s_pSelf->s_TotalMoney;
//			int Tera = dwMoney/1000000;
//			int Mega = dwMoney%1000000;
//			int Bit = Mega%1000;
//			Mega = Mega/1000;
//			if( Tera )
//				pSheet->Cell( siInputRow, 2 )->SetInteger( Tera );
//			if( ( Tera != 0 )||Mega )
//				pSheet->Cell( siInputRow, 3 )->SetInteger( Mega );
//			pSheet->Cell( siInputRow, 4 )->SetInteger( Bit );
//			++siInputRow;
//
//
//			//if( mapBadTamer.find( bt->s_pSelf->s_dwTamerIDX ) == mapBadTamer.end() )
//			//	pSheet->Cell( siInputRow, 1 )->SetString( "X" );
//			//else
//			//	pSheet->Cell( siInputRow, 1 )->SetString( "O" );
//
//			//std::list< sEXCEL* >::iterator itList = bt->s_list.begin();
//			//std::list< sEXCEL* >::iterator itListEnd = bt->s_list.end();
//
//			//int nTotalMoney = 0;
//			//size_t nOldRow = siInputRow;
//			//for( ; itList!=itListEnd; ++itList )
//			//{
//			//	sEXCEL* pListExcelInfo = *itList;
//			//	/*pSheet->Cell( siInputRow, 2 )->SetInteger( pListExcelInfo->s_dwTamerIDX );
//			//	if( mapBadTamer.find( pListExcelInfo->s_dwTamerIDX ) == mapBadTamer.end() )
//			//		pSheet->Cell( siInputRow, 3 )->SetString( "X" );
//			//	else
//			//		pSheet->Cell( siInputRow, 3 )->SetString( "O" );*/
//
//
//			//	int dwMoney = pListExcelInfo->s_TotalMoney;
//			//	nTotalMoney += dwMoney;
//			//	int Tera = dwMoney/1000000;
//			//	int Mega = dwMoney%1000000;
//			//	int Bit = Mega%1000;
//			//	Mega = Mega/1000;
//			//	if( Tera )
//			//		pSheet->Cell( siInputRow, 4 )->SetInteger( Tera );
//			//	if( ( Tera != 0 )||Mega )
//			//		pSheet->Cell( siInputRow, 5 )->SetInteger( Mega );
//			//	pSheet->Cell( siInputRow, 6 )->SetInteger( Bit );
//			//	pSheet->Cell( siInputRow, 7 )->SetInteger( pListExcelInfo->s_ID );
//			//	pSheet->Cell( siInputRow, 8 )->SetInteger( pListExcelInfo->s_dwSequence );
//			//	pSheet->Cell( siInputRow, 9 )->SetInteger( pListExcelInfo->s_Protocol );
//			//	pSheet->Cell( siInputRow, 10 )->SetInteger( pListExcelInfo->s_dwTime );				
//			//	pSheet->Cell( siInputRow, 11 )->SetInteger( pListExcelInfo->s_Mac1 );
//			//	pSheet->Cell( siInputRow, 12 )->SetInteger( pListExcelInfo->s_Mac2 );
//
//			//	++siInputRow;
//			//}
//
//			//// 합계
//			//pSheet->Cell( nOldRow, 12 )->SetString( "TotalMoney" );	
//			//int Tera = nTotalMoney/1000000;
//			//int Mega = nTotalMoney%1000000;
//			//int Bit = Mega%1000;
//			//Mega = Mega/1000;
//			//if( Tera )
//			//	pSheet->Cell( nOldRow, 13 )->SetInteger( Tera );
//			//if( ( Tera != 0 )||Mega )
//			//	pSheet->Cell( nOldRow, 14 )->SetInteger( Mega );
//			//pSheet->Cell( nOldRow, 15 )->SetInteger( Bit );
//			//++siInputRow;
//		}
//		excel.Save();
//		excel.Close();
//
//	}
//
//	{
//		std::map< DWORD, sEXCEL* >::iterator itExcel = mapExcel.begin();
//		std::map< DWORD, sEXCEL* >::iterator itExcelEnd = mapExcel.end();
//		for( ;itExcel!=itExcelEnd; ++itExcel )
//		{
//			delete itExcel->second;
//		}
//	}
//
//	{
//		std::map< DWORD, sBAD_TAMER* >::iterator it = mapBadTamer.begin();
//		std::map< DWORD, sBAD_TAMER* >::iterator itEnd = mapBadTamer.end();
//		for( ; it!=itEnd; ++it )
//		{
//			delete it->second;
//		}
//	}
//
//	{
//		std::map< DWORD, sBAD_TAMER* >::iterator it = mapRecvTamer.begin();
//		std::map< DWORD, sBAD_TAMER* >::iterator itEnd = mapRecvTamer.end();
//		for( ; it!=itEnd; ++it )
//		{
//			delete it->second;
//		}
//	}
//
//	{
//		std::map< DWORD, sSAME_SEQ* >::iterator it = mapSameSeq.begin();
//		std::map< DWORD, sSAME_SEQ* >::iterator itEnd = mapSameSeq.end();
//		for( ; it!=itEnd; ++it )
//		{
//			delete it->second;
//		}
//	}
//
//	{
//		std::map< DWORD, sSAME_SEQ* >::iterator it = mapSameSeq2.begin();
//		std::map< DWORD, sSAME_SEQ* >::iterator itEnd = mapSameSeq2.end();
//		for( ; it!=itEnd; ++it )
//		{
//			delete it->second;
//		}
//	}
//
//	{
//		std::map< DWORD, sSAME_SEQ* >::iterator it = mapSameSeq3.begin();
//		std::map< DWORD, sSAME_SEQ* >::iterator itEnd = mapSameSeq3.end();
//		for( ; it!=itEnd; ++it )
//		{
//			delete it->second;
//		}
//	}
//}