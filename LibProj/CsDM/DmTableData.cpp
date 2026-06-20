#include "stdafx.h"
#include "DmTableData.h"
#include <sstream>

namespace DmCS
{
	namespace DmData
	{
		CReader::CReader():m_pCurrenRow(NULL),m_iCurRowIdx(0), m_iCurFieldIdx(0),m_NewLineTag(L"¢Ó"),m_FieldTag(L'|')
		{
		}
	//---------------------------------------------------------------------------
		CReader::~CReader()
		{
			Close();
		}	
		//---------------------------------------------------------------------------
		bool CReader::SetData( dm_string_t conref wsData )
		{
			//UniCode BOM °Ë»ç
			wchar_t tmpBuf[1];
			tmpBuf[0] = wsData.at(0);
			if( tmpBuf[0] != (wchar_t)0xFEFF )
			{
				// Text File Not Unicode Save
				return false;
			}

			std::wstringstream wss(wsData.substr(1));

			dm_string_t msg;
			dm_stringlist_t result;
			while( std::getline( wss, msg, L'\n' ) )
				result.push_back( msg );

			SetData( result );
			return true;
		}

		void CReader::SetData(dm_stringlist_t conref wsData)
		{
			dm_stringlist_t::const_iterator it = wsData.begin();
			for( int n = 0; it != wsData.end(); ++it, ++n )
			{
				FIELD_DATA_CON FieldData;
				std::wstringstream wss2(*it);
				int nFieldCount = 0;
				dm_string_t msg;
				while( std::getline( wss2, msg, m_FieldTag ) )
				{
					DmCS::StringFn::ReplaceAll( msg, m_NewLineTag, L"\n" );
					FieldData.insert( std::make_pair( nFieldCount, msg ) );
					nFieldCount++;
				}

				m_TotalData.insert( std::make_pair( n, FieldData ) );
			}
		}


		//---------------------------------------------------------------------------
		void CReader::Close( )
		{
			m_iCurRowIdx = 0;
			m_iCurFieldIdx = 0;
			m_pCurrenRow = NULL;
			m_TotalData.clear();
		}

		bool CReader::MoveFirst()
		{
			return GotoRow( 0 );
		}

		bool CReader::MoveNext()
		{
			if( m_iCurRowIdx + 1 >= GetRowCount() )
				return false;
			m_iCurFieldIdx = 0;
			m_iCurRowIdx++;
			return GotoRow( m_iCurRowIdx );
		}

		int	CReader::GetFieldCount() const
		{ 
			if( NULL == m_pCurrenRow )
				return 0;
			return (int)m_pCurrenRow->size(); 
		}

		int	CReader::GetRowCount() const
		{ 
			return (int)m_TotalData.size();
		}

		bool CReader::GotoRow( int conref p_iRow )
		{
			ROW_DATA_CON_IT it = m_TotalData.find( p_iRow );
			if( it == m_TotalData.end() )
				return false;

			m_iCurRowIdx = p_iRow;
			m_pCurrenRow = &(it->second);

			return true;
		}

		dm_string_t CReader::GetData(int conref nFieldIdx) const
		{
			if( NULL == m_pCurrenRow )
				return L"";

			FIELD_DATA_CON_CIT it = m_pCurrenRow->find( nFieldIdx );
			if( it == m_pCurrenRow->end() )
				return L"";

			return it->second;
		}

		//---------------------------------------------------------------------------
		// Read Function
		//---------------------------------------------------------------------------
		void CReader::Read_Value( int conref nFieldIdx, bool& value ) const
		{	
			dm_string_t wsValue = GetData(nFieldIdx);	
			value = (DmCS::StringFn::ToI( wsValue ) > 0 )? true:false;
		}

		void CReader::Read_Value( int conref nFieldIdx, char& value ) const
		{
			dm_string_t wsValue = GetData(nFieldIdx);		
			value = static_cast<char>(DmCS::StringFn::ToI( wsValue ));
		}

