#pragma once

#include <WinCrypt.h>

namespace EnDe
{
	#define	KeyLen		0x0080 * 0x10000    // 128-bit
	#define	MAX_PASS	1024
	#define	HASH_KEY	"Mo#129#@$)2Gameiod-$()3845%9203472S%^Dfied!*w(}rjxlkw+0!{@$fsFu8va9du7$#@,.;fi#+-Efx?>F*43Gs(f+-r(_!%3848g)fk{3jvxB*g*HJz89d*&f3" // 128

	// 암호화
	static LPTSTR Encryption( LPCTSTR Valu )
	{
		HCRYPTPROV		hProv;
		if(!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0))
		{
			// 유저용 키 컨테이너 만들기
			if(!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))
				return NULL;                // 유저용 키 켄테이너 만들기 에러
		}

		HCRYPTHASH		hHash;
		// 해쉬 만들기
		if(!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash))
			return NULL;
		// 해쉬 값 계산
		if(!CryptHashData(hHash, (BYTE*)HASH_KEY, (DWORD)strlen(HASH_KEY), 0))
			return NULL;


		DWORD dwDataLen = (DWORD)_tcslen(Valu);
		LPTSTR bBuff = new TCHAR [dwDataLen + 1];

		_tcscpy_s( bBuff, dwDataLen + 1, Valu );

		HCRYPTKEY		hKey;
		// 키 만들기
		if(!CryptDeriveKey(hProv, CALG_RC4, hHash, KeyLen, &hKey))
		{
			delete[] bBuff;
			return NULL;
		}

		// 암호화		
		DWORD sizedd = sizeof(TCHAR) * dwDataLen;
		DWORD dwEncryptSize = MAX_PASS;
		if( sizedd > dwEncryptSize )
			dwEncryptSize = sizedd;

		if(!CryptEncrypt(hKey, 0, TRUE, 0, (BYTE*)bBuff, &sizedd, dwEncryptSize))
		{
			delete[] bBuff;
			return NULL;
		}

		// 해쉬 없애기
		if( !CryptDestroyHash(hHash) )
		{
			delete[] bBuff;
			return NULL;
		}

		// CSP 핸들 풀어주기
		if( !CryptReleaseContext(hProv, 0) )
		{
			delete[] bBuff;
			return NULL;
		}

		return bBuff;
	}

	// 복호화
	static LPTSTR Decryption( LPCTSTR Valu)
	{
		HCRYPTPROV	hProv;
		// CSP(Crystographic Service Provider) 핸들 얻기
		if(!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0))
		{
			if(!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))
				return NULL;
		}

		HCRYPTHASH	hHash;		
		if( !CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash) )// 해쉬 만들기		
			return NULL;

		if( !CryptHashData(hHash, (BYTE*)HASH_KEY, (DWORD)strlen(HASH_KEY), 0) )// 해쉬 값 계산
			return NULL;

		DWORD dwDataLen = (DWORD)_tcslen(Valu);
		LPTSTR bBuff = new TCHAR [dwDataLen+1];

		_tcscpy_s( bBuff, dwDataLen+1, Valu );
		HCRYPTKEY	hKey;
		// 키 만들기	
		if( !CryptDeriveKey(hProv, CALG_RC4, hHash, KeyLen, &hKey) )
		{
			delete[] bBuff;
			return NULL;
		}

		// 복호화
		DWORD sizedd =  sizeof(TCHAR) * dwDataLen;
		if( !CryptDecrypt(hKey, 0, TRUE, 0, (BYTE*)bBuff, &sizedd) )
		{
			delete[] bBuff;
			return NULL;
		}
		// 해쉬 없애기
		
		if( !CryptDestroyHash(hHash) )
		{
			delete[] bBuff;
			return NULL;
		}

		// CSP 핸들 풀어주기
		if( !CryptReleaseContext(hProv, 0) )
		{
			delete[] bBuff;
			return NULL;
		}

		return bBuff;
	}

}
