/**********************************************************************
 *<
	FILE: CMILoad.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#include "CMILoad.h"

#define CMILoad_CLASS_ID	Class_ID(0xa9ab6e95, 0xa10b56dd)


class CMILoad : public SceneImport {
	public:

		static HWND hParams;
		
		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box
		int				DoImport(const TCHAR *name,ImpInterface *i,Interface *gi, BOOL suppressPrompts=FALSE);	// Import file
		
		//Constructor/Destructor
		CMILoad();
		~CMILoad();		


public:
	struct sCMI_HEADER
	{
		int		s_nRootInfo_Size;
	};

	struct sCMI_ROOTINFO 
	{
		int		s_nXVertCount;
		int		s_nYVertCount;
		int		s_nGridSize;
		int		s_nTotalVertCount;
	};

	struct sCMI_OBJECTINFO
	{
		void Reset(){
			s_ptMinVert = Point3( FLT_MAX, FLT_MAX, FLT_MAX );
			s_ptMaxVert = Point3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
			s_cObjName[ 0 ] = NULL;
		}
		Point3	s_ptPos;
		Point3	s_ptRot;
		Point3	s_ptMinVert;
		Point3	s_ptMaxVert;
		char	s_cObjName[ 64 ];
	};

private:
	FILE*			m_pInput;
	ImpInterface*	m_pImpInterface;
	Interface*		m_pInterface;

	int				m_nHeaderSize;		// 헤더 사이즈

	sCMI_HEADER		m_sCH;				// 지형 헤더
	sCMI_ROOTINFO	m_sCRI;				// 지형 루트 정보
	float*			m_pfHeightField;	// 지형 높이

	int				m_nObjectCount;		// 오브젝트 개수

	std::list< sCMI_OBJECTINFO* >		m_listObjectInfo;

public:

	bool SetTerrain();
	bool SetObject();

};



class CMILoadClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new CMILoad(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_IMPORT_CLASS_ID; }
	Class_ID		ClassID() { return CMILoad_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("CMILoad"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
	

};

static CMILoadClassDesc CMILoadDesc;
ClassDesc2* GetCMILoadDesc() { return &CMILoadDesc; }




INT_PTR CALLBACK CMILoadOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static CMILoad *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (CMILoad *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}


//--- CMILoad -------------------------------------------------------
CMILoad::CMILoad()
{

}

CMILoad::~CMILoad() 
{

}

int CMILoad::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *CMILoad::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	switch(n)
	{
	case 0:
		return _T("CMI");
	}
	return _T("");
}

const TCHAR *CMILoad::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("");
}
	
const TCHAR *CMILoad::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("****** CMI Load ******");
}

const TCHAR *CMILoad::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("");
}

const TCHAR *CMILoad::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *CMILoad::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *CMILoad::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int CMILoad::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void CMILoad::ShowAbout(HWND hWnd)
{			
	// Optional
}

int CMILoad::DoImport(const TCHAR *filename,ImpInterface *i,
						Interface *gi, BOOL suppressPrompts)
{
	//TODO: Implement the actual file import here and 
	//		return TRUE If the file is imported properly

	//=============================================================================
	// Data Setting
	//=============================================================================

	m_pInterface = gi;
	m_pImpInterface = i;

	// Open the stream
	m_pInput = _tfopen(filename,_T("rb"));
	if (!m_pInput) {
		return 0;
	}

	// Get the file size
	fseek(m_pInput, 0, SEEK_END);
	long fsize = ftell(m_pInput);
	fseek(m_pInput, 0, SEEK_SET);

	fread( &m_nHeaderSize, sizeof(int), 1, m_pInput );

	fread( &m_sCH, sizeof(sCMI_HEADER), 1, m_pInput );
	fread( &m_sCRI, sizeof(sCMI_ROOTINFO), 1, m_pInput );

	m_pfHeightField = new float[m_sCRI.s_nTotalVertCount];

	fread( m_pfHeightField, sizeof(float), m_sCRI.s_nTotalVertCount, m_pInput );

	fread( &m_nObjectCount, sizeof(int), 1, m_pInput );

	int nCount = m_nObjectCount;

	while( nCount/*!feof(m_pInput)*/ )
	{
		sCMI_OBJECTINFO* pInfo = new sCMI_OBJECTINFO;
		pInfo->Reset();

		fread( &pInfo->s_ptPos.x, sizeof(float), 1, m_pInput );
		fread( &pInfo->s_ptPos.z, sizeof(float), 1, m_pInput );
		fread( &pInfo->s_ptPos.y, sizeof(float), 1, m_pInput );

		fread( &pInfo->s_ptRot.x, sizeof(float), 1, m_pInput );
		fread( &pInfo->s_ptRot.z, sizeof(float), 1, m_pInput );
		fread( &pInfo->s_ptRot.y, sizeof(float), 1, m_pInput );

		fread( &pInfo->s_ptMinVert.x, sizeof(float), 1, m_pInput );
		fread( &pInfo->s_ptMinVert.y, sizeof(float), 1, m_pInput );
		fread( &pInfo->s_ptMinVert.z, sizeof(float), 1, m_pInput );

		fread( &pInfo->s_ptMaxVert.x, sizeof(float), 1, m_pInput );
		fread( &pInfo->s_ptMaxVert.y, sizeof(float), 1, m_pInput );
		fread( &pInfo->s_ptMaxVert.z, sizeof(float), 1, m_pInput );

		fread( pInfo->s_cObjName, sizeof(char), 64, m_pInput );

		m_listObjectInfo.push_back( pInfo );

		nCount--;
	}

	// 화면 clean up
	m_pImpInterface->NewScene();

	// 지형 설정
	if( !SetTerrain() )
		return FALSE;


	if( !SetObject() )
		return FALSE;


	m_pImpInterface->RedrawViews();


	return TRUE;

