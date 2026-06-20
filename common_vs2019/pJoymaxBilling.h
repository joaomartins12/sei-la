

#pragma once

#include "nScope.h"


struct sJoyMaxRequestInvoice
{
	sJoyMaxRequestInvoice()
	{
		ZeroMemory(this, sizeof(sJoyMaxRequestInvoice));
	}
	char invoiceID[32];
	int  itemPrice;
	int  SilkType;	// 0 : normal, 1 : premium
	int  PortalJid;
	char UserIP[15];
	char ItemCode[128];
	char ItemName[64];
	int   ItemCount;
	char ServerName[32];
	char CharacterName[64];
	int  CharacterID;
};






