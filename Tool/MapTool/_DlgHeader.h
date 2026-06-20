
#pragma once

int		_Control2Int( CWnd* pWnd );
float	_Control2float( CWnd* pWnd );
void	_Int2Control( CWnd* pWnd, int nValue );
void	_Float2Control( CWnd* pWnd, float fValue );
void	_Float2Control( CWnd* pWnd, float fValue, int nCommaPos );
void	_ComboSelect( CComboBox* pCombo, int nValue );
void	_ComboSelect( CComboBox* pCombo, float fValue );


#include "_DlgBase.h"
#include "_DlgMTEtc.h"
#include "_DlgMTWeather.h"
#include "_DlgMTPath.h"
#include "_DlgMTPlayer.h"
#include "_DlgMTShadow.h"
#include "_DlgMTLight_Direction.h"
#include "_DlgMTLight_Point.h"
#include "_DlgMTLight.h"
#include "_DlgMTModel_Portal.h"
#include "_DlgMTModel.h"
#include "_DlgMTMonster.h"

#include "_DlgMTObj_Ani.h"
#include "_DlgMTObj_Merge.h"
#include "_DlgMTObj_TexType.h"
#include "_DlgMTObj_Portal.h"
#include "_DlgMTObject.h"

#include "_DlgMTCellObject.h"

#include "_DlgMTHeight_Polygon.h"
#include "_DlgMTHeight_Region.h"
#include "_DlgMTHeight.h"
#include "_DlgMTMapping.h"
#include "_DlgMTMap_Start.h"
#include "_DlgMTMap_Region.h"
#include "_DlgMTMap_Resurrection.h"
#include "_DlgMTMap_LimitEvo.h"
#include "_DlgMTMap_BuffRegion.h"
#include "_DlgMTMap.h"
#include "_DlgMTMain.h"
#include "_DlgMainTab.h"
#include "_DlgNode.h"
#include "_DlgSetup.h"
#include "_DlgObjectControl.h"
#include "_DlgObjectList.h"

#include "_DlgNotice.h"
#include "_DlgNewTerrain.h"
#include "_DlgInputKey.h"
#include "_DlgConfig.h"
#include "_DlgEdit.h"




