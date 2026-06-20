

#pragma once 

#undef null
#import "..\..\..\msado15.dll" no_namespace rename("EOF", "adoEOF")

#define null 0

#define CEXCLE_CELL_LEN		2048

class cExcelReader
{
public:
	cExcelReader(void);
   ~cExcelReader(void);

public:
	bool Open(char const*fname, char const*sheet);
	void Close(void);

	bool MoveFirst(void);
	bool MoveNext(void);

	char* GetData(long col, char *buf);
	wchar_t* GetData(long col, wchar_t *buf);

	UINT GetRowCount(void) { return m_nRowCount; }
	UINT GetColCount(void) { return m_nColCount; }

	bool FirstOfSkip(long const& skipCount);
	bool RowSkip(long const& skipCount);


	void Read_Value( long col, float& value );
	void Read_Value( long col, double& value );
	void Read_Value( long col, __int64& value );
	void Read_Value( long col, unsigned __int64& value );
	void Read_Value( long col, long& value );
	void Read_Value( long col, unsigned long& value );

	template< typename T >
	void Read_Value( long col, T& value )
	{
		char cBuffer[CEXCLE_CELL_LEN]={0,};
		GetData( col , cBuffer );
		value = static_cast<T>(atoi( cBuffer ));
	}

	template< typename T >
	void Read_Value( long col, std::basic_string< T > & value )
	{
		T cBuffer[CEXCLE_CELL_LEN]={0,};
		GetData( col, cBuffer );
		value = cBuffer;
	}

	// iBuffLen은 char 를 sizeof로 계산한 값이 아닌 배열의 갯수를 넣어야 함.
	template< typename T >
	void Read_String( long col, T* value, int const& iBuffLen )
	{
		if( NULL == value )
			return;

		T cBuffer[CEXCLE_CELL_LEN]={0,};
		GetData( col, cBuffer );
		for( int n = 0; n < iBuffLen; ++n )
		{
			if( cBuffer[n] == '\0' )
			{
				value[n] = '\0';
				break;
			}
			else
				value[n] = cBuffer[n];
		}
		value[iBuffLen - 1] = '\0';
	}

///////////////////////////////////////////////////
// 내부 처리 데이타
private:
	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;

private:
	UINT m_nRowCount;
	UINT m_nColCount;
};




