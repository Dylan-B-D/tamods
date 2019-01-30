#include "Mods.h"

#define ACTION_NUM_SWITCH_GAMEMODE 3

void GFxTrPage_Main_SpecialAction(UGFxTrPage_Main* that, UGFxTrPage_Main_execSpecialAction_Parms* params) {
	UGFxTrMenuMoviePlayer* moviePlayer = (UGFxTrMenuMoviePlayer*)that->Outer;
	
	if (params->Action->ActionNumber == ACTION_NUM_SWITCH_GAMEMODE) {
		ATrPlayerController* pc = (ATrPlayerController*)moviePlayer->eventGetPC();
		
		// Ask the login server to switch the player's game mode
		g_TAServerControlClient.sendSwitchMode();
		return;
	}

	that->SpecialAction(params->Action);
}

bool GFxTrMenuMoviePlayer_ChatMessageReceived(int ID, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult) {
	UGFxTrMenuMoviePlayer* that = (UGFxTrMenuMoviePlayer*)dwCallingObject;
	UGFxTrMenuMoviePlayer_execChatMessageReceived_Parms* params = (UGFxTrMenuMoviePlayer_execChatMessageReceived_Parms*)pParams;

	// Check whether this is a control message from the server
	if (params->Channel == GC_CC_HELP) {
		std::wstring msg(params->Message.Data);
		g_TAServerControlClient.handleControlMessage(msg);
		return true;
	}

	return false;
}

static void GFxTrPage_Main_SetItems(UGFxTrPage_Main* that, bool gotyMode) {
	UGFxTrMenuMoviePlayer* moviePlayer = (UGFxTrMenuMoviePlayer*)that->Outer;

	that->ClearActions();

	if (!moviePlayer->User->m_ChoseRegion || !moviePlayer->User->m_ChoseRegion2) {
		that->AddActionPage(moviePlayer->Pages->RegionSettingsPage);
	}
	else {
		that->AddActionPage(moviePlayer->Pages->NewPlayNowPage);
	}

	that->AddActionPage(moviePlayer->Pages->ClassesPage);
	that->AddActionPage(moviePlayer->Pages->TrainingMatchPage);

	// Replace the Watch Now page with a button to switch server mode
	that->OptionTitles.Set(3, FString(L"SWITCH GAME TYPE"));
	that->AddActionNumber(ACTION_NUM_SWITCH_GAMEMODE);

	that->AddActionPage(moviePlayer->Pages->SocialPage);
	that->AddActionPage(moviePlayer->Pages->StorePage);
	that->AddActionPage(moviePlayer->Pages->SettingsPage);
	that->AddActionNumber(that->NumQuit);
}

static void GFxTrPage_Class_SetItems(UGFxTrPage_Class* that, bool gotyMode) {
	that->ClearActions();

	UGFxTrMenuMoviePlayer* moviePlayer = (UGFxTrMenuMoviePlayer*)that->Outer;

	std::vector<int> eqpPointsToShow = {
		EQP_Primary,
		EQP_Secondary,
		EQP_Tertiary,
		EQP_Belt,
		EQP_Pack,
		EQP_PerkA,
		EQP_PerkB,
		EQP_Skin,
		EQP_Voice
	};

	if (gotyMode) {
		// Don't show the tertiary weapon slot
		eqpPointsToShow.erase(std::remove(eqpPointsToShow.begin(), eqpPointsToShow.end(), EQP_Tertiary), eqpPointsToShow.end());

		// Rename perk menu options
		that->OptionSubtext = TArray<FString>();
		that->OptionSubtext.Add(FString(L"PRIMARY WEAPON"));
		that->OptionSubtext.Add(FString(L"SECONDARY WEAPON"));
		that->OptionSubtext.Add(FString(L"BELT ITEM"));
		that->OptionSubtext.Add(FString(L"PACK"));
		that->OptionSubtext.Add(FString(L"PRIMARY PERK"));
		that->OptionSubtext.Add(FString(L"SECONDARY PERK"));
		that->OptionSubtext.Add(FString(L"SKIN"));
		that->OptionSubtext.Add(FString(L"VOICE"));
	}
	else {
		// Don't show the perk slots
		eqpPointsToShow.erase(std::remove(eqpPointsToShow.begin(), eqpPointsToShow.end(), EQP_PerkA), eqpPointsToShow.end());
		eqpPointsToShow.erase(std::remove(eqpPointsToShow.begin(), eqpPointsToShow.end(), EQP_PerkB), eqpPointsToShow.end());

		// Rename class menu options
		that->OptionSubtext = TArray<FString>();
		that->OptionSubtext.Add(FString(L"SLOT ONE"));
		that->OptionSubtext.Add(FString(L"SLOT TWO"));
		that->OptionSubtext.Add(FString(L"SLOT THREE"));
		that->OptionSubtext.Add(FString(L"BELT ITEM"));
		that->OptionSubtext.Add(FString(L"PACK"));
		that->OptionSubtext.Add(FString(L"SKIN"));
		that->OptionSubtext.Add(FString(L"VOICE"));
	}

	for (auto& eqp : eqpPointsToShow) {
		if (!gotyMode && (eqp == EQP_Primary || eqp == EQP_Secondary || eqp == EQP_Tertiary)) {
			that->AddActionSet(moviePlayer->Pages->EquipSubPage, eqp, FString());
		}
		else {
			that->AddActionSet(moviePlayer->Pages->EquipPage, eqp, FString());
		}
		
	}
	that->AddActionNumber(that->NumRenameLoadout);
}

