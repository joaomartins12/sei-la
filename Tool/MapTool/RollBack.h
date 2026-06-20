
#pragma once 

class cRollBack
{
public:
	cRollBack();

public:
	enum eROLLBACK_TYPE
	{
		RT_ALPHA_MAPPING,
		RT_EFFECT_MAPPING,
		RT_OBJ_RESIST,
		RT_OBJ_DELETE,
		RT_HEIGHT,
		RT_CELL_RESIST,
		RT_CELL_DELETE,
	};

	struct sRT_BASE{ eROLLBACK_TYPE s_eType; };
	struct sRT_AM_INFO : public sRT_BASE
	{
		sRT_AM_INFO(){ s_eType = RT_ALPHA_MAPPING;  }
		int		s_nObjectID;
		int		s_nNTM;
		int		s_nStride;		
		BYTE*	s_pData;
	};
	struct sRT_EM_INFO : public sRT_BASE
	{
		sRT_EM_INFO(){ s_eType = RT_EFFECT_MAPPING;  }
		int		s_nObjectID;
		int		s_nNTM;
		int		s_nStride;
		BYTE*	s_pData;
	};
	struct sRT_OBJ_RESIST_INFO : public sRT_BASE
	{
		sRT_OBJ_RESIST_INFO(){ s_eType = RT_OBJ_RESIST; }
		CsGBObject*			s_pObject;
	};
	struct sRT_OBJ_DELETE_INFO : public sRT_BASE
	{
		sRT_OBJ_DELETE_INFO(){ s_eType = RT_OBJ_DELETE; }
		CsGBObject*			s_pObject;
	};

	struct sRT_HEIGHT_INFO : public sRT_BASE
	{
		sRT_HEIGHT_INFO(){ s_eType = RT_HEIGHT; }
		int					s_npfHeightCount;
		float**				s_ppfHeight;
	};
	struct sRT_CELL_RESIST_INFO : public sRT_BASE
	{
		sRT_CELL_RESIST_INFO(){ s_eType = RT_CELL_RESIST; }
		CsGBCell::sINFO		s_Info;
		int					s_nBaseIndex;
	};
	struct sRT_CELL_DELETE_INFO : public sRT_BASE
	{
		sRT_CELL_DELETE_INFO(){ s_eType = RT_CELL_DELETE; }
		CsGBCell::sINFO		s_Info;
		int					s_nBaseIndex;
	};

	typedef std::list< sRT_BASE* >				LIST_RT_BASE;
	typedef std::list< sRT_BASE* >::iterator	LIST_RT_BASE_IT;
	struct sRB_DATA
	{
		void Delete();
		LIST_RT_BASE	s_List;
	};
protected:
	CsMemPool< sRB_DATA >	m_poolRBData;

protected:
	bool		m_bBegin;
	bool		m_bCheckLeak;
	
	int			m_nRollBackIndex;

public:
	void		Delete();
	void		Init();

	void		Reset();

	void		Begin();
	//void		Push( int nObjectIndex, int nTriIndex, NiPoint2 ptUV );
	void		Push( sRT_BASE* pData );
	void		End();

	bool		IsBegin(){ return m_bBegin; }

protected:
	void		_DeleteMemPool();

	// ====== UnDo
protected:
	int			m_nUnDoCount;
	sRB_DATA**	m_ppUnDoData;
	sRB_DATA*	m_pTempData;

protected:
	void		_DeleteUnDo();
	void		_InitUnDo();
public:
	void		UnDo();

	// ====== ReDo
protected:
	bool		m_bSetFirstReDo;
	sRB_DATA**	m_ppReDoData;

protected:
	void		_DeleteReDo();
	void		_EmptyReDo();
	void		_InitReDo();
	void		_WriteReDo();
public:
	void		ReDo();

	// ====== AlphaMapping
public:
	std::stack< sRT_AM_INFO* >	m_poolAM;
protected:
	sRT_AM_INFO*_GetCurTRMappingInfo( sRT_AM_INFO* pInfo );
	void		_PushAM( sRT_AM_INFO* pInfo );
	void		_DoAM( sRT_AM_INFO* pInfo );

	// ====== EffectMapping
public:
	std::stack< sRT_EM_INFO* >	m_poolEM;
protected:
	sRT_EM_INFO*_GetCurTRMappingInfo( sRT_EM_INFO* pInfo );
	void		_PushEM( sRT_EM_INFO* pInfo );
	void		_DoEM( sRT_EM_INFO* pInfo );

	// ====== Object Resist
public:
	CsMemPool< sRT_OBJ_RESIST_INFO >		m_poolObjResist;
protected:
	sRT_OBJ_RESIST_INFO*	_GetNewObjResistInfo( sRT_OBJ_RESIST_INFO* pInfo );
	void		_PushObjResist( sRT_OBJ_RESIST_INFO* pInfo );
	void		_UnDoObjResist( sRT_OBJ_RESIST_INFO* pInfo );
	void		_ReDoObjResist( sRT_OBJ_RESIST_INFO* pInfo );

	// ====== Object Delete
public:
	CsMemPool< sRT_OBJ_DELETE_INFO >		m_poolObjDelete;
protected:
	sRT_OBJ_DELETE_INFO*	_GetNewObjDeleteInfo( sRT_OBJ_DELETE_INFO* pInfo );
	void		_PushObjDelete( sRT_OBJ_DELETE_INFO* pInfo );
	void		_UnDoObjDelete( sRT_OBJ_DELETE_INFO* pInfo );
	void		_ReDoObjDelete( sRT_OBJ_DELETE_INFO* pInfo );

	// ======= Height
public:
	std::stack< sRT_HEIGHT_INFO* >	m_poolHeight;
protected:
	sRT_HEIGHT_INFO*	_GetNewHeightInfo( sRT_HEIGHT_INFO* pInfo, bool bSrcTerrain );
	void		_PushHeight( sRT_HEIGHT_INFO* pInfo );
	void		_UnDoHeight( sRT_HEIGHT_INFO* pInfo );
	void		_ReDoHeight( sRT_HEIGHT_INFO* pInfo );

	// ======= CellResist
public:
	CsMemPool< sRT_CELL_RESIST_INFO >		m_poolCellResist;
protected:
	sRT_CELL_RESIST_INFO*	_GetNewCellResistInfo( sRT_CELL_RESIST_INFO* pInfo );
	void		_PushCellResist( sRT_CELL_RESIST_INFO* pInfo );
	void		_UnDoCellResist( sRT_CELL_RESIST_INFO* pInfo );
	void		_ReDoCellResist( sRT_CELL_RESIST_INFO* pInfo );

	// ======= CellDelete
public:
	CsMemPool< sRT_CELL_DELETE_INFO >		m_poolCellDelete;
protected:
	sRT_CELL_DELETE_INFO*	_GetNewCellDeleteInfo( sRT_CELL_DELETE_INFO* pInfo );
	void		_PushCellDelete( sRT_CELL_DELETE_INFO* pInfo );
	void		_UnDoCellDelete( sRT_CELL_DELETE_INFO* pInfo );
	void		_ReDoCellDelete( sRT_CELL_DELETE_INFO* pInfo );
};

extern cRollBack	g_RollBack;