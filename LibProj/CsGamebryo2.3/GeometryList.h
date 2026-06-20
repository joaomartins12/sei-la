

#pragma once

class CGeometry;

struct sNODE_SHADER_BASE : public NiMemObject
{
	enum eTYPE{ BLUR_EFFECT, };
	sNODE_SHADER_BASE() {};
	virtual ~sNODE_SHADER_BASE() {};

	virtual eTYPE	GetType() = 0;
	virtual void	PreRender( CGeometry* pGeom ) = 0;
	virtual void	PostRender( CGeometry* pGeom ) = 0;
	virtual void	Delete() = 0;
};

struct sNODE_SHADER_BLUREFFECT : public sNODE_SHADER_BASE
{
	THREAD_MEMPOOL_H( sNODE_SHADER_BLUREFFECT );
	
	sNODE_SHADER_BLUREFFECT():s_pmatLastWorld(NULL),s_fStartAlpha(0.0f),s_fEndAlpha(0.0f),s_fColorRate(0.0f)
		,s_nMatrixCnt(0),s_nCurInputIndex(0),s_nSyncIndex(0)
	{

	}
	virtual ~sNODE_SHADER_BLUREFFECT() {};

	virtual eTYPE	GetType(){ return BLUR_EFFECT; }
	virtual void	Delete()
	{ 
		SAFE_DELETE_ARRAY( s_pmatLastWorld ); s_fStartAlpha = 0.0f, s_fEndAlpha = 0.0f; s_fColorRate = 0.0f;
		s_nMatrixCnt = 0; s_nCurInputIndex = 0; s_nSyncIndex = 0;
	}

	void			Init( float fColorRate, float fStartAlpha, float fEndAlpha, int nMatCnt );
	
	void			PreRender( CGeometry* pGeom );
	void			PostRender( CGeometry* pGeom );

	D3DXMATRIX*		s_pmatLastWorld;
	float			s_fStartAlpha;
	float			s_fEndAlpha;
	float			s_fColorRate;

	int				s_nMatrixCnt;
	int				s_nCurInputIndex;
	int				s_nSyncIndex;
};

class CGeometry : public NiMemObject
{
public:
	enum eGEOM_TYPE
	{
		Normal, UseTR, CharOutLine, CharOutLine_Gray, MeshOutLine, MeshOutLine_Gray,
	};
	virtual eGEOM_TYPE	GetType() = 0;

	enum eSTATE_TYPE
	{
		None, 
		Color_Sampling		=	1, 
		Color_Gray			=	2,
	};

	virtual eSTATE_TYPE GetStateType() { return None; }

public:
	CGeometry():m_nPlag(0),m_pGeometry(NULL),m_fOutLineAlpha(1.0f),m_nOrgNiBit(0),m_StateType(None)
	{
	}

	virtual ~CGeometry(){};


public:
	static eGEOM_TYPE	GetMeshOutLineType();
	static eGEOM_TYPE	GetCharOutLineType();
	static eGEOM_TYPE	GetCharOutLineGrayType();

public:
	static CGeometry*	GetInstance( NiGeometry* pGeomety, int nGeomType );
	static void			DelInstance( CGeometry* pGeom );

public:
	virtual void		Delete();
	void				DeleteShader();
	inline virtual void	SetGeometry( CsNiGeometry* pGeom );	
	inline void			ResetZBuffer();
	virtual float		RenderBegin(){ return -1.0f; }
	virtual void		SetCameraDist( float fDist ){}
	virtual void		Render() = 0;


	void				ResetAlpha();
	void				SetAlpha( float fAlpha );
	float				GetAlpha(){ return m_fOutLineAlpha; }

public:
	enum eGEOMPLAG{
		GP_PARTICLE			=	0x00000001,
		GP_ALPHA			=	0x00000002,
		GP_NO_ZBUFFER_WRITE	=	0x00000004,
		GP_SPECULAR			=	0x01000008,
	};
public:
	CsNiGeometry*						m_pGeometry;
	DWORD								m_nOrgNiBit;
	DWORD								m_nPlag;
	std::list< sNODE_SHADER_BASE* >		m_listShader;

protected:
	float			m_fOutLineAlpha;
	eSTATE_TYPE		m_StateType;

public:
	void			AddPlag( DWORD nPlag ){ m_nPlag |= nPlag; }
	void			DelPlag( DWORD nPlag ){ m_nPlag &= (~nPlag); }
	bool			IsPlag( DWORD nPlag ){ return ( ( m_nPlag & nPlag ) != 0 ); }
	void			SetStateType(int nType){ m_StateType = (eSTATE_TYPE)nType;}
};

class CGeometryData : public CGeometry
{
	THREAD_MEMPOOL_H( CGeometryData )
public:
	CGeometryData() {};
	virtual ~CGeometryData() {};

	virtual eGEOM_TYPE GetType(){ return Normal; }
public:
	virtual void Render();
};

class CGeomTR : public CGeometry
{	
public:
	CGeomTR() {};
	virtual ~CGeomTR() {};

	virtual eGEOM_TYPE GetType(){ return UseTR; }

public:
	virtual void Render();
};

class CGeomCharOutLine : public CGeometryData
{
	THREAD_MEMPOOL_H( CGeomCharOutLine )

public:
	virtual eGEOM_TYPE	GetType(){ return CharOutLine; }

public:
	CGeomCharOutLine():m_fCameraDist(0.0f){};
	virtual ~CGeomCharOutLine(){};

public:
	static	float		g_fOldSetupCameraDist;
	float				m_fCameraDist;	
public:
	virtual void	Render();
	virtual float	RenderBegin(){ return m_fCameraDist = ( m_pGeometry->GetWorldTranslate() - CAMERA_ST.GetWorldTranslate() ).Length(); }	

	virtual void	SetCameraDist( float fDist ){ m_fCameraDist = fDist; }
	virtual	eSTATE_TYPE	GetStateType(){return m_StateType;}
};

class CGeomMeshOutLine : public CGeomCharOutLine
{
	THREAD_MEMPOOL_H( CGeomMeshOutLine )
public:
	CGeomMeshOutLine() {};
	virtual ~CGeomMeshOutLine() {};

	virtual eGEOM_TYPE	GetType(){ return MeshOutLine; }
};

class CGeomCharOutLine_Gray : public CGeomCharOutLine
{
	THREAD_MEMPOOL_H( CGeomCharOutLine_Gray )
public:
	CGeomCharOutLine_Gray() {};
	virtual ~CGeomCharOutLine_Gray() {};
	virtual eGEOM_TYPE	GetType(){ return CharOutLine_Gray; }
};


class CGeomMeshOutLine_Gray : public CGeomMeshOutLine
{
	THREAD_MEMPOOL_H( CGeomMeshOutLine_Gray )
public:
	CGeomMeshOutLine_Gray() {};
	virtual ~CGeomMeshOutLine_Gray() {};
	virtual eGEOM_TYPE	GetType(){ return MeshOutLine_Gray; }
};


#include "GeometryList.inl"