static void initialiseMenus(UGFxTrMenuMoviePlayer* menuMovie, std::string gameSettingMode) {
	// Initialize the main menu changes
	GFxTrPage_Main_SetItems(Utils::tr_menuMovie->Pages->MainPage, gameSettingMode != "ootb");

	// Initialise the class changes
	GFxTrPage_Class_SetItems(Utils::tr_menuMovie->Pages->ClassPage, gameSettingMode != "ootb");
}

bool TrLoginManager_Login(int id, UObject *dwCallingObject, UFunction* pFunction, void* pParams, void* pResult) {
	//UTrLoginManager* that = (UTrLoginManager*)dwCallingObject;
	//UTrLoginManager_execLogin_Parms* params = (UTrLoginManager_execLogin_Parms*)pParams;
	//bool* result = (bool*)pResult;

	//Logger::log("LOGIN");

	//// Initialise the menus on login
	//initCustomMenus((UGFxTrMenuMoviePlayer*)that->Outer);

	//// Normal login
	//*result = that->Login(params->UserName, params->Password, params->bShouldRemember);
	return true;
}

static bool hasConnectedControlChannel = false;
// Override so that we can do initialization when the player first clicks on something in the menus
void GFxTrPage_Main_TakeAction(UGFxTrPage_Main* that, UGFxTrPage_Main_execTakeAction_Parms* params, int* result, Hooks::CallInfo callInfo) {
	// Set the global reference to the main menu instance
	Utils::tr_menuMovie = (UGFxTrMenuMoviePlayer*)that->Outer;

	if (!hasConnectedControlChannel) {
		// Indicate to the server that we are a modded client, so we can receive control messages
		g_TAServerControlClient.sendConnect();
		hasConnectedControlChannel = true;
	}

	// Perform the normal action
	*result = that->TakeAction(params->ActionIndex, params->DataList);
}

static void fillClassPageEquipMenu(UGFxTrPage_Class* that, UGFxTrMenuMoviePlayer* mp, int eqpPoint, std::vector<int> items) {
	mp->Pages->EquipPage->ClearActions();
	mp->Pages->EquipPage->bParentLocked = that->bClassLocked;
	mp->Pages->EquipPage->ActiveLoadout = that->ActiveLoadout;
	mp->Pages->EquipPage->LoadoutClassId = that->LoadoutClassId;
	mp->Pages->EquipPage->LoadoutEquipType = eqpPoint;

	that->bViewingEquip = true;

	for (int id : items) {
		mp->Pages->EquipPage->AddActionNumber(id);
	}
}

