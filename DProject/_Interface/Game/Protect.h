
#pragma once 
#include "../00.Contents/MacroProtectContents.h"

class cProtect : public cBaseWindow , public CMacroProtectContents::ObserverType
{
#ifndef SDM_MACRO_PROTECT_RENEW_20181217
	enum eProtectPattern
	{
		eRandomPos,			// 윈도우 위치 랜덤
		eAnswerColorMask,		// 정답에 컬러 마스크 적용
		eAnswerRandomSize,		// 정답에 랜덤 사이즈 
		eAnswerRandomRotat,		// 정답에 랜덤 회전
		eQuestionColorMask,		// 문제에 컬러 마스크 적용
		eAnswerColorFlow,		// 정답의 마스크 컬러 로테이션
		eQuestionColorFlow,		// 문제에 마스크 컬러 로테이션
	};

	struct sImgNumberData : public CUSTOMDATA
	{
		sImgNumberData(int idx ) : m_nTypeIdx(idx)
		{}
		~sImgNumberData() {};

		int m_nTypeIdx;
	};
#endif

public:
	cProtect();
	virtual ~cProtect();

public:
	virtual void			Create( int nValue = 0 );

	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			PreResetMap();
	virtual void			ResetDevice();
	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);	

private:

#ifdef SDM_MACRO_PROTECT_RENEW_20181217

	void					_ResetQuestion(int const& nMaxTimeSec, int const& nRemainTimeSec, int const& nChangeCount, int const& nQuestionType);
	void					_SendAnswer( std::wstring const& wsAnswer );
	void					_OnOkButtonClicked(void* pSender, void* pData);
#else

	void					_AnswerTimeOver( void* pSender, void* pData );
	void					_SetTextureUV( int const& nValue, cSprite* pTexture );
	void					_ResetQuestionImg( int const& nCorrectValue );

	void					_GetTextureUV( int const& nCorrectValue, NiPoint2& sUV, NiPoint2& eUV );
	void					_SelectAnswer(void* pSender, void* pData);

	NiColor					_GetRandomColor();
	void					_ResetQuestion(int const& nValue);

private:
	void					_RestPattern();
	void					_SetAnswerColorMask();
	void					_SetRandomWindowPosition();
	void					_SetRandomAnswerSize();// 정답 이미지 랜덤 사이즈
	void					_SetRandomAnswerRotat();// 정답 이미지 랜덤 회전
	void					_ProgressbarRandomHieght();

#endif

private:				
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	cSprite*				m_pQeustionImg;
	cEditBox*				m_pInputEditBox;
	cButton*				m_pOkButton;
	cText*					m_pRemaincount;
	cText*					m_pQeustionMessage;
#else
	cSprite*				m_pCorrectAnswerImg;
	cSprite*				m_pMaskAnswerImg;
	cGridListBox*			m_pSelectList;
#endif

	cUI_TimerProgressBar*	m_pProgress;
};

