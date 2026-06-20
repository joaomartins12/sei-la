#pragma once 


namespace NUMTYPE
{
	// 데미지 타입
	enum eType
	{
		DIGIMON_ATTACK,
		MONSTER_ATTACK,		
		DIGIMON_RECOVER,

		MAX_TYPE,
	};

	// 효과 타입
	enum eEffectType
	{
		ET_NORMAL,
		ET_CRITICAL,		

		MAX_EFFECT,
	};
}

struct sCONTROLLER_INFO
{
	sCONTROLLER_INFO();
	~sCONTROLLER_INFO();

	void		Delete();
	void		SetNiNode( NiNode* pNiNode );

	NiNodePtr				s_pNiNode;
	NiTransformController*	s_pController;
	float					s_fAniEndTime;
};

struct sNUMBER_INFO
{
	sNUMBER_INFO();
	~sNUMBER_INFO();

	void			Delete();
	void			SetTexture( char const* pFileName );

	CBillboard*		s_pNumber[ 10 ];
};

class CNumber3D : public NiMemObject
{
	CSGBMEMPOOL_H( CNumber3D );

public:
	CNumber3D();
	~CNumber3D();

	void	Init( NiPoint3 vPos, float fRot, int nCount, sCONTROLLER_INFO* pControllerInfo, sNUMBER_INFO* pNumberInfo );
	void	Setting(int nCount,NiSourceTexturePtr pkTexPtr);


	bool	Update();

	void	Render();


public:	
	sCONTROLLER_INFO*	m_pControllerInfo;
	sNUMBER_INFO*		m_pNumberInfo;	

	float		m_fAlpha;
	NiPoint3	m_vPos;
	float		m_fAniTime;
	NiTransform	m_tr;
	NiTransform	m_tr2;

	std::list<CBillboard*> m_lNumber;
};


class CNumber3DMng : public NiMemObject
{
public:
	static void		GlobalInit();
	static void		GlobalShotDown();


protected:
	sNUMBER_INFO			m_Number[ NUMTYPE::MAX_TYPE ];
	sCONTROLLER_INFO		m_Controller[ NUMTYPE::MAX_EFFECT ];
	std::list< CNumber3D* >	m_NumberList;


public:
	void	Destroy();
	void	Init();

	void	Add( int nNum, NiPoint3 vPos, float fRot, NUMTYPE::eType etype, NUMTYPE::eEffectType eEffecttype = NUMTYPE::ET_NORMAL);

	void	Update();
	void	Render();

	static	void		CALLBACK_Damage( int nEffect, int nType, int nDamage, NiPoint3 vPos, float fDamageRot );
};

extern CNumber3DMng*		g_pNumber3DMng;
