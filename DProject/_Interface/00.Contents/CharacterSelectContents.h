#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class CharacterSelectContents
	: public IUIContentSubject< CharacterSelectContents >
{
public:
	enum eEventType
	{
		eOPEN_DELTE_WINDOW,
		eCHAR_DELETE_SUCCESS,
		eCHANGE_SELECT_CHAR,
		eADD_NEW_CHARACTER,
	};
	
	struct sCharacterInfo
	{
		friend class CharacterSelectContents;

		struct sDigimonElement	// 디지몬 속성 정보
		{
			int nIconType;		// 속성 타입
			int nIconIdx;		// 속성 인덱스
			int nIconItemType;	// 아이콘 아이템 타입
		};

	public:
		sCharacterInfo(int nSlotNum, bool bLock);
		sCharacterInfo();
		~sCharacterInfo();

		bool			IsServerChangeWaitCharactor() const;
		bool			IsHaveCharInfo() const;
		bool			IsLockSlot() const;

	protected:
		void			CreateCharacter( sSimpleCharacterInfo const& info );

		void			Update(float const& fAccumTime, float const& fDeltaTime);
		void			Render();
		void			ResetData();

	private:
		CTamer*			CreateTamer(cItemData const* pEquipInfo,uint const& mTbIdx, bool const& bGray);
		CDigimon* 		CreateDigimon(uint const& mTbIdx, uint const& nEffectType, bool const& bGray);
		CPat*			CreatePat(uint const& mTbIdx, bool const& bGray);
		CsNodeObj*		Create_Dummy( std::string const& dummyFile, NiPoint3 const& nPos, float const& fRotate );
		int				GetCharacterImageIdx( uint const& mTbIdx );
		void			SetDigimonElement(uint const& unTBIdx);

		void			DeleteData();

	public:
		int				m_nSlotNumber;	// 캐릭터 슬롯 번호
		uint			m_nMapID;		// 마지막 접속 맵 번호
		std::wstring	m_szMapName;	// 맵 이름

		int				m_nCharImgIdx;	// 캐릭터 이미지 번호
		std::wstring	m_szTamerName;	// 테이머 이름
		uint			m_nTamerObjectTBIdx;	// 테이머 오브젝트 테이블 ID
		uint			m_nTamerLevel;		// 테이머 레벨
		int				m_nServerChangeWait;	// 서버 변경 대기 중인 캐릭터인지 체크
		bool			m_bLocked;				// 잠겨 있는 슬롯인가?

		std::wstring	m_szDigimonName;		// 디지몬 이름
		uint			m_nDigimonObjectTBIdx;	// 디지몬 오브젝트 테이블 ID
		uint			m_nDigimonLevel;		// 디지몬 레벨
		float			m_fDigimonScale;		// 디지몬 스케일;; 현재는 사용 안함.
		uint			m_nDigimonEffectType;	// 디지몬 이펙트 타입

		uint			m_nPat_Leader;			// 카드 마스터의 오브젝트 테이블 ID

		CTamer*			m_pTamer;				// 테이머 오브젝트
		CsNodeObj*		m_pTamerDummy;			// 서버 이전 중인 테이머오브젝트 대신 보여줄 더미 오브젝트
		CsNodeObj*		m_pDigimonDummy;		// 서버 이전 중인 디지몬오브젝트 대신 보여줄 더미 오브젝트
		CDigimon*		m_pDigimon;				// 디지몬 오브젝트
		CPat*			m_pPat;					// 카드 마스터 오브젝트

		std::list<sDigimonElement>	m_listDigimonEl;	// 디지몬 속성 리스트
	};

	typedef		std::list<CharacterSelectContents::sCharacterInfo>					LIST_CHARLIST;
	typedef		std::list<CharacterSelectContents::sCharacterInfo>::iterator		LIST_CHARLIST_IT;
	typedef		std::list<CharacterSelectContents::sCharacterInfo>::const_iterator	LIST_CHARLIST_CIT;

	static int const IsContentsIdentity(void);

public:
	CharacterSelectContents(void);
	virtual ~CharacterSelectContents(void);

	virtual int const	GetContentsIdentity(void) const;
	virtual bool		Initialize(void);
	virtual void		UnInitialize(void);
	virtual bool		IntraConnection(ContentsSystem& System);
	virtual void		Update(float const& fElapsedTime);
	virtual void		MakeMainActorData(void);
	virtual void		ClearMainActorData(void);

	void				UpdateModel( float const& fAccumTime, float const& fDeltaTime );
	void				RenderModel();

	void				gotoBack();
	bool				gotoCharCreate();
	bool				IsHaveEmptySlot() const;
	bool				IsHaveCharacter() const;
	bool				IsHaveCharacter( int const& nSlotNumber ) const;
	bool				IsLockSlot( int const& nSlotNumber ) const;
	bool				IsServerChangeWaitCharacter( int const& nSlotNumber ) const;

	LIST_CHARLIST const& GetCharacterList() const;

	sCharacterInfo const* GetCharacterInfo(int const& nSlotNumber) const;

	void				SetChangeCharacter( int const& nSlotNumber );

	// Sender Func
	bool				SendGameStart();
	bool				SendDeleteCharacter(TCHAR const* pPassword );

	virtual void		NotifyEvent(int const& iNotifiedEvt);
	virtual void		NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);

	int					GetSelectCharIdx() const;

	int					GetEmptySlotNumber() const;
	bool				IsPcbangConnect() const;

	int					GetMaxSlotCount() const;

	void				RestData();

private:

	// Recv Func
	void				RecvCharDel( void* pData );
	void				RecvCharList(void* pData);
	void				RecvCharSelected(void* pData);
	void				RecvNewCharacterInfo(void* pData);
	void				Get_EmptySlotNumber(void* pData);

	sCharacterInfo *	GetCharacterInfoFromSlot(int const& nSlotNumber);
	void				makeEmptyCharSlotInfo( int const& nOpenSlotCount, int const& nLockSlotCount );
	bool				AddNewCharacterData(sSimpleCharacterInfo const& addCharInfo );
	bool				DeleteCharacter( int const& nDelSlotNumber );
	void				OpenCharDelWindow(void* pData);

	void				DatePacing( char const* szDate );
	void				Get_Date( int nYear, int nMonth, int nDay, char* pTime, char* szDate );

private:
	LIST_CHARLIST		m_listCharInfo;		// 캐릭터 리스트 정보
	int					m_nSelectCharIdx;	// 선택된 캐릭터 인덱스 번호 
};
