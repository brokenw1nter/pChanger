#include "Esp.h"

#include "../../Cheat/BulletBeams/beambullets.h"
#include "../../Cheat/BulletBeams/BeamsInclude.h"
std::vector<cbullet_tracer_info> logs;


using namespace Client;
//[enc_string_enable /]
//[junk_enable /]



enum
{
	PITCH = 0, // up / down
	YAW, // left / right
	ROLL // fall over
};

void inline SinCos(float radians, float* sine, float* cosine)
{
	*sine = sin(radians);
	*cosine = cos(radians);
}

void AngleVectors(const QAngle& angles, Vector* forward)
{
	float sp, sy, cp, cy;

	SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}



CSoundEsp::CSoundEsp()
{
	SoundColor = Color::White();
}

void CSoundEsp::Update()
{
	for (size_t i = 0; i < Sound.size(); i++)
	{
		if (Sound[i].dwTime + 800 <= GetTickCount64())
		{
			Sound.erase(Sound.begin() + i);
		}
	}
}

void CSoundEsp::AddSound(Vector vOrigin)
{
	Sound_s Sound_Entry;

	Sound_Entry.dwTime = GetTickCount64();
	Sound_Entry.vOrigin = vOrigin;

	Sound.push_back(Sound_Entry);
}

void CSoundEsp::DrawSoundEsp()
{
	for (size_t i = 0; i < Sound.size(); i++)
	{
		Vector vScreen;

		if (WorldToScreen(Sound[i].vOrigin, vScreen))
		{
			if (Settings::Esp::esp_Sound)
				g_pRender->DrawBox((int)vScreen.x, (int)vScreen.y, 10, 10, SoundColor);
		}
	}
}



CEsp::CEsp()
{
	fExplodeC4Timer = 0.f;
	fC4Timer = 0.f;
}

Color CEsp::GetPlayerColor(CPlayer* pPlayer)
{
	Color CT_Color(int(Settings::Esp::esp_Color_CT[0] * 255.f), int(Settings::Esp::esp_Color_CT[1] * 255.f), int(Settings::Esp::esp_Color_CT[2] * 255.f));
	Color TT_Color(int(Settings::Esp::esp_Color_TT[0] * 255.f), int(Settings::Esp::esp_Color_TT[1] * 255.f), int(Settings::Esp::esp_Color_TT[2] * 255.f));

	if (pPlayer->Team == TEAM_CT)
	{
		return CT_Color;
	}
	else if (pPlayer->Team == TEAM_TT)
	{
		return TT_Color;
	}

	return Color::White();
}



