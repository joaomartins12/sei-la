#pragma once
#include "../../ContentsSystem/ContentsSystem.h"

#define MAX_TRANSCENDENCE_MAX_MATERIAL_COUNT 10

class CDigimonArchiveContents
	: public IUIContentSubject< CDigimonArchiveContents >
{
public:
	class ArchiveInitInfo
	{
	public:
		typedef 	std::map<int, cData_PostLoad::sDATA*>  Digimons;
		typedef	Digimons::iterator DigimonsIter;
		typedef	Digimons::const_iterator DigimonsConstIter;

		typedef std::map<int, int> IncuInfo;
		typedef IncuInfo::iterator IncuIter;
	public:
		ArchiveInitInfo()
			:OpenedArchiveSlotCount(0),
			MaxArchiveCount(0)
		{
			mIncuInfo.clear();
			mDigimonsInArchive.clear();
		}
		~ArchiveInitInfo(){}

		void AddData(int SlotID, cData_PostLoad::sDATA* pkDigimonData)
		{
			DigimonsIter kIter= mDigimonsInArchive.find(SlotID);
			if(kIter == mDigimonsInArchive.end())
			{
				mDigimonsInArchive.insert(Digimons::value_type(SlotID, pkDigimonData));
			}
		};
		void AddIncuInfo(int IncuSlotidx, int ArchiveSlotIdx)
		{	
			IncuIter kIter = mIncuInfo.find(ArchiveSlotIdx);
			if( kIter == mIncuInfo.end())
			{
				mIncuInfo.insert(IncuInfo::value_type(ArchiveSlotIdx, IncuSlotidx));
			}
		}
		const Digimons* GetRecvInfo(void){return &mDigimonsInArchive;}
		const int&	GetMaxArchiveCnt(void){return MaxArchiveCount;}
		const int&	GetOpenedArchiveCnt(void){return OpenedArchiveSlotCount;}
		const IncuInfo* GetRecvIncuInfo(void){return &mIncuInfo;}
		const int		GetSlotNumber(void){return nSlotNum;};
		const int		GetSlotType(void){return nType;};

		void			SetMaxArchiveCnt(int iValue){MaxArchiveCount = iValue;}
		void			SetSlotNumber(int iSlotNum){nSlotNum = iSlotNum;}
		void			SetType(int iType){nType = iType;}
		void			SetOpenedArchiveSlotCount(int iCount){OpenedArchiveSlotCount = iCount;}


	private:
		int OpenedArchiveSlotCount;
		int MaxArchiveCount;
		IncuInfo mIncuInfo;
		int nSlotNum;//1888이면 마지막 패킷입니다.
		int nType;
		Digimons mDigimonsInArchive;//int : Slot ID //c..::DATA* : Digimon Info
	};
	enum SORTMODE
	{
		ALL,
		ASCENDINGLV,
		DESCENDINGLV,
		EMPTY,
		INCU,
		ASCENDINGNAME,
		END,
	};
public:
	struct ViewInfo
	{
		DWORD dwbaseDigimonID;
		int		   iArchiveSlotIdx;
	};
	typedef std::map<wstring, ViewInfo> Sorted;
	typedef	Sorted::iterator SortedIter;
	typedef	Sorted::const_iterator SortedConstIter;
	enum SELECT_PART
	{
		USING_DIGIMON,
		ACCOMPANY_DIGIMON,
		ARCHIVE_DIGIMOM,
		INCUBATING_DIGIMON,
		INCUBATING_ITEM,
		PART_END,
	};

	struct ChangeData
	{
		int accompanyslotidx;
		int archiveslotidx;
		int changecost;
	};
	
	//////////////////////////////////////////////////////////////////////////
	class DigimonInArchive
	{
	public:
		int				miSlotIndex;			//보관함 슬롯 인덱스
		DWORD			mdwModelID;			//보관함 디지몬 모델 ID
		bool			mbIsInIncubator;	//인큐베이터에 들어있는지
		bool			mbIsInTranscend;	//초월 재료 아이템으로 들어있는지
		
		cData_PostLoad::sDATA* mpData;
		
	public:
		void Initialize(int SlotID, cData_PostLoad::sDATA* pkDigimonData);
		void Release(void);

		const int& GetSlotIndex(void);
		const DWORD& GetModelID(void);
		cData_PostLoad::sDATA* GetDigimonData(void);
		DigimonInArchive();
		~DigimonInArchive();
	};

	typedef std::map<int, DigimonInArchive*>	Digimons;
	typedef	Digimons::iterator					DigimonsIter;
	typedef	Digimons::const_iterator			DigimonsConstIter;
	//////////////////////////////////////////////////////////////////////////

	class DigimonIncuSlotInfo
	{
	public:
		const DWORD& GetReg_DigimonID(void){return RegisteredDigimonID;}
		const int& GetReg_DigimonArchiveSlotID(void){return RegisteredDigimonArchiveSlotIdx;}
		const int& GetReg_ItemInvenSlotIdx(void){return RegisteredItemInvenSlotIdx;}
		const int& GetReg_ItemType(void){return RegisteredItemType;}
		bool		GetisActivated(void){return isActiated;}
		std::wstring GetRemainTime(void);
		bool		isRemainTime(void);
	
		void SetReg_DigimonID(const DWORD kDigimonID){RegisteredDigimonID = kDigimonID;}
		void SetReg_DigimonArchiveSlotID(const int ArchiveSlotIdx){RegisteredDigimonArchiveSlotIdx = ArchiveSlotIdx;}
		void SetReg_ItemInvenSlotIdx(const int InvenSlotIdx){RegisteredItemInvenSlotIdx = InvenSlotIdx;}
		void SetReg_ItemType(const int iType){RegisteredItemType = iType;}
		void SetisActivated(bool TOF){isActiated = TOF;}
		void SetRemianTime(int iRemainTime){RemainTime = iRemainTime;}

		void ResetAll(void);
	private:
		DWORD RegisteredDigimonID;
		int	RegisteredDigimonArchiveSlotIdx;
		int RegisteredItemInvenSlotIdx;
		int RegisteredItemType;//ex 98XX
		int RemainTime;
		bool isActiated;
	public:
		DigimonIncuSlotInfo()
			:RegisteredDigimonArchiveSlotIdx(0),
			RegisteredItemInvenSlotIdx(0),
			RegisteredDigimonID(0),
			RemainTime(0),
			isActiated(false)
		{
		}
	};

	typedef std::map<int, DigimonIncuSlotInfo*> IncuSlotInfo;
	typedef IncuSlotInfo::iterator IncuSlotIter;


public:



	enum eEventType
	{
		Initailze_Archive,
		Initailze_InfoViewer,
		OnShow_IncuViewer,
		OnShow_TranscendViewer,
		Update_Viewer,
		Update_IncuViewer,
		Update_TranscendViewer,
		OnShow_ByItem,
		OnShow_ByNPC,
		OnHide_Viewer,
		Selected_Digimon,
		Recv_TranscendChargeExpResult,
		Recv_TranscendChargeExpResult_false,
		Recv_TranscendResult,
		Set_Window_Lock,
	};
	enum DragedDigimonType
	{	
		SRC_ACCOMPANY,
		SRC_ARCHIVE,
		DEST_ACCOMPANY,
		DEST_ARCHIVE,
		SRC_INCU,
		DEST_INCU_DIGIMON,
		DEST_INCU_ITEM,
		SRC_INVEN_ITEM,
	};

public:
	static int const IsContentsIdentity(void);

public:
	CDigimonArchiveContents(void);
	virtual ~CDigimonArchiveContents(void);

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

//Packet
	void OnRecvArchiveInfo(void* pkData);
	void OnRecvEndArchiveInfo(void* pkData);
	void OnRecvExtendArchiveCount(void* pkData);
	//Archive <-> Accompany
	void OnRecvChangeResult(void* pkData);
	//Archive <-> Archive
	void OnRecvMoveResultInArchive(void* pkDtata);
	void OnRecvIncubarotInfo(void* pkData);
	void OnRecvIncubatorChanged(void* pkData);
	void OnRecv_Start_Incubating(void* pkData);
	void OnRecv_Stop_Incubating(void* pkData);
	void OnRecvArchiveOpenCnt(void* pkData);
	void OnRecvExtendAccompanySlot(void* pkData);
	void OnChangePartnerDigimonInfo(void* pkData);
	void OnRecvEndIncubating(void* pkData);
//Click
	void OnClick_Show_by_NPC(void* pkData);
	void OnClick_Show_by_ITEM(void* pkData);
	void OnClick_SetFilteringMode(void* pData);
	void OnClick_SetFilteringKeyword(void* pData);
	void OnClick_SetSortingMode(void* pData);
	void OnDragUpArchieveToAccompany(int AccompanySlotIdx, int ArchiveSlotIdx, CsC_AvObject* pNPC );
	void OnDragUp_ArchieveToIncubator(void);
	void OnDragUp_InvenToIncubator(void);
	void OnDragUp_IncuToArchive(int Src_IncuSlotIdx, int Dest_ArchiveSlotidx);
	void OnDragUp_ArchiveToArchive(int Src_ArchiveSlotIdx, int Dest_ArchiveSlotIdx);
	void OnRClick_Incubator_Digimon(int IncuSlotIdx);
	void OnRClick_Incubator_Item(int IncuSlotIdx);
	void OnRClick_Archive_Digimon_In_IncuMode(int ArchiveSlotIdx);
	void OnRClick_Archive_Digimon_In_InfoMode(int ArchiveSlotIdx);
	void OnRClick_Inventory_Item(const int ItemInvenSlotIdx);
	void OnRClick_AccompanyDigimon(int AccompanySlotIdx);
	void OnRClick_LockedArchiveSlot(int ArchiveSlotIdx);
	void	OnClick_StartIncubating(int IncuSlotIdx);
	void	Req_ShowIncubatorViewer(void);
	void	Req_ShowArchiveViewer(void);
	void	Req_ShowTranscendViewer(void);
	void	Req_StartIncubating(void* pData);
	void	Req_StopIncubating(void* pData);
	void	Req_Start_Extraction(void);
	void	Req_UnRegister_Incubated_Digimon(void* pData);
	void	Req_Register_Digimon_To_ActivatedIncuslot(void* pData);
	void	Req_Change_Digimon_In_ActivatingIncuslot(void* pData);
	void	Req_Change_Digimon(void* pData);
	void	Req_CloseViewer(void* pData);
	void	OnShowToolTip(SELECT_PART ePart, int SlotIdx);
	void	OnShowToolTipIncubatorItem(int SlotIdx);
	void	OnShowToolTipIncubatorDigimon(int SlotIdx);
	void   OnMouseMoveOut(void);
	void	OnCancelIncubating(void* pData);
//Getter
	const Digimons* GetDigimonsInArchive(void){return &mDigimons;}
	const Sorted* GetSortedDigimons(void){return &mSorted;}
	const int& GetMaxArchiveCount(void){return MaxArchiveCount;}
	const int& GetOpenedArchiveCount(void){return OpenedArchiveCount;}
	const int GetDigimonCntInArchive(void){return mDigimons.size();}
	const int& GetFilterMode(void){return FilterMode;}
	const int& GetSortMode(void){return SortMode;}
	const int GetOpenedAccompanySlotCnt(void);
	const bool isCompanyTactics(int iTacticsidx);
	const bool CanStartIncubating(int iIncuSlotID);
	const int GetLastSaveCursorIndex(void){return LastCursorInfo_Index;}
	const int GetLastSaveCursorCount(void){return LastCursorInfo_Count;}
	const DWORD GetUsingDigimon(void);
	const DWORD GetCompanyID(int iIdx);
	const DWORD GetModelID(int iSlotIdx);
	const DWORD GetBaseIDInArchive(int iSlotIdx);
	cData_PostLoad::sDATA* GetDigimonDataInArchive(int SlotIdx);
	DigimonInArchive* GetArchiveSlotInfo(int SlotIdx);
	cData_PostLoad::sDATA* GetDigimonDataInAccompany(int SlotIdx);
	cData_PostLoad::sDATA* GetDigimonDataInIncubator(int IncuSlotIdx);
	const cData_PostLoad::sDATA* GetSelectedDigimon(void){return mpkSelectedDigimon;}
	const IncuSlotInfo* GetIncuSlotInfo(void){return &mIncuSlotInfos;}
	const NiPoint2& GetOpenedPos(void){return mOpenedPos;}
	CsC_AvObject* GetOpenedNPC(void){return m_pOpenNPC;}
	const std::wstring GetSerchingKeyword(void);
	const int GetEmptyArchiveSlotIdx(void);
	bool GetNowDrag(void){return m_bisNowDrag;}
	bool  UpdateIncuActivating();

//Setter
	void	SetFilterMode(const int iFilterMode){FilterMode = iFilterMode;}
	void	SetSortMode(const int iSortMode){SortMode = iSortMode;}
	void	SetSerchingKeyword(const wstring kKeyword){SerchingKeyword =kKeyword;}
	void	SetNowDrag(bool TOF){m_bisNowDrag = TOF;}
//Logic
	void	UpdateSlotData(void);
	void	OnShowPartnerDigimonInfo();
	void	ReleaseSelectedDigimon();
	void	OnShowDigimonInfoInArchieve(int ArchiveSlotIdx);
	void	OnShowDigimonInfoInAccompany(int AccompanySlotIdx);
	bool	ChangeDigimon( int iAccompanySlotIdx, int iArchieveSlotIdx , CsC_AvObject* pNPC );
	u8		CalCostMoney( int nLevel );
	bool	SwapDigimon(int iAccompanySlotidx, int iArchiveSlotIdx);
	bool	SwapDigimonInArchive(int Src_ArchiveIdx, int Dest_ArchiveIdx);
	void	SetDragInfo(DragedDigimonType eType, int SlotIdx);
	void	ExcuteDragInfo(void);
	void	ResetDragInfo();
	void	UnRegistDigimonAll(void);
	void	UnRegistDigimon(int IncuSlotIdx);
	void	RegistDigimonToIncubator(int IncuSlotIdx, int ArchiveSlotIdx);
	void	RegistItemToIncubator(int IncuSlotidx, int ItemSlotIdx);
	void	UnLockRegisteredItem(void);
	void	UnLockRegisteredItem(int iIncuSlotIdx);
	void	MoveDigimonsAccomToArchive(int ArchiveSlotIdx, cData_PostLoad::sDATA* pkData);
	void	RemoveDigimonsInArchive(int ArchiveSlotIdx);
	void	SaveLastCursorInfo(int iIndex, int iCount);
	void	ResetKeyword(void);
	void	ResetAllData(void);
	void	ResetBuffer(void);
	void	OnShowViewer(void);



private:
//Flags
	int Draged_Src_AccompanySlotID;
	int Draged_Src_ArchiveSlotID;
	int Draged_Src_IncuSlotID;
	int Draged_Src_InvenSlotID;
	int Draged_Dest_AccompanySlotID;
	int Draged_Dest_ArchiveSlotID;
	int Draged_Dest_IncuSlotID;

	//업데이트 순서때문에 아이콘 생성후 업 이벤트가 Grid MouseUPEvent 보다 빨라서 
	//아이콘이 업될때 들고 있던 정보를 저장
	// Mouse Down 에서 갱신합니다.
	int	LastCursorInfo_Index;
	int LastCursorInfo_Count;

	int Be_Change_IncuSlotIdx;
	int Be_Regist_ArchiveSlotIdx;

	int FilterMode;
	int SortMode;

	wstring SerchingKeyword;
	const int EMPTY_SLOT;
	const int EMPTY_INCU_DIGIMON_SLOT;

//Data
	int MaxArchiveCount;
	int OpenedArchiveCount;

	Digimons mDigimons;
	Sorted mSorted;

	int SelectedDigimonIdx;
	cData_PostLoad::sDATA* mpkSelectedDigimon;
	cData_PostLoad::sDATA* mpkDragedDigimon;
	
	IncuSlotInfo mIncuSlotInfos;

	ArchiveInitInfo::IncuInfo mInitIncuInfo;
	NiPoint2 mOpenedPos;
	CsC_AvObject*		m_pOpenNPC;

	bool m_bisReceiveData;

	bool m_bisNowDrag;

	int m_nPotableIndex;

	//////////////////////////////////////////////////////////////////////////
public:
	struct sTranscendMaterialInfo
	{
		sTranscendMaterialInfo():RegisteredDigimonID(0), RegisteredDigimonArchiveSlotIdx(0),RegisteredExp(0) {}
		~sTranscendMaterialInfo() {}

		DWORD	RegisteredDigimonID;
		int		RegisteredDigimonArchiveSlotIdx;
		DWORD	RegisteredExp;
	};

	struct sSubMaterialInfo
	{
		sSubMaterialInfo():dwShowItemCode(0),nHaveCount(0),nNeedCount(0),dwItemType(0), nMinAddExp(0) {}
		~sSubMaterialInfo() {}

		DWORD	dwShowItemCode; // UI 상에 표시해줄 아이템 아이콘
		DWORD	dwItemType;
		DWORD	nHaveCount;
		int		nNeedCount;
		DWORD	nMinAddExp;
	};

	typedef std::list<sTranscendMaterialInfo>	TransMaterialInfo;
	typedef TransMaterialInfo::iterator			TransMaterialIter;
	typedef TransMaterialInfo::const_iterator	TransMaterialConstIter;
	//////////////////////////////////////////////////////////////////////////

	TransMaterialInfo const& GetTransMaterialInfo(void) const {return mTransMaterialInfos;}
	void				RegistDigimonToTranscend( int ArchiveSlotIdx);
	void				UnRegistDigimonToTranscend(int materialSlotIdx);
	void				UnRegistAllDigimonToTranscend();
	void				OnRClick_Archive_Digimon_In_TranscendMode( int ArchiveSlotIdx );
	void				UnRegistAllTranscend();
	bool				IsEnableTranscendDigimon( cData_PostLoad::sDATA const* pDigimonInfo );
	bool				IsEnableTranscend_MaterialDigimon( cData_PostLoad::sDATA const* pDigimonInfo );
	sSubMaterialInfo	GetSubMaterial( DWORD const& dwDigimonIDX, int const& nType );
	void				LockSubMaterialItems();
	bool				IsSameDigimonType( cData_PostLoad::sDATA* pSel, cData_PostLoad::sDATA const* pDist );
	DWORD				GetMaterialDigimonExp( cData_PostLoad::sDATA const* pData );
	DWORD				GetInvenItemCount(DWORD const& dwItemType);
	DWORD				GetTotalNeedMoney(cData_PostLoad::sDATA const* pkDATA);
	DWORD				GetNextGrowthMaxExp( cData_PostLoad::sDATA const* pkDATA );
	DWORD				GetTranscendOriginNeedMoney( cData_PostLoad::sDATA const* pkDATA );
	void				GetCurrentDigimonMaxNExp( cData_PostLoad::sDATA const* pkDATA, DWORD & dwCurrentExp, DWORD & dwMaxpExp );
	bool				IsEnableExpCharge( int const& nChargeType );
	void				SetWindowLock(bool bLock);

	bool				SendExpCharge(int const& nChargeType);
	void				RecvExpCharge(void* pData);
	void				SendDigimonTranscendence();
	void				RecvDigimonHatchUp(void* pData);

	DWORD				GetRegistMaterialExp() const;
	bool				IsEnableTranscendenceOverExp() const;

	std::string			GetDigimonImgFileName( DWORD dwBaseDigimonID );
	std::wstring		GetDigimonFileTableName( DWORD dwBaseDigimonID );

	void				OnClickTranscendenceHelp();

private:
	void				_UpdateTranscendenceExp( u1  const& nTacticsPos, u8 const& totalExp );
	bool				_UpdateTranscendenceHatchLv( u1 const& nTacticsPos, u1 const& nHatchLv );

private:
	TransMaterialInfo	mTransMaterialInfos;
	bool				m_bTranscendenceOverExp;	// 경험치 추가 충전을 사용할 것인지에 대한 변수
	//////////////////////////////////////////////////////////////////////////
};
