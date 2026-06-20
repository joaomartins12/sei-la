
#pragma once 

class CsGBSpeedCell : public NiMemObject
{
public:
	CsGBSpeedCell();
	~CsGBSpeedCell();

public:
	void			SaveExtraData( CsNiNodePtr pBaseNiNode );
	void			LoadExtraData( CsNiNodePtr pNiNode );

	void			Destroy();
	void			Init();

	void			Update( float fElapsedTime );
	void			Update_ByTool( float fElapsedTime );
	void			Render();
	
	//=========================================================================================
	//
	//		Cell Info
	//
	//=========================================================================================
protected:
	int				m_nWidthCellCount;
	int				m_nHeightCellCount;
	CsGBCell**		m_ppCellArray;	
	CsVectorPB< CsGBCell* >	m_RenderCell;

public:
	int				GetCellTotalCount(){ return m_nWidthCellCount*m_nHeightCellCount; }
	CsGBCell*		GetCell( int nCellIndex ){ return &m_ppCellArray[ nCellIndex/m_nWidthCellCount ][ nCellIndex%m_nWidthCellCount ]; }

	//=========================================================================================
	//
	//		Light
	//
	//=========================================================================================
public:
	void			ApplyLight( CsGBLight* pLight );
	void			ReleaseLight( CsGBLight* pLight );

	//=========================================================================================
	//
	//		Object
	//
	//=========================================================================================
public:
	void				DeleteCellObject( int nBaseIndex, CsGBCell::sINFO* pCellInfo );
	CsGBCell::sINFO*	ResistObject( int nBaseIndex, NiTransform* trParent, NiTransform* trObject );

	//=========================================================================================
	//
	//		Base Object
	//
	//=========================================================================================
public:
	struct sOBJECT_INFO
	{
		sOBJECT_INFO(){ s_cPath[ 0 ] = NULL; s_fCheckTimeMin = 2.0f; s_fCheckTimeMax = 5.0f; s_fAniSpeedMin = 0.2f; s_fAniSpeedMax = 1.2f; }
		float	CalAniSpeed(){ return s_fAniSpeedMin + ( ( rand()*rand() )%(int)( ( s_fAniSpeedMax - s_fAniSpeedMin )*100.0f ) ) * 0.01f; }
		float	CalRemainCheckTime(){ return s_fCheckTimeMin + ( ( rand()*rand() )%(int)( ( s_fCheckTimeMax - s_fCheckTimeMin )*100.0f ) ) * 0.01f; }
		char	s_cPath[ OBJECT_PATH_LEN ];
		float	s_fCheckTimeMin;
		float	s_fCheckTimeMax;
		float	s_fAniSpeedMin;
		float	s_fAniSpeedMax;
	};
	struct sOBJECT : public NiMemObject
	{
		sOBJECT_INFO			s_ObjectInfo;
		CsNodeObj				s_Node;
		NiGeometry*				s_pGeometry;
		NiGeometryData*			s_pModelData;
		NiSkinInstance*			s_pSkinInstance;
		NiBound*				s_pBound;
		NiTransformController*	s_pTransformController;

		void		Render( NiTransform tr ){
			( NiTransform& )( s_pGeometry->GetWorldTransform() ) = tr;
			s_Node.m_vpGeom[ 0 ]->Render();
		}
	};
protected:
	int						m_nBaseObjectKind;
	CsVectorPB< sOBJECT* >	m_vpBaseObject;

public:
	void			DeleteBaseObject( int nObjectID );
	sOBJECT*		AddBaseObject( int nObjectID, char* cPath );
	int				GetBaseObjectKind(){ return m_nBaseObjectKind; }
	sOBJECT*		GetBaseObject( int nIndex ){ return m_vpBaseObject[ nIndex ]; }
};

