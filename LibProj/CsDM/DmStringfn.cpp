#include "stdafx.h"
#include "DmStringfn.h"

#include <stdarg.h>
#include <math.h>
namespace DmCS
{
	namespace
	{
		static dm_sint_t const CONV_STR_BUFFER_LENGTH = 32;
		static float	 const DIV_CAPACITY_SIZE = 1024.f;

		DmDeclStrNameCustom( Spec_I,		"%d"	);
		DmDeclStrNameCustom( Spec_UI,		"%ud"	);
		DmDeclStrNameCustom( Spec_I64,		"%I64d"	);
		DmDeclStrNameCustom( Spec_U,		"%u"	);
		DmDeclStrNameCustom( Spec_U64,		"%I64u"	);
		DmDeclStrNameCustom( Spec_F,		"%f"	);
		DmDeclStrNameCustom( Spec_D,		"%lf"	);
		DmDeclStrNameCustom( Spec_Hex,		"%X"	);
		DmDeclStrNameCustom( Spec_True,		"true"	);
		DmDeclStrNameCustom( Spec_False,	"false"	);

		namespace Capacity
		{
			namespace Unit
			{
				enum eDefined
				{
					Byte,
					KB,
					MB,
					GB,
					TB,
					End
				};
			}

			static dm_str_t cptr const Spec_Unit[ Unit::End ] = { L"Byte", L"KB", L"MB", L"GB", L"TB" };
		}


		DmFnUnicodeToUTF8		SFUniToUTF8	= 0;
		DmFnUnicodeToMultiByte	SFUniToMB	= 0;
		DmFnUTF8ToUnicode		SFUTF8ToUni	= 0;
		DmFnMultiByteToUnicode	SFMBToUni	= 0;

		template< typename T >
		void FromTmpl(dm_string_t& stext, dm_str_t cptr szSpec, T conref Val)
		{
			dm_str_t szTemp[ CONV_STR_BUFFER_LENGTH ] = { 0, };
			::swprintf_s( szTemp, CONV_STR_BUFFER_LENGTH, szSpec, Val );
			stext = szTemp;
		}

		template< typename T >
		T ToTmpl(dm_string_t conref stext, dm_str_t cptr szSpec)
		{
			T Converted = 0;
			if( !stext.empty() )
				::swscanf_s( stext.c_str(), szSpec, &Converted );

			return Converted;
		}
	}

	namespace StringFn
	{
		void SetConvFn( DmFnUnicodeToUTF8 ToUTF8, DmFnUnicodeToMultiByte ToMB, 
						DmFnUTF8ToUnicode U8ToUni, DmFnMultiByteToUnicode MBToUni )
		{
			SFUniToUTF8	= ToUTF8;
			SFUniToMB	= ToMB;
			SFUTF8ToUni	= U8ToUni;
			SFMBToUni	= MBToUni;
		}

		void Lwr(dm_string_t& stext)
		{
			std::transform( stext.begin(), stext.end(), stext.begin(), tolower );
		}

		void Upr(dm_string_t& stext)
		{
			std::transform( stext.begin(), stext.end(), stext.begin(), toupper );
		}

		void Lwr(dm_string& stext)
		{
			std::transform( stext.begin(), stext.end(), stext.begin(), tolower );
		}

		void Upr(dm_string& stext)
		{
			std::transform( stext.begin(), stext.end(), stext.begin(), toupper );
		}
		
		bool iCmp(dm_string_t conref slhs, dm_string_t conref srhs)
		{
			struct
			{
				bool operator()(dm_str_t lhs, dm_str_t rhs)
				{
					if( toupper( lhs ) == toupper( rhs ) )
						return true;

					return false;
				}
			} Pred;

			if( slhs.size() != srhs.size() )
				return false;

			return std::equal( slhs.begin(), slhs.end(), srhs.begin(), Pred );
		}