void GFxTrPage_Class_SpecialAction(UGFxTrPage_Class* that, UGFxTrPage_Class_execSpecialAction_Parms* params, void* result, Hooks::CallInfo callInfo) {
	if (g_TAServerControlClient.getCurrentGameSettingMode() == "ootb") {
		// Want to show OOTB menus
		that->SpecialAction(params->Action);
		return;
	}

	UGFxTrMenuMoviePlayer* mp = (UGFxTrMenuMoviePlayer*)that->Outer;
	UTrEquipInterface* eqpInterface = mp->EquipInterface;

	// Rename loadout case
	if (params->Action->ActionNumber == that->NumRenameLoadout) {
		if (eqpInterface->IsClassOwned(that->LoadoutClassId) && eqpInterface->IsLoadoutOwned(that->LoadoutClassId, that->ActiveLoadout)) {
			that->PopupNum = that->NumRenameLoadout;
			mp->QueuePopup();
		}
		return;
	}

	// Equipment slot case
	int eqpPoint = params->Action->ActionNumber;
	
	// Show equip page directly
	std::vector<int> itemsToShow;

	if (eqpPoint == EQP_Primary) {
		// Running in GOTY mode; need to show the primary ('impact') weapons directly
		int filter = 0;
		switch (that->LoadoutClassId) {
		case CONST_CLASS_TYPE_LIGHT:
			filter = 11126;
			break;
		case CONST_CLASS_TYPE_MEDIUM:
			filter = 11131;
			break;
		case CONST_CLASS_TYPE_HEAVY:
			filter = 11136;
			break;
		}


		for (int id = eqpInterface->GetFirstEquipIdFiltered(that->LoadoutClassId, eqpPoint, filter); id != 0; id = eqpInterface->GetNextEquipIdFiltered(that->LoadoutClassId, eqpPoint, filter, id)) {
			itemsToShow.push_back(id);
		}
	}
	else if (eqpPoint == EQP_Secondary) {
		// Running in GOTY mode; need to show the secondary ('timed') weapons directly
		int filter = 0;
		switch (that->LoadoutClassId) {
		case CONST_CLASS_TYPE_LIGHT:
			filter = 11142;
			break;
		case CONST_CLASS_TYPE_MEDIUM:
			filter = 11133;
			break;
		case CONST_CLASS_TYPE_HEAVY:
			filter = 11139;
			break;
		}


		for (int id = eqpInterface->GetFirstEquipIdFiltered(that->LoadoutClassId, eqpPoint, filter); id != 0; id = eqpInterface->GetNextEquipIdFiltered(that->LoadoutClassId, eqpPoint, filter, id)) {
			itemsToShow.push_back(id);
		}
	}
	else if (eqpPoint == EQP_PerkA) {
		itemsToShow = Data::perks_by_slot[0];
	}
	else if (eqpPoint == EQP_PerkB) {
		itemsToShow = Data::perks_by_slot[1];
	}
	else {
		for (int i = 0; i < eqpInterface->GetEquipCount(that->LoadoutClassId, eqpPoint); ++i) {
			itemsToShow.push_back(eqpInterface->GetEquipId(that->LoadoutClassId, eqpPoint, i));
		}
	}

	fillClassPageEquipMenu(that, mp, eqpPoint, itemsToShow);
}

void GFxTrPage_Class_FillOption(UGFxTrPage_Class* that, UGFxTrPage_Class_execFillOption_Parms* params, UGFxObject** result, Hooks::CallInfo callInfo) {
	if ((g_TAServerControlClient.getCurrentGameSettingMode() == "ootb") || that->PageActions.GetStd(params->ActionIndex)->ActionNumber == that->NumRenameLoadout) {
		// For OOTB and the rename option, offload to the normal implementation
		*result = that->FillOption(params->ActionIndex);
		return;
	}

	UGFxTrMenuMoviePlayer* mp = (UGFxTrMenuMoviePlayer*)that->Outer;
	UTrEquipInterface* eqpInterface = mp->EquipInterface;

	int equipPoint = that->PageActions.GetStd(params->ActionIndex)->ActionNumber;

	if (equipPoint != EQP_PerkA && equipPoint != EQP_PerkB) {
		// For non-perk slots, offload to the normal implementation
		*result = that->FillOption(params->ActionIndex);
		return;
	}

	int encodedPerks = eqpInterface->GetActiveEquipId(that->LoadoutClassId, EQP_Tertiary, that->ActiveLoadout);
	int perkA = Utils::perks_DecodeA(encodedPerks);
	int perkB = Utils::perks_DecodeB(encodedPerks);

	int equipId = equipPoint == EQP_PerkA ? perkA : perkB;

	*result = that->FillEquipTypes(equipId, params->ActionIndex);
}

