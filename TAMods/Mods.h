#pragma once

#include <sstream>
#include "Config.h"
#include "Utils.h"
#include "Data.h"
#include "Geom.h"
#include "Logger.h"
#include "stats.h"

extern Config g_config;
extern Stats g_stats;

// Loadouts
bool GFxTrHUD_LoadVGSMenu(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);
bool TrPI_OnVGSNumKeyPressed(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);

// Damage numbers
bool TrPC_ClientShowOverheadNumber(int id, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);

// HUD Modification
bool TrHUD_eventPostRender(int id, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);
bool TrScoreboard_Tick(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);
bool GFxTrScenePS_LoadPlayerMiscData(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);
bool TrGO_PostRenderFor(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);
bool TrFlagBase_PostRenderFor(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);
bool TrCTFBase_PostRenderFor(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);
bool TrNugget_PostRenderFor(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);
bool TrPawn_PostRenderFor(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);
bool TrVehicle_PostRenderFor(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);

// Per weapon reticule customization
bool TrPC_PressedZoom(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);
bool TrPC_ReleasedZoom(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);
bool TrPC_CallUpdateReticule(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);

// Show/hide/scale reticule, show/hide weapons
bool TrVehicle_Tick(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);
bool TrPlayerPawn_Tick(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);

// General (used to print in the console)
bool TrGVC_PostRender(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult);
