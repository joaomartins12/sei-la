// ExcelToText.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <atlstr.h>
#include <list>
#include <sstream>

// 로드할 데이터의 정보
struct sLoadData
{
	sLoadData():nSkipRow(0) {};
	~sLoadData() {};

	int		nSkipRow;
	CString sLoadfile;
	CString sLoadSheet;
	CString sSaveFile;

	bool	IsValidData() const
	{
		if( sLoadfile.IsEmpty() )
			return false;

		if( sLoadSheet.IsEmpty() )
			return false;

		if( sSaveFile.IsEmpty() )
			return false;
		return true;
	}

	void	MakeReadFieldIdx( TCHAR* pPointer )
	{
		if( NULL == pPointer )
			return;

		CString value = pPointer;
		int n = 0;
		while(1)
		{
			CString tok = value.Tokenize( _T(","), n);			
			if( tok.IsEmpty() )
				break;

			int nValue = _ttoi(tok);
			vecReadFieldIdx.push_back( nValue );
		}		

		vecReadFieldIdx.sort();
	}

	std::list<int>	vecReadFieldIdx;
};

#define STREAM_MAX_SIZE 1024
void ReadExcelToTextConfigFile(std::list<sLoadData>& saveData, TCHAR* pFile )
{
	std::ifstream openFile(pFile);
	if( !openFile.is_open() )
		return;

	std::list< CString > commandLines;
	std::string line;
 	while( std::getline( openFile, line ) )
	{
		CString sLine = line.c_str();
		int nPos = sLine.Find(_T(";"));// 주석 처리 읽지 않음
		if( -1 != nPos  )
			sLine = sLine.Left( nPos );

		sLine.TrimLeft();
		sLine.TrimRight();

		if( sLine.IsEmpty() )
			continue;

 		commandLines.push_back( sLine );
	}

	openFile.close();

	std::list< CString >::const_iterator it = commandLines.begin();
	for( ; it != commandLines.end(); ++it )
	{
		sLoadData addData;
		int pos = 0;
		for( int n = 0; n < 5; ++n )
		{
			CString tok = (*it).Tokenize( _T(" "), pos);
			if( -1 == pos )
				break;

			switch( n )
			{
			case 0:	addData.nSkipRow = _ttoi(tok);		break;
			case 1:	addData.sLoadfile = tok;			break;
			case 2:	addData.sLoadSheet = tok;			break;
			case 3:	addData.sSaveFile = tok;			break;
			case 4: addData.MakeReadFieldIdx( tok.GetBuffer() );	break;
			}
		}
		saveData.push_back( addData );
	}
}

bool IsExistFile( TCHAR const * file )
{
	if( _waccess_s( file, 0 ) == 0 )
		return true;
	return false;
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::list<sLoadData>	datas;
	if( argv[1] && argv[2] && argv[3] && argv[4] )
	{
		sLoadData loadData;
		loadData.nSkipRow = _ttoi(argv[1]);
		loadData.sLoadfile = argv[2];
		loadData.sLoadSheet = argv[3];
		loadData.sSaveFile = argv[4];
		loadData.MakeReadFieldIdx( argv[5] );
		
		datas.push_back( loadData );
	}
	else if( argv[1] )
	{
		ReadExcelToTextConfigFile( datas, argv[1] );
	}
	else
		ReadExcelToTextConfigFile( datas, _T("config.txt") );

	if( datas.empty() )
	{
		_tprintf( _T("Error : There is no data to convert.\n"));
		return 0;
	}

	DmCS::Storage::BAdoAutoInit autoinit;	
	std::list<sLoadData>::iterator it = datas.begin();
	for( int n = 0; it != datas.end(); ++it, ++n )
	{
		_tprintf( _T("----------------------------------------------------------------------\n"));

		if( !(*it).IsValidData() )
		{
			_tprintf( _T("[%d] Error : Is Data Not Valid\n"), n);
			continue;
		}

		if( !IsExistFile( (*it).sLoadfile.GetBuffer() ) )
		{
			_tprintf( _T("[%d] Error : File Is Not Access(%s)\n"), n, (*it).sLoadfile.GetBuffer());
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		// Excel File Open
		//////////////////////////////////////////////////////////////////////////
		
		_tprintf( _T("[%d] File Open : %s, %s ... "), n, (*it).sLoadfile.GetBuffer(), (*it).sLoadSheet.GetBuffer() );
		DmCS::Storage::Excel::Read excel;
		if( !excel.Open( (*it).sLoadfile.GetBuffer(), (*it).sLoadSheet.GetBuffer() ) )
		{
			_tprintf( _T("False\n") );
			continue;
		}
		_tprintf( _T("Done\n") );
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Excel File Data Read
		//////////////////////////////////////////////////////////////////////////
		excel.SetSkipLine( (*it).nSkipRow );
		excel.SetFieldIdx( (*it).vecReadFieldIdx );

		_tprintf( _T("[%d] File Load : "), n );
		dm_stringlist_t readData;
		bool bSuccess = excel.Exec( readData );
		if( !bSuccess )
		{
			_tprintf( _T("False\n") );
			continue;
		}	
		excel.Close();
		_tprintf( _T("Done\n") );
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Binary Data Save
		//////////////////////////////////////////////////////////////////////////
 		DmCS::Storage::File file;
		std::wstring saveFile = (*it).sSaveFile.GetBuffer();
		std::wstring tmp;
		std::wstring fileexe;
		DmCS::FileFn::FindFileNameExt( saveFile.c_str(), tmp, fileexe);
		
		bool bIsBinary = true;
		tmp = L"wb";
		if( 0 == fileexe.compare( L".txt" ) )
		{
			tmp = L"wt, ccs=UNICODE";
			bIsBinary = false;
		}

		_tprintf( _T("[%d] File Create : %s ..."), n, (*it).sSaveFile.GetBuffer() );
		if( !file.Create( saveFile.c_str(), tmp.c_str() ) )
		{
			_tprintf( _T("False\n") );
			continue;
		}

		_tprintf( _T("Done\n") );

		_tprintf( _T("[%d] File Save : %s ... "), n, (*it).sSaveFile.GetBuffer() );
		if( bIsBinary )
		{
			//////////////////////////////////////////////////////////////////////////
			// Read Data To Binary
			//////////////////////////////////////////////////////////////////////////
			dm_bytevec data;
			dm_size_t size = 0;
			data.resize( DmCS::Measure::exec( readData ) );
			DmCS::Write::exec( readData, data, size );

			bSuccess = DmCS::CompressZip::Compress( data, data );
			if( !bSuccess )
			{
				file.Close();
				_tprintf( _T("Compress False\n") );
				continue;
			}

			bSuccess = DmFS::SecuritySelf::Encript( 0xFF, data, data );
			if( !bSuccess )
			{
				file.Close();
				_tprintf( _T("Encript False\n") );
				continue;
			}

			if( !file.Write( data ) )	_tprintf( _T("False\n") );
			else						_tprintf( _T("Done\n")  );
		}
		else
		{
			dm_stringlist_t::const_iterator it = readData.begin();
			for( ; it != readData.end(); ++it )
			{
				file.Write( *it );
				file.Write( std::wstring(L"\n") );
			}
			_tprintf( _T("Done\n")  );
		}

		file.Close();

		//////////////////////////////////////////////////////////////////////////

	}

	return 0;
}