#pragma once

#ifndef DM_SYSTEM_STRING_FN_H
#define DM_SYSTEM_STRING_FN_H

typedef void (__stdcall *DmFnUnicodeToUTF8)(std::wstring conref wsSource, std::string& sDest);
typedef void (__stdcall *DmFnUnicodeToMultiByte)(std::wstring conref wsSource, std::string& sDest);
typedef void (__stdcall *DmFnUTF8ToUnicode)(std::string conref sSource, std::wstring& wsDest);
typedef void (__stdcall *DmFnMultiByteToUnicode)(std::string conref sSource, std::wstring& wsDest);

namespace DmCS
{
	namespace StringFn
	{
		void SetConvFn( DmFnUnicodeToUTF8 ToUTF8, DmFnUnicodeToMultiByte ToMB, 
						DmFnUTF8ToUnicode U8ToUni, DmFnMultiByteToUnicode MBToUni );

		void Lwr(dm_string_t& stext);
		void Upr(dm_string_t& stext);
		void Lwr(dm_string& stext);
		void Upr(dm_string& stext);

		bool iCmp(dm_string_t conref slhs, dm_string_t conref srhs);
		bool iCmp(dm_string conref slhs, dm_string conref srhs);
		bool iCmp( char cptr slhs, char cptr srhs);
	
		void Replace(dm_string_t& stext, dm_string_t conref sOld, dm_string_t conref sNew);	
		void Replace(dm_string_t& stext, dm_string_t conref sOld, dm_string conref sNew);
		void Replace(dm_string& stext, dm_string conref sOld, dm_string conref sNew);
		void Replace(dm_string_t& stext, dm_string_t conref sOld, dm_sint_t conref sNew);
		void Replace(dm_string_t& stext, dm_string_t conref sOld, dm_dword_t conref sNew);
		void Replace(dm_string_t& stext, dm_string_t conref sOld, dm_sint64_t conref sNew);
		void Replace(dm_string_t& stext, dm_string_t conref sOld, dm_qword_t conref sNew);
		void Replace(dm_string_t& stext, dm_string_t conref sOld, bool conref sNew);
		void Replace(dm_string_t& stext, dm_string_t conref sOld, float conref sNew);
		void Replace(dm_string_t& stext, dm_string_t conref sOld, double conref sNew);
		void Replace(dm_string_t& stext, dm_string_t conref sOld, wchar_t cptr sNew);
		void Replace(dm_string_t& stext, dm_string_t conref sOld, char cptr sNew);

		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, dm_string_t conref sNew);
		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, dm_string conref sNew);
		void ReplaceAll(dm_string& stext, dm_string conref sOld, dm_string conref sNew);
		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, dm_sint_t conref sNew);
		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, dm_dword_t conref sNew);
		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, dm_sint64_t conref sNew);
		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, dm_qword_t conref sNew);
		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, bool conref sNew);
		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, float conref sNew);
		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, double conref sNew);
		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, wchar_t cptr sNew);
		void ReplaceAll(dm_string_t& stext, dm_string_t conref sOld, char cptr sNew);

		void RemoveAll( dm_string_t& stext, dm_str_t conref sRemove );
		

		void Cut(dm_string_t conref stext, dm_string_t conref sKeyword, dm_stringvec_t& stringvec);
		void Cut(dm_string conref stext, dm_string conref sKeyword,	dm_stringvec& stringvec);

		void Add( dm_string_t& stext, dm_string_t conref sAddText, dm_string_t conref tokken = L"" );
		void Add( dm_string& stext, dm_string conref sAddText, dm_string conref tokken = "" );

		void ToMB(dm_string_t conref stext, dm_string& mbs);
		void ToUTF8(dm_string_t conref stext, dm_string& u8s);

		void From(dm_string_t& stext, dm_sint_t conref Val);
		void From(dm_string_t& stext, dm_ulong_t conref Val);
		void From(dm_string_t& stext, dm_dword_t conref Val);
		void From(dm_string_t& stext, dm_sint64_t conref Val);
		void From(dm_string_t& stext, dm_qword_t conref Val);
		void From(dm_string_t& stext, bool conref Val);
		void From(dm_string_t& stext, float conref Val);
		void From(dm_string_t& stext, double conref Val);
		void From(dm_string_t& stext, dm_string conref mbs);
		void From(dm_string_t& stext, dm_string::value_type cptr mbs);

		float FloatRounding( float x, int digit = 0 );
		float FloatCeil(float x, int digit = 0);
		float FloatFloor(float x, int digit = 0);

		void FloatToString( dm_string_t& stext, float conref Val, int const& dig );

		void FromUTF8(dm_string_t& stext, dm_string conref u8s);
		void FromCapacity(dm_string_t& stext, dm_qword_t conref Val);

		void HexaFrom(dm_string_t& stext, dm_dword_t conref Val);
		void Trim(dm_string_t& stext, dm_string_t conref removeStr);
		void Trim( dm_string& stext, dm_string conref removeStr );

		dm_sint_t const ToI(dm_string_t conref stext);
		dm_dword_t const ToU(dm_string_t conref stext);
		dm_sint64_t const ToI64(dm_string_t conref stext);
		dm_qword_t const ToU64(dm_string_t conref stext);
		dm_dword_t const HexaTo(dm_string_t conref stext);
		bool const ToBool(dm_string_t conref stext);
		float const ToFloat(dm_string_t conref stext);
		double const ToDouble(dm_string_t conref stext);

		void Format(dm_string_t& stext, dm_str_t cptr szformat, ...);
		void FormatA(dm_string& stext, dm_str cptr szformat, ...);

		dm_string tokenize(dm_string& str, const dm_string& delimiters);
		dm_string_t tokenize(std::wstring& str, const std::wstring& delimiters);

		dm_string_t getNumberFormatW( dm_qword_t conref i64Value );
		dm_string getNumberFormatA( dm_qword_t conref i64Value );

		struct iCmpless
		{
			bool operator()(dm_string_t conref lhs, dm_string_t conref rhs) const
			{
				if( iCmp( lhs, rhs ) )
					return false;

				return ( lhs < rhs );
			}
		};
	}
}

#endif//DM_SYSTEM_STRING_FN_H
