#include "Misc.h"


using namespace Client;
//[junk_enable /]
//[enc_string_enable /]
void CMisc::OnRender()
{
	int punch_x = (int)g_vCenterScreen.x + (int)g_pPlayers->GetLocal()->vAimPunch.y * 10;
	int punch_y = (int)g_vCenterScreen.y + (int)g_pPlayers->GetLocal()->vAimPunch.x * 10;
}

void CMisc::OnCreateMove(CUserCmd* pCmd)
{

	CBaseEntity* pPlayer = (CBaseEntity*)Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer());
	int flag = *(PINT)((DWORD)pPlayer + 600); // offset for movetype 

	if (Settings::Misc::misc_Bhop)
	{
		if (!(flag & MOVETYPE_LADDER))
		{
			if (pCmd->buttons & IN_JUMP && !(g_pPlayers->GetLocal()->iFlags & FL_ONGROUND))
			{
				pCmd->buttons &= ~IN_JUMP;
			}
		}
	}

	if (Settings::Misc::misc_spamregular)
		ChatSpamRegular();

	if (Settings::Misc::misc_spamrandom)
		ChatSpamRandom();

	if (Settings::Misc::misc_Clan > 0)
		ClanTag();

	ConVar* skybox = Interfaces::GetConVar()->FindVar("sv_skyname");

	if (Settings::Misc::misc_SkyName) { if (skybox) skybox->SetValue(Settings::Misc::misc_SkyName); }
	if (Settings::Misc::misc_NoSky) { if (skybox) skybox->SetValue("sky_l4d_rural02_ldr"); }

	ConVar* snow = Interfaces::GetConVar()->FindVar("mat_drawgray");

	if (Settings::Misc::misc_Snow)
	{
		snow->SetValue(1);
	}
	else
	{
		snow->SetValue(0);
	}

	ConVar* postprocess = Interfaces::GetConVar()->FindVar("mat_postprocess_enable");

	if (Settings::Misc::misc_EPostprocess)
	{
		if (Interfaces::Engine()->IsInGame())
		{
			postprocess->SetValue(0);
		}
	}
	else
	{
		if (Interfaces::Engine()->IsInGame())
		{
			postprocess->SetValue(1);
		}
	}

	static ConVar* fullbright = Interfaces::GetConVar()->FindVar("mat_fullbright");
	if (Settings::Misc::misc_FullBright)
	{
		if (Interfaces::Engine()->IsInGame())
		{
			fullbright->SetValue(1);
		}
	}
	else {
		if (Interfaces::Engine()->IsInGame())
		{
			fullbright->SetValue(0);
		}
	}
}

void CMisc::FrameStageNotify(ClientFrameStage_t Stage)
{
	CBaseEntity* localplayer = (CBaseEntity*)Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer());

	if (Interfaces::Engine()->IsInGame() && localplayer && Stage == ClientFrameStage_t::FRAME_RENDER_START)
	{
		static QAngle vecAngles;
		Interfaces::Engine()->GetViewAngles(vecAngles);

		if (Settings::Misc::misc_ThirdPerson && !localplayer->IsDead())
		{
			if (!Interfaces::Input()->m_fCameraInThirdPerson)
				Interfaces::Input()->m_fCameraInThirdPerson = true;

			Interfaces::Input()->m_vecCameraOffset = QAngle(vecAngles.x, vecAngles.y, Settings::Misc::misc_ThirdPersonRange);

			*localplayer->GetVAngles() = Settings::Misc::qLastTickAngle;
		}
		else
		{
			if (Interfaces::Input()->m_fCameraInThirdPerson || localplayer->GetIsScoped())
			{
				Interfaces::Input()->m_fCameraInThirdPerson = false;
				Interfaces::Input()->m_vecCameraOffset = QAngle(vecAngles.x, vecAngles.y, 0);
			}
		}
	}
}

