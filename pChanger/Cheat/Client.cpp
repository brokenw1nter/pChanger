#include "Client.h"
#include <ctime>
#include <Shlobj.h>
#include "../ConfigPop/cfgmsgpop.h"
#include <stdint.h>
#include "../ImGui/imgui.h"


//[enc_string_enable /]
//[junk_enable /]




namespace Client
{
	//[swap_lines
	int	iScreenWidth = 0;
	int	iScreenHeight = 0;

	char* username = getenv("USERPROFILE");

	//string BaseDir = std::string(username) + "\\Documents\\pChanger";
	string BaseDir = std::string(username) + "\\Documents\\pChanger";
	string LogFile = "";
	string GuiFile = "";
	string IniFile = "";

	vector<string> ConfigList;

	Vector2D	g_vCenterScreen = Vector2D(0.f, 0.f);

	CPlayers*	g_pPlayers = nullptr;
	CRender*	g_pRender = nullptr;
	CGui*		g_pGui = nullptr;
	CEsp*		g_pEsp = nullptr;
	CSkin*		g_pSkin = nullptr;
	CMisc*		g_pMisc = nullptr;

	CInventoryChanger* g_pInventoryChanger = nullptr;
	CInventoryChanger1* g_pInventoryChanger1 = nullptr;

	bool		bC4Timer = false;
	int			iC4Timer = 40;

	int			iWeaponID = 0;
	int			iWeaponSelectIndex = WEAPON_DEAGLE;
	int			iWeaponSelectSkinIndex = -1;

	float watermarkRainbowSpeed = 0.005f;
	float watermarkScrollSpeed = 2.5f;
	//[/swap_lines]

	void ReadConfigs(LPCTSTR lpszFileName)
	{
		if (!strstr(lpszFileName, "gui.ini"))
		{
			ConfigList.push_back(lpszFileName);
		}
	}