void GFxTrPage_Equip_SpecialAction(UGFxTrPage_Equip* that, UGFxTrPage_Equip_execSpecialAction_Parms* params, void* result, Hooks::CallInfo callInfo) {
	if ((g_TAServerControlClient.getCurrentGameSettingMode() == "ootb") || (that->LoadoutEquipType != EQP_PerkA && that->LoadoutEquipType != EQP_PerkB)) {
		// Normal logic in OOTB mode, and in GOTY mode for everything except perks
		that->SpecialAction(params->Action);
		return;
	}

	UGFxTrMenuMoviePlayer* mp = (UGFxTrMenuMoviePlayer*)that->Outer;
	UTrEquipInterface* eqpInterface = mp->EquipInterface;

	int encodedPerks = eqpInterface->GetActiveEquipId(that->LoadoutClassId, EQP_Tertiary, that->ActiveLoadout);
	int perkA = Utils::perks_DecodeA(encodedPerks);
	int perkB = Utils::perks_DecodeB(encodedPerks);

	// Validate these are real perks in case the login server loadouts are old
	// And just remove invalid perks
	if (Data::perk_id_to_name.find(perkA) == Data::perk_id_to_name.end()) {
		perkA = 0;
	}
	if (Data::perk_id_to_name.find(perkB) == Data::perk_id_to_name.end()) {
		perkB = 0;
	}

	if (params->Action->ActionNumber == ((that->LoadoutEquipType == EQP_PerkA) ? perkA : perkB)) {
		// Perk not changed, exit early
		return;
	}

	if (that->LoadoutEquipType == EQP_PerkA) {
		perkA = params->Action->ActionNumber;
	}
	else {
		perkB = params->Action->ActionNumber;
	}

	int updatedPerk = (that->LoadoutEquipType == EQP_PerkA) ? perkA : perkB;
	int updatedEncodedPerks = Utils::perks_Encode(perkA, perkB);

	ATrPlayerController* pc = (ATrPlayerController*)mp->eventGetPC();

	if (!eqpInterface->SetActiveEquipId(that->LoadoutClassId, EQP_Tertiary, that->ActiveLoadout, updatedEncodedPerks)) {
		// Failed to set tertiary weapon to new encoded perks
		if (pc) {
			pc->TestTrainingSlot(EQP_Tertiary, updatedEncodedPerks);
		}
		return;
	}

	UClass* equipClass = mp->EquipHelper->GetEquipClass(updatedPerk);
	if (equipClass) {
		ATrDevice* equipDef = (ATrDevice*)equipClass->Default;
		if (equipDef) {
			FString updateStr = that->Concat_StrStr(equipDef->ItemName, FString(L" EQUIPPED"));
			mp->UpdateStatus(mp->Caps(updateStr));
		}
	}

	that->RefreshButtons();
}

void GFxTrPage_Equip_FillOption(UGFxTrPage_Equip* that, UGFxTrPage_Equip_execFillOption_Parms* params, UGFxObject** result, Hooks::CallInfo callInfo) {
	*result = that->FillOption(params->ActionIndex);

	if ((g_TAServerControlClient.getCurrentGameSettingMode() == "ootb") || (that->LoadoutEquipType != EQP_PerkA && that->LoadoutEquipType != EQP_PerkB)) {
		// Don't adjust in OOTB mode, or for non-perk slots
		return;
	}

	// Unlock all perks
	(*result)->SetFloat(FString(L"bLocked"), (float)that->bParentLocked);

	UGFxTrMenuMoviePlayer* mp = (UGFxTrMenuMoviePlayer*)that->Outer;
	UTrEquipInterface* eqpInterface = mp->EquipInterface;

	int itemId = that->PageActions.GetStd(params->ActionIndex)->ActionNumber;

	int encodedPerks = eqpInterface->GetActiveEquipId(that->LoadoutClassId, EQP_Tertiary, that->ActiveLoadout);
	int perkA = Utils::perks_DecodeA(encodedPerks);
	int perkB = Utils::perks_DecodeB(encodedPerks);
	int equipId = that->LoadoutEquipType == EQP_PerkA ? perkA : perkB;

	// Mark this perk as selected if needed
	if (itemId == equipId) {
		(*result)->SetFloat(FString(L"bItemSelected"), 2);
	}
	else {
		(*result)->SetFloat(FString(L"bItemSelected"), 1);
	}
}