void CMisc::OnDrawModelExecute()
{
	static bool NoSmoke = false;
	static bool NoFlashReset = false;
	static bool WireFrameSmoke = false;

	if (Settings::Misc::misc_NoFlash && !NoFlashReset)
	{
		IMaterial* flash = Interfaces::MaterialSystem()->FindMaterial(
			"effects\\flashbang", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* flashWhite = Interfaces::MaterialSystem()->FindMaterial("effects\\flashbang_white",
			TEXTURE_GROUP_CLIENT_EFFECTS);

		if (flash && flashWhite)
		{
			flash->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			flashWhite->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			NoFlashReset = true;
		}
	}
	else if (!Settings::Misc::misc_NoFlash && NoFlashReset)
	{
		IMaterial* flash = Interfaces::MaterialSystem()->FindMaterial(
			"effects\\flashbang", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* flashWhite = Interfaces::MaterialSystem()->FindMaterial("effects\\flashbang_white",
			TEXTURE_GROUP_CLIENT_EFFECTS);

		if (flash && flashWhite)
		{
			flash->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
			flashWhite->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);

			NoFlashReset = false;
		}
	}

	if (Settings::Misc::misc_NoSmoke)
	{
		IMaterial* vistasmokev1_smokegrenade = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_smokegrenade", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* vistasmokev1_emods = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_emods", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* vistasmokev1_emods_impactdust = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_emods_impactdust", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* vistasmokev1_fire = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_fire", TEXTURE_GROUP_CLIENT_EFFECTS);

		if (vistasmokev1_smokegrenade && vistasmokev1_emods && vistasmokev1_emods_impactdust && vistasmokev1_fire)
		{
			vistasmokev1_smokegrenade->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			vistasmokev1_emods->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			vistasmokev1_emods_impactdust->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			vistasmokev1_fire->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			NoSmoke = true;
		}
	}


	else if (!Settings::Misc::misc_NoSmoke)
	{
		IMaterial* vistasmokev1_smokegrenade = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_smokegrenade", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* vistasmokev1_emods = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_emods", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* vistasmokev1_emods_impactdust = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_emods_impactdust", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* vistasmokev1_fire = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_fire", TEXTURE_GROUP_CLIENT_EFFECTS);

		if (vistasmokev1_smokegrenade && vistasmokev1_emods && vistasmokev1_emods_impactdust && vistasmokev1_fire)
		{
			vistasmokev1_smokegrenade->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
			vistasmokev1_emods->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
			vistasmokev1_emods_impactdust->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
			vistasmokev1_fire->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);

			NoSmoke = false;

		}
	}


	if (Settings::Misc::misc_wireframesmoke)

	{
		IMaterial* vistasmokev1_smokegrenade = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_smokegrenade", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* vistasmokev1_emods = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_emods", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* vistasmokev1_emods_impactdust = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_emods_impactdust", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* vistasmokev1_fire = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_fire", TEXTURE_GROUP_CLIENT_EFFECTS);

		if (vistasmokev1_smokegrenade && vistasmokev1_emods && vistasmokev1_emods_impactdust && vistasmokev1_fire)
		{
			vistasmokev1_smokegrenade->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
			vistasmokev1_emods->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
			vistasmokev1_emods_impactdust->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
			vistasmokev1_fire->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);

			WireFrameSmoke = true;
		}
	}

	else if (!Settings::Misc::misc_wireframesmoke)

	{
		IMaterial* vistasmokev1_smokegrenade = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_smokegrenade", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* vistasmokev1_emods = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_emods", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* vistasmokev1_emods_impactdust = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_emods_impactdust", TEXTURE_GROUP_CLIENT_EFFECTS);

		IMaterial* vistasmokev1_fire = Interfaces::MaterialSystem()->FindMaterial("particle/vistasmokev1/vistasmokev1_fire", TEXTURE_GROUP_CLIENT_EFFECTS);

		if (vistasmokev1_smokegrenade && vistasmokev1_emods && vistasmokev1_emods_impactdust && vistasmokev1_fire)
		{
			vistasmokev1_smokegrenade->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
			vistasmokev1_emods->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
			vistasmokev1_emods_impactdust->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
			vistasmokev1_fire->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);

			WireFrameSmoke = false;
		}
	}

}

void CMisc::OnPlaySound(const char* pszSoundName)
{
	if (Settings::Misc::misc_AutoAccept && !strcmp(pszSoundName, "!UI/competitive_accept_beep.wav"))
	{
		typedef void(*IsReadyCallBackFn)();

		IsReadyCallBackFn IsReadyCallBack = 0;

		if (!IsReadyCallBack)
		{
			IsReadyCallBack = (IsReadyCallBackFn)(
				CSX::Memory::FindPattern(CLIENT_DLL, "55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ? ? ? ? 57 83 BE", 0));

#if ENABLE_DEBUG_FILE == 1
			CSX::Log::Add("::IsReadyCallBack = %X", IsReadyCallBack);
#endif
		}

		if (IsReadyCallBack)
		{
			IsReadyCallBack();
		}
	}
}

void CMisc::OnOverrideView(CViewSetup * pSetup)
{
	// if (Settings::Misc::misc_FovChanger && !Interfaces::Engine()->IsTakingScreenshot()) - screenshot shit
	if (Settings::Misc::misc_FovChanger)
	{
		CBaseEntity* pPlayer = (CBaseEntity*)Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer());

		if (!pPlayer)
			return;

		if (pPlayer->GetIsScoped())
			return;

		if (pPlayer->IsDead())
		{
			if (pPlayer->GetObserverMode() == ObserverMode_t::OBS_MODE_IN_EYE && pPlayer->GetObserverTarget())
				pPlayer = (CBaseEntity*)Interfaces::EntityList()->GetClientEntityFromHandle(pPlayer->GetObserverTarget());

			if (!pPlayer)
				return;
		}

		pSetup->fov = (float)Settings::Misc::misc_FovView;
	}
}

void CMisc::OnGetViewModelFOV(float& fov)
{
	// if (Settings::Misc::misc_FovChanger && !Interfaces::Engine()->IsTakingScreenshot()) - screenshot shit
	if (Settings::Misc::misc_FovChanger)
	{
		CBaseEntity* pPlayer = (CBaseEntity*)Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer());

		if (!pPlayer)
			return;

		if (pPlayer->IsDead())
		{
			if (pPlayer->GetObserverMode() == ObserverMode_t::OBS_MODE_IN_EYE && pPlayer->GetObserverTarget())
				pPlayer = (CBaseEntity*)Interfaces::EntityList()->GetClientEntityFromHandle(pPlayer->GetObserverTarget());

			if (!pPlayer)
				return;
		}

		fov = (float)Settings::Misc::misc_FovModelView;
	}
}