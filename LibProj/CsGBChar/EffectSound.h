#pragma once
#pragma pack(push)
#pragma pack(4)

class CsEffectSound : public NiMemObject
{
public:
	CsEffectSound(void); 
	~CsEffectSound(void);

	void DeleteSoundList();

public:
	struct sES_Info : public NiMemObject
	{		
		sES_Info():HashCode(0),nSoundNum(0)
		{
			memset(szFilePath, 0, sizeof(char)*MAX_PATH);
		}

		size_t		HashCode; /**< 헤시코드 */	
		char		szFilePath[ MAX_PATH ];	/**< 이펙트의 파일명 */	
		int			nSoundNum; /**< 사운드리스트 수*/
	};

	struct sEffectOption : public NiMemObject
	{
		sEffectOption():bChrScale(false),fScale(1.0f),dwEffectFlag(0),vOffset(NiPoint3::ZERO)
		{}

		//스케일 값
		bool	    bChrScale;  		/**< 캐릭터의 스케일값 적용할것인가? */
		float		fScale;			/**< 이펙트 자체의 스케일값 */

		//이펙트 옵션
		DWORD		dwEffectFlag;		/**< 이펙트 옵션 flag값 */
		NiPoint3    vOffset;			/**< offset 값 */
	};	

	struct sSound_Info : public NiMemObject
	{
		sSound_Info():fSec(0.0f)
		{
			memset(szFileName, 0, sizeof(char)*OBJECT_PATH_LEN);
		}

		float		fSec;	/**< 사운드 나오는 시간 */
		char		szFileName[ OBJECT_PATH_LEN ]; /**< 플레이될 사운드 파일명 */
	};

public:	

	//이펙트 사운드 정보
	sES_Info m_sESInfo;	

	//이펙트 옵션
	sEffectOption m_sEffectOptin;	

	//사운드 정보
	std::list< sSound_Info* > m_listSound;

};


#pragma pack(pop)