// 	if(!suppressPrompts)
// 		DialogBoxParam(hInstance, 
// 				MAKEINTRESOURCE(IDD_PANEL), 
// 				GetActiveWindow(), 
// 				CMILoadOptionsDlgProc, (LPARAM)this);
// 	return FALSE;
}
	

bool CMILoad::SetTerrain()
{
	//=============================================================================
	// 지형 생성
	//=============================================================================
	{
		INode* parentNode = NULL;
		TriObject* tri = CreateNewTriObject();

		// Node 생성
		ImpNode* node = m_pImpInterface->CreateNode();

		Mtl* mtl = NewDefaultStdMat();

		Matrix3 tm;
		tm.IdentityMatrix();

		// Node의 접근권한을 Tri에 설정
		node->Reference(tri);

		// Node를 화면에 추가
		m_pImpInterface->AddNodeToScene( node );
		INode* realNode = node->GetINode();

		// data setting
		node->SetName( _T("Object1") );
		tri->mesh.setNumVerts( m_sCRI.s_nTotalVertCount );
		tri->mesh.setNumTVerts( m_sCRI.s_nTotalVertCount );
		tri->mesh.setNumFaces( (m_sCRI.s_nXVertCount - 1) * 2 * (m_sCRI.s_nYVertCount - 1) );
		tri->mesh.setNumTVFaces( (m_sCRI.s_nXVertCount - 1) * 2 * (m_sCRI.s_nYVertCount - 1) );

		// 정점 설정
		for( int y = 0; y<m_sCRI.s_nYVertCount; ++y )
		{
			for( int x=0; x<m_sCRI.s_nXVertCount; ++x )
			{
				tri->mesh.setVert( (y*m_sCRI.s_nXVertCount) + x, x * m_sCRI.s_nGridSize, y * m_sCRI.s_nGridSize, m_pfHeightField[ (y*m_sCRI.s_nXVertCount) + x ] );
				tri->mesh.setNormal( (y*m_sCRI.s_nXVertCount) + x, Point3( 0.3f, 0.3f, 0.3f) );
				tri->mesh.setTVert( (y*m_sCRI.s_nXVertCount) + x, Point3( 0.3f, 0.3f, 0.3f) );
			}
		}


		// 폴리곤 버텍스 적용
		int nIndexAlpha = 0;
		for( int y = 0; y<m_sCRI.s_nYVertCount - 1; ++y )
		{
			for( int x=0; x<m_sCRI.s_nXVertCount - 1; /*++x */)
			{
				tri->mesh.faces[(y*m_sCRI.s_nXVertCount) + x + nIndexAlpha].setVerts( (y*m_sCRI.s_nXVertCount) + x, (y*m_sCRI.s_nXVertCount) + x + m_sCRI.s_nXVertCount,
					(y*m_sCRI.s_nXVertCount) + x + m_sCRI.s_nXVertCount + 1 );
				tri->mesh.faces[(y*m_sCRI.s_nXVertCount) + x + nIndexAlpha].setSmGroup( 1 );
				tri->mesh.faces[(y*m_sCRI.s_nXVertCount) + x + nIndexAlpha].setEdgeVisFlags( 1, 1, 1 );
				tri->mesh.tvFace[(y*m_sCRI.s_nXVertCount) + x + nIndexAlpha].t[0] = (y*m_sCRI.s_nXVertCount) + x;
				tri->mesh.tvFace[(y*m_sCRI.s_nXVertCount) + x + nIndexAlpha].t[1] = (y*m_sCRI.s_nXVertCount) + x + m_sCRI.s_nXVertCount;
				tri->mesh.tvFace[(y*m_sCRI.s_nXVertCount) + x + nIndexAlpha].t[2] = (y*m_sCRI.s_nXVertCount) + x + m_sCRI.s_nXVertCount + 1;


				tri->mesh.faces[(y*m_sCRI.s_nXVertCount) + x + 1 + nIndexAlpha].setVerts( (y*m_sCRI.s_nXVertCount) + x, (y*m_sCRI.s_nXVertCount) + x + m_sCRI.s_nXVertCount + 1,
					(y*m_sCRI.s_nXVertCount) + x + 1 );
				tri->mesh.faces[(y*m_sCRI.s_nXVertCount) + x + 1 + nIndexAlpha].setSmGroup( 1 );
				tri->mesh.faces[(y*m_sCRI.s_nXVertCount) + x + 1 + nIndexAlpha].setEdgeVisFlags( 1, 1, 1 );
				tri->mesh.tvFace[(y*m_sCRI.s_nXVertCount) + x + 1 + nIndexAlpha].t[0] = (y*m_sCRI.s_nXVertCount) + x;
				tri->mesh.tvFace[(y*m_sCRI.s_nXVertCount) + x + 1 + nIndexAlpha].t[1] = (y*m_sCRI.s_nXVertCount) + x + m_sCRI.s_nXVertCount;
				tri->mesh.tvFace[(y*m_sCRI.s_nXVertCount) + x + 1 + nIndexAlpha].t[2] = (y*m_sCRI.s_nXVertCount) + x + m_sCRI.s_nXVertCount + 1;

				x++;
				if( x < m_sCRI.s_nXVertCount - 1 )
					nIndexAlpha++;
			}
		}

		node->SetTransform( 0, tm );

		realNode->SetMtl( mtl );
	}

	//=============================================================================
	//	ground_Main 오브젝트 생성
	//=============================================================================
	{
		TriObject* tri = CreateNewTriObject();

		// Node 생성
		ImpNode* node = m_pImpInterface->CreateNode();

		Mtl* mtl = NewDefaultStdMat();

		Matrix3 tm;
		tm.IdentityMatrix();

		// Node의 접근권한을 Tri에 설정
		node->Reference(tri);

		// Node를 화면에 추가
		m_pImpInterface->AddNodeToScene( node );
		INode* realNode = node->GetINode();

		// ground_Main은 최초 보이지 않게 설정함.
		realNode->Hide( TRUE );

		// data setting
		node->SetName( _T("ground_Main") );
		tri->mesh.setNumVerts( 4 );
		tri->mesh.setNumTVerts( 4 );
		tri->mesh.setNumFaces( 2 );
		tri->mesh.setNumTVFaces( 2 );


		float fWidth = m_sCRI.s_nGridSize * (m_sCRI.s_nXVertCount - 1);
		float fAddWidth = (fWidth / 100) * 2;
		float fHeight = m_sCRI.s_nGridSize * (m_sCRI.s_nYVertCount - 1);
		float fAddHeight = (fHeight / 100) * 2;

		// 정점 설정
		for( int y = 0; y<2; ++y )
		{
			for( int x=0; x<2; ++x )
			{
				tri->mesh.setVert( (y * 2) + x, x == 0 ? (x * fWidth) - fAddWidth : (x * fWidth) + fAddWidth, 
					y == 0 ? (y * fHeight) - fAddHeight : (y * fHeight) + fAddHeight, m_pfHeightField[ (y*m_sCRI.s_nXVertCount) + x ] );
				tri->mesh.setNormal( (y * 2) + x, Point3( 0.3f, 0.3f, 0.3f) );
				tri->mesh.setTVert( (y * 2) + x, Point3( 0.3f, 0.3f, 0.3f) );
			}
		}

		// 폴리곤 버텍스 적용
		tri->mesh.faces[0].setVerts( 0, 2, 3 );
		tri->mesh.faces[0].setSmGroup( 1 );
		tri->mesh.faces[0].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[0].t[0] = 0;
		tri->mesh.tvFace[0].t[1] = 2;
		tri->mesh.tvFace[0].t[2] = 3;

		tri->mesh.faces[1].setVerts( 0, 3, 1 );
		tri->mesh.faces[1].setSmGroup( 1 );
		tri->mesh.faces[1].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[1].t[0] = 0;
		tri->mesh.tvFace[1].t[1] = 3;
		tri->mesh.tvFace[1].t[2] = 1;

		node->SetTransform( 0, tm );

		realNode->SetMtl( mtl );
	}

	return true;
}

