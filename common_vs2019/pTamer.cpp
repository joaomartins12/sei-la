#include "pTamer.h"

bool nTamer::IsValidType(uint nType)
{
	switch(nType)
	{
		case nTamer::Masaru		: return true;	// 80001 Marcus Damon
		case nTamer::Tohma		: return true;	// 80002 Thomas Norstein
		case nTamer::Yoshino	: return true;	// 80003 Yoshino Fujieda
		case nTamer::Ikuto		: return true;	// 80004 Keenan Krier
		case nTamer::Tai		: return true;	// 80005 Tai Kamiya
		case nTamer::Mina		: return true;	// 80006 Mimi Tachikawa
		case nTamer::Matt		: return true;	// 80007 Matt Ishida
		case nTamer::Takeru		: return true;	// 80008 Takeru Kaishi
		case nTamer::Hikari		: return true;	// 80009 Hikari Kamiya
		case nTamer::Sora		: return true;	// 80010 Sora Takenoushi
		case nTamer::Izumi		: return true;	// 80014 Izzy Izumi
		case nTamer::Joe		: return true;	// 80015 Joe Kido
		case nTamer::Takato		: return true;	// 80011 Takato Matsuki
		case nTamer::Rika		: return true;	// 80012 Rika Nonaka
		case nTamer::Henry		: return true;	// 80013 Henry Wong
		case nTamer::KatoJeri	: return true;	// 80016 Kato Jeri
		case nTamer::AkiyamaRyo	: return true;	// 80017 Akiyama Ryo
		case nTamer::Hiro		: return true;	// 80018 Hiro Amanokawa
		case nTamer::Ruli		: return true;	// 80019 Ruli Tsukiyono
		case nTamer::Kiyoshirou	: return true;	// 80020 Kiyoshirou Higashimitarai
	}
	return false;
}








