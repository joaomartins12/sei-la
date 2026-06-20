/**********************************************************************
 *<
	FILE: TerrainExportXML.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#include "TerrainExportXML.h"
#include "XMLUtility.h"
#include <decomp.h>
#include <vector>


#define TerrainExportXML_CLASS_ID	Class_ID(0xac9305a5, 0x8d603a16)


// XML helper class		
class CCoInitialize {
public:
	CCoInitialize() : m_hr(CoInitialize(NULL)) { }
	~CCoInitialize() { if (SUCCEEDED(m_hr)) CoUninitialize(); }
	operator HRESULT() const { return m_hr; }
	HRESULT m_hr;
};


class TerrainExportXML : public SceneExport {
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

		BOOL SupportsOptions(int ext, DWORD options);
		int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		//Constructor/Destructor
		TerrainExportXML();
		~TerrainExportXML();		

public:
		CCoInitialize init;              //must be declared before any IXMLDOM objects
		CComPtr<IXMLDOMDocument>  pXMLDoc;
		CComPtr<IXMLDOMNode> pRoot;		//this is our root node 	
		CComPtr<IXMLDOMNode> iGameNode;	//the IGame child - which is the main node
		CComPtr<IXMLDOMNode> rootNode;


private:
		void			Export( INode* pNode, int nTreeDepth = 0 );

		void			ExportObject(INode* node, int treeDepth, GEOMINFO* pGeom );
		void			ExportPRS(INode* node, int treeDepth, GEOMINFO* pGeom );

		std::vector<GEOMINFO*>		m_vecAnchor;
		std::vector<GEOMINFO*>		m_vecShape;
		std::vector<GEOMINFO*>		m_vecGroundMain;

};



class TerrainExportXMLClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new TerrainExportXML(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return TerrainExportXML_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("TerrainExportXML"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
	

};

static TerrainExportXMLClassDesc TerrainExportXMLDesc;
ClassDesc2* GetTerrainExportXMLDesc() { return &TerrainExportXMLDesc; }





INT_PTR CALLBACK TerrainExportXMLOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static TerrainExportXML *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (TerrainExportXML *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}


//--- TerrainExportXML -------------------------------------------------------
TerrainExportXML::TerrainExportXML()
{

}

TerrainExportXML::~TerrainExportXML() 
{

}

int TerrainExportXML::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *TerrainExportXML::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("XML");
}

const TCHAR *TerrainExportXML::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("");
}
	
const TCHAR *TerrainExportXML::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("*****TerrainExportXML*****");
}

const TCHAR *TerrainExportXML::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("");
}

const TCHAR *TerrainExportXML::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *TerrainExportXML::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *TerrainExportXML::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int TerrainExportXML::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void TerrainExportXML::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL TerrainExportXML::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}


int	TerrainExportXML::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	//TODO: Implement the actual file Export here and 
	//		return TRUE If the file is exported properly


	// 부모 노드 읽어오기~~
	INode* pRootNode = i->GetRootNode();

	// XML로 Export 하기 전 Node에 행렬 및 정점, 각종 정보 저장
	Export(pRootNode);

	//=============================================================================
	// XML 저장
	//=============================================================================
	HRESULT hr;

	bool succeeded = SUCCEEDED(init) &&SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,  IID_IXMLDOMDocument, (void**)&pXMLDoc));
	if(!succeeded)
		return false;

	// Check the return value, hr...
	hr = pXMLDoc->QueryInterface(IID_IXMLDOMNode, (void **)&pRoot);
	if(FAILED(hr))
		return false;

	TSTR path,file,ext;

	SplitFilename(TSTR(name),&path,&file,&ext);


	// XML 출력
	TSTR buf;
	CComPtr <IXMLDOMNode> mainNode;

	CreateXMLNode( pXMLDoc, pRoot, _T("mesh"), &mainNode );
	AddXMLAttribute( mainNode, _T("majorRelease"), _T("5") );
	AddXMLAttribute( mainNode, _T("minorRelease"), _T("8") );

	//=============================================================================
	// mesh3D
	//=============================================================================
	CComPtr <IXMLDOMNode> mesh3D;
	CComPtr <IXMLDOMNode> verts;
	CComPtr <IXMLDOMNode> vertex;
	CreateXMLNode( pXMLDoc, mainNode, _T("mesh3D"), &mesh3D );
	CreateXMLNode( pXMLDoc, mesh3D, _T("verts"), &verts );
	for( int i=0; i<m_vecGroundMain[0]->s_nVertexCount; ++i )
	{
		CreateXMLNode( pXMLDoc, verts, _T("vert"), &vertex );

		if( i < 2 )
		{
			buf.printf( "%d", (int)m_vecGroundMain[0]->s_Vertex[i+2].x );
			AddXMLAttribute( vertex, _T("x"), buf.data() );
			buf.printf( "%d", (int)m_vecGroundMain[0]->s_Vertex[i+2].y );
			AddXMLAttribute( vertex, _T("y"), buf.data() );
		}
		else
		{
			buf.printf( "%d", (int)m_vecGroundMain[0]->s_Vertex[i-2].x );
			AddXMLAttribute( vertex, _T("x"), buf.data() );
			buf.printf( "%d", (int)m_vecGroundMain[0]->s_Vertex[i-2].y );
			AddXMLAttribute( vertex, _T("y"), buf.data() );
		}

		vertex = NULL;
	}

	CComPtr <IXMLDOMNode> tris;
	CComPtr <IXMLDOMNode> tri;
	CreateXMLNode( pXMLDoc, mesh3D, _T("tris"), &tris );
	for( int i=0; i<m_vecGroundMain[0]->s_nFaceCount; ++i )
	{
		CreateXMLNode( pXMLDoc, tris, _T("tri"), &tri );

		if( i == 0 )
		{
			buf.printf( "%d", 0 );
			AddXMLAttribute( tri, _T("edge0StartVert"), buf.data() );
			buf.printf( "%d", 1 );
			AddXMLAttribute( tri, _T("edge1StartVert"), buf.data() );
			buf.printf( "%d", 2 );
			AddXMLAttribute( tri, _T("edge2StartVert"), buf.data() );
		}
		else
		{
			buf.printf( "%d", 1 );
			AddXMLAttribute( tri, _T("edge0StartVert"), buf.data() );
			buf.printf( "%d", 3 );
			AddXMLAttribute( tri, _T("edge1StartVert"), buf.data() );
			buf.printf( "%d", 2 );
			AddXMLAttribute( tri, _T("edge2StartVert"), buf.data() );
			buf.printf( "%d", 0 );
			AddXMLAttribute( tri, _T("edge2Connection"), buf.data() );
		}

		tri = NULL;
	}

	//=============================================================================
	// mapping
	//=============================================================================
	CComPtr <IXMLDOMNode> mapping;
	CComPtr <IXMLDOMNode> poly;
	CComPtr <IXMLDOMNode> edge;
	CreateXMLNode( pXMLDoc, mainNode, _T("mappingTo2D"), &mapping );
	CreateXMLNode( pXMLDoc, mapping, _T("poly"), &poly );
	for( int i=0; i<4; ++i )
	{
		CreateXMLNode( pXMLDoc, poly, _T("edge"), &edge );

		if( i == 2 )
			buf.printf("%d", i+1);
		else if( i == 3 )
			buf.printf("%d", i-1);
		else
			buf.printf("%d", i);

		AddXMLAttribute( edge, _T("startVert"), buf.data() );

		edge = NULL;
	}

	//=============================================================================
	// baseObstacles
	//=============================================================================
	if( m_vecShape.size() > 0 && (strncmp( m_vecShape[0]->s_pParentNodeName, m_vecAnchor[0]->s_pNodeName, 6 ) == 0) )
	{
		CComPtr <IXMLDOMNode> baseObstacles;
		CComPtr <IXMLDOMNode> obstacle;
		CreateXMLNode( pXMLDoc, mainNode, _T("baseObstacles"), &baseObstacles );
		int nCount = (int)m_vecShape.size();
		for( int i=0; i<nCount; ++i )
		{
			CreateXMLNode( pXMLDoc, baseObstacles, _T("obstacle"), &obstacle );

			for( int j=0; j<m_vecAnchor.size(); ++j )
			{
				if( strcmp( m_vecShape[i]->s_pParentNodeName, m_vecAnchor[j]->s_pNodeName ) != 0 )
					continue;

				buf.printf( "0:%d,%d", (int)m_vecAnchor[j]->s_Position.x, (int)m_vecAnchor[j]->s_Position.y );
				AddXMLAttribute( obstacle, _T("position"), buf.data() );

				break;
			}

			buf.printf( "%d,%d,%d,%d,%d,%d,%d,%d", (int)(m_vecShape[i]->s_Vertex[3].x - m_vecShape[i]->s_PerentTrans.x), (int)(m_vecShape[i]->s_Vertex[3].y - m_vecShape[i]->s_PerentTrans.y),
				(int)(m_vecShape[i]->s_Vertex[2].x - m_vecShape[i]->s_PerentTrans.x), (int)(m_vecShape[i]->s_Vertex[2].y - m_vecShape[i]->s_PerentTrans.y),
				(int)(m_vecShape[i]->s_Vertex[1].x - m_vecShape[i]->s_PerentTrans.x), (int)(m_vecShape[i]->s_Vertex[1].y - m_vecShape[i]->s_PerentTrans.y),
				(int)(m_vecShape[i]->s_Vertex[0].x - m_vecShape[i]->s_PerentTrans.x), (int)(m_vecShape[i]->s_Vertex[0].y - m_vecShape[i]->s_PerentTrans.y) );
			AddXMLAttribute( obstacle, _T("vertices"), buf.data() );

			obstacle = NULL;
		}
	}


	//=============================================================================
	// anchors
	//=============================================================================
	if( m_vecAnchor.size() > 0 )
	{
		CComPtr <IXMLDOMNode> anchors;
		CComPtr <IXMLDOMNode> anchor;
		CreateXMLNode( pXMLDoc, mainNode, _T("anchors"), &anchors );
		int nCount = (int)m_vecAnchor.size();

		for( int i=0; i<nCount; ++i )
		{
			CreateXMLNode( pXMLDoc, anchors, _T("anchor"), &anchor );

			buf.printf( "0:%d,%d", (int)m_vecAnchor[i]->s_Position.x, (int)m_vecAnchor[i]->s_Position.y );
			AddXMLAttribute( anchor, _T("position"), buf.data() );

			char szTemp[32] = {0, };
			strcpy( szTemp, &m_vecAnchor[i]->s_pNodeName[7] );
			buf.printf( "%s", szTemp );
			AddXMLAttribute( anchor, _T("id"), buf.data() );

			buf.printf( "%u", m_vecAnchor[i]->s_nOrientation );
			AddXMLAttribute( anchor, _T("orientation"), buf.data() );

			anchor = NULL;
		}
	}

	// 여기까지


	PrettyPrint(name, pXMLDoc);

	pXMLDoc.Release();
	pRoot.Release();
	rootNode.Release();
	iGameNode.Release();
	// This is a totally bogus call, but it seems to return memory to the system.
	if (SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,  IID_IXMLDOMDocument, (void**)&pXMLDoc)))
		pXMLDoc.Release();

	return TRUE; // We have successfully exported to our file!

// 	if(!suppressPrompts)
// 		DialogBoxParam(hInstance, 
// 				MAKEINTRESOURCE(IDD_PANEL), 
// 				GetActiveWindow(), 
// 				TerrainExportXMLOptionsDlgProc, (LPARAM)this);
// 	return FALSE;
}

void TerrainExportXML::Export( INode* pNode, int nTreeDepth/* = 0*/ )
{
	// 1. 노드 이름
	MCHAR* nodeName = pNode->GetName();		

	// 2. 노드의 자식 갯수
	int cntChildren   = pNode->NumberOfChildren();


	Object* pObject = pNode->GetObjectRef();
	if( pObject != NULL && strcmp(nodeName, "Object01") != 0 )
	{
		GEOMINFO* pGeom = new GEOMINFO;

		// 3. 노드 이름 저장
		pGeom->s_pNodeName = nodeName;


		// 4. 현재 노드의 local 행렬 정보 및 부모 노드의 행렬 저장
		ExportPRS( pNode, nTreeDepth, pGeom );

		// 5. 이 node들 object 속성을 저장
		ExportObject( pNode, nTreeDepth, pGeom );


		if( strncmp( pGeom->s_pNodeName, "anchor_01", 6 ) == 0 )
			m_vecAnchor.push_back( pGeom );
		else if(strncmp( pGeom->s_pNodeName, "shape_01", 5 ) == 0 )
			m_vecShape.push_back( pGeom );
		else
			m_vecGroundMain.push_back( pGeom );

	}

	// 6. 재귀함수
	nTreeDepth++;
	for(int i=0; i<cntChildren; i++)
	{
		Export(pNode->GetChildNode(i), nTreeDepth);
	}
}