void CEsp::HitmarkerEvents(IGameEvent* event)
{

	if (!strcmp(event->GetName(), "bullet_impact"))
	{
		CBaseEntity* LocalPlayer = (CBaseEntity*)Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer());

		if (LocalPlayer)
		{
			auto index = Interfaces::Engine()->GetPlayerForUserID(event->GetInt("userid"));

			if (index != Interfaces::Engine()->GetLocalPlayer())
				return;

			auto local = static_cast<CBaseEntity*>(Interfaces::EntityList()->GetClientEntity(index));

			Vector position(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));

			Ray_t ray;

			ray.Init(local->GetEyePosition(), position);

			CTraceFilter filter;

			filter.pSkip = local;

			trace_t tr;
			Interfaces::EngineTrace()->TraceRay(ray, MASK_SHOT, &filter, &tr);

			logs.push_back(cbullet_tracer_info(local->GetEyePosition(), position, Interfaces::GlobalVars()->curtime, Color(255, 255, 255, 255)));

			if (!local)
				return;

			for (size_t i = 0; i < logs.size(); i++)
			{
				auto current = logs.at(i);
				current.color = Color(int(Settings::Esp::flTracer_Beam[0] * 255.f), int(Settings::Esp::flTracer_Beam[1] * 255.f), int(Settings::Esp::flTracer_Beam[2] * 255.f)); //color of local player's tracers
				BeamInfo_t beamInfo;
				beamInfo.m_nType = TE_BEAMPOINTS;
				beamInfo.m_pszModelName = "sprites/physbeam.vmt";
				beamInfo.m_nModelIndex = -1;
				beamInfo.m_flHaloScale = 0.0f;
				beamInfo.m_flLife = Settings::Esp::flTracersDuration;
				beamInfo.m_flWidth = Settings::Esp::flTracersWidth;
				beamInfo.m_flEndWidth = Settings::Esp::flTracersWidth;
				beamInfo.m_flFadeLength = 0.0f;
				beamInfo.m_flAmplitude = 2.0f;
				beamInfo.m_flBrightness = 255.f;
				beamInfo.m_flSpeed = 0.2f;
				beamInfo.m_nStartFrame = 0;
				beamInfo.m_flFrameRate = 0.f;
				beamInfo.m_flRed = current.color.r();
				beamInfo.m_flGreen = current.color.g();
				beamInfo.m_flBlue = current.color.b();
				beamInfo.m_nSegments = 2;
				beamInfo.m_bRenderable = true;
				beamInfo.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

				beamInfo.m_vecStart = LocalPlayer->GetEyePosition();
				beamInfo.m_vecEnd = current.dst;
				if (Settings::Esp::esp_beams_tracer)
				{

					auto beam = shit::g_pViewRenderBeams()->CreateBeamPoints(beamInfo);
					if (beam)
						shit::g_pViewRenderBeams()->DrawBeam(beam);
				}

				logs.erase(logs.begin() + i);
			}


		}
	}


	if (!strcmp(event->GetName(), "player_hurt"))
	{
		if (Settings::Esp::esp_HitMarker)
		{

			int attacker = event->GetInt("attacker");
			if (Interfaces::Engine()->GetPlayerForUserID(attacker) == Interfaces::Engine()->GetLocalPlayer())
			{
				switch (Settings::Esp::esp_HitMarkerSound)
				{
				case 0: break;
				case 1: PlaySoundA(rawData, NULL, SND_ASYNC | SND_MEMORY); break;
				case 2: PlaySoundA(metallic, NULL, SND_ASYNC | SND_MEMORY); break;
				case 3: PlaySoundA(roblox, NULL, SND_ASYNC | SND_MEMORY); break;
				case 4: PlaySoundA(winxp_wav, NULL, SND_ASYNC | SND_MEMORY); break;


				}
				Settings::hitmarkerAlpha = 1.f;
			}
		}
	}
}

void CEsp::renderBeams()
{

	if (Settings::Esp::esp_beams_tracer)
		return;

	auto local = static_cast<CBaseEntity*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer()));

	if (!local)
		return;
	for (size_t i = 0; i < logs.size(); i++)
	{

		auto current = logs.at(i);

		current.color = Color(int(Settings::Esp::flTracer_Beam[0] * 255.f), int(Settings::Esp::flTracer_Beam[1] * 255.f), int(Settings::Esp::flTracer_Beam[2] * 255.f));

		if (Settings::Esp::esp_beams_tracer)
			Interfaces::DebugOverlay()->AddLineOverlay(current.src, current.dst, current.color.r(), current.color.g(), current.color.g(), true, -1.f);


		Interfaces::DebugOverlay()->AddBoxOverlay(current.dst, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), current.color.r(), current.color.g(), current.color.g(), 127, -1.f);

		if (fabs(Interfaces::GlobalVars()->curtime - current.time) > 5.f)
			logs.erase(logs.begin() + i);
	}
}

void CEsp::DrawHitmarker()
{
	if (Settings::hitmarkerAlpha < 0.f)
		Settings::hitmarkerAlpha = 0.f;
	else if (Settings::hitmarkerAlpha > 0.f)
		Settings::hitmarkerAlpha -= 0.01f;

	int W, H;
	Interfaces::Engine()->GetScreenSize(W, H);

	float r = Settings::Esp::esp_HitMarkerColor[0] * 255.f;
	float g = Settings::Esp::esp_HitMarkerColor[1] * 255.f;
	float b = Settings::Esp::esp_HitMarkerColor[2] * 255.f;

	if (Settings::hitmarkerAlpha > 0.f)
	{
		g_pRender->DrawLine(W / 2 - 10, H / 2 - 10, W / 2 - 5, H / 2 - 5, Color(r, g, b, (Settings::hitmarkerAlpha * 255.f)));
		g_pRender->DrawLine(W / 2 - 10, H / 2 + 10, W / 2 - 5, H / 2 + 5, Color(r, g, b, (Settings::hitmarkerAlpha * 255.f)));
		g_pRender->DrawLine(W / 2 + 10, H / 2 - 10, W / 2 + 5, H / 2 - 5, Color(r, g, b, (Settings::hitmarkerAlpha * 255.f)));
		g_pRender->DrawLine(W / 2 + 10, H / 2 + 10, W / 2 + 5, H / 2 + 5, Color(r, g, b, (Settings::hitmarkerAlpha * 255.f)));

	}
}

