#include "stdafx.h"
#include "SecuritySelf.h"
#include <time.h>

namespace SimpleSecurity
{
	namespace Private
	{
		void TypeXor( std::string const& src, std::string& dest )
		{
			char bt, cXorValue = 28;
			int nLen = (int)src.length();
			for( int i=0; i<nLen; ++i)
			{
				bt = src[i];
				bt = bt ^ cXorValue;
				dest += bt;
			}
		}

		/* 아래의 charset 코드 순서를 바꾸면 새로운 암호문 생성이 가능 */
		/* 중복 문자는 사용하면 안된다. 암호화할 문자가 꼭 포함되어 있어야 한다.*/
		static const std::string charset = "VX#EDC`qax35OL>)P:}\\~!QAZ@|tg,%TGB^YHN&1UrfvSed-[6yc4$9ol.0p; 2/hn'=]WRFJM*IK<(zwsb7uj?_{\"+m8ik";
		static const size_t CHARSET_SZ = charset.length();

		int strpos(std::string const& key, char ch)
		{
			for ( size_t i=0; i<CHARSET_SZ; ++i ) 
			{
				if (ch == key.at(i))
					return (int)i;
			}
			return -1;
		}

		char rotate(std::string const& key, char ch, int cnt)
		{
			int pos;

			if (cnt == 0)
				return ch;

			if ((pos = strpos(key, ch)) < 0)
				return '\0';

			pos += cnt;

			while (pos < 0) 
			{
				pos += key.length();
			}

			pos %= key.length();

			return key.at(pos);
		}

		int apEncrypt(std::string const& key, std::string const&data, std::string& out)
		{
			int len = data.length();
			int i;
			int sum = 0;

			for (i=0; i<len; ++i)
				sum += data.at(i);

			sum %= key.length();

			out = key.at(sum);
			for (i=0; i<len; ++i) 
			{
				char ch = rotate(key, data[i], (i+1)*13 - (len + out[0]));

				if (ch == '\0') 
					return -1;

				out += ch;
			}

			return i;
		}

		int apDecrypt(std::string const& key, std::string const&data, std::string& out)
		{
			char ch;
			const char *udata = data.c_str() + 1;
			int len = strlen(udata);
			for ( int i=0; i<len; i++) 
			{
				ch = rotate(key, udata[i], (i+1)*-13 + (len + data[0]));

				if (ch == '\0') 
					return -1;

				out += ch;
			}

			return 0;
		}

		void MakeSecurityKeyVector(std::string& keyvec)
		{
			::srand((unsigned)time(NULL));

			keyvec = charset;

			for( size_t n = 0; n < CHARSET_SZ; ++n )
			{
				int nStart = 0;
				int nEnd = 0;
				do 
				{
					nStart = rand() % CHARSET_SZ;
					nEnd = rand() % CHARSET_SZ;
				} while ( nStart == nEnd );

				std::swap( keyvec[nStart], keyvec[nEnd] );
			}
		}

		std::string TextToHex( std::string const& sStr) 
		{
			std::string out;
			char buffer[3]={0,};
			for (size_t i=0; i<sStr.size(); ++i)
			{
				sprintf( buffer, "%02x", (unsigned int) sStr[i]);
				out += buffer;
			}

			return out;
		}

		std::string HexToText(std::string const& sHex ) 
		{
			std::string out;
			char buffer[3]={0,};
			for (size_t i=0; i<sHex.size() / 2; i++)
			{
				std::string hex = sHex.substr( i*2, 2 );
				int num = (int)strtol(hex.c_str(), NULL, 16);
				sprintf( buffer, "%c", num);
				out += buffer;
			}

			return out;
		}
	}

	bool SecuritySelf::EncriptHex(std::string const& msg, std::string& out)
	{
		std::string en;
		if( !Encript(msg,en) )
			return false;

		out = Private::TextToHex( en );
		return true;
	}

	bool SecuritySelf::DecriptHex(std::string const& msg, std::string& out)
	{
		std::string hexToText = Private::HexToText( msg );
		if( !Decript(hexToText,out) )
			return false;
		return true;
	}

	// 암호화
	bool SecuritySelf::Encript( std::string const& msg, std::string& out )
	{
		std::string key;
		Private::MakeSecurityKeyVector( key );

		std::string MsgEn;
		if( -1 == Private::apEncrypt(key, msg, MsgEn) )
			return false;

		Private::TypeXor( key + MsgEn, out );

		return true;
	}

	// 복호화
	bool SecuritySelf::Decript( std::string const& msg, std::string& out )
	{
		std::string Decryt;
		Private::TypeXor( msg, Decryt );

		if( Decryt.length() <= Private::CHARSET_SZ )
			return false;

		std::string key = Decryt.substr( 0, Private::CHARSET_SZ );
		std::string msgValue = Decryt.substr( key.size() );
		if( msgValue.empty() )
			return false;

		if( -1 == Private::apDecrypt(key, msgValue, out) )
			return false;

		return true;
	}

	size_t SecuritySelf::GetSecuritySize( std::string const& AuthKey )
	{
		// ap암호화하면 암호화할 문자열 + 1 한 문자열의 길이가 나오기 때문에 +1함.
		return AuthKey.length() + Private::CHARSET_SZ + 1; 
	}
}
