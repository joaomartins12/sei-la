#include "stdafx.h"
#include "Global.h"
//#include "main.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

//int g_nSkipAniCount = 0;

void Update()
{
	/*if( g_nSkipAniCount == 0 )
	{
		CsC_AniProp::g_bUpdateActor = true;

		if( g_fDeltaTime < 0.07f )
		{			
		}
		else if( g_fDeltaTime < 0.08f )
		{
			g_nSkipAniCount = 1;
		}
		else if( g_fDeltaTime < 0.1f )
		{
			g_nSkipAniCount = 2;
		}
		else if( g_fDeltaTime < 0.15f )
		{
			g_nSkipAniCount = 3;
		}
		else if( g_fDeltaTime < 0.2f )
		{
			g_nSkipAniCount = 4;
		}
		else 
		{
			g_nSkipAniCount = 5;
		}
	}
	else
	{
		CsC_AniProp::g_bUpdateActor = false;
		--g_nSkipAniCount;
	}*/

	nsDIRECTSHOW::UpdateDShow();

#ifdef USE_BARCODE_REDER
	nsBARCODE::Update();
#endif

	g_GlobalInput.SystemInput();
	if( net::game )
		nsCsGBTerrain::g_ServerTime.s_nServerTime = net::game->GetTimeTS();

	cGlobalInput::eMOUSE_INPUT mi = g_pGameIF->Update();
	g_GlobalInput.Input( mi );

	g_Camera._UpdateCamera();
	g_pMngCollector->Update( mi );
	g_Camera.Update( g_fDeltaTime );
	g_pWeather->SetPos( g_pCharMng->GetTamerUser()->GetPos() );
	g_pWeather->Update( g_fDeltaTime, g_pEngine->GetD3DView() );	
}


void Update_Tutorial()
{
	nsDIRECTSHOW::UpdateDShow();

#ifdef USE_BARCODE_REDER
	nsBARCODE::Update();
#endif

	g_GlobalInput.SystemInput();
	if( net::game )
		nsCsGBTerrain::g_ServerTime.s_nServerTime = net::game->GetTimeTS();

	cGlobalInput::eMOUSE_INPUT mi = g_pGameIF->Update();
	g_GlobalInput.Input_Tutorial( mi );

	g_Camera._UpdateCamera();
	g_pMngCollector->Update_Tutorial( mi );
	g_Camera.Update( g_fDeltaTime );
	g_pWeather->SetPos( g_pCharMng->GetTamerUser()->GetPos() );
	g_pWeather->Update( g_fDeltaTime, g_pEngine->GetD3DView() );	
}