static void performClassRename(std::string fiName, FString& friendlyName, FString& abbreviation) {
	std::string baseName = "TrFamilyInfo_" + fiName + " TribesGame.Default__TrFamilyInfo_" + fiName;
	std::string beName = "TrFamilyInfo_" + fiName + "_BE" + " TribesGame.Default__TrFamilyInfo_" + fiName + "_BE";
	std::string dsName = "TrFamilyInfo_" + fiName + "_DS" + " TribesGame.Default__TrFamilyInfo_" + fiName + "_DS";

	std::vector<UTrFamilyInfo*> fis;
	fis.push_back(UObject::FindObject<UTrFamilyInfo>(baseName.c_str()));
	fis.push_back(UObject::FindObject<UTrFamilyInfo>(beName.c_str()));
	fis.push_back(UObject::FindObject<UTrFamilyInfo>(dsName.c_str()));

	static FString laserStr(L"TribesGameContent.TrDevice_LaserTargeter_Content");

	for (auto& fi : fis) {
		fi->FriendlyName = friendlyName;
		fi->Abbreviation = abbreviation;
	}
}

static void performItemRename(std::string itemPrefix, std::string itemClassName, FString& itemName, FString& infoDescription) {
	std::string baseName = itemPrefix + "_" + itemClassName + " TribesGame.Default__" + itemPrefix + "_" + itemClassName;
	ATrDevice* item = UObject::FindObject<ATrDevice>(baseName.c_str());

	item->ItemName = itemName;
	item->InfoPanelDescription = infoDescription;
}

static void performSkinRename(std::string itemClassName, FString& itemName, FString& infoDescription) {
	std::string baseName = "TrSkin_" + itemClassName + " TribesGame.Default__TrSkin_" + itemClassName;
	UTrSkin* item = UObject::FindObject<UTrSkin>(baseName.c_str());

	item->ItemName = itemName;
	item->InfoPanelDescription = infoDescription;
}

// Rename items/classes that changed in OOTB back to their GOTY values
void performGOTYRename() {
	// Light -> Pathfinder
	static FString pthName(L"Pathfinder");
	static FString pthAbbrev(L"PTH");
	performClassRename("Light_Pathfinder", pthName, pthAbbrev);

	// Medium -> Soldier
	static FString sldName(L"Soldier");
	static FString sldAbbrev(L"SLD");
	performClassRename("Medium_Soldier", sldName, sldAbbrev);

	// Heavy -> Juggernaut
	static FString jugName(L"Juggernaut");
	static FString jugAbbrev(L"JUG");
	performClassRename("Heavy_Juggernaught", jugName, jugAbbrev);

	// Default skins
	performSkinRename("Pathfinder", pthName, FString());
	performSkinRename("Soldier", sldName, FString());
	performSkinRename("Juggernaut", jugName, FString());


	// Spare Spin
	static FString spareSpinName(L"Spare Spinfusor");
	static FString spareSpinInfo(L"Some Soldiers like to bring an extra Spinfusor as a secondary. This variant has a reduced direct-hit damage bonus compared to Thumpers or other Spinfusors, but retains all other benefits of the Disk.");
	performItemRename("TrDevice", "Spinfusor_100X", spareSpinName, spareSpinInfo);

	// Devastator Spin
	static FString devastatorSpinName(L"Devastator Spinfusor");
	static FString devastatorSpinInfo(L"A favorite among heavies, the Devastator variant deals a little less damage overall, but packs a significantly harder punch on a direct hit, and has a larger explosion radius.");
	performItemRename("TrDevice", "HeavySpinfusor_MKD", devastatorSpinName, devastatorSpinInfo);

	//// Light Utility Pack -> Lightweight Pack
	//static FString lightUtilName(L"Lightweight Pack");
	//static FString lightUtilInfo(L"Reduces your mass by 30% and gives 25 extra energy");
	//performItemRename("TrDevice", "ERechargePack_Sentinel", lightUtilName, lightUtilInfo);

	// Light Energy Pack -> Energy Recharge Pack
	//static FString lightEnergyName(L"Light Energy Pack");
	//static FString lightEnergyInfo(L"Improves energy regeneration by 18%");
	//performItemRename("TrDevice", "ERechargePack_Pathfinder", lightEnergyName, lightEnergyInfo);

	// Set up perk lists correctly
	UTrPerkList* perkListDef = (UTrPerkList*)UTrPerkList::StaticClass()->Default;

	perkListDef->PerkListA.Clear();
	perkListDef->PerkListA.Add(FString(L"TrPerk_Rage"));
	perkListDef->PerkListA.Add(FString(L"TrPerk_SuperCapacitor"));
	perkListDef->PerkListA.Add(FString(L"TrPerk_Reach"));
	perkListDef->PerkListA.Add(FString(L"TrPerk_Looter"));
	perkListDef->PerkListA.Add(FString(L"TrPerk_SafeFall"));
	perkListDef->PerkListA.Add(FString(L"TrPerk_WheelDeal"));
	perkListDef->PerkListA.Add(FString(L"TrPerk_BountyHunter"));
	perkListDef->PerkListA.Add(FString(L"TrPerk_CloseCombat"));
	perkListDef->PerkListA.Add(FString(L"TrPerk_SafetyThird"));
	perkListDef->PerkListA.Add(FString(L"TrPerk_Stealthy"));

	perkListDef->PerkListB.Clear();
	perkListDef->PerkListB.Add(FString(L"TrPerk_SonicPunch"));
	perkListDef->PerkListB.Add(FString(L"TrPerk_PotentialEnergy"));
	perkListDef->PerkListB.Add(FString(L"TrPerk_Determination"));
	perkListDef->PerkListB.Add(FString(L"TrPerk_Egocentric"));
	perkListDef->PerkListB.Add(FString(L"TrPerk_Pilot"));
	perkListDef->PerkListB.Add(FString(L"TrPerk_Survivalist"));
	perkListDef->PerkListB.Add(FString(L"TrPerk_SuperHeavy"));
	perkListDef->PerkListB.Add(FString(L"TrPerk_UltraCapacitor"));
	perkListDef->PerkListB.Add(FString(L"TrPerk_QuickDraw"));
	perkListDef->PerkListB.Add(FString(L"TrPerk_Mechanic"));
	perkListDef->PerkListB.Add(FString(L"TrPerk_Lightweight"));


}

