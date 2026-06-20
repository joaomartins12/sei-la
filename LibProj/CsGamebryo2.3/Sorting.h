

#pragma once

class CsSortObj;
class CsGBSorting : public NiMemObject
{
public:
	CsGBSorting()
	{
		m_ppkItems = NULL;
		m_pfDepths = NULL;

		m_nNumItems = 0;
		m_nMaxItems = 0;
		m_nMinTRID = INT_MAX;
		m_nMaxTRID = 0;		
	}

	~CsGBSorting() 
	{

	}


public:
	void			Delete();
	void			PreMapLoad();
	void			PostMapLoad();
	void			SetID( int nID );

public:
	void			Sort( NiCamera* pNiCamera );

protected:	
	void			_SetMax( int i );	
	void			_SortObjectsByDepth(int l, int r);
	float			_ChoosePivot(int l, int r) const;

public:
	void			ClearGeomList();
	void			Set( CGeometry* pkGeo );
	void			Render( NiCamera* pNiCamera );
	void			Render_NoClear();
	bool			empty();

protected:
	UINT						m_nNumItems;
	UINT						m_nMaxItems;
	float*						m_pfDepths;
	CGeometry**					m_ppkItems;	
	CsVectorPB< CGeometry* >	m_vpAlphaGeom;

protected:
	struct sELEMET_INFO
	{		
		CsVectorPB< CGeometry* >	s_vpGeom;					// 실제 같은 것의 목록
	};

	struct sGETTER_INFO
	{
		int							s_nType;
		CsVectorPB< sELEMET_INFO >	s_vpElement;				// 메터리얼 요소별 배열
	};

protected:
	int								m_nMinTRID;
	int								m_nMaxTRID;
	CsVectorPB< sGETTER_INFO* >		m_vpGetterInfoArray;		// ID별 오브젝트 배열
	CsVectorPB< int >				m_vpNoClipping;
	CsVectorPB< int >				m_vpF1;

public:
	enum eGETTER_TYPE
	{
		GT_NONE, GT_NOCLIPPING, GT_F1,
	};
	
public:
	void				SetShader_GetterTRObject( NiMaterial* pShader );
	void				GetterTRObject( CsSortObj* pObj, int nGetID, int nType = GT_NONE );	
	void				RenderTRObject( bool bClearGetter );
	void				RenderTRObject_Type( int nType, bool bClearGetter );
	void				ClearGetter();
};

extern CsGBSorting		g_Sorting;