
#pragma once

enum eRTTI_CLIENT
{
	RTTI_MAPNPC = RTTI_LIB_MAXCOUNT,
};

//====== Version
#define MAPTOOL_VERSION								0x00010010

#define __IS_FRAME( mv )	( ( theApp._GetMapFrame()->find( mv ) != theApp._GetMapFrame()->end() ) == true )
#define __IS_VIEW( mv )		( (*theApp._GetMapFrame())[ mv ]->_GetView() != NULL )
#define IS_ENABLEFRM( mv )	( ( __IS_FRAME( mv )&&__IS_VIEW( mv ) ) == true )

#define GET_MV_VIEWFRAME	( (*theApp._GetMapFrame())[ nsMAPTOOL::MV_VIEWER ] )
#define	GET_MV_VIEW			( (CViewViewer*)(*theApp._GetMapFrame())[ nsMAPTOOL::MV_VIEWER ]->_GetView() )
#define	GET_MV_NODE			( (_DlgNode*)( (*theApp._GetMapFrame())[ nsMAPTOOL::MV_NODE ]->_GetView() )->_GetDlg() )
#define	GET_MV_MAINTAB		( (CViewMainTab*)(*theApp._GetMapFrame())[ nsMAPTOOL::MV_MAINTAB ]->_GetView() )
#define	GET_MV_MAINTAB_DLG	( (_DlgMainTab*)( (CViewMainTab*)(*theApp._GetMapFrame())[ nsMAPTOOL::MV_MAINTAB ]->_GetView() )->_GetDlg() )
#define GET_MV_OBJECTCONTROL_FRM ( (*theApp._GetMapFrame())[ nsMAPTOOL::MV_OBJECT_CONTROL ] )
#define GET_MV_OBJECTCONTROL_DLG ( (_DlgObjectControl* )( (CViewObjectControl*)(*theApp._GetMapFrame())[ nsMAPTOOL::MV_OBJECT_CONTROL ]->_GetView() )->_GetDlg() )
#define GET_MV_OBJECTLIST_FRM ( (*theApp._GetMapFrame())[ nsMAPTOOL::MV_OBJECT_LIST ] )
#define GET_MV_OBJECTLIST_DLG ( (_DlgObjectList* )( (CViewObjectList*)(*theApp._GetMapFrame())[ nsMAPTOOL::MV_OBJECT_LIST ]->_GetView() )->_GetDlg() )
#define OBJ_ROUTE_CHANGE	//저장된 맵 불러올 때 오브젝트 경로 변경용..




