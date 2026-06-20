

#pragma once 

class cHpBar : public NiMemObject
{
public:
	enum eTYPE{ HP, HPDS, DS, HPDSFP, MAX_TYPE, };
public:
	struct sBAR : public NiMemObject
	{
		static void			GlobalShotDown();
		static void			DeleteInstance( sBAR* pInst );
		static sBAR*		NewInstance( cHpBar::eTYPE type );
		static std::queue< sBAR* >		g_MemPool[ cHpBar::MAX_TYPE ];


		bool			Init( cHpBar::eTYPE type, uint nTargetUID );
		bool			Update( NiPoint3 vDestPos );

		cHpBar::eTYPE	s_eType;

		uint			s_nTargetUID;
		cSprite*		s_pBarBG;
		cSprite*		s_pBarHP;
		cSprite*		s_pBarHP2;
		cSprite*		s_pBarDS;
		cSprite*		s_pBarFP;

		CsPoint			s_vPos;
		int				s_nHPBarSize;
		int				s_nDSBarSize;
		int				s_nFPBarSize;
		
		float			s_fDepth;		
		bool			s_bFind;
	};

public:
	static	void		GlobalInit();
	static	void		GlobalShotDown();

public:
	void				Init();
	void				PreResetMap();
	void				Delete();

	void				SetBuddy( eTYPE type, uint nTargetUID );
	void				DeleteBuddy( uint nTargetUID );
	
	void				Render();

protected:
	void				_Update();
	void				_Cal3D_to_2D();
	void				_Sort();

protected:
	std::list< sBAR* >			m_BarList;
	CsVectorPB< sBAR* >			m_vpSort;
	CsVectorPB< sBAR* >			m_vpRender;
	int							m_nNumSortItem;
	int							m_nMaxSortItem;
};

extern cHpBar*				g_pHpBar;