void hitmarker::player_hurt_listener::start()
{
	if (!Interfaces::GameEvent()->AddListener(this, "player_hurt", false)) {
		throw exception("Failed to register the event");
	}
	if (!Interfaces::GameEvent()->AddListener(this, "bullet_impact", false))
	{
		throw exception("Failed to register the event");
	}
}
void hitmarker::player_hurt_listener::stop()
{
	Interfaces::GameEvent()->RemoveListener(this);
}
void hitmarker::player_hurt_listener::FireGameEvent(IGameEvent *event)
{
	g_pEsp->HitmarkerEvents(event);
}
int hitmarker::player_hurt_listener::GetEventDebugID(void)
{
	return 0x2A;
}

void CEsp::ChromeWorld() //chromeworld you blind nibba
{
	if (Settings::Esp::esp_ChromeWorld)
	{
		ConVar* Chromeworld = Interfaces::GetConVar()->FindVar("r_showenvcubemap");
		if (Chromeworld)
		{
			Chromeworld->SetValue(1);
		}
	}
	else
	{
		ConVar* Chromeworld = Interfaces::GetConVar()->FindVar("r_showenvcubemap");
		if (Chromeworld)
		{
			Chromeworld->SetValue(0);
		}
	}
}

void CEsp::MinecraftMode()
{
	if (Settings::Esp::esp_MinecraftMode)
	{
		ConVar* Minecraftmode = Interfaces::GetConVar()->FindVar("mat_showlowresimage");
		if (Minecraftmode)
		{
			Minecraftmode->SetValue(1);
		}
	}
	else
	{
		ConVar* Minecraftmode = Interfaces::GetConVar()->FindVar("mat_showlowresimage");
		if (Minecraftmode)
		{
			Minecraftmode->SetValue(0);
		}
	}
}

void CEsp::LSDMode()
{
	if (Settings::Esp::esp_LSDMode)
	{
		ConVar* lsdmode = Interfaces::GetConVar()->FindVar("mat_showmiplevels");
		if (lsdmode)
		{
			lsdmode->SetValue(1);
		}
	}
	else
	{
		ConVar* lsdmode = Interfaces::GetConVar()->FindVar("mat_showmiplevels");
		if (lsdmode)
		{
			lsdmode->SetValue(0);
		}
	}
}

//CustomModels
bool PrecacheModel(const char* szModelName)
{
	auto m_pModelPrecacheTable = Interfaces::ClientStringTableContainer()->FindTable("modelprecache");

	if (m_pModelPrecacheTable)
	{
		Interfaces::ModelInfo()->FindOrLoadModel(szModelName);
		int idx = m_pModelPrecacheTable->AddString(false, szModelName);
		if (idx == INVALID_STRING_INDEX)
			return false;
	}
	return true;
}

void CEsp::OnRender()
{
	if (Settings::Esp::esp_HitMarker)
		DrawHitmarker();


	LSDMode();
	MinecraftMode();
	ChromeWorld();
	renderBeams();

	if (Settings::Misc::misc_AwpAim && IsLocalAlive())
	{
		ConVar* crosshair1 = Interfaces::GetConVar()->FindVar("weapon_debug_spread_show");
		crosshair1->SetValue(1);
		ConVar* crosshair2 = Interfaces::GetConVar()->FindVar("weapon_debug_spread_gap");
		crosshair2->SetValue(999999999);
	}
	else
	{
		ConVar* crosshair1 = Interfaces::GetConVar()->FindVar("weapon_debug_spread_show");
		crosshair1->SetValue(0);
		ConVar* crosshair2 = Interfaces::GetConVar()->FindVar("weapon_debug_spread_gap");
		crosshair2->SetValue(0);
	}
	NightMode();

	if (Settings::Esp::esp_BombTimer)
	{
		if (bC4Timer && iC4Timer)
		{
			float fTimeStamp = Interfaces::Engine()->GetLastTimeStamp();

			if (!fExplodeC4Timer)
			{
				fExplodeC4Timer = fTimeStamp + (float)iC4Timer;
			}
			else
			{
				fC4Timer = fExplodeC4Timer - fTimeStamp;

				if (fC4Timer < 0.f)
					fC4Timer = 0.f;
			}
		}
		else
		{
			fExplodeC4Timer = 0.f;
			fC4Timer = 0.f;
		}
	}

	if (Settings::Misc::custommodelson)
	{
		for (int i = 0; i < Interfaces::EntityList()->GetHighestEntityIndex(); i++)
		{
			CBaseEntity* local = (CBaseEntity*)Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer());
			CBaseEntity *entity = (CBaseEntity*)Interfaces::EntityList()->GetClientEntity(i);
			PlayerInfo pinfo;
			if (entity == nullptr)
				continue;
			if (entity == local)
				continue;
			if (entity->IsDormant())
				continue;
			if (Interfaces::Engine()->GetPlayerInfo(i, &pinfo) && !entity->IsDead())
			{
				CustomModels(entity);
			}
		}
	}

}