		bool iCmp(dm_string conref slhs, dm_string conref srhs)
		{
			struct
			{
				bool operator()(char lhs, char rhs)
				{
					if( toupper( lhs ) == toupper( rhs ) )
						return true;

					return false;
				}
			} Pred;

			if( slhs.size() != srhs.size() )
				return false;

			return std::equal( slhs.begin(), slhs.end(), srhs.begin(), Pred );
		}


		bool iCmp( char cptr slhs, char cptr srhs)
		{
			if( NULL == slhs || NULL == srhs )
				return false;

			std::string lHs = slhs;
			std::string rHs = srhs;
			return iCmp( lHs, rHs );
		}

		void Replace(dm_string_t& stext, dm_string_t conref sOld, dm_string_t conref sNew)
		{
			dm_string_t::size_type pos = stext.find( sOld );
			if( dm_string_t::npos != pos )
				stext.replace( pos, sOld.length(), sNew );
		}

		void Replace(dm_string_t& stext, dm_string_t conref sOld, dm_string conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			Replace( stext, sOld, value );
		}

		void Replace(dm_string& stext, dm_string conref sOld, dm_string conref sNew)
		{
			dm_string::size_type pos = stext.find( sOld );
			if( dm_string::npos != pos )
				stext.replace( pos, sOld.length(), sNew );
		}

		void Replace(dm_string_t& stext, dm_string_t conref sOld, dm_sint_t conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			Replace( stext, sOld, value );
		}

		void Replace(dm_string_t& stext, dm_string_t conref sOld, dm_dword_t conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			Replace( stext, sOld, value );
		}

		void Replace(dm_string_t& stext, dm_string_t conref sOld, dm_sint64_t conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			ReplaceAll( stext, sOld, value );
		}

		void Replace(dm_string_t& stext, dm_string_t conref sOld, dm_qword_t conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			Replace( stext, sOld, value );
		}

		void Replace(dm_string_t& stext, dm_string_t conref sOld, bool conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			Replace( stext, sOld, value );
		}

		void Replace(dm_string_t& stext, dm_string_t conref sOld, float conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			Replace( stext, sOld, value );
		}

		void Replace(dm_string_t& stext, dm_string_t conref sOld, double conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			Replace( stext, sOld, value );
		}

		void Replace(dm_string_t& stext, dm_string_t conref sOld, wchar_t cptr sNew)
		{
			std::wstring value(sNew);
			Replace( stext, sOld, value );
		}

