#pragma once 

#define CODE_MASARU				80001	// Marcus Damon
#define CODE_TOHMA				80002	// Thomas Norstein
#define CODE_YOSHINO			80003	// Yoshino Fujieda
#define CODE_IKUTO				80004	// Keenan Krier
#define CODE_TAICHI				80005	// Tai Kamiya
#define CODE_MIMI				80006	// Mimi Tachikawa
#define CODE_MATT				80007	// Matt Ishida
#define CODE_TAKERU				80008	// Takeru Kaishi
#define CODE_HIKARI				80009	// Hikari Kamiya
#define CODE_SORA				80010	// Sora Takenoushi
#define CODE_JANG				80014	// Izzy Izumi
#define CODE_JUNG				80015	// Joe Kido
#define CODE_TAKATO				80011	// Takato Matsuki
#define CODE_RIKA				80012	// Rika Nonaka
#define CODE_HENRY				80013	// Henry Wong
#define CODE_JERI				80016	// Kato Jeri
#define CODE_RYO				80017	// Akiyama Ryo
#define CODE_HIRO				80018	// Hiro Amanokawa
#define CODE_RULI				80019	// Ruli Tsukiyono
#define CODE_KIYOSHIROU			80020	// Kiyoshirou Higashimitarai

// ==================================================================================================== 

#define CODE_AGUMON				31001
#define CODE_LALAMON			31002
#define CODE_GAOMON				31003
#define CODE_FALCOMON			31004

#define CODE_AGUMON_BUST			71015
#define CODE_LALAMON_BUST			71016
#define CODE_GAOMON_BUST			71017
#define CODE_FALCOMON_BUST			71018

#define CODE_OMEGAMON				71072
#define CODE_ALPHAMON_BUST			71075
#define CODE_EXAMON					71024
#define CODE_SHAKKOUMON				71079
#define CODE_CROSS_SHOUTMONX2		71080
#define CODE_SHILPHYMON				71089

#define CODE_HUANGLONGMON			71035
#define CODE_SUSANOOMON				75042	// 스사노몬


#define CODE_CARD				95003

// 모델 로딩 안 함
#define	COSTUME_ID_EMPTY	0		// 소분류값 0

// 코스튬 ID 2601
#define COSTUME_ID_2601		1		// 공통
#define COSTUME_ID_2602		2		// 세이버즈 !> 어드벤쳐도 사용은 가능하지만 리소스 로딩은 하지 않습니다.
#define COSTUME_ID_2603		3		// 어드벤쳐
#define COSTUME_ID_2604		4		// 남성전용
#define COSTUME_ID_2605		5		// 여성전용

// 세이버즈
#define COSTUME_ID_2606		6		// 최건우
#define COSTUME_ID_2607		7		// 토마
#define COSTUME_ID_2608		8		// 유진
#define COSTUME_ID_2609		9		// 한지호

// 어드벤쳐 01 Version
#define COSTUME_ID_2610		10		// 신태일
#define COSTUME_ID_2611		11		// 이미나
#define COSTUME_ID_2612		12		// 매튜
#define COSTUME_ID_2613		13		// 리키

// 어드벤쳐 02 Version		
#define COSTUME_ID_2614		14		// 신나리
#define COSTUME_ID_2615		15		// 한소라
#define COSTUME_ID_2616		16		// 신규1
#define COSTUME_ID_2617		17		// 신규2

// ==================================================================================================== 

class CsModelDataMng : public NiMemObject
{
public:
	CsModelDataMng();
	~CsModelDataMng();

	void			Delete();
	void			Init();

protected:
	std::map< DWORD, CsModelData* >		m_mapModelData;


public:
	//SrcID 는 기존에 있던 DwID 이여야 하고 
	//DestID는 기존에 없던 ID 이여야함니다.
	//Kfm Path 예시  "Data\\Digimon\\DorumonX\\0075102_Dorumon.kfm" 
	int				CreateDataFromAnother(int SrcID, int DestID, std::string NewKfmPath, bool ForceSave = false);
	void			Save();	
protected:
	void			_LoadByFilePack();
	void			_LoadByFile();



public:
	bool			IsData( DWORD const& dwID ) const;
	CsModelData*	GetData( DWORD const& dwID );
	std::string		GetKfmPath( DWORD const& dwID ) const;
	void			InsertData( CsModelData::sINFO* pModelInfo );
	void			DeleteData( DWORD dwID );
	std::string		GetBigModelIconFile( DWORD const& dwID );
	std::string		GetBossModelIconFile( DWORD const& dwID );
	std::string		GetEvoModelIconFile( DWORD const& dwID );
	std::string		GetLargeModelIconFile( DWORD const& dwID );
	std::string		GetSmallModelIconFile( DWORD const& dwID );

	std::map< DWORD, CsModelData* >* GetDataMap();

	
protected:		// Model 구분 함수 (성별 코드 추가 전까지)
	bool _IsSaversCharacter(int nModelTamerIdx)const;			//!< 세이버즈 캐릭터
	bool _IsAdventureCharacter(int nModelTamerIdx)const;		//!< 어드벤쳐 캐릭터
	bool _IsMaleCharacter(int nModelTamerIdx)const;				//!< 남자 캐릭터
	bool _IsFemaleCharacter(int nModelTamerIdx)const;			//!< 여자 캐릭터

public:
	bool CostumeCategory(int nTamerModelIdx, int nValue)const;			// GBChar 에서 코스튬 검사 함수
	bool CostumeCategory_DProject(int nTamerModelIdx, int nValue)const;	// 위 함수 오버랩, 클라이언트 전용 코스튬 검사 함수
	DWORD CostumeToTamerCode(int nValue)const;	// 코스튬 착용 가능한 캐릭터 ID 리턴
};

extern CsModelDataMng*		g_pModelDataMng;