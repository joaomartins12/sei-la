#pragma once

#ifndef DM_SYSTEM_TABLE_STRING_H
#define DM_SYSTEM_TABLE_STRING_H

namespace DmCS
{	
	namespace DmData
	{
		class CReader
		{
		public:
			CReader();
			~CReader();

		public:
			bool	SetData( dm_string_t conref wsData );
			void	SetData( dm_stringlist_t conref wsData );
			void	Close();
			bool	MoveFirst();
			bool	MoveNext();
			int		GetFieldCount() const;
			int		GetRowCount() const;

			//////////////////////////////////////////////////////////////////////////
			// Read Value
			//////////////////////////////////////////////////////////////////////////
			template<typename T>
			void	pop( T& value )
			{
				Read_Value( m_iCurFieldIdx, value );
				m_iCurFieldIdx++;
			}

			void	Read_Value( int conref nFieldIdx, bool& value ) const;
			void	Read_Value( int conref nFieldIdx, char& value ) const;
			void	Read_Value( int conref nFieldIdx, unsigned char& value ) const;
			void	Read_Value( int conref nFieldIdx, int& value ) const;
			void	Read_Value( int conref nFieldIdx, short& value ) const;
			void	Read_Value( int conref nFieldIdx, float& value ) const;
			void	Read_Value( int conref nFieldIdx, double& value ) const;
			void	Read_Value( int conref nFieldIdx, __int64& value ) const;
			void	Read_Value( int conref nFieldIdx, unsigned __int64& value ) const;
			void	Read_Value( int conref nFieldIdx, long& value ) const;
			void	Read_Value( int conref nFieldIdx, unsigned long& value ) const;
			void	Read_Value( int conref nFieldIdx, dm_string& value ) const;
			void	Read_Value( int conref nFieldIdx, dm_string_t& value ) const;

			template<typename T>
			void	Read_Value( int conref nFieldIdx, T& value ) const
			{
				dm_string_t wsValue = GetData(nFieldIdx);		
				value = static_cast<T>(DmCS::StringFn::ToI( wsValue ));
			}			

		private:
			bool	GotoRow( int conref p_iRow );
			void	SetData(int conref nFieldIdx, dm_string_t conref AddData );
			dm_string_t	GetData( int conref iFieldIdx ) const;

		private:
			typedef std::map<int,dm_string_t>						FIELD_DATA_CON;
			typedef std::map<int,dm_string_t>::iterator				FIELD_DATA_CON_IT;
			typedef std::map<int,dm_string_t>::const_iterator		FIELD_DATA_CON_CIT;

			typedef std::map<int,FIELD_DATA_CON>					ROW_DATA_CON;
			typedef std::map<int,FIELD_DATA_CON>::iterator			ROW_DATA_CON_IT;
			typedef std::map<int,FIELD_DATA_CON>::const_iterator	ROW_DATA_CON_CIT;

		private:
			int						m_iCurRowIdx;
			int						m_iCurFieldIdx;
			dm_string_t				m_NewLineTag;
			wchar_t					m_FieldTag;
			ROW_DATA_CON			m_TotalData;
			FIELD_DATA_CON *		m_pCurrenRow;
		};

		//////////////////////////////////////////////////////////////////////////
		// Save Value
		//////////////////////////////////////////////////////////////////////////
		class CSaver
		{
		public:
			CSaver();
			~CSaver();

			void	Make_Row( int conref nRowIdx );
			void	Close();

			template<typename T>
			void	push( T conref value )
			{
				Save_Value( m_iCurFieldIdx, value );
				m_iCurFieldIdx++;
			}

			void	GetData( dm_string_t & wsData );
			void	GetData( dm_stringlist_t & wsData );

			void	Save_Value( int conref nFieldIdx, bool conref value );
			void	Save_Value( int conref nFieldIdx, char conref value );
			void	Save_Value( int conref nFieldIdx, unsigned char conref value );
			void	Save_Value( int conref nFieldIdx, int conref value );
			void	Save_Value( int conref nFieldIdx, short conref value );
			void	Save_Value( int conref nFieldIdx, float conref value );
			void	Save_Value( int conref nFieldIdx, double conref value );
			void	Save_Value( int conref nFieldIdx, __int64 conref value );
			void	Save_Value( int conref nFieldIdx, unsigned __int64 conref value );
			void	Save_Value( int conref nFieldIdx, long conref value );
			void	Save_Value( int conref nFieldIdx, unsigned long conref value );
			void	Save_Value( int conref nFieldIdx, dm_string conref value );
			void	Save_Value( int conref nFieldIdx, dm_string_t conref value );

		private:
			void	SetData(int conref nFieldIdx, dm_string_t conref AddData );

		private:
			typedef std::map<int,dm_string_t>							FIELD_DATA_CON;
			typedef std::map<int,dm_string_t>::iterator					FIELD_DATA_CON_IT;
			typedef std::map<int,dm_string_t>::const_iterator			FIELD_DATA_CON_CIT;

			typedef std::map<int,FIELD_DATA_CON>						ROW_DATA_CON;
			typedef std::map<int,FIELD_DATA_CON>::iterator				ROW_DATA_CON_IT;
			typedef std::map<int,FIELD_DATA_CON>::const_iterator		ROW_DATA_CON_CIT;

		private:
			dm_string_t				m_NewLineTag;
			wchar_t					m_FieldTag;
			int						m_iCurRowIdx;
			int						m_iCurFieldIdx;
			ROW_DATA_CON			m_TotalData;
			FIELD_DATA_CON *		m_pCurrenRow;
		};
	};
};

#endif//DM_SYSTEM_TABLE_STRING_H