// Rename items/classes to what they should be in OOTB
void performOOTBRename() {
	// Light -> Pathfinder
	static FString pthName(L"Light");
	static FString pthAbbrev(L"LHT");
	performClassRename("Light_Pathfinder", pthName, pthAbbrev);

	// Medium -> Soldier
	static FString sldName(L"Medium");
	static FString sldAbbrev(L"MED");
	performClassRename("Medium_Soldier", sldName, sldAbbrev);

	// Heavy -> Juggernaut
	static FString jugName(L"Heavy");
	static FString jugAbbrev(L"HVY");
	performClassRename("Heavy_Juggernaught", jugName, jugAbbrev);

	// Default skins
	performSkinRename("Pathfinder", pthName, FString());
	performSkinRename("Soldier", sldName, FString());
	performSkinRename("Juggernaut", jugName, FString());


	// Spare Spin
	static FString spareSpinName(L"Blinksfusor");
	static FString spareSpinInfo(L"100 percent inheritence variant of the spinfusor");
	performItemRename("TrDevice", "Spinfusor_100X", spareSpinName, spareSpinInfo);

	// Devastator Spin
	static FString devastatorSpinName(L"Heavy Blinksfusor");
	static FString devastatorSpinInfo(L"100 percent inheritence variant of the heavy spinfusor");
	performItemRename("TrDevice", "HeavySpinfusor_MKD", devastatorSpinName, devastatorSpinInfo);

	//// Light Utility Pack -> Lightweight Pack
	//static FString lightUtilName(L"Lightweight Pack");
	//static FString lightUtilInfo(L"Reduces your mass by 30% and gives 25 extra energy");
	//performItemRename("TrDevice", "ERechargePack_Sentinel", lightUtilName, lightUtilInfo);

	// Light Energy Pack -> Energy Recharge Pack
	//static FString lightEnergyName(L"Light Energy Pack");
	//static FString lightEnergyInfo(L"Improves energy regeneration by 18%");
	//performItemRename("TrDevice", "ERechargePack_Pathfinder", lightEnergyName, lightEnergyInfo);
}

static void setupGameSettingMode(std::string gameSettingMode) {
	if (gameSettingMode == "ootb") {
		performOOTBRename();
	}
	else {
		performGOTYRename();
	}

	initialiseMenus(Utils::tr_menuMovie, gameSettingMode);
}

void TAServerControl::Client::handle_ModeInfoMessage(const json& j) {
	ModeInfoMessage msg;

	if (!msg.fromJson(j)) {
		Logger::log("Failed to read mode info control message");
		return;
	}

	gameSettingMode = msg.game_setting_mode;
	setupGameSettingMode(gameSettingMode);
	Logger::log("Switched game mode to %s", gameSettingMode.c_str());

	//if (Utils::tr_menuMovie) {
	//	Utils::tr_menuMovie->eventSetPlayerInLogin(false);
	//}
}