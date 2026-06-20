#pragma once
#include "../../ContentsSystem/ContentsSystem.h"


class ChannelContents
	: public IUIContentSubject< ChannelContents >
{
public:
	enum eEventType
	{
		eSpreadList,
		eSetChannelTxt,
	};
	struct CHANNELINFO
	{
		CHANNELINFO()
		{
			memset( channel, -1, sizeof(channel) );
		}

		void ResetData()
		{
			memset( channel, -1, sizeof(channel) );
		}

		unsigned int	channel_idx;		// 현재 접속 중인 채널
		unsigned int	channel_Tryidx;		// 접속 시도한 채널
		char	channel[nLimit::Channel];	// 현재 가동중인 채널 상태, -1:미사용, 0:원활, 1:많음, 2:너무많음, 3~10:혼잡
	};
public:

	ChannelContents(void);
	virtual ~ChannelContents(void);

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
	
	bool		RequestChannelList(void);

	CHANNELINFO const & GetChannelInfo();

	unsigned int const	GetCurrentChannelIdx() const;
	bool		Set_ChannelTryIndexNChat(int nChannelIDX);//접속 시도 채널 저장 및 채팅창 표시

private:
	
	void		ReceiveChannel(void* pData);//각 채널의 상태 정보 받음
	void		ReceiveChannelIndex(void* pData);//현재 접속 채널 받음
	void		GetChannelInfo(void* pData);//채널정보 참조
	void		PopupChangeChannel(void* pData);//선택 채널 이동 확인 팝업
	void		DisplayChangeChannel(void* pData);//채널 이동 정보 채팅창 표시
	void		ChangeFail(void* pData);//채널 이동 실패 해당 채널 폐쇄
	void		SendChangeChannel(void* pData);//채널 변경 패킷 전송 및 채팅 메세지 세트
	void		SendChangeChannelByMaster(void* pData);//채널 변경 패킷 전송 및 채팅 메세지 세트

	CHANNELINFO channelInfo;
};
