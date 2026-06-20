
#pragma once 
#include "../../EventHandler/UIEventMng.h"


class CUSTOMDATA
{
public:
	CUSTOMDATA(){};
	virtual ~CUSTOMDATA(){};
};


#include "Window.h"
#include "Sprite.h"

#include "Billboard.h"
#include "Number3DMng.h"

#include "Font.h"
#include "FontCheck.h"
#include "Text.h"

#include "Image.h"
#include "Button.h"
#include "AniButton.h"

#include "CheckBox.h"
#include "SpriteAni.h"
#include "ScrollBar.h"
#include "Slider.h"

#include "ProgressBar.h"
#include "UI_ProgressBar.h"
#include "UI_TimerProgressBar.h"

#include "String.h"
#include "StringAnalysis.h"

#include "ListBox.h"
#include "TreeBox.h"

#include "GridListBox.h"


#define IF_TALK_STRING_CUT		480
#define IF_TARGET_TAMER_MAX_DIST		500.0f