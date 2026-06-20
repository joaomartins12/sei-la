#pragma once
#include "../../ContentsSystem/ContentsSystem.h"

#define STATEKINDS	9
class cSealMasterContents
	: public IUIContentSubject< cSealMasterContents >
{

public:
	//씰 정보
	struct sSealInfo
	{
		sSealInfo() : sSealID(0),sGrade(0),sModelID(0), sSealCount(0),sFavorite(false), sEffectType(0), sEffectValue(0), sTableCode(0){}
		virtual ~sSealInfo(){};

		std::wstring	sSealName;
		DWORD			sSealID;
		bool			sFavorite;		//즐찾
		int				sEffectType;	//효과 종류
		int				sEffectValue;	//효과 값
		int				sSealCount;		//카드 숫자
		int				sGrade;			//등급
		int				sModelID;		//디지몬 모델 ID
		int				sTableCode;		//seal table의 인덱스, 코드값은 0이 없다. 

	};

	//맵 정보
	struct sMapInfo
	{
		//sGroupType not have 0. if u have 0, was wrong.
		sMapInfo() : sMapID(0),sCount(0) {}
		virtual ~sMapInfo()		{};
		std::wstring			sMapName;
		int						sMapID;
		int						sCount;				//맵에 속한 씰 숫자
		//씰 ID, 씰 몬스터 네임
		std::map<DWORD, std::wstring>				sSealInfoMap;
	};

	//맵 ID, 맵정보
	typedef	std::map<DWORD,sMapInfo>					MapInfo;
	typedef	std::map<DWORD,sMapInfo>::iterator			MItr;
	typedef	std::map<DWORD,sMapInfo>::const_iterator	CMItr;
	//맵 그룹 정보
	struct sMapGroup
	{
		sMapGroup(): sGoupType(0){};
		virtual ~sMapGroup()	{};
		DWORD					sGoupType;
		std::wstring			sMapGroupName;
		int						sCount;				//맵 그룹에 속한 씰 숫자
		//맵 ID, 맵 정보
		std::map<DWORD,sMapInfo>	sMapInfoMap;
	};
	//그룹 ID, 맵 그룹 정보
	typedef std::map<DWORD, sMapGroup>					MapGroup;
	typedef std::map<DWORD, sMapGroup>::iterator		MGItr;
	typedef std::map<DWORD, sMapGroup>::const_iterator	CMGItr;

	enum eEventType
	{
		eSetFavorite,	//즐겨찾기 변경
		eSetSealCount,	//씰의 숫자가 변경
		eSetPlusValu,	//추가 능력치 합산 변경
	};

	typedef std::map<DWORD, sSealInfo>				SealInfoMap;
	typedef std::map<DWORD, sSealInfo>::iterator	SealInfoMapItr;

public:


	cSealMasterContents(void);
	~cSealMasterContents(void);
	// 상속 기본 선언
	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);

	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);

	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);

	static int const IsContentsIdentity(void);

	const SealInfoMap&		GetSealInfoMap();
	const MapGroup&			GetMapGroupMap();
	const int				GetSealLeader() const;

	void				SetSelectSealID(int nSelectSealID);
	const int			GetSelectSealID() const;

	bool				IsHaveGroup(DWORD groupID);
	bool				IsHaveMap(DWORD groupID, DWORD mapID);
	bool				IsHaveSeal(DWORD groupID, DWORD mapID, DWORD sealID);

	int*				GetPlusValue();

	NiColor				GetSealItemColor( DWORD const & dwItemCode );
	std::string			GetSealCardObjectFile_NifFile( DWORD const & dwSealID );
	std::string			GetSealCardObjectFile_GradBack( DWORD const & dwSealID );
	std::string			GetSealCardObjectFile_Elemental( DWORD const & dwSealID );
	std::string			GetSealCardObjectFile_Attribute( DWORD const & dwSealID );
	std::string			GetSealCardObjectFile_Digimon( DWORD const & dwSealID );

private:
	void				SetMapGroupMap();
	void				SetMapInfo(DWORD groupID, DWORD mapID, DWORD sealID);
	void				SetSealInfo(DWORD groupID, DWORD mapID, DWORD sealID);
	void				ClearMapGroupMap();
	void				ClearSealInfoMap();

	bool				AddSealInfoVecter(DWORD groupID, DWORD mapID, DWORD sealID);
	void				ClearSealInfoVecter();

	int					CalculationGrade(DWORD SealID, int SealCount);
	int					FindSealIDbyTableCode(int nTableCode);	//테이블 코드(몇 번째 줄) 값을 가지고 SealID 찾기
	void				SetPlusValue();

private:
	//Event 함수
	void		UIOpenClose(void* pData);			// UI 열기/닫기

	void		RecvSealInfo(void* pData);			// 씰정보 받기
	void		RecvFavorite(void* pData);			// 즐겨찾기 정보 받기
	void		RecvLeader(void* pData);			// 리더 설정 된 씰
	void		RecvSealOpen(void* pData);			// 씰 오픈 받기
	void		RecvReSealing(void* pData);			// 씰 재봉인 받기

	void		SendLeader(void* pData);			// 리더 설정 검증 밑 보내기
	void		CheckUnSeal(void* pData);			// 언씰 전 언씰 가능 여부 체크
	
	void		ReSealing(void* pData);				// 다시 씰로 만들기
	void		GetSelectedSealID(void* pData);		// 현재 선택 중인 SEALID 가져오기
	void		GetPlusValue(void* pData);			// 추가된 능력치 합 가져오기

private:
	
	MapGroup				m_MapGroupMap;
	SealInfoMap				m_SealInfoMap;

	int						m_nSelectSealID;
	int						m_nLeader;
	int						m_nSeadFavoriteSealID;
	
	int						m_pPlusValue[STATEKINDS];		//추가 능력치별 추가량 합산
};