void CEsp::CustomModels(CBaseEntity* entity)
{
	CBaseEntity* local = (CBaseEntity*)Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer());

	// if (entity->GetTeam() == TEAM_CT && Interfaces::Engine()->IsConnected())
	if (entity->GetTeam() == TEAM_CT)
	{
		switch (Settings::Misc::customodelsct)
		{
		case 0:
			break;

		case 1:
			PrecacheModel("models/pChanger/deadpool/deadpool.mdl");
			entity->SetModelIndex(Interfaces::ModelInfo()->GetModelIndex("models/pChanger/deadpool/deadpool.mdl"));
			break;

		case 2:
			PrecacheModel("models/pChanger/hitler/hitler.mdl");
			entity->SetModelIndex(Interfaces::ModelInfo()->GetModelIndex("models/pChanger/hitler/hitler.mdl"));
			break;

		case 3:
			PrecacheModel("models/pChanger/trump/trump.mdl");
			entity->SetModelIndex(Interfaces::ModelInfo()->GetModelIndex("models/pChanger/trump/trump.mdl"));
			break;
		}
	}
	// else if (entity->GetTeam() == TEAM_TT && Interfaces::Engine()->IsConnected())
	else if (entity->GetTeam() == TEAM_TT)
	{
		switch (Settings::Misc::customodelst)
		{
		case 0:
			break;

		case 1:
			PrecacheModel("models/pChanger/deadpool/deadpool.mdl");
			entity->SetModelIndex(Interfaces::ModelInfo()->GetModelIndex("models/pChanger/deadpool/deadpool.mdl"));
			break;

		case 2:
			PrecacheModel("models/pChanger/hitler/hitler.mdl");
			entity->SetModelIndex(Interfaces::ModelInfo()->GetModelIndex("models/pChanger/hitler/hitler.mdl"));
			break;

		case 3:
			PrecacheModel("models/pChanger/trump/trump.mdl");
			entity->SetModelIndex(Interfaces::ModelInfo()->GetModelIndex("models/pChanger/trump/trump.mdl"));
			break;
		}
	}
}

void MsgFunc_ServerRankRevealAll()
{
	using tServerRankRevealAllFn = bool(__cdecl*)(int*);
	static tServerRankRevealAllFn ServerRankRevealAll = 0;

	if (!ServerRankRevealAll)
	{
		ServerRankRevealAll = (tServerRankRevealAllFn)(
			CSX::Memory::FindPattern(CLIENT_DLL, "55 8B EC 8B 0D ? ? ? ? 68", 0));
	}

	if (ServerRankRevealAll)
	{
		int fArray[3] = { 0,0,0 };
		ServerRankRevealAll(fArray);
	}
}

void CEsp::OnCreateMove(CUserCmd* pCmd)
{
	if (Settings::Esp::esp_Rank && pCmd->buttons & IN_SCORE)
		MsgFunc_ServerRankRevealAll();

	g_pEsp->SoundEsp.Update();
}

void CEsp::OnReset()
{
	g_pEsp->SoundEsp.Sound.clear();

	if (Settings::Esp::esp_BombTimer)
	{
		if (Settings::Esp::esp_BombTimer > 60)
			Settings::Esp::esp_BombTimer = 60;

		bC4Timer = false;
		iC4Timer = Settings::Esp::esp_BombTimer;
	}
}