		void Replace(dm_string_t& stext, dm_string_t conref sOld, char cptr sNew)
		{
			dm_string_t value;
			From( value, dm_string(sNew) );			
			Replace( stext, sOld, value );
		}

		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, dm_string_t conref sNew)
		{
			dm_string_t::size_type pos = stext.find( sOld );
			while( dm_string_t::npos != pos )
			{
				stext.replace( pos, sOld.length(), sNew );
				++pos;
				pos = stext.find( sOld, pos );
			}
		}

		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, dm_string conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			ReplaceAll( stext, sOld, value );
		}

		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, dm_sint_t conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			ReplaceAll( stext, sOld, value );
		}

		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, dm_dword_t conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			ReplaceAll( stext, sOld, value );
		}

		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, dm_sint64_t conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			ReplaceAll( stext, sOld, value );
		}

		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, dm_qword_t conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			ReplaceAll( stext, sOld, value );
		}

		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, bool conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			ReplaceAll( stext, sOld, value );
		}

		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, float conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			ReplaceAll( stext, sOld, value );
		}

		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, double conref sNew)
		{
			dm_string_t value;
			From( value, sNew );
			ReplaceAll( stext, sOld, value );
		}

		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, wchar_t cptr sNew)
		{
			std::wstring value(sNew);
			ReplaceAll( stext, sOld, value );
		}

		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, char cptr sNew)
		{
			dm_string_t value;
			From( value, dm_string(sNew) );			
			ReplaceAll( stext, sOld, value );
		}

		void ReplaceAll(dm_string& stext, dm_string conref sOld, dm_string conref sNew)
		{
			dm_string::size_type pos = stext.find( sOld );
			while( dm_string::npos != pos )
			{
				stext.replace( pos, sOld.length(), sNew );
				++pos;
				pos = stext.find( sOld, pos );
			}
		}
	
		void RemoveAll(dm_string_t& stext, dm_str_t conref sRemove)
		{
			dm_string::size_type pos = stext.find( sRemove );
			while( dm_string::npos != pos )
			{
				stext.erase( pos, 1 );
				pos = stext.find( sRemove );
			}
		}

		void Cut(dm_string_t conref stext, dm_string_t conref sKeyword, dm_stringvec_t& stringvec)
		{
			stringvec.clear();
			dm_string_t sInput = stext;
			dm_string_t::size_type pos = sInput.find_first_of( sKeyword );
			while( dm_string_t::npos != pos )
			{
				stringvec.push_back( sInput.substr( 0, pos ) );
				sInput = sInput.substr( pos + sKeyword.size(), sInput.size() - pos );

				pos = sInput.find_first_of( sKeyword );
			}

			if( !sInput.empty() )
				stringvec.push_back( sInput );
		}

		void Cut(dm_string conref stext, dm_string conref sKeyword, dm_stringvec& stringvec)
		{
			stringvec.clear();
			dm_string sInput = stext;
			dm_string::size_type pos = sInput.find_first_of( sKeyword );
			while( dm_string::npos != pos )
			{
				stringvec.push_back( sInput.substr( 0, pos ) );
				sInput = sInput.substr( pos + sKeyword.size(), sInput.size() - pos );

				pos = sInput.find_first_of( sKeyword );
			}

			if( !sInput.empty() )
				stringvec.push_back( sInput );
		}


		void Add( dm_string_t& stext, dm_string_t conref sAddText, dm_string_t conref tokken )
		{
			if( !tokken.empty() )
				stext += tokken;
			stext += sAddText;
		}

		void Add( dm_string& stext, dm_string conref sAddText, dm_string conref tokken )
		{
			if( !tokken.empty() )
				stext += tokken;
			stext += sAddText;
		}

		void ToMB(dm_string_t conref stext, dm_string& mbs)
		{
			if (!SFUniToMB) {
#ifdef DEBUG_NETWORK
				DBG("Failed to convert string.\n");
#endif
				return;
			}

			SFUniToMB( stext, mbs );
		}

		void ToUTF8(dm_string_t conref stext, dm_string& u8s)
		{
			if( !SFUniToUTF8 )
				return;

			SFUniToUTF8( stext, u8s );
		}

		void From(dm_string_t& stext, dm_sint_t conref Val)
		{
			FromTmpl( stext, Spec_I, Val );
		}

		void From(dm_string_t& stext, dm_ulong_t conref Val)
		{
			FromTmpl( stext, Spec_UI, Val );
		}

		void From(dm_string_t& stext, dm_dword_t conref Val)
		{
			FromTmpl( stext, Spec_U, Val );
		}

		void From(dm_string_t& stext, dm_sint64_t conref Val)
		{
			FromTmpl( stext, Spec_I64, Val );
		}

		void From(dm_string_t& stext, dm_qword_t conref Val)
		{
			FromTmpl( stext, Spec_U64, Val );
		}

		void From(dm_string_t& stext, bool conref Val)
		{
			stext = ( Val )?( Spec_True ):( Spec_False );
		}

		void From(dm_string_t& stext, float conref Val)
		{
			FromTmpl( stext, Spec_F, Val );
		}

		void From(dm_string_t& stext, double conref Val)
		{
			FromTmpl( stext, Spec_D, Val );
		}

		void HexaFrom(dm_string_t& stext, dm_dword_t conref Val)
		{
			FromTmpl( stext, Spec_Hex, Val );
		}

		void From(dm_string_t& stext, dm_string conref mbs)
		{
	 
			if( !SFMBToUni )
				return;

			SFMBToUni( mbs, stext );
		}

		void From(dm_string_t& stext, dm_string::value_type cptr mbs)
		{

			if (!SFMBToUni)
				return;

			SFMBToUni(mbs, stext);
		}

		// 반올림
		/* x는 반올림하고자 하는 실수값 */
		/* digit는 반올림하고자 하는 소수점 자리수 */
		float FloatRounding( float x, int digit )
		{
			return ( (float)floor( (x) * (float)pow( float(10), digit ) + 0.5f ) / (float)pow( float(10), digit ) );
		}

		// 올림
		/* x는 반올림하고자 하는 실수값 */
		/* digit는 올림하고자 하는 소수점 자리수 */
		float FloatCeil(float x, int digit)
		{
			return ( (float)ceil( (x) * (float)pow( float(10), digit ) ) / (float)pow( float(10), digit ) );
		}

		// 내림
		/* x는 반올림하고자 하는 실수값 */
		/* digit는 내림하고자 하는 소수점 자리수 */
		float FloatFloor(float x, int digit)
		{
			return ( (float)floor( (x) * (float)pow( float(10), digit ) ) / (float)pow( float(10), digit ) );
		}

		void FloatToString( dm_string_t& stext, float conref Val, int const& dig )
		{
			dm_string_t sInput;
			FromTmpl( sInput, Spec_F, Val );
			dm_string::size_type pos = sInput.find_first_of( L"." );
			stext = sInput.substr( 0, pos + dig );
		}

		void FromUTF8(dm_string_t& stext, dm_string conref u8s)
		{
			if( !SFUTF8ToUni )
				return;

			SFUTF8ToUni( u8s, stext );
		}

		void FromCapacity(dm_string_t& stext, dm_qword_t conref Val)
		{
			dm_sint_t	iUnit	= Capacity::Unit::Byte;
			double		Conv	= static_cast< double >( Val );

			while( Conv >= DIV_CAPACITY_SIZE )
			{
				++iUnit;

				if( iUnit >= Capacity::Unit::End )
				{
					--iUnit;
					break;
				}

				Conv /= DIV_CAPACITY_SIZE;
			}

			Format( stext, L"%.2f%s", Conv, Capacity::Spec_Unit[ iUnit ] );
		}

		dm_sint_t const ToI(dm_string_t conref stext)
		{
			return ToTmpl< dm_sint_t >( stext, Spec_I );
		}

		dm_dword_t const ToU(dm_string_t conref stext)
		{
			return ToTmpl< dm_dword_t >( stext, Spec_U );
		}

		dm_sint64_t const ToI64(dm_string_t conref stext)
		{
			return ToTmpl< dm_sint64_t >( stext, Spec_I64 );
		}

		dm_qword_t const ToU64(dm_string_t conref stext)
		{
			return ToTmpl< dm_qword_t >( stext, Spec_U64 );
		}

		dm_dword_t const HexaTo(dm_string_t conref stext)
		{
			return ToTmpl< dm_dword_t >( stext, Spec_Hex );
		}

		bool const ToBool(dm_string_t conref stext)
		{
			return iCmp( stext, Spec_True );
		}

		float const ToFloat(dm_string_t conref stext)
		{
			return ToTmpl< float >( stext, Spec_F );
		}

		double const ToDouble(dm_string_t conref stext)
		{
			return ToTmpl< double >( stext, Spec_D );
		}

		void Format(dm_string_t& stext, dm_str_t cptr szformat, ...)
		{
			va_list		args;
			dm_sint_t	iLength = 0;

			dm_string_t stmp;
			stmp.resize( 1024 );

			va_start( args, szformat );
			iLength = ::vswprintf_s( &stmp.at( 0 ), 1024, szformat, args );
			va_end( args );

			stext = stmp.c_str();
		}

		void FormatA(dm_string& stext, dm_str cptr szformat, ...)
		{
			va_list		args;
			int			iLength = 0;

			dm_string stmp;
			stmp.resize( 1024 );

			va_start( args, szformat );
			iLength = ::vsprintf_s( &stmp.at( 0 ), 1024, szformat, args );
			va_end( args );

			stext = stmp.c_str();
		}

		void Trim( dm_string_t& stext, dm_string_t conref removeStr )
		{
			if(stext.empty() || removeStr.empty())
				return;

			dm_string_t::size_type LeftPos = stext.find_first_not_of(removeStr);
			if(LeftPos != dm_string_t::npos)
				stext = stext.substr(LeftPos, stext.length());

			dm_string_t::size_type RightPos = stext.find_last_not_of(removeStr);
			if(RightPos != dm_string_t::npos)
				stext = stext.substr(0, RightPos + 1);
		}

		void Trim( dm_string& stext, dm_string conref removeStr )
		{
			if(stext.empty() || removeStr.empty())
				return;

			dm_string::size_type LeftPos = stext.find_first_not_of(removeStr);
			if(LeftPos != dm_string_t::npos)
				stext = stext.substr(LeftPos, stext.length());

			dm_string::size_type RightPos = stext.find_last_not_of(removeStr);
			if(RightPos != dm_string::npos)
				stext = stext.substr(0, RightPos + 1);
		}

		dm_string tokenize(dm_string& str, const dm_string& delimiters)
		{
			dm_string token;
			// skip delimiters at beginning.
			dm_string::size_type lastPos = str.find_first_not_of(delimiters, 0);

			// find first "non-delimiter".
			dm_string::size_type pos = str.find_first_of(delimiters, lastPos);

			if (dm_string::npos != pos || dm_string::npos != lastPos)
			{
				token = str.substr(lastPos, pos - lastPos);

				str.replace( lastPos, (pos + delimiters.size()) - lastPos, "" );				
			}
			else
			{
				token = str;
				str.clear();
			}

			return token;
		}

		std::wstring tokenize(std::wstring& str, const std::wstring& delimiters)
		{
			std::wstring token;
			// skip delimiters at beginning.
			std::wstring::size_type lastPos = str.find_first_not_of(delimiters, 0);

			// find first "non-delimiter".
			std::wstring::size_type pos = str.find_first_of(delimiters, lastPos);

			if (std::wstring::npos != pos || std::wstring::npos != lastPos)
			{
				token = str.substr(lastPos, pos - lastPos);

				str.replace( lastPos, (pos + delimiters.size()) - lastPos, L"" );				
			}
			else
			{
				token = str;
				str.clear();
			}

			return token;
		}

		dm_string_t getNumberFormatW( dm_qword_t conref i64Value )
		{
			NUMBERFMTW fmt = {0,0,3,L".",L",",1};

			wchar_t MoneyText[32]={0,};
			swprintf_s( MoneyText, L"%I64d",i64Value );

			static wchar_t moneyFormat[255]={0,};
			memset(moneyFormat, 0, sizeof(moneyFormat));

			::GetNumberFormatW( NULL, NULL, MoneyText, &fmt, moneyFormat, 255 );
			return moneyFormat;
		};

		dm_string getNumberFormatA( dm_qword_t conref i64Value )
		{
			NUMBERFMTA fmt = {0,0,3,".",",",1};

			char MoneyText[32]={0,};
			sprintf_s( MoneyText, 32, "%I64d",i64Value );

			static char moneyFormat[255]={0,};
			memset(moneyFormat, 0, sizeof(moneyFormat));

			::GetNumberFormatA( NULL, NULL, MoneyText, &fmt, moneyFormat, 255 );
			return moneyFormat;
		};
	}
}