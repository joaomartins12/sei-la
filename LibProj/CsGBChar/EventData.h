#pragma once

class CEventData : public NiMemObject
{
public:
	CEventData(void);
	~CEventData(void);

public:	
	struct sEvent_Info : public NiMemObject
	{
		float		s_fStartTime;	/**< 이벤트 딜레이시간 */
		float		s_fEventTime;	/**< 이벤트 전체시간 */				
		UINT		s_nEventNum;	/**< 이벤트 번호 */	
		TCHAR		s_strTitle[ 32 ];	/**< 이벤트 이름 */				
	};

	enum eEvent_ObjectType { EO_New = 1, EO_Npc, EO_END };
	struct sEventObject_Info : public NiMemObject
	{
		DWORD		s_dwEventObjectID;    /**< 주체의 번호 */
		short		s_sType;     	/**< 이벤트 타입 */
		NiPoint3	s_vPos;			/**< 초기 위치 */
		NiPoint3	s_vOffset;		/**< 초기 위치 오프셋 */
		float		s_fRot;			/**< 초기 회전값 */
		float		s_fScale;		/**< 오브젝트 스케일값 */
		TCHAR		s_strName[ 32 ];	/**< 캐릭터 이름 */
	};

	enum eEvent_Type { ET_Ani = 1, ET_Move, ET_Talk, ET_Sound, ET_Effect, ET_END };
	struct sBase_Info : public NiMemObject
	{
	public:
		eEvent_Type GetType(){ return s_InfoType; }
		float		GetEventStartTime(){ return s_fSec; }
		void		SetEventStartTime( float fTime ) { s_fSec = fTime; }

	protected:
		eEvent_Type s_InfoType;
		float		s_fSec;							/**< 이벤트 나오는 시간 */
	};	

	struct sAnimation_Info : public sBase_Info
	{	
		sAnimation_Info(){ s_InfoType = ET_Ani; }
		
		DWORD		s_dwMotionID;			        /**< 모션ID 값 */
	};

	struct sMovePos_Info : public sBase_Info
	{
		sMovePos_Info(){ s_InfoType = ET_Move; }
	
		float		s_fSpeed;						/**< 이동속도 */
		NiPoint3	s_vPos;							/**< 위치이동값 */		
	};
	
	struct sEventTalk_Info : public sBase_Info
	{
		sEventTalk_Info(){ s_InfoType = ET_Talk; }

		DWORD		s_dwTalkID;						/**< 대사ID 값 */
	}; 

	struct sEventSound_Info : public sBase_Info
	{
		sEventSound_Info(){ s_InfoType = ET_Sound; }

		enum eEventSound { ES_MUSIC, ES_SOUND, ES_END };
		eEventSound			nType;							/**< 효과음인지 배경음인지 */
		char				szFileName[ MAX_PATH ];			/**< 플레이될 사운드 파일명 */
	}; 

	struct sEventEffect_Info : public sBase_Info
	{
		sEventEffect_Info(){ s_InfoType = ET_Effect; }

		size_t		s_EffectHashCode;				/**< 이펙트 해쉬코드값 */
	};

public:
	void			Delete();

	void			Init( sEvent_Info* pEventInfo, sEventObject_Info* pObjectInfo ); 

	void			InsertEventData( sBase_Info* pEventData ) { m_vEventData.push_back( pEventData ); }
	int				GetEventSize() { return (int)m_vEventData.size(); }
	sBase_Info*		GetEventData( int nIdx ) { return m_vEventData[ nIdx ]; }	
	sEvent_Info*	GetEventInfo() { return &m_sEventInfo; }
	sEventObject_Info* GetObjectInfo() { return &m_sObjectInfo; }	

protected:
	sEvent_Info			m_sEventInfo;
	sEventObject_Info	m_sObjectInfo;	
	std::vector< sBase_Info* >	m_vEventData;
};