bool CMILoad::SetObject()
{
	std::list<sCMI_OBJECTINFO*>::iterator it = m_listObjectInfo.begin();
	std::list<sCMI_OBJECTINFO*>::iterator itEnd = m_listObjectInfo.end();

	int nCount = 0;

	for( ; it != itEnd; ++it )
	{
		if( m_nObjectCount == nCount )
			break;
		nCount++;

		INode* parentNode = NULL;
		TriObject* tri = CreateNewTriObject();

		// Node 생성
		ImpNode* node = m_pImpInterface->CreateNode();

		Mtl* mtl = NewDefaultStdMat();

		Matrix3 tm;
		tm.IdentityMatrix();

		// Node의 접근권한을 Tri에 설정
		node->Reference(tri);

		// Node를 화면에 추가
		m_pImpInterface->AddNodeToScene( node );
		INode* realNode = node->GetINode();

		// data setting
		// 모든 오브젝트 세팅은 정육면체를 기준으로 함. (정점 8개, face 12개)
		node->SetName( (*it)->s_cObjName );
		tri->mesh.setNumVerts( 8 );
		tri->mesh.setNumTVerts( 8 );
		tri->mesh.setNumFaces( 12 );
		tri->mesh.setNumTVFaces( 12 );

		// 정점 설정
		tri->mesh.setVert( 0, (*it)->s_ptMinVert.x, (*it)->s_ptMinVert.y, (*it)->s_ptMinVert.z );
		tri->mesh.setNormal( 0, Point3( 0.4f, 0.4f, 0.4f) );
		tri->mesh.setTVert( 0, Point3( 0.4f, 0.4f, 0.4f) );

		tri->mesh.setVert( 1, (*it)->s_ptMinVert.x, (*it)->s_ptMaxVert.y, (*it)->s_ptMinVert.z );
		tri->mesh.setNormal( 1, Point3( 0.4f, 0.4f, 0.4f) );
		tri->mesh.setTVert( 1, Point3( 0.4f, 0.4f, 0.4f) );

		tri->mesh.setVert( 2, (*it)->s_ptMaxVert.x, (*it)->s_ptMaxVert.y, (*it)->s_ptMinVert.z );
		tri->mesh.setNormal( 2, Point3( 0.4f, 0.4f, 0.4f) );
		tri->mesh.setTVert( 2, Point3( 0.4f, 0.4f, 0.4f) );

		tri->mesh.setVert( 3, (*it)->s_ptMaxVert.x, (*it)->s_ptMinVert.y, (*it)->s_ptMinVert.z );
		tri->mesh.setNormal( 3, Point3( 0.4f, 0.4f, 0.4f) );
		tri->mesh.setTVert( 3, Point3( 0.4f, 0.4f, 0.4f) );

		tri->mesh.setVert( 4, (*it)->s_ptMinVert.x, (*it)->s_ptMinVert.y, (*it)->s_ptMaxVert.z );
		tri->mesh.setNormal( 4, Point3( 0.4f, 0.4f, 0.4f) );
		tri->mesh.setTVert( 4, Point3( 0.4f, 0.4f, 0.4f) );

		tri->mesh.setVert( 5, (*it)->s_ptMinVert.x, (*it)->s_ptMaxVert.y, (*it)->s_ptMaxVert.z );
		tri->mesh.setNormal( 5, Point3( 0.4f, 0.4f, 0.4f) );
		tri->mesh.setTVert( 5, Point3( 0.4f, 0.4f, 0.4f) );

		tri->mesh.setVert( 6, (*it)->s_ptMaxVert.x, (*it)->s_ptMaxVert.y, (*it)->s_ptMaxVert.z );
		tri->mesh.setNormal( 6, Point3( 0.4f, 0.4f, 0.4f) );
		tri->mesh.setTVert( 6, Point3( 0.4f, 0.4f, 0.4f) );

		tri->mesh.setVert( 7, (*it)->s_ptMaxVert.x, (*it)->s_ptMinVert.y, (*it)->s_ptMaxVert.z );
		tri->mesh.setNormal( 7, Point3( 0.4f, 0.4f, 0.4f) );
		tri->mesh.setTVert( 7, Point3( 0.4f, 0.4f, 0.4f) );


		// 폴리곤 버텍스 적용
		tri->mesh.faces[0].setVerts( 0, 1, 2 );
		tri->mesh.faces[0].setSmGroup( 1 );
		tri->mesh.faces[0].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[0].t[0] = 0;
		tri->mesh.tvFace[0].t[1] = 1;
		tri->mesh.tvFace[0].t[2] = 2;

		tri->mesh.faces[1].setVerts( 0, 2, 3 );
		tri->mesh.faces[1].setSmGroup( 1 );
		tri->mesh.faces[1].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[1].t[0] = 0;
		tri->mesh.tvFace[1].t[1] = 2;
		tri->mesh.tvFace[1].t[2] = 3;

		tri->mesh.faces[2].setVerts( 3, 2, 7 );
		tri->mesh.faces[2].setSmGroup( 1 );
		tri->mesh.faces[2].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[2].t[0] = 3;
		tri->mesh.tvFace[2].t[1] = 2;
		tri->mesh.tvFace[2].t[2] = 7;

		tri->mesh.faces[3].setVerts( 2, 6, 7 );
		tri->mesh.faces[3].setSmGroup( 1 );
		tri->mesh.faces[3].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[3].t[0] = 2;
		tri->mesh.tvFace[3].t[1] = 6;
		tri->mesh.tvFace[3].t[2] = 7;

		tri->mesh.faces[4].setVerts( 4, 7, 6 );
		tri->mesh.faces[4].setSmGroup( 1 );
		tri->mesh.faces[4].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[4].t[0] = 4;
		tri->mesh.tvFace[4].t[1] = 7;
		tri->mesh.tvFace[4].t[2] = 6;

		tri->mesh.faces[5].setVerts( 4, 6, 5 );
		tri->mesh.faces[5].setSmGroup( 1 );
		tri->mesh.faces[5].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[5].t[0] = 4;
		tri->mesh.tvFace[5].t[1] = 6;
		tri->mesh.tvFace[5].t[2] = 5;

		tri->mesh.faces[6].setVerts( 4, 5, 1 );
		tri->mesh.faces[6].setSmGroup( 1 );
		tri->mesh.faces[6].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[6].t[0] = 4;
		tri->mesh.tvFace[6].t[1] = 5;
		tri->mesh.tvFace[6].t[2] = 1;

		tri->mesh.faces[7].setVerts( 4, 1, 0 );
		tri->mesh.faces[7].setSmGroup( 1 );
		tri->mesh.faces[7].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[7].t[0] = 4;
		tri->mesh.tvFace[7].t[1] = 1;
		tri->mesh.tvFace[7].t[2] = 0;

		tri->mesh.faces[8].setVerts( 1, 5, 6 );
		tri->mesh.faces[8].setSmGroup( 1 );
		tri->mesh.faces[8].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[8].t[0] = 1;
		tri->mesh.tvFace[8].t[1] = 5;
		tri->mesh.tvFace[8].t[2] = 6;

		tri->mesh.faces[9].setVerts( 1, 6, 2 );
		tri->mesh.faces[9].setSmGroup( 1 );
		tri->mesh.faces[9].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[9].t[0] = 1;
		tri->mesh.tvFace[9].t[1] = 6;
		tri->mesh.tvFace[9].t[2] = 2;

		tri->mesh.faces[10].setVerts( 4, 3, 7 );
		tri->mesh.faces[10].setSmGroup( 1 );
		tri->mesh.faces[10].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[10].t[0] = 4;
		tri->mesh.tvFace[10].t[1] = 3;
		tri->mesh.tvFace[10].t[2] = 7;

		tri->mesh.faces[11].setVerts( 4, 0, 3 );
		tri->mesh.faces[11].setSmGroup( 1 );
		tri->mesh.faces[11].setEdgeVisFlags( 1, 1, 1 );
		tri->mesh.tvFace[11].t[0] = 4;
		tri->mesh.tvFace[11].t[1] = 0;
		tri->mesh.tvFace[11].t[2] = 3;

		// YawPitchRoll
		tm.SetRotate( (*it)->s_ptRot.z, -((*it)->s_ptRot.x), (*it)->s_ptRot.y );
		//tm.SetRotateX( (*it)->s_ptRot.x );
		//tm.SetRotateY( (*it)->s_ptRot.z );
		//tm.SetRotateZ( (*it)->s_ptRot.y );

		// Translate
		tm.SetTrans( Point3((*it)->s_ptPos.x, (*it)->s_ptPos.z, (*it)->s_ptPos.y ));

		node->SetTransform( 0, tm );

		realNode->SetMtl( mtl );
	}

	return true;
}