void CEsp::OnEvents(IGameEvent* pEvent)
{
	if (g_pEsp && Settings::Esp::esp_BombTimer)
	{
		if (!strcmp(pEvent->GetName(), "bomb_defused") || !strcmp(pEvent->GetName(), "bomb_exploded"))
		{
			bC4Timer = false;
		}
		else if (!strcmp(pEvent->GetName(), "bomb_planted"))
		{
			bC4Timer = true;
		}
	}
}

void CEsp::OnDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
{
	// if (!g_pPlayers || Interfaces::Engine()->IsTakingScreenshot() || !Interfaces::Engine()->IsConnected() || !pInfo.pModel) - screenshot shit
	if (!g_pPlayers || !Interfaces::Engine()->IsConnected() || !pInfo.pModel)
		return;

	static bool InitalizeMaterial = false;

	if (!InitalizeMaterial)
	{
		visible_flat = CreateMaterial(true, false);
		visible_tex = CreateMaterial(false, false);
		hidden_flat = CreateMaterial(true, true);
		hidden_tex = CreateMaterial(false, true);

		InitalizeMaterial = true;

		return;
	}

	string strModelName = Interfaces::ModelInfo()->GetModelName(pInfo.pModel);

	if (strModelName.size() <= 1)
		return;

	if (Settings::Misc::misc_ArmMaterials)
	{
		switch (Settings::Misc::misc_ArmMaterialsType)
		{
		case 0: if (strModelName.find("arms") != std::string::npos)
		{
			IMaterial *material;
			switch (Settings::Misc::misc_ArmMaterialsList)
			{
			case 0: material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_OTHER); break; // Glass
			case 1:	material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_OTHER); break; // Crystal
			case 2:	material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/trophy_majors/gold", TEXTURE_GROUP_OTHER); break; // Gold
			case 3:	material = Interfaces::MaterialSystem()->FindMaterial("models/gibs/glass/glass", TEXTURE_GROUP_OTHER); break; // Dark Chrome
			case 4: material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_OTHER); break; // Plastic Glass
			case 5:	material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/trophy_majors/velvet", TEXTURE_GROUP_OTHER); break; // Velvet
			case 6: material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/trophy_majors/crystal_blue", TEXTURE_GROUP_OTHER); break; // Crystal Blue
			case 7: material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/wildfire_gold/wildfire_gold_detail", TEXTURE_GROUP_OTHER); break; // Detailed Gold
			}
			Color color = Color(255, 255, 255, 255);
			ForceMaterial(color, material);
			Interfaces::ModelRender()->DrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);
		} break;
		case 1: if (strModelName.find("weapons/v") != std::string::npos)
		{
			IMaterial *material;
			switch (Settings::Misc::misc_ArmMaterialsList)
			{
			case 0: material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_OTHER); break; // Glass
			case 1:	material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_OTHER); break; // Crystal
			case 2:	material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/trophy_majors/gold", TEXTURE_GROUP_OTHER); break; // Gold
			case 3:	material = Interfaces::MaterialSystem()->FindMaterial("models/gibs/glass/glass", TEXTURE_GROUP_OTHER); break; // Dark Chrome
			case 4: material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_OTHER); break; // Plastic Glass
			case 5:	material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/trophy_majors/velvet", TEXTURE_GROUP_OTHER); break; // Velvet
			case 6: material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/trophy_majors/crystal_blue", TEXTURE_GROUP_OTHER); break; // Crystal Blue
			case 7: material = Interfaces::MaterialSystem()->FindMaterial("models/inventory_items/wildfire_gold/wildfire_gold_detail", TEXTURE_GROUP_OTHER); break; // Detailed Gold
			}
			Color color = Color(255, 255, 255, 255);
			ForceMaterial(color, material);
			Interfaces::ModelRender()->DrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);
		} break;
		}
	}

	if (Settings::Misc::misc_NoHands)
	{
		if (strModelName.find("arms") != string::npos && Settings::Misc::misc_NoHands)
		{
			IMaterial* Hands = Interfaces::MaterialSystem()->FindMaterial(strModelName.c_str(), TEXTURE_GROUP_MODEL);
			Hands->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			Interfaces::ModelRender()->ForcedMaterialOverride(Hands);
		}
	}
	else
	{
		if (strModelName.find("arms") != string::npos)
		{
			IMaterial* Hands = Interfaces::MaterialSystem()->FindMaterial(strModelName.c_str(), TEXTURE_GROUP_MODEL);
			Hands->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
		}
	}

	if (Settings::Misc::misc_WireHands) //Wireframe Hands
	{
		if (strModelName.find("arms") != string::npos)
		{
			IMaterial* WireHands = Interfaces::MaterialSystem()->FindMaterial(strModelName.c_str(), TEXTURE_GROUP_MODEL);
			WireHands->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
			Interfaces::ModelRender()->ForcedMaterialOverride(WireHands);
		}
	}
	else
	{
		if (strModelName.find("arms") != string::npos)
		{
			IMaterial* WireHands = Interfaces::MaterialSystem()->FindMaterial(strModelName.c_str(), TEXTURE_GROUP_MODEL);
			WireHands->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
		}
	}
}