		void CReader::Read_Value( int conref nFieldIdx, unsigned char& value ) const
		{
			dm_string_t wsValue = GetData(nFieldIdx);		
			value = static_cast<unsigned char>(DmCS::StringFn::ToI( wsValue ));
		}

		void CReader::Read_Value( int conref nFieldIdx, int& value ) const
		{
			dm_string_t wsValue = GetData(nFieldIdx);	
			value = DmCS::StringFn::ToI( wsValue );
		}

		void CReader::Read_Value( int conref nFieldIdx, short& value ) const
		{
			dm_string_t wsValue = GetData(nFieldIdx);	
			value = static_cast<short>(DmCS::StringFn::ToI( wsValue ));
		}

		void CReader::Read_Value( int conref nFieldIdx, float& value ) const
		{
			dm_string_t wsValue = GetData(nFieldIdx);		
			value = DmCS::StringFn::ToFloat( wsValue );
		}

		void CReader::Read_Value( int conref nFieldIdx, double& value ) const
		{
			dm_string_t wsValue = GetData(nFieldIdx);		
			value = DmCS::StringFn::ToDouble( wsValue );
		}

		void CReader::Read_Value( int conref nFieldIdx, __int64& value ) const
		{
			dm_string_t wsValue = GetData(nFieldIdx);	
			value = DmCS::StringFn::ToI64( wsValue );
		}

		void CReader::Read_Value( int conref nFieldIdx, unsigned __int64& value ) const
		{
			dm_string_t wsValue = GetData(nFieldIdx);	
			value = DmCS::StringFn::ToU64( wsValue );
		}

		void CReader::Read_Value( int conref nFieldIdx, long& value ) const
		{
			dm_string_t wsValue = GetData(nFieldIdx);
			value = wcstol(wsValue.c_str(), NULL, 10);
		}

		void CReader::Read_Value( int conref nFieldIdx, unsigned long& value ) const
		{
			dm_string_t wsValue = GetData(nFieldIdx);	
			value = wcstoul(wsValue.c_str(), NULL, 10);	
		}

		void CReader::Read_Value( int conref nFieldIdx, dm_string & value ) const
		{
			dm_string_t wsValue;
			Read_Value( nFieldIdx, wsValue );
			DmCS::StringFn::ToMB( wsValue, value );
		}

		void CReader::Read_Value( int conref nFieldIdx, dm_string_t& value ) const
		{
			value = GetData( nFieldIdx );
		}

		void CReader::SetData(int conref nFieldIdx, dm_string_t conref AddData )
		{
			if( NULL == m_pCurrenRow )
				return;

			FIELD_DATA_CON_IT it = m_pCurrenRow->find( nFieldIdx );
			if( it == m_pCurrenRow->end() )
				m_pCurrenRow->insert( std::make_pair( nFieldIdx, AddData ) );
			else
				it->second = AddData;
		}

		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Save Function
		//////////////////////////////////////////////////////////////////////////

		CSaver::CSaver():m_pCurrenRow(NULL),m_iCurRowIdx(0), m_iCurFieldIdx(0),m_NewLineTag(L"¢Ó"),m_FieldTag(L'|')
		{
		}

		CSaver::~CSaver()
		{
			Close();
		}

		void CSaver::Close( )
		{
			m_iCurRowIdx = 0;
			m_iCurFieldIdx = 0;
			m_pCurrenRow = NULL;
			m_TotalData.clear();
		}

		void CSaver::Make_Row(int conref nRowIdx)
		{
			ROW_DATA_CON_IT it = m_TotalData.find( nRowIdx );
			if( it == m_TotalData.end() )
			{
				auto rib = m_TotalData.insert( std::make_pair( nRowIdx, FIELD_DATA_CON() ) );
				it = rib.first;
			}	

			m_pCurrenRow = &(it->second);
			m_iCurFieldIdx = 0;
		}