void TerrainExportXML::ExportObject( INode* node, int treeDepth, GEOMINFO* pGeom )
{
	// 해당 노드의 Object의 reference를 얻는다. 
	Object* object = node->GetObjectRef();

	AnimPropertyList* pList = node->GetAProps();
	int nCount = 0;
	if( pList != NULL )
		nCount = pList->GetnallocCount();

	// root node는 Object를 가지지 않는다.
	if(NULL != object)
	{
		SClass_ID superClassID  = object->SuperClassID();
		Class_ID  classID		= object->ClassID();


		// 삼각형 메시 오브젝트, 모든 절차적 오브젝트들은 반드시 자신을 TriObject로 변환 시킬 수 있어야 함
		// TriObject는 변형가능, 이것은 TriObject들이 수정자가 기하도형을 변형 시키기 위해 연산을 수행 할 수 있다는 지점을 가진다는 의미
		// 여기서는 Class_ID를 사용해서 TriObject로 변환한다.
		if(object->CanConvertToType(triObjectClassID))
		{
			TriObject* triObject = (TriObject*)object->ConvertToType(0, triObjectClassID);
			if(NULL != triObject)
			{
				// mech의 계층 출력
				// 메쉬 정보를 얻오옴
				Mesh& mesh = triObject->GetMesh();

				pGeom->s_nOrientation = nCount != 0 ? triObject->GetAFlag() * nCount : triObject->GetAFlag();
				//				pGeom->s_nOrientation = triObject->GetAFlag();

				int trisCnt = mesh.getNumFaces();
				pGeom->s_nFaceCount = trisCnt;

				for( int i=0; i<trisCnt; ++i )
				{
					// Face와 인덱스 정보를 얻어옴
					Face& face = mesh.faces[ i ];
					DWORD* pIndices = face.getAllVerts();

					pGeom->s_Face[i].s_nFace1 = pIndices[0];
					pGeom->s_Face[i].s_nFace2 = pIndices[1];
					pGeom->s_Face[i].s_nFace3 = pIndices[2];
				}

				// 정점의 월드좌표를 알기위함.
				Matrix3 mat3Object = node->GetObjectTM(0);

				int vertsCnt = mesh.getNumVerts();
				pGeom->s_nVertexCount = vertsCnt;

				for( int i=0; i<vertsCnt; ++i )
				{
					// 버텍스 정보를 얻어옴
					Point3& vtx = mesh.getVert( i );
					pGeom->s_Vertex[i] = vtx * mat3Object;	// 로컬 정점 좌표에 월드 행렬을 곱해 월드 좌표를 알아냄(MAX상의 월드 좌표)
				}

				INode* pNode = node->GetParentNode();
				if( pNode != NULL )
					pGeom->s_pParentNodeName = pNode->GetName();

				// TriObject pointer가 원래의 Object를 가르키지 않는다면
				// 새로운 인스턴스란 말이고 이제 그걸 쓰면 된다.
				if(triObject != object)
				{
					// 어떠한 max class에서도 결코 직접적으로 delete를 호출하면 안된다.
					triObject->DeleteMe();
				}
			}
		}
	}
}

