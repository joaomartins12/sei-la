
#include "StdAfx.h"
#include "cExcel.h"


#include <atlstr.h>

#include <wchar.h>
#pragma warning( disable : 4101 )

static int s_nRunCount = 0;



cExcelReader::cExcelReader(void) : 
m_pConnection(null),
m_pRecordset(null)
{
	if(s_nRunCount == 0)
	{
		CoInitialize(null);
	}
	++ s_nRunCount;
}


cExcelReader::~cExcelReader(void)
{
	-- s_nRunCount;
	if(s_nRunCount == 0)
	{
		CoUninitialize();
	}

	Close();
}

// ACE 사용할 때
//#define d_szConnection	"Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Extended Properties=\"Excel 12.0;HDR=NO;IMEX=1\";"
#define d_szConnection	"Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Extended Properties=\"Excel 12.0 Macro;HDR=NO;TypeGuessRows=0;IMEX=1\";"
// Jet 사용할 때
//#define d_szConnection	"PROVIDER=Microsoft.Jet.OLEDB.4.0;DATA SOURCE=%s;EXTENDED PROPERTIES=\"Excel 8.0;HDR=NO;IMEX=1\";"
#define d_szQuery		"SELECT * FROM [%s$]"	 // Sheet 번호도 가능

// HDR=YES 첫번째 행 데이터를 컬럼헤더로 사용할 것인지 설정함 YES = 첫행을 읽지 않는다. NO = 첫행부터 읽는다.
// IMEX=1 컬럼을 문자타입 또는 숫자타입을 혼합해서 쓸것긴지 설정함 1 : 혼합사용, 0 : 혼합사용 안함.
bool cExcelReader::Open(char const*fname, char const*sheet)
{
	Close();

	char str_conn[256];
	char str_query[256];

	sprintf_s(str_conn, 256, d_szConnection, fname);
	sprintf_s(str_query, 256, d_szQuery, sheet);


	try
	{
		m_pConnection.CreateInstance( __uuidof(Connection) );
		m_pConnection->Open((_bstr_t)str_conn, _T( "" ), _T( "" ), null);
	}
	catch(_com_error &e)
	{        
		m_pConnection = null;
		return false;
	}

	try
	{
		m_pRecordset.CreateInstance( __uuidof(Recordset) );
		m_pRecordset->Open
			(
			str_query,
			_variant_t((IDispatch*)m_pConnection), 
			adOpenKeyset, 
			adLockReadOnly, 
			adConnectUnspecified
			);
	}
	catch(_com_error &e)
	{        
		m_pRecordset = null;
		return false;
	}

	try
	{
		m_nRowCount = m_pRecordset->GetRecordCount();
		m_nColCount = m_pRecordset->GetFields()->GetCount();
	}
	catch(_com_error &e)
	{        
		m_pRecordset = null;
		return false;
	}

	return true;
}


void cExcelReader::Close(void)
{
	if(m_pRecordset)
	{
		m_pRecordset->Close();
		m_pRecordset = null;
	}

	if(m_pConnection)
	{
		m_pConnection->Close();
		m_pConnection = null;
	}
}

bool cExcelReader::FirstOfSkip(long const& skipCount)
{
	if( skipCount < 1 )
		return true;

	if( !MoveFirst() )
		return false;

	return RowSkip(skipCount-1);
}

bool cExcelReader::RowSkip(long const& skipCount)
{
	for( long n = 0; n < skipCount; ++n )
	{
		if( !MoveNext() )
			return false;
	}
	return true;
}

bool cExcelReader::MoveFirst(void)
{
	m_pRecordset->MoveFirst();
	return !m_pRecordset->GetadoEOF();
}

bool cExcelReader::MoveNext(void)
{
	if(m_pRecordset->GetadoEOF())
	{
		return false;
	}

	m_pRecordset->MoveNext();

	return !m_pRecordset->GetadoEOF();
}

char* cExcelReader::GetData(long col, char *buf)
{
	try
	{
		strcpy_s(buf, CEXCLE_CELL_LEN, (char*)(_bstr_t)m_pRecordset->Fields->GetItem(col)->GetValue());
	}
	catch(_com_error &e)
	{        
		buf[0] = 0;		
	}
	return buf;
}

wchar_t* cExcelReader::GetData(long col, wchar_t *buf)
{
	try
	{
		wcscpy_s(buf, CEXCLE_CELL_LEN, (_bstr_t)m_pRecordset->Fields->GetItem(col)->GetValue());
	}
	catch(_com_error &e)
	{        
		buf[0] = 0;		
	}
	return buf;
}


void cExcelReader::Read_Value( long col, float& value )
{
	char cBuffer[CEXCLE_CELL_LEN]={0,};
	GetData( col , cBuffer );
	value = static_cast<float>(atof( cBuffer ));
}

void cExcelReader::Read_Value( long col, double& value )
{
	char cBuffer[CEXCLE_CELL_LEN]={0,};
	GetData( col , cBuffer );
	value = atof( cBuffer );
}

void cExcelReader::Read_Value( long col, __int64& value )
{
	char cBuffer[CEXCLE_CELL_LEN]={0,};
	GetData( col , cBuffer );
	value = _atoi64( cBuffer );
}

void cExcelReader::Read_Value( long col, unsigned __int64& value )
{
	char cBuffer[CEXCLE_CELL_LEN]={0,};
	GetData( col , cBuffer );
	value = _strtoui64( cBuffer, NULL, 10 );
}

void cExcelReader::Read_Value( long col, long& value )
{
	char cBuffer[CEXCLE_CELL_LEN]={0,};
	GetData( col , cBuffer );
	value = atol( cBuffer );
}

void cExcelReader::Read_Value( long col, unsigned long& value )
{
	char cBuffer[CEXCLE_CELL_LEN]={0,};
	GetData( col , cBuffer );
	value = strtoul( cBuffer, NULL, 10 );
}

#pragma warning( default : 4101 )