void CEsp::DrawPlayerBulletTrace(CPlayer* pPlayer)
{
	Color EspColor = GetPlayerColor(pPlayer);

	if (!pPlayer->vBulletTraceArray[0].IsZero() && !pPlayer->vBulletTraceArray[1].IsZero())
	{
		g_pRender->DrawLine(
			(int)pPlayer->vBulletTraceArray[0].x, (int)pPlayer->vBulletTraceArray[0].y,
			(int)pPlayer->vBulletTraceArray[1].x, (int)pPlayer->vBulletTraceArray[1].y, EspColor);

		g_pRender->DrawFillBox((int)pPlayer->vBulletTraceArray[1].x - 2, (int)pPlayer->vBulletTraceArray[1].y - 2, 4, 4, EspColor);
	}
}

bool done = false;
void CEsp::NightMode()
{
	if (Settings::Esp::esp_NightMode)
	{
		if (!done)
		{
			static auto sv_skyname = Interfaces::GetConVar()->FindVar("sv_skyname");
			static auto r_DrawSpecificStaticProp = Interfaces::GetConVar()->FindVar("r_DrawSpecificStaticProp");
			r_DrawSpecificStaticProp->SetValue(1);
			sv_skyname->SetValue("sky_csgo_night02");

			for (MaterialHandle_t i = Interfaces::MaterialSystem()->FirstMaterial(); i != Interfaces::MaterialSystem()->InvalidMaterial(); i = Interfaces::MaterialSystem()->NextMaterial(i))
			{
				IMaterial *pMaterial = Interfaces::MaterialSystem()->GetMaterial(i);

				if (!pMaterial)
					continue;

				const char* group = pMaterial->GetTextureGroupName();
				const char* name = pMaterial->GetName();

				if (strstr(group, "World textures"))
				{
					pMaterial->ColorModulate(0.10, 0.10, 0.10);
				}
				if (strstr(group, "StaticProp"))
				{
					pMaterial->ColorModulate(0.30, 0.30, 0.30);
				}
				if (strstr(name, "models/props/de_dust/palace_bigdome"))
				{
					pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
				}
				if (strstr(name, "models/props/de_dust/palace_pillars"))
				{
					pMaterial->ColorModulate(0.30, 0.30, 0.30);
				}

				if (strstr(group, "Particle textures"))
				{
					pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
				}
				done = true;
			}

		}
	}
	else
	{
		if (done)
		{
			for (MaterialHandle_t i = Interfaces::MaterialSystem()->FirstMaterial(); i != Interfaces::MaterialSystem()->InvalidMaterial(); i = Interfaces::MaterialSystem()->NextMaterial(i))
			{
				IMaterial *pMaterial = Interfaces::MaterialSystem()->GetMaterial(i);

				if (!pMaterial)
					continue;

				const char* group = pMaterial->GetTextureGroupName();
				const char* name = pMaterial->GetName();

				if (strstr(group, "World textures"))
				{

					pMaterial->ColorModulate(1, 1, 1);
				}
				if (strstr(group, "StaticProp"))
				{

					pMaterial->ColorModulate(1, 1, 1);
				}
				if (strstr(name, "models/props/de_dust/palace_bigdome"))
				{
					pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
				}
				if (strstr(name, "models/props/de_dust/palace_pillars"))
				{

					pMaterial->ColorModulate(1, 1, 1);
				}
				if (strstr(group, "Particle textures"))
				{
					pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
				}
			}

			done = false;
		}
	}
}