void TerrainExportXML::ExportPRS( INode* node, int treeDepth, GEOMINFO* pGeom )
{
	Matrix3 tmWorld		= node->GetNodeTM(0);
	Matrix3 tmParent	= node->GetParentTM(0);
	Matrix3 tmLocal		= tmWorld * Inverse(tmParent);


	AffineParts affine;					// max용 구조체, (이동, 회전, 스케일회전, 스케일, 부호)값이 있음.
	float rotEuler[3];

	decomp_affine(tmLocal, &affine);		// 구조체의 값을 얻음.

	// affine의 rotation quaternion를 rotation euler로 변환
	affine.q.GetEuler(&rotEuler[0], &rotEuler[1], &rotEuler[2]);

	// position 출력
	pGeom->s_Position = Point3( affine.t.x, affine.t.y, affine.t.z );

	// euler rotation
	pGeom->s_Rotate = Point3( rotEuler[0], rotEuler[1], rotEuler[2] );

	// scale
	pGeom->s_Scale = Point3( affine.k.x, affine.k.y, affine.k.z );

	// obstacle 좌표 구할때 부모 매트릭스 필요함.
	decomp_affine(tmParent, &affine);
	pGeom->s_PerentTrans = Point3( affine.t.x, affine.t.y, affine.t.z );
}