		void CSaver::GetData(dm_string_t & wsData)
		{
// 			wchar_t tmpBuf[1];
// 			tmpBuf[0] = (wchar_t)0xFEFF;
// 			wsData += tmpBuf;
			wsData = (wchar_t)0xFEFF;
			ROW_DATA_CON_CIT it = m_TotalData.begin();
			for( ; it != m_TotalData.end(); ++it )
			{
				FIELD_DATA_CON_CIT subIT = it->second.begin();
				for( ; subIT != it->second.end(); ++subIT )
				{
					wsData += subIT->second;
					wsData += m_FieldTag;
				}
				wsData.append( m_NewLineTag );
			}
		}

		void CSaver::GetData(dm_stringlist_t & wsData)
		{
			ROW_DATA_CON_CIT it = m_TotalData.begin();
			for( ; it != m_TotalData.end(); ++it )
			{
				dm_string_t appendStr;
				FIELD_DATA_CON_CIT subIT = it->second.begin();
				for( ; subIT != it->second.end(); ++subIT )
				{
					appendStr = subIT->second;
					appendStr += m_FieldTag;
				}
				wsData.push_back(appendStr);
			}
		}

		void CSaver::Save_Value( int conref nFieldIdx, bool conref value )
		{
			std::wstring addData;
			DmCS::StringFn::From( addData, (int)value );
			SetData( nFieldIdx, addData );
		}

		void CSaver::Save_Value( int conref nFieldIdx, char conref value )
		{
			std::wstring addData;
			DmCS::StringFn::From( addData, value );
			SetData( nFieldIdx, addData );
		}

		void CSaver::Save_Value( int conref nFieldIdx, unsigned char conref value )
		{
			std::wstring addData;
			DmCS::StringFn::From( addData, value );
			SetData( nFieldIdx, addData );
		}

		void CSaver::Save_Value( int conref nFieldIdx, int conref value )
		{
			std::wstring addData;
			DmCS::StringFn::From( addData, value );
			SetData( nFieldIdx, addData );
		}

		void CSaver::Save_Value( int conref nFieldIdx, short conref value )
		{
			std::wstring addData;
			DmCS::StringFn::From( addData, value );
			SetData( nFieldIdx, addData );
		}

		void CSaver::Save_Value( int conref nFieldIdx, float conref value )
		{
			std::wstring addData;
			DmCS::StringFn::From( addData, value );
			SetData( nFieldIdx, addData );
		}

		void CSaver::Save_Value( int conref nFieldIdx, double conref value )
		{
			std::wstring addData;
			DmCS::StringFn::From( addData, value );
			SetData( nFieldIdx, addData );
		}

		void CSaver::Save_Value( int conref nFieldIdx, __int64 conref value )
		{
			std::wstring addData;
			DmCS::StringFn::From( addData, value );
			SetData( nFieldIdx, addData );
		}

		void CSaver::Save_Value( int conref nFieldIdx, unsigned __int64 conref value )
		{
			std::wstring addData;
			DmCS::StringFn::From( addData, value );
			SetData( nFieldIdx, addData );
		}

		void CSaver::Save_Value( int conref nFieldIdx, long conref value )
		{
			std::wstring addData;
			DmCS::StringFn::From( addData, value );
			SetData( nFieldIdx, addData );
		}

		void CSaver::Save_Value( int conref nFieldIdx, unsigned long conref value )
		{
			std::wstring addData;
			DmCS::StringFn::From( addData, value );
			SetData( nFieldIdx, addData );
		}

		void CSaver::Save_Value( int conref nFieldIdx, dm_string conref value )
		{
			std::wstring addData;
			DmCS::StringFn::FromUTF8( addData, value );
			SetData( nFieldIdx, addData );
		}

		void CSaver::Save_Value( int conref nFieldIdx, dm_string_t conref value )
		{
			SetData( nFieldIdx, value );
		}

		void CSaver::SetData(int conref nFieldIdx, dm_string_t conref AddData )
		{
			if( NULL == m_pCurrenRow )
				return;

			FIELD_DATA_CON_IT it = m_pCurrenRow->find( nFieldIdx );
			if( it == m_pCurrenRow->end() )
				m_pCurrenRow->insert( std::make_pair( nFieldIdx, AddData ) );
			else
				it->second = AddData;
		}
	};
};