#include <iostream>
#include <memory.h>

#include "interfaces.h"
#include "draw.h"
#include "hooker.h"
#include "aimbot.h"
#include "esp.h"
#include "bhop.h"
#include "spammer.h"
#include "autostrafe.h"
#include "netvarmanager.h"
#include "antiaim.h"
#include "triggerbot.h"
#include "skinchanger.h"

// UI
#include "UI/ui_container.h"

#define CONV(c) cwConvert(c)

FONT title_font = 0;
FONT normal_font = 0;
FONT esp_font = 0;

UI_Container* gui;

static wchar_t* cwConvert(const char* text)
{
	const size_t size = strlen(text) + 1;
	wchar_t* wText = new wchar_t[size];
	mbstowcs(wText, text, size);
	return wText;
}

void SetupFonts ()
{
	title_font		= Draw::CreateFont ("Arial", 20, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	normal_font		= Draw::CreateFont ("Arial", 17, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	esp_font		= Draw::CreateFont ("TeX Gyre Adventor", 17, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
}

bool hkCreateMove (void* thisptr, float flInputSampleTime, CUserCmd* cmd)
{
	clientMode_vmt->GetOriginalMethod<CreateMoveFn>(25)(thisptr, flInputSampleTime, cmd);
	
	// This code should be moved into a tick hook of some sorts...
	Spammer::Tick ();
	
	if (cmd && cmd->command_number)
	{
		BHop::CreateMove (cmd);
		AutoStrafe::CreateMove (cmd);
		Aimbot::CreateMove (cmd);
		Triggerbot::CreateMove (cmd);
		AntiAim::CreateMove (cmd);
	}
	
	return false;
}


void hkPaintTraverse(void* thisptr, VPANEL vgui_panel, bool force_repaint, bool allow_force)
{
	panel_vmt->GetOriginalMethod<PaintTraverseFn>(42)(thisptr, vgui_panel, force_repaint, allow_force);

	if (strcmp(panel->GetName(vgui_panel), "FocusOverlayPanel"))
		return;
	
	gui->Draw ();
	
	ESP::Tick ();
}

/* replacement FrameStageNotify function */
void hkFrameStageNotify(void* thisptr, ClientFrameStage_t stage) {
	SkinChanger::FrameStageNotify (stage);

	return client_vmt->GetOriginalMethod<FrameStageNotifyFn>(36)(thisptr, stage);
}

/* called when the library is loading */
int __attribute__((constructor)) aimtux_init()
{
	Hooker::HookInterfaces ();
	Hooker::HookVMethods ();
	
	PRINT ("AimTux was successfully injected.");
	
	client_vmt->HookVM ((void*)hkFrameStageNotify, 36);
	client_vmt->ApplyVMT ();
	
	panel_vmt->HookVM ((void*)hkPaintTraverse, 42);
	panel_vmt->ApplyVMT ();
	
	Hooker::HookIClientMode ();
	
	clientMode_vmt->HookVM ((void*)hkCreateMove, 25);
	clientMode_vmt->ApplyVMT ();
	
	SetupFonts ();
	
	NetVarManager::dumpNetvars ();
	Offsets::getOffsets ();
	
	gui = new UI_Container;
	
	// Initialize GUI with a single main window
	Window* window = new Window ("Main", Vector2D (300, 400), Vector2D (100, 100));
	window->Show ();
	
	gui->AddWindow (window);
	
	return 0;
}

void __attribute__((destructor)) aimtux_shutdown()
{
	client_vmt->ReleaseVMT ();
	panel_vmt->ReleaseVMT ();
	clientMode_vmt->ReleaseVMT ();
	
	PRINT ("AimTux has been unloaded successfully.");
}