	void RefreshConfigs()
	{
		CHAR my_documents[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents)))
		{
			ConfigList.clear();
			string ConfigDir = string(my_documents) + "\\pChanger\\" "\\*.ini";
			SearchFiles(ConfigDir.c_str(), ReadConfigs, FALSE);
		}
	}

	bool Initialize(IDirect3DDevice9* pDevice)
	{
		// CONSOLE ON OPEN - uncomment if you want it
		/*
		Interfaces::Engine()->ClientCmd_Unrestricted2("clear");
		Interfaces::Engine()->ClientCmd_Unrestricted2("toggleconsole");
		Sleep(1000);
		Interfaces::Engine()->ClientCmd_Unrestricted2("echo [PCHANGER] SYSTEMS INITIALIZED");
		Interfaces::Engine()->ClientCmd_Unrestricted2("echo -----------------------------");
		Interfaces::Engine()->ClientCmd_Unrestricted2("echo [PCHANGER]: JOIN OUR DISCORD AT discord.gg/");
		Interfaces::Engine()->ClientCmd_Unrestricted2("echo");
		Interfaces::Engine()->ClientCmd_Unrestricted2("echo [PCHANGER]: USE THE INSERT KEY TO OPEN THE MENU");
		*/


		g_pPlayers = new CPlayers();
		g_pRender = new CRender(pDevice);
		g_pGui = new CGui();
		g_pEsp = new CEsp();
		g_pSkin = new CSkin();
		g_pMisc = new CMisc();

		g_pInventoryChanger = new CInventoryChanger();

		CHAR my_documents[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents)))
		{
			GuiFile = (string(my_documents) + "\\pChanger\\" + "gui.ini");
			IniFile = (string(my_documents) + "\\pChanger\\" + "settings.ini");
		}

		g_pSkin->InitalizeSkins();

		Settings::LoadSettings(IniFile);

		iWeaponSelectSkinIndex = GetWeaponSkinIndexFromPaintKit(g_SkinChangerCfg[iWeaponSelectIndex].nFallbackPaintKit);

		g_pGui->GUI_Init(pDevice);

		RefreshConfigs();

		return true;
	}

	void Shutdown()
	{
		DELETE_MOD(g_pPlayers);
		DELETE_MOD(g_pRender);
		DELETE_MOD(g_pGui);
		DELETE_MOD(g_pEsp);
		DELETE_MOD(g_pSkin);
		DELETE_MOD(g_pMisc);
	}


	void OnRender()
	{

		

		// if (g_pRender && !Interfaces::Engine()->IsTakingScreenshot() && Interfaces::Engine()->IsActiveApp()) - screenshot shit
		if (g_pRender && Interfaces::Engine()->IsActiveApp())
		{
			g_pRender->BeginRender();

			if (g_pGui)
				g_pGui->GUI_Draw_Elements();

			Interfaces::Engine()->GetScreenSize(iScreenWidth, iScreenHeight);

			g_vCenterScreen.x = iScreenWidth / 2.f;
			g_vCenterScreen.y = iScreenHeight / 2.f;



			if (Settings::Esp::esp_Watermark)
			{
				//bool rainbow; 
				static float rainbow;
				rainbow += watermarkRainbowSpeed;
				if (rainbow > 1.f) rainbow = 0.f;
				static int u = 0;
				if (u <= 100)                g_pRender->Text(15, 15, false, true, Color::FromHSB(rainbow, 1.f, 1.f), "<>pChanger");
				if (u > 100 && u <= 200)    g_pRender->Text(15, 15, false, true, Color::FromHSB(rainbow, 1.f, 1.f), "r<>pChange");
				if (u > 200 && u <= 300)    g_pRender->Text(15, 15, false, true, Color::FromHSB(rainbow, 1.f, 1.f), "er<>pChang");
				if (u > 300 && u <= 400)    g_pRender->Text(15, 15, false, true, Color::FromHSB(rainbow, 1.f, 1.f), "ger<>pChan");
				if (u > 400 && u <= 500)    g_pRender->Text(15, 15, false, true, Color::FromHSB(rainbow, 1.f, 1.f), "nger<>pCha");
				if (u > 500 && u <= 600)    g_pRender->Text(15, 15, false, true, Color::FromHSB(rainbow, 1.f, 1.f), "anger<>pCh");
				if (u > 600 && u <= 700)    g_pRender->Text(15, 15, false, true, Color::FromHSB(rainbow, 1.f, 1.f), "hanger<>pC");
				if (u > 700 && u <= 800)    g_pRender->Text(15, 15, false, true, Color::FromHSB(rainbow, 1.f, 1.f), "Changer<>p");
				if (u > 800 && u <= 900)    g_pRender->Text(15, 15, false, true, Color::FromHSB(rainbow, 1.f, 1.f), "pChanger<>");
				u += watermarkScrollSpeed;
				if (u > 900) u = 0;
			}

			if (Settings::Esp::esp_Cheatbuild)
				g_pRender->Text(15, 45, false, true, Color::White(), "Latest Build: %s : %s", __DATE__, __TIME__);

			if (Settings::Misc::misc_RainbowMenu)
			{
				g_pGui->MenuColor();
			}

			{
				if (g_pEsp)
					g_pEsp->OnRender();

				if (g_pMisc)
				{
					g_pMisc->OnRender();
				}
			}

			std::time_t result = std::time(nullptr);

			if (Settings::Esp::esp_Time)
				g_pRender->Text(15, 30, false, true, Color::White(), std::asctime(std::localtime(&result)));

			g_pRender->EndRender();
		}
	}

	void OnLostDevice()
	{
		if (g_pRender)
			g_pRender->OnLostDevice();

		if (g_pGui)
			ImGui_ImplDX9_InvalidateDeviceObjects();
	}

	void OnResetDevice()
	{
		if (g_pRender)
			g_pRender->OnResetDevice();

		if (g_pGui)
			ImGui_ImplDX9_CreateDeviceObjects();
	}

	void OnRetrieveMessage(void* ecx, void* edx, uint32_t *punMsgType, void *pubDest, uint32_t cubDest, uint32_t *pcubMsgSize)
	{
		g_pInventoryChanger->PostRetrieveMessage(punMsgType, pubDest, cubDest, pcubMsgSize);
	}

	void OnSendMessage(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData)
	{

		void* pubDataMutable = const_cast<void*>(pubData);
		g_pInventoryChanger->PreSendMessage(unMsgType, pubDataMutable, cubData);
	}

	void OnCreateMove(CUserCmd* pCmd)
	{
		if (g_pPlayers && Interfaces::Engine()->IsInGame())
		{
			g_pPlayers->Update();

			if (g_pEsp)
				g_pEsp->OnCreateMove(pCmd);

			if (IsLocalAlive())
			{
				if (!bIsGuiVisible)
				{
					int iWeaponSettingsSelectID = GetWeaponSettingsSelectID();

					if (iWeaponSettingsSelectID >= 0)
						iWeaponID = iWeaponSettingsSelectID;
				}

				if (g_pMisc)
					g_pMisc->OnCreateMove(pCmd);

			}
		}
	}

	void OnFireEventClientSideThink(IGameEvent* pEvent)
	{
		if (!strcmp(pEvent->GetName(), "player_connect_full") ||
			!strcmp(pEvent->GetName(), "round_start") ||
			!strcmp(pEvent->GetName(), "cs_game_disconnected"))
		{
			if (g_pPlayers)
				g_pPlayers->Clear();

			if (g_pEsp)
				g_pEsp->OnReset();
		}

		if (Interfaces::Engine()->IsConnected())
		{
			hitmarker::singleton()->initialize();

			if (g_pEsp)
				g_pEsp->OnEvents(pEvent);

			if (g_pSkin)
				g_pSkin->OnEvents(pEvent);
		}
	}

	void OnFrameStageNotify(ClientFrameStage_t Stage)
	{
		if (Interfaces::Engine()->IsInGame() && Interfaces::Engine()->IsConnected())
		{

			ConVar* sv_cheats = Interfaces::GetConVar()->FindVar("sv_cheats");
			SpoofedConvar* sv_cheats_spoofed = new SpoofedConvar(sv_cheats);
			sv_cheats_spoofed->SetInt(1);


			if (g_pMisc)
				g_pMisc->FrameStageNotify(Stage);

			Skin_OnFrameStageNotify(Stage);
			Gloves_OnFrameStageNotify(Stage);
		}
	}

	void OnDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t &state,
		const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
	{
		if (Interfaces::Engine()->IsInGame() && ctx && pCustomBoneToWorld)
		{
			if (g_pEsp)
				g_pEsp->OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);

			if (g_pMisc)
				g_pMisc->OnDrawModelExecute();
		}
	}

	void OnPlaySound(const Vector* pOrigin, const char* pszSoundName)
	{
		if (!pszSoundName || !Interfaces::Engine()->IsInGame())
			return;

		if (!strstr(pszSoundName, "bulletLtoR") &&
			!strstr(pszSoundName, "rics/ric") &&
			!strstr(pszSoundName, "impact_bullet"))
		{
			if (g_pEsp && IsLocalAlive() && Settings::Esp::esp_Sound && pOrigin)
			{
				if (!GetVisibleOrigin(*pOrigin))
					g_pEsp->SoundEsp.AddSound(*pOrigin);
			}
		}
	}

	void OnPlaySound(const char* pszSoundName)
	{
		if (g_pMisc)
			g_pMisc->OnPlaySound(pszSoundName);
	}

	void OnOverrideView(CViewSetup* pSetup)
	{
		if (g_pMisc)
			g_pMisc->OnOverrideView(pSetup);
	}

	void OnGetViewModelFOV(float& fov)
	{
		if (g_pMisc)
			g_pMisc->OnGetViewModelFOV(fov);
	}

	BOOL DirectoryExists(LPCTSTR szPath)
	{
		DWORD dwAttrib = GetFileAttributes(szPath);

		return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
			(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}


	void ImDrawRectRainbow(int x, int y, int width, int height, float flSpeed, float &flRainbow)
	{
		ImDrawList* windowDrawList = ImGui::GetWindowDrawList();

		Color colColor(0, 0, 0, 255);

		flRainbow += flSpeed;
		if (flRainbow > 1.f) flRainbow = 0.f;//1 0 

		for (int i = 0; i < width; i = i + 1)
		{
			float hue = (1.f / (float)width) * i;
			hue -= flRainbow;
			if (hue < 0.f) hue += 1.f;

			Color colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
			windowDrawList->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height), colRainbow.GetU32());
		}
	}

	void OnRenderGUI()
	{


		ImGui::SetNextWindowSize(ImVec2(840.f, 445.f));
		g_pGui->NameFont();
		if (ImGui::Begin("pChanger - A Changer by TimDaHacka", &bIsGuiVisible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
			g_pRender->DrawFillBox(0, 0, iScreenWidth, iScreenHeight, Color(0, 0, 0, 170));
		{





			static float flRainbow;
			// float flSpeed = 0.001f;
			static float flSpeed = 0.003;
			int curWidth = 3;
			ImVec2 curPos = ImGui::GetCursorPos();
			ImVec2 curWindowPos = ImGui::GetWindowPos();
			curPos.x += curWindowPos.x;
			curPos.y += curWindowPos.y;
			int size = 2;
			int y;
			Interfaces::Engine()->GetScreenSize(y, size);
			ImDrawRectRainbow(curPos.x - 10, curPos.y - 1, ImGui::GetWindowSize().x + size, curPos.y + -4, flSpeed, flRainbow);








			ImGui::BeginGroup();



			ImGui::Separator();
			g_pGui->IconFont();
			const char* tabNames[] = {
				"I" , "B" ,"G" , "C" };

			static int tabOrder[] = { 0 , 1 , 2 , 3 };
			static int tabSelected = 0;
			const bool tabChanged = ImGui::TabLabels(tabNames,
				sizeof(tabNames) / sizeof(tabNames[0]),
				tabSelected, tabOrder);

			ImGui::Columns();

			float SpaceLineTitleOne = 120.f;
			float SpaceLineTitleTwo = 230.f;
			float SpaceLineTitleThr = 460.f;
			float SpaceLineTitleFour = 460.f;


			float SpaceLineBoxOne = 135.f;
			float SpaceLineBoxTwo = 275.f;
			float SpaceLineBoxThr = 410.f;
			float SpaceLineBoxFour = 475.f;


			float SpaceLineOne = 120.f;
			float SpaceLineTwo = 240.f;
			float SpaceLineThr = 360.f;
			float SpaceLineFour = 440.f;

			ImGui::EndGroup();
			ImGui::SameLine();




			if (tabSelected == 0) // Visuals
			{

				g_pGui->bluefont();
				static int otherpages = 0;

				ImGui::BeginGroup();
				ImGui::BeginChild(1, ImVec2(0, 405), true);
				{
					const char* iHitSound[] =
					{
						"Off",
						"Default",
						"Metallic",
						"Roblox 'OOF'",
						"Windows XP",
					};

					const char* material_items[] =
					{
						"Glass",
						"Crystal",
						"Dark Gold",
						"Dark Chrome",
						"Plastic Glass",
						"Velvet",
						"Crystal Blue",
						"Bright Gold"
					};

					const char* armtype_items[] =
					{
						"Arms Only",
						"Arms and Weapon"
					};

					const char* skybox_items[] =
					{
						"cs_baggage_skybox_",
						"cs_tibet",
						"embassy",
						"italy",
						"jungle",
						"nukeblank",
						"office",
						"sky_cs15_daylight01_hdr",
						"sky_cs15_daylight02_hdr",
						"sky_cs15_daylight03_hdr",
						"sky_cs15_daylight04_hdr",
						"sky_csgo_cloudy01",
						"sky_csgo_night02",
						"sky_csgo_night02b",
						"sky_csgo_night_flat",
						"sky_day02_05_hdr",
						"sky_day02_05",
						"sky_dust",
						"sky_l4d_rural02_ldr",
						"sky_venice",
						"vertigo_hdr",
						"vertigoblue_hdr",
						"vertigo",
						"vietnam",
						"amethyst",
						"bartuc_canyon",
						"bartuc_grey_sky",
						"blue1",
						"blue2",
						"blue3",
						"blue5",
						"blue6",
						"cssdefault",
						"dark1",
						"dark2",
						"dark3",
						"dark4",
						"dark5",
						"extreme_glaciation",
						"green1",
						"green2",
						"green3",
						"green4",
						"green5",
						"greenscreen",
						"greysky",
						"night1",
						"night2",
						"night3",
						"night4",
						"night5",
						"orange1",
						"orange2",
						"orange3",
						"orange4",
						"orange5",
						"orange6",
						"persistent_fog",
						"pink1",
						"pink2",
						"pink3",
						"pink4",
						"pink5",
						"polluted_atm",
						"toxic_atm",
						"water_sunset﻿"
					};






					if (ImGui::Button("Page #1", ImVec2(87.0f, 25.0f))) // <---- customize these to your liking.
					{
						otherpages = 0;
					}
					ImGui::SameLine();
					if (ImGui::Button("Page #2", ImVec2(87.0f, 25.0f))) // <---- again, customize to your liking.
					{
						otherpages = 1;
					}

					ImGui::Separator();

					if (otherpages == 0)
					{
						ImGui::BeginGroup();

						ImGui::Spacing();

						ImGui::SliderInt("Bomb Timer", &Settings::Esp::esp_BombTimer, 0, 65);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Shows a bomb timer, Valve Servers = 40s.");

						ImGui::Checkbox("Reveal Ranks", &Settings::Esp::esp_Rank);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Shows the ranks of other players in the scoreboard.");
						ImGui::SameLine(SpaceLineTwo);
						ImGui::Checkbox("Watermark", &Settings::Esp::esp_Watermark);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Renders an animated pChanger text at the top left of your screen.");
						ImGui::SameLine(SpaceLineFour);
						ImGui::Checkbox("No Smoke", &Settings::Misc::misc_NoSmoke);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Removes smoke particles completely, will trigger a SMAC ban!");

						ImGui::Checkbox("No Hands", &Settings::Misc::misc_NoHands);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Stops rendering arms but still renders weapons.");
						ImGui::SameLine(SpaceLineTwo);
						ImGui::Checkbox("Current Time", &Settings::Esp::esp_Time);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Renders the current time at the top left of your screen.");
						ImGui::SameLine(SpaceLineFour);
						ImGui::Checkbox("Wire Smoke", &Settings::Misc::misc_wireframesmoke);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Makes the smoke particles wired, will trigger a SMAC ban!");

						ImGui::Checkbox("Wire Hands", &Settings::Misc::misc_WireHands);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Renders hands with a wireframe.");
						ImGui::SameLine(SpaceLineTwo);
						ImGui::Checkbox("Last Build", &Settings::Esp::esp_Cheatbuild);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Renders the time of the last compilation at the top left of your screen.");
						ImGui::SameLine(SpaceLineFour);
						ImGui::Checkbox("No Flash", &Settings::Misc::misc_NoFlash);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Disables rendering of flash blinding.");

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						static bool sh_save_cfg = false;

						ImGui::Text("Render Effects");
						ImGui::Checkbox("Full Bright", &Settings::Misc::misc_FullBright);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Makes everything as bright as possible, will trigger a SMAC ban!");
						ImGui::SameLine(SpaceLineOne);
						ImGui::Checkbox("Night", &Settings::Esp::esp_NightMode);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Makes everything dark but visible, will trigger a SMAC ban!");
						ImGui::SameLine(SpaceLineTwo);
						ImGui::Checkbox("Chrome", &Settings::Esp::esp_ChromeWorld);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Makes everything chrome and shiny, will trigger a SMAC ban!");
						ImGui::SameLine(SpaceLineThr);
						ImGui::Checkbox("LSD", &Settings::Esp::esp_LSDMode);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Ever wonder what it was like to play CS:GO while on LSD? Will trigger a SMAC ban!");
						ImGui::SameLine(SpaceLineFour);
						ImGui::Checkbox("Minecraft", &Settings::Esp::esp_MinecraftMode);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Makes everything pixelated, will trigger a SMAC ban!");

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						ImGui::Text("Skybox Changer");
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Changes the skybox to one from another map.");
						ImGui::Spacing();
						if (ImGui::Combo("", &Settings::Misc::misc_CurrentSky, skybox_items, IM_ARRAYSIZE(skybox_items)))
						{
							Settings::Misc::misc_SkyName = skybox_items[Settings::Misc::misc_CurrentSky];
						}

						ImGui::Checkbox("Greyscale", &Settings::Misc::misc_Snow);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Makes the map 'grey', will trigger a SMAC ban!");
						ImGui::SameLine(SpaceLineTwo);
						ImGui::Checkbox("No Sky", &Settings::Misc::misc_NoSky);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Removes the sky completely, will trigger a SMAC ban!");
						ImGui::SameLine(SpaceLineFour);
						ImGui::Checkbox("Disable Post Processing", &Settings::Misc::misc_EPostprocess);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Removes a lot of saturation and bloom, giving an FPS boost but will trigger a SMAC ban!");

						ImGui::EndGroup();
					}

					if (otherpages == 1)
					{

						ImGui::BeginGroup();

						ImGui::Spacing();

						static char buf2[128] = { 0 };
						ImGui::Checkbox("Bullet Tracers", &Settings::Esp::esp_beams_tracer); // Only for your own Bullets. You have to wait 15 Seconds after toggle it. Also its kind of buggy.
						if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Leaves a trail after your bullets, will trigger a SMAC ban!");
						ImGui::SliderFloat("Duration", &Settings::Esp::flTracersDuration, 0.f, 10.f, "%.2f");
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("How long the tracers last in seconds.");
						ImGui::SliderFloat("Width", &Settings::Esp::flTracersWidth, 0.f, 10.f, "%.2f");
						ImGui::ColorEdit3("Tracer Color", Settings::Esp::flTracer_Beam);

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						ImGui::Checkbox("Hitmarker", &Settings::Esp::esp_HitMarker);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Visual representation of hitting a shot.");
						ImGui::Combo("Hitmarker Sound", &Settings::Esp::esp_HitMarkerSound, iHitSound, ARRAYSIZE(iHitSound));
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Audial representation of hitting a shot.");
						ImGui::ColorEdit3("Hitmarker Color", Settings::Esp::esp_HitMarkerColor);

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						ImGui::Checkbox("Arm Material", &Settings::Misc::misc_ArmMaterials);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Changes arm or weapon material to something else.");
						ImGui::PushItemWidth(362.f);
						ImGui::Combo("Type", &Settings::Misc::misc_ArmMaterialsType, armtype_items, ARRAYSIZE(armtype_items));
						ImGui::PushItemWidth(362.f);
						ImGui::Combo("Material", &Settings::Misc::misc_ArmMaterialsList, material_items, ARRAYSIZE(material_items));

					}
				}

				ImGui::EndChild();
				ImGui::EndGroup();

			}
			else if (tabSelected == 1) // Skins
			{
				g_pGui->bluefont();
				ImGui::BeginGroup();
				ImGui::BeginChild(1, ImVec2(0, 405), true);
				{

					ImGui::Spacing();



					ImGui::Text("Server-Side Changers");
					ImGui::Spacing();
					ImGui::Checkbox("Profile Changer", &Settings::Misc::window_profilechanger);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Server-Sided - Anyone in your lobby or in game can see it.");



					ImGui::Checkbox("Medal Changer", &Settings::Misc::window_medalchanger);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Server-Sided - Anyone in your lobby or in game can see it.");

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Text("Client-Side Changers");
					ImGui::Spacing();

					ImGui::Checkbox("Skin Changer", &Settings::Misc::misc_SkinChanger);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Client-Sided - You're the only one who can see it.");


					ImGui::Checkbox("Knife Changer", &Settings::Misc::misc_KnifeChanger);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Client-Sided - You're the only one who can see it.");


					ImGui::Checkbox("Sticker Changer", &Settings::Misc::misc_StickerChanger);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Client-Sided - You're the only one who can see it.");

					ImGui::Checkbox("Inventory Changer", &Settings::Misc::window_inventorychanger);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Client-Sided - You're the only one who can see it.");

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Text("Filters");
					ImGui::Checkbox("Skin IDs", &Settings::Misc::skinids);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Weapon IDs", &Settings::Misc::weaponids);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Medal IDs", &Settings::Misc::medalids);
					ImGui::SameLine(SpaceLineThr);
					ImGui::Checkbox("Sticker IDs", &Settings::Misc::stickerids);
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::EndChild();
					ImGui::EndGroup();
				}
			}
			else if (tabSelected == 2) // Misc
			{
				g_pGui->bluefont();
				ImGui::BeginGroup();
				ImGui::BeginChild(1, ImVec2(0, 405), true);
				{

					ImGui::BeginGroup();


					static int otherpages = 0;



					if (ImGui::Button("Page #1", ImVec2(87.0f, 25.0f))) // <---- customize these to your liking.
					{
						otherpages = 0;
					}
					ImGui::SameLine();
					if (ImGui::Button("Page #2", ImVec2(87.0f, 25.0f))) // <---- again, customize to your liking.
					{
						otherpages = 1;
					}

					ImGui::Separator();

					if (otherpages == 0)
					{
						ImGui::BeginGroup();

						ImGui::Spacing();

						ImGui::Checkbox("Rainbow GUI", &Settings::Misc::misc_RainbowMenu);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Changes the GUI's color constantly, no longer works in the Main GUI.");
						ImGui::PushItemWidth(362.f);
						if (ImGui::InputFloat("Fade Speed", &Settings::Misc::misc_RainbowSpeed, 0.001f, 0.01f))
						{
							if (Settings::Misc::misc_RainbowSpeed < 0.001f)
								Settings::Misc::misc_RainbowSpeed = 0.001f;
							if (Settings::Misc::misc_RainbowSpeed > 0.01f)
								Settings::Misc::misc_RainbowSpeed = 0.01f;
						};

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						ImGui::Checkbox("Third Person", &Settings::Misc::misc_ThirdPerson);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Changes your view from first person to third person.");
						ImGui::SliderFloat("##THIRDPERSONRANGE", &Settings::Misc::misc_ThirdPersonRange, 0.f, 100.f, "third person range: %0.f");


						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						ImGui::Checkbox("Auto Bunnyhop", &Settings::Misc::misc_Bhop);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Jumps for you when you hit the ground. Strafing is required still.");
						ImGui::SameLine(SpaceLineTwo);
						ImGui::Checkbox("Auto Accept", &Settings::Misc::misc_AutoAccept);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Automatically presses the ACCEPT button when queuing.");

						ImGui::Spacing();

						ImGui::Checkbox("Inventory Always On", &Settings::Misc::invalwayson);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Allows you to access inventory in competitive match post-warmup.");
						ImGui::SameLine(SpaceLineTwo);
						ImGui::Checkbox("Sniper Crosshair", &Settings::Misc::misc_AwpAim);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Shows your ingame crosshair while holding a sniper.");

						ImGui::Spacing();

						ImGui::Checkbox("Slogan Chat Spam", &Settings::Misc::misc_spamregular);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Spams chat with pChanger's slogan.");
						ImGui::SameLine(SpaceLineTwo);
						ImGui::Checkbox("Random Chat Spam", &Settings::Misc::misc_spamrandom);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Just a fun feature to promote pChanger through chat spamming.");

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						string clan_1 = "None";
						string clan_2 = "Clear";
						string clan_3 = "No-Name";
						string clan_4 = "pChanger";
						string clan_5 = "pChanger No-Name";
						string clan_6 = "Valve";
						string clan_7 = "Valve No-Name";
						string clan_8 = "pChanger Animated";
						const char* items5[] = { clan_1.c_str() , clan_2.c_str() , clan_3.c_str() , clan_4.c_str() , clan_5.c_str() , clan_6.c_str() , clan_7.c_str(), clan_8.c_str(), /*clan_9.c_str()*/ };
						ImGui::Combo("Clantag Changer", &Settings::Misc::misc_Clan, items5, IM_ARRAYSIZE(items5));

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();


						ImGui::Checkbox("Field of View Changer", &Settings::Misc::misc_FovChanger);
						ImGui::PushItemWidth(362.f);
						ImGui::SliderInt("View Position", &Settings::Misc::misc_FovView, 1, 170);
						ImGui::SliderInt("View Model", &Settings::Misc::misc_FovModelView, 1, 190);

						ImGui::EndGroup();
					}
					if (otherpages == 1)
					{

						ImGui::BeginGroup();

						ImGui::Spacing();


						static char buf2[128] = { 0 };
						ImGui::Text("Custom Name");
						ImGui::InputText("##CustomName", buf2, 16, Settings::Misc::misc_NameChange);
						if (ImGui::Button("Apply Custom Name")) {
							ConVar* Name = Interfaces::GetConVar()->FindVar("Name");
							*(int*)((DWORD)&Name->fnChangeCallback + 0xC) = 0;
							Name->SetValue(buf2);
						}

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();



						static char misc_CustomClanTag[128] = { 0 };
						ImGui::Text("Custom Clantag");
						ImGui::InputText("##CustomClanTag", misc_CustomClanTag, 128);

						if (ImGui::Button("Apply Custom Clantag"))
						{
							Engine::ClanTagApply(misc_CustomClanTag);
						}

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						if (ImGui::Button("AyyWare Crasher"))    // Crashes AyyWare's Name ESP/Spectators list. Most AyyWare users already fixed it.
						{

							ConVar* Name = Interfaces::GetConVar()->FindVar("Name");
							*(int*)((DWORD)&Name->fnChangeCallback + 0xC) = 0;

							Name->SetValue("GETCRASHEDAYY?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????");
						}
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Changes your name to crash AyyWare's Name ESP/Spectators list.");

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						ImGui::Text("Custom Models");

						
						ImGui::Checkbox("Enable Custom Models", &Settings::Misc::custommodelson);
						if (ImGui::IsItemHovered())
						ImGui::SetTooltip("WARNING, MAY BE BUGGY AND CAN CAUSE CRASHES!");
						const char* customModelsct[] =
						{
						"Off",
						"Deadpool",
						"Hitler",
						"Donald Trump"

						};

						const char* customModelst[] =
						{
						"Off",
						"Deadpool",
						"Hitler",
						"Donald Trump"

						};

						if (Settings::Misc::custommodelson) {
						ImGui::Combo(("CT Model"), &Settings::Misc::customodelsct, customModelsct, ARRAYSIZE(customModelsct));
						ImGui::Combo(("T Model"), &Settings::Misc::customodelst, customModelst, ARRAYSIZE(customModelst));
						}

						if (ImGui::Button("Update"))
						{
						ForceFullUpdate();
						}
						
					}
					ImGui::EndChild();
					ImGui::EndGroup();


				}

			}
			else if (tabSelected == 3)
			{
				g_pGui->bluefont();
				ImGui::BeginGroup();
				ImGui::BeginChild(1, ImVec2(0, 405), true);
				{

					static int otherpages = 0;



					if (ImGui::Button("Configs", ImVec2(87.0f, 25.0f))) // <---- Customize these to your liking
					{
						otherpages = 0;
					}
					ImGui::SameLine();
					if (ImGui::Button("Settings", ImVec2(87.0f, 25.0f))) // <---- Customize these to your liking
					{
						otherpages = 1;
					}

					ImGui::Separator();


					if (otherpages == 0)
					{
						g_pGui->bluefont();

						ImGui::BeginGroup();
						static int iConfigSelect = 0;
						static int iMenuSheme = 1;
						static char ConfigName[64] = { 0 };
						static bool sh_save_cfg = false;
						static bool sh_load_cfg = false;

						ImGui::ComboBoxArray("Select Config", &iConfigSelect, ConfigList);


						CHAR my_documents[MAX_PATH];
						if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents)))
						{

							if (ImGui::Button("Load Config"))
							{
								sh_load_cfg = true;
								Settings::LoadSettings(string(my_documents) + "\\pChanger\\" + ConfigList[iConfigSelect]);
							}
							ImGui::SameLine();
							if (ImGui::Button("Save Config"))
							{
								Settings::SaveSettings(string(my_documents) + "\\pChanger\\" + ConfigList[iConfigSelect]);
								sh_save_cfg = true;
							}
							ImGui::SameLine();
							if (ImGui::Button("Refresh Config List"))
							{
								RefreshConfigs();
							}
						}
						if (sh_save_cfg)
						{
							//save ur cfg
							bool done = false;
							Anime::Popup("Config Saved", 2000, &done);
							if (done)
								sh_save_cfg = false;
						}

						if (sh_load_cfg)
						{
							//load ur cfg
							bool done = false;
							Anime::Popup("Loaded Config", 2000, &done);
							if (done)
								sh_load_cfg = false;
						}





						ImGui::InputText("Config Name", ConfigName, 64);
						ImGui::Text("If you are making a new config, click 'Create & Save New Config'.");
						ImGui::Text("Configs are saved in 'Documents, pChanger'.");
						if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents)))
						{
							if (ImGui::Button("Create & Save New Config"))
							{
								string ConfigFileName = ConfigName;

								if (ConfigFileName.size() < 1)
								{
									ConfigFileName = "Settings";
								}
								string pChanger = "\\pChanger\\";
								string both = my_documents + pChanger;

								if (!DirectoryExists(both.c_str())) // Only creates directory if it doesn't exist
									CreateDirectoryA(both.c_str(), NULL); // Creates pChanger directory



								Settings::SaveSettings(string(my_documents) + "\\pChanger\\" + ConfigFileName + ".ini");
								RefreshConfigs();
							}
						}


						ImGui::Text(" ");


						const char* ThemesList[] = { "Purple" , "White" , "Light Pink" , "Narcisisti.cc" , "Midnight" , "Night" , "Black/Red" , "Blue"  , "Black" , "Green" , "Yellow" , "Light Blue" , "Light Grey" , "pHook" };

						ImGui::Combo("Menu Theme", &iMenuSheme, ThemesList, IM_ARRAYSIZE(ThemesList));
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Currently bugged and will be fixed.");


						if (ImGui::Button("Apply Color"))
						{
							if (iMenuSheme == 0)
							{
								g_pGui->purple();
							}
							else if (iMenuSheme == 1)
							{
								g_pGui->DefaultSheme1();
							}
							else if (iMenuSheme == 2)
							{
								g_pGui->RedSheme();
							}
							else if (iMenuSheme == 3)
							{
								g_pGui->darkblue();
							}
							else if (iMenuSheme == 4)
							{
								g_pGui->MidNightSheme();
							}
							else if (iMenuSheme == 5)
							{
								g_pGui->NightSheme();
							}
							else if (iMenuSheme == 6)
							{
								g_pGui->DunnoSheme();
							}
							else if (iMenuSheme == 7)
							{
								g_pGui->BlueSheme();
							}
							//else if (iMenuSheme == 10)
							//{
							//	g_pGui->MidNight2();
							//}
							else if (iMenuSheme == 8)
							{
								g_pGui->BlackSheme2();
							}
							else if (iMenuSheme == 9)
							{
								g_pGui->green();
							}
							else if (iMenuSheme == 10)
							{
								g_pGui->pink();
							}
							else if (iMenuSheme == 11)
							{
								g_pGui->blue();
							}
							else if (iMenuSheme == 12)
							{
								g_pGui->yellow();
							}
							else if (iMenuSheme == 13)
							{
								g_pGui->BlackSheme();
							}
						}
						ImGui::EndGroup();

					}
					if (otherpages == 1)
					{

						ImGui::BeginGroup();
						ImGui::Text("Menu");
						if (ImGui::ColorEdit3("Menu Color", Settings::Misc::misc_MenuColor))
							g_pGui->MenuColor();
						if (ImGui::ColorEdit3("Menu Text Color", Settings::Misc::misc_TextColor))
							g_pGui->MenuColor();
						ImGui::SliderFloat("Rainbow Bar Speed", &flSpeed, 0.000f, 0.005f);
						ImGui::SliderFloat("Watermark Rainbow Speed", &watermarkRainbowSpeed, 0.000f, 0.05f);
						ImGui::SliderFloat("Watermark Scroll Speed", &watermarkScrollSpeed, 1.0f, 10.0f);

					}


					ImGui::EndChild();
					ImGui::EndGroup();

				}

			}
			ImGui::EndGroup();
			ImGui::End();
		}


	}
}