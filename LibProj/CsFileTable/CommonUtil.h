#pragma once
#include "stdafx.h"

namespace CmUtil
{
#define MAX_BUFFER 2048
	namespace CmBinRead
	{
		template< typename T >
		void Read_Value( FILE* fp, T* value )
		{
			fread( value, sizeof( T ), 1, fp );
		}

		template< typename T >
		void Read_Value( FILE* fp, std::basic_string< T > & value )
		{
			T wsName[ MAX_BUFFER ] = {0,};
			int nTextSize =0;
			fread( &nTextSize, sizeof(int), 1, fp );
			fread( wsName, sizeof(T), nTextSize, fp );
			value = wsName;
		}

		// iBuffLen은 char 를 sizeof로 계산한 값이 아닌 배열의 갯수를 넣어야 함.
		template< typename T >
		void Read_String( FILE* fp, T* value, int const& iBuffLen )
		{
			fread( value, sizeof( T ), iBuffLen, fp );
		}
	}

	namespace CmBinSave
	{
		template< typename T >
		void Save_Value( FILE* fp, T* value )
		{
			fwrite( value, sizeof( T ), 1, fp );
		}

		template< typename T >		
		void Save_Value( FILE* fp, std::basic_string< T > const& value )
		{
			int nTextSize = (int)value.size();
			fwrite( &nTextSize, sizeof(int), 1, fp );
			fwrite( value.c_str(), sizeof(T), nTextSize, fp );
		}

		// iBuffLen은 char를 sizeof로 계산한 값이 아닌 배열의 갯수를 넣어야 함.
		template< typename T >		
		void Save_String( FILE* fp, T* value, int const& iBuffLen )
		{
			fwrite( value, sizeof(T), iBuffLen, fp );	
		}
	}


	namespace CmPackRead
	{
		template< typename T >
		void Read_Value( int nHandle, T* value )
		{
			_read( nHandle, value, sizeof(T) );
		}

		template< typename T >
		void Read_Value( int nHandle, std::basic_string< T > & value )
		{
			T wsName[ MAX_BUFFER ] = {0,};
			int nTextSize =0;
			_read( nHandle, &nTextSize, sizeof(int) );
			_read( nHandle, wsName, sizeof(T) * nTextSize );
			value = wsName;
		}

		// iBuffLen은 char 를 sizeof로 계산한 값이 아닌 배열의 갯수를 넣어야 함.
		template< typename T >
		void Read_String( int nHandle, T* value, int const& iBuffLen )
		{
			_read( nHandle, value, sizeof(T) * iBuffLen );
		}
	}
}