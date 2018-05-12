#include "Gui.h"
#include "../Gui/memoryfonts.h"

using namespace Client;

//[enc_string_enable /]
//[junk_enable /]

ImFont* Tabfont;
ImFont* font;
ImFont* Titlefont;
ImFont* Iconfont;
ImFont* Namefont;

bool bIsGuiInitalize = false;
bool bIsGuiVisible = false;
WNDPROC WndProc_o = nullptr;
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

LRESULT WINAPI GUI_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

CGui::CGui() {}

CGui::~CGui()
{
	ImGui_ImplDX9_Shutdown();
}

void CGui::GUI_Init(IDirect3DDevice9 * pDevice)
{
	HWND hWindow = FindWindowA("Valve001", 0);

	ImGui_ImplDX9_Init(hWindow, pDevice);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	io.IniFilename = GuiFile.c_str();

	font = io.Fonts->AddFontFromMemoryCompressedTTF(
		Font_Font_data, Font_Font_size, 16.f);



	Iconfont = io.Fonts->AddFontFromMemoryCompressedTTF(
		Font_Icon_data, Font_Icon_size, 30.f);

	Namefont = io.Fonts->AddFontFromMemoryCompressedTTF(
		Font_name_data, Font_name_size, 18.f);

	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(8, 8);
	style.WindowMinSize = ImVec2(32, 32);
	style.WindowRounding = 10.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ChildWindowRounding = 10.0f;
	style.FramePadding = ImVec2(4, 2);
	style.FrameRounding = 10.0f;
	style.ItemSpacing = ImVec2(8, 4);
	style.ItemInnerSpacing = ImVec2(8, 8);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 3.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 10.0f;
	style.GrabMinSize = 10.f;
	style.GrabRounding = 5.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.DisplayWindowPadding = ImVec2(22, 22);
	style.DisplaySafeAreaPadding = ImVec2(4, 4);
	style.AntiAliasedLines = true;
	style.AntiAliasedShapes = true;
	style.CurveTessellationTol = 1.25f;

	blue();

	ImGui_ImplDX9_CreateDeviceObjects();

	WndProc_o = (WNDPROC)SetWindowLongA(hWindow, GWL_WNDPROC, (LONG)(LONG_PTR)GUI_WndProc);

	bIsGuiInitalize = true;
}

void CGui::GUI_Begin_Render()
{
	ImGui_ImplDX9_NewFrame();
}

void CGui::GUI_End_Render()
{
	ImGui::Render();
}

bool SendClientHello()
{
	CMsgClientHello Message;

	Message.set_client_session_need(1);
	Message.clear_socache_have_versions();

	void* ptr = malloc(Message.ByteSize() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCClientHello | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	Message.SerializeToArray((void*)((DWORD)ptr + 8), Message.ByteSize());

	bool result = Interfaces::SteamGameCoordinator()->SendMessage(k_EMsgGCClientHello | ((DWORD)1 << 31), ptr, Message.ByteSize() + 8) == k_EGCResultOK;

	free(ptr);

	return result;
}

bool SendMMHello()
{
	CMsgGCCStrike15_v2_MatchmakingClient2GCHello Message;
	void* ptr = malloc(Message.ByteSize() + 8);
	if (!ptr)
		return false;

	auto unMsgType = k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31);
	((uint32_t*)ptr)[0] = unMsgType;
	((uint32_t*)ptr)[1] = 0;

	Message.SerializeToArray((void*)((DWORD)ptr + 8), Message.ByteSize());

	bool result = Interfaces::SteamGameCoordinator()->SendMessage(k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31), ptr, Message.ByteSize() + 8) == k_EGCResultOK;

	free(ptr);
	return result;
}

LRESULT WINAPI GUI_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static bool is_down = false;
	static bool is_clicked = false;
	static bool check_closed = false;

	if (GUI_KEY_DOWN(VK_INSERT))
	{
		is_clicked = false;
		is_down = true;
	}
	else if (!GUI_KEY_DOWN(VK_INSERT) && is_down)
	{
		is_clicked = true;
		is_down = false;
	}
	else
	{
		is_clicked = false;
		is_down = false;
	}

	if (!bIsGuiVisible && !is_clicked && check_closed)
	{
		string msg = "cl_mouseenable " + to_string(!bIsGuiVisible);
		Interfaces::Engine()->ClientCmd_Unrestricted2(msg.c_str());
		check_closed = false;
	}

	if (is_clicked)
	{
		bIsGuiVisible = !bIsGuiVisible;

		string msg = "cl_mouseenable " + to_string(!bIsGuiVisible);
		Interfaces::Engine()->ClientCmd_Unrestricted2(msg.c_str());

		if (!check_closed)
			check_closed = true;
	}

	if (bIsGuiVisible && ImGui_ImplDX9_WndProcHandler(hwnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProcA(WndProc_o, hwnd, uMsg, wParam, lParam);
}

void CGui::GUI_Draw_Elements()
{
	// if (!bIsGuiInitalize || Interfaces::Engine()->IsTakingScreenshot() || !Interfaces::Engine()->IsActiveApp()) - screenshot shit
	if (!bIsGuiInitalize || !Interfaces::Engine()->IsActiveApp())
		return;

	g_pGui->GUI_Begin_Render();

	ImGui::GetIO().MouseDrawCursor = bIsGuiVisible;

	bool bOpenTimer = (bIsGuiVisible || (bC4Timer && iC4Timer));

	if (g_pEsp && Settings::Esp::esp_BombTimer && bOpenTimer)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);

		ImGui::SetNextWindowSize(ImVec2(125.f, 30.f));

		if (ImGui::Begin("Bomb Timer", &bOpenTimer,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
		{
			ImGui::Text(C4_TIMER_STRING, g_pEsp->fC4Timer);
			ImGui::End();
		}

		ImGui::GetStyle().WindowMinSize = OldMinSize;
	}

	if (Settings::Misc::misc_SkinChanger && bIsGuiVisible)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);
		ImGui::SetNextWindowSize(ImVec2(500.f, 340.f));

		if (ImGui::Begin("Skin Changer", &Settings::Misc::misc_SkinChanger,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{

			const char* quality_items[] =
			{
				"Normal","Genuine","Vintage","Unusual","Community","Developer",
				"Self-Made","Customized","Strange","Completed","Tournament"
			};

			const char* gloves_listbox_items[49] =
			{
				"Default",
				"Sport Gloves | Superconductor",
				"Sport Gloves | Pandora's Box",
				"Sport Gloves | Hedge Maze",
				"Sport Gloves | Arid",
				"Sport Gloves | Vice",
				"Sport Gloves | Omega",
				"Sport Gloves | Bronze Morph",
				"Sport Gloves | Amphibious",
				"Moto Gloves | Eclipse",
				"Moto Gloves | Spearmint",
				"Moto Gloves | Boom!",
				"Moto Gloves | Cool Mint",
				"Moto Gloves | Polygon",
				"Moto Gloves | Transport",
				"Moto Gloves | Turtle",
				"Moto Gloves | POW!",
				"Specialist Gloves | Crimson Kimono",
				"Specialist Gloves | Emerald Web",
				"Specialist Gloves | Foundation",
				"Specialist Gloves | Forest DDPAT",
				"Specialist Gloves | Mogul",
				"Specialist Gloves | Fade",
				"Specialist Gloves | Buckshot",
				"Specialist Gloves | Crimson Web",
				"Driver Gloves | Lunar Weave",
				"Driver Gloves | Convoy",
				"Driver Gloves | Crimson Weave",
				"Driver Gloves | Diamondback",
				"Driver Gloves | Racing Green",
				"Driver Gloves | Overtake",
				"Driver Gloves | Imperial Plaid",
				"Driver Gloves | King Snake",
				"Hand Wraps | Leather",
				"Hand Wraps | Spruce DDPAT",
				"Hand Wraps | Badlands",
				"Hand Wraps | Slaughter",
				"Hand Wraps | Aboreal",
				"Hand Wraps | Duct Tape",
				"Hand Wraps | Overprint",
				"Hand Wraps | Cobalt Skulls",
				"Bloodhound Gloves | Charred",
				"Bloodhound Gloves | Snakebite",
				"Bloodhound Gloves | Bronzed",
				"Bloodhound Gloves | Guerrilla",
				"Hydra Gloves | Case Hardened",
				"Hydra Gloves | Rattler",
				"Hydra Gloves | Mangrove",
				"Hydra Gloves | Emerald",
			};
			//[enc_string_enable /]

			const char* skins_items[] =
			{
				"", "",  "2: Groundwater","3: Candy Apple", "",
				"5: Forest Ddpat","6: Arctic Camo",  "",
				"8: Desert Storm","9: Bengal Tiger","10: Copperhead",
				"11: Skulls","12: Crimson Web","13: Blue Streak","14: Red Laminate"
				,"15: Gunsmoke","16: Jungle Tiger","17: Urban Ddpat",  "",
				"", "20: Virus","21: Granite Marbleized","22: Contrast Spray",
				"",  "", "25: Forest Leaves","26: Lichen Dashed",
				"27: Bone Mask","28: Anodized Navy",  "",  "30: Snake Camo",
				"",  "32: Silver","33: Hot Rod","34: Metallic Ddpat",  "",
				"36: Ossified","37: Blaze","38: Fade","39: Bulldozer","40: Night",
				"41: Copper","42: Blue Steel","43: Stained","44: Case Hardened",
				"",  "46: Contractor","47: Colony","48: Dragon Tattoo",
				"",   "",  "51: Lightning Strike",  "",
				"",   "",   "",   "",   "",
				"",    "59: Slaughter","60: Dark Water"
				,"61: Hypnotic","62: Bloomstick",  "",   ""
				,   "",   "",  "67: Cold Blooded",  "",   ""
				,  "70: Carbon Fiber","71: Scorpion","72: Safari Mesh","73: Wings","74: Polar Camo"
				,"75: Blizzard Marbleized","76: Winter Forest","77: Boreal Forest","78: Forest Night"
				,  "",   "",   "",   "",  "83: Orange Ddpat","84: Pink Ddpat"
				,  "",   "",   "",   "",  "",  "90: Mudder",  ""
				,  "92: Cyanospatter","93: Caramel",  "", "95: Grassland","96: Blue Spruce",  ""
				,  "98: Ultraviolet","99: Sand Dune","100: Storm","101: Tornado","102: Whiteout",  ""
				,  "104: Grassland Leaves",  "",   "", "107: Polar Mesh",  "",   ""
				, "110: Condemned","111: Glacier Mesh",  "",   "",   "",   "",  "116: Sand Mesh",  "",   "", "119: Sage Spray",  "",   "", "122: Jungle Spray",  "",  "124: Sand Spray",  ""
				,  "",  "",  "",  "",  "",  "",  "",  "",  "", "135: Urban Perforated"
				,"136: Waves Perforated",  "",   "",   "",  "", "141: Orange Peel",  "",  "143: Urban Masked", "", "", "", "147: Jungle Dashed"
				,"148: Sand Dashed","149: Urban Dashed", "", "151: Jungle", "", "153: Demolition","154: Afterimage","155: Bullet Rain","156: Death by Kitty","157: Palm","158: Walnut","159: Brass", "", "", "162: Splash", "","164: Modern Hunter","165: Splash Jam","166: Blaze Orange","167: Radiation Hazard","168: Nuclear Threat","169: Fallout Warning","170: Predator","171: Irradiated Alert","172: Black Laminate", "","174: Boom","175: Scorched","176: Faded Zebra","177: Memento","178: Doomkitty","179: Nuclear Threat","180: Fire Serpent","181: Corticera","182: Emerald Dragon","183: Overgrowth","184: Corticera","185: Golden Koi","186: Wave Spray","187: Zirka","188: Graven","189: Bright Water","190: Black Limba","191: Tempest","192: Shattered","193: Bone Pile","194: Spitfire","195: Demeter","196: Emerald","197: Anodized Navy","198: Hazard","199: Dry Season","200: Mayan Dreams","201: Palm","202: Jungle Ddpat","203: Rust Coat","204: Mosaico","205: Jungle","206: Tornado","207: Facets","208: Sand Dune","209: Groundwater","210: Anodized Gunmetal","211: Ocean Foam","212: Graphite","213: Ocean Foam","214: Graphite","215: X-Ray","216: Blue Titanium","217: Blood Tiger","218: Hexane","219: Hive","220: Hemoglobin","221: Serum","222: Blood in the Water","223: Nightshade","224: Water Sigil","225: Ghost Camo","226: Blue Laminate","227: Electric Hive","228: Blind Spot","229: Azure Zebra","230: Steel Disruption","231: Cobalt Disruption","232: Crimson Web","233: Tropical Storm","234: Ash Wood","235: Varicamo","236: Night Ops","237: Urban Rubble","238: Varicamo Blue", "", "240: Calicamo","241: Hunting Blind","242: Army Mesh","243: Gator Mesh","244: Teardown","245: Army Recon","246: Amber Fade","247: Damascus Steel","248: Red Quartz","249: Cobalt Quartz","250: Full Stop","251: Pit Viper","252: Silver Quartz","253: Acid Fade","254: Nitro","255: Asiimov","256: The Kraken","257: Guardian","258: Mehndi","259: Redline","260: Pulse","261: Marina","262: Rose Iron","263: Rising Skull","264: Sandstorm","265: Kami","266: Magma","267: Cobalt Halftone","268: Tread Plate","269: The Fuschia Is Now","270: Victoria","271: Undertow","272: Titanium Bit","273: Heirloom","274: Copper Galaxy","275: Red Fragcam","276: Panther","277: Stainless","278: Blue Fissure","279: Asiimov","280: Chameleon","281: Corporal","282: Redline","283: Trigon","284: Heat","285: Terrain","286: Antique","287: Pulse","288: Sergeant","289: Sandstorm","290: Guardian","291: Heaven Guard", "", "293: Death Rattle","294: Green Apple","295: Franklin","296: Meteorite","297: Tuxedo","298: Army Sheen","299: Caged Steel","300: Emerald Pinstripe","301: Atomic Alloy","302: Vulcan","303: Isaac","304: Slashed","305: Torque","306: Antique","307: Retribution","308: Kami","309: Howl","310: Curse","311: Desert Warfare","312: Cyrex","313: Orion","314: Heaven Guard","315: Poison Dart","316: Jaguar","317: Bratatat","318: Road Rash","319: Detour","320: Red Python","321: Master Piece","322: Nitro","323: Rust Coat", "", "325: Chalice","326: Knight","327: Chainmail","328: Hand Cannon","329: Dark Age","330: Briar", "", "332: Royal Blue","333: Indigo","334: Twist","335: Module","336: Desert-Strike","337: Tatter","338: Pulse","339: Caiman","340: Jet Set","341: First Class","342: Leather","343: Commuter","344: Dragon Lore","345: First Class","346: Coach Class","347: Pilot","348: Red Leather","349: Osiris","350: Tigris","351: Conspiracy","352: Fowl Play","353: Water Elemental","354: Urban Hazard","355: Desert-Strike","356: Koi","357: Ivory","358: Supernova","359: Asiimov","360: Cyrex","361: Abyss","362: Labyrinth","363: Traveler","364: Business Class","365: Olive Plaid","366: Green Plaid","367: Reactor","368: Setting Sun","369: Nuclear Waste","370: Bone Machine","371: Styx","372: Nuclear Garden","373: Contamination","374: Toxic","375: Radiation Hazard","376: Chemical Green","377: Hot Shot","378: Fallout Warning","379: Cerberus","380: Wasteland Rebel","381: Grinder","382: Murky","383: Basilisk","384: Griffin","385: Firestarter","386: Dart","387: Urban Hazard","388: Cartel","389: Fire Elemental","390: Highwayman","391: Cardiac","392: Delusion","393: Tranquility","394: Cartel","395: Man-O'-War","396: Urban Shock","397: Naga","398: Chatterbox","399: Catacombs","400: 龍王 (Dragon King)","401: System Lock","402: Malachite","403: Deadly Poison","404: Muertos","405: Serenity","406: Grotto","407: Quicksilver","", "409: Tiger Tooth","410: Damascus Steel","411: Damascus Steel", "", "413: Marble Fade","414: Rust Coat","415: Doppler","416: Doppler","417: Doppler","418: Doppler","419: Doppler","420: Doppler","421: Doppler","422: Elite Build","423: Armor Core","424: Worm God","425: Bronze Deco","426: Valence","427: Monkey Business","428: Eco","429: Djinn","430: Hyper Beast","431: Heat","432: Man-O'-War","433: Neon Rider","434: Origami","435: Pole Position","436: Grand Prix","437: Twilight Galaxy","438: Chronos","439: Hades","440: Icarus Fell","441: Minotaur's Labyrinth","442: Asterion","443: Pathfinder","444: Daedalus","445: Hot Rod","446: Medusa","447: Duelist","448: Pandora's Box","449: Poseidon","450: Moon in Libra","451: Sun in Leo","452: Shipping Forecast","453: Emerald","454: Para Green","455: Akihabara Accept","456: Hydroponic","457: Bamboo Print","458: Bamboo Shadow","459: Bamboo Forest","460: Aqua Terrace", "", "462: Counter Terrace","463: Terrace","464: Neon Kimono","465: Orange Kimono","466: Crimson Kimono","467: Mint Kimono","468: Midnight Storm","469: Sunset Storm 壱","470: Sunset Storm 弐","471: Daybreak","472: Impact Drill","473: Seabird","474: Aquamarine Revenge","475: Hyper Beast","476: Yellow Jacket","477: Neural Net","478: Rocket Pop","479: Bunsen Burner","480: Evil Daimyo","481: Nemesis","482: Ruby Poison Dart","483: Loudmouth","484: Ranger","485: Handgun","486: Elite Build","487: Cyrex","488: Riot","489: Torque","490: Frontside Misty","491: Dualing Dragons","492: Survivor Z","493: Flux","494: Stone Cold","495: Wraiths","496: Nebula Crusader","497: Golden Coil","498: Rangeen","499: Cobalt Core","500: Special Delivery","501: Wingshot","502: Green Marine","503: Big Iron","504: Kill Confirmed","505: Scumbria","506: Point Disarray","507: Ricochet","508: Fuel Rod","509: Corinthian","510: Retrobution","511: The Executioner","512: Royal Paladin", "", "514: Power Loader","515: Imperial","516: Shapewood","517: Yorick","518: Outbreak","519: Tiger Moth","520: Avalanche","521: Teclu Burner","522: Fade","523: Amber Fade","524: Fuel Injector","525: Elite Build","526: Photic Zone","527: Kumicho Dragon","528: Cartel","529: Valence","530: Triumvirate", "", "532: Royal Legion","533: The Battlestar","534: Lapis Gator","535: Praetorian","536: Impire","537: Hyper Beast","538: Necropos","539: Jambiya","540: Lead Conduit","541: Fleet Flock","542: Judgement of Anubis","543: Red Astor","544: Ventilators","545: Orange Crash","546: Firefight","547: Spectre","548: Chantico's Fire","549: Bioleak","550: Oceanic","551: Asiimov","552: Fubar","553: Atlas","554: Ghost Crusader","555: Re-Entry","556: Primal Saber","557: Black Tie","558: Lore","559: Lore","560: Lore","561: Lore","562: Lore","563: Black Laminate","564: Black Laminate","565: Black Laminate","566: Black Laminate","567: Black Laminate","568: Gamma Doppler","569: Gamma Doppler","570: Gamma Doppler","571: Gamma Doppler","572: Gamma Doppler","573: Autotronic","574: Autotronic","575: Autotronic","576: Autotronic","577: Autotronic","578: Bright Water","579: Bright Water","580: Freehand","581: Freehand","582: Freehand","583: Aristocrat","584: Phobos","585: Violent Daimyo","586: Wasteland Rebel","587: Mecha Industries","588: Desolate Space","589: Carnivore","590: Exo","591: Imperial Dragon","592: Iron Clad","593: Chopper","594: Harvester","595: Reboot","596: Limelight","597: Bloodsport","598: Aerial","599: Ice Cap","600: Neon Revolution","601: Syd Mead","602: Imprint","603: Directive","604: Roll Cage","605: Scumbria","606: Ventilator","607: Weasel","608: Petroglyph","609: Airlock","610: Dazzle","611: Grim","612: Powercore","613: Triarch","614: Fuel Injector","615: Briefing","616: Slipstream","617: Doppler","618: Doppler","619: Doppler","620: Ultraviolet","621: Ultraviolet","622: Polymer","623: Ironwork","624: Dragonfire","625: Royal Consorts","626: Mecha Industries","627: Cirrus","628: Stinger","629: Black Sand","630: Sand Scale","631: Flashback","632: Buzz Kill","633: Sonar","634: Gila","635: Turf","636: Shallow Grave","637: Cyrex","638: Wasteland Princess","639: Bloodsport","640: Fever Dream","641: Jungle Slipstream","642: Blueprint","643: Xiangliu","644: Decimator","645: Oxide Blaze","646: Capillary","647: Crimson Tsunami","648: Emerald Poison Dart","649: Akoben","650: Ripple","651: Last Dive","652: Scaffold","653: Neo-Noir","654: Seasons","655: Zander","656: Orbit mk01","657: Blueprint","658: Cobra Strike","659: Macabre","660: Hyper Beast","661: Sugar Rush","662: Oni Taiji","663: Briefing","664: Hellfire","665: Aloha","666: Hard Water","667: Woodsman","668: Red Rock","669: Death Grip","670: Death's Head","671: Cut out","672: Metal Flowers","673: Morris","674: Triqua","675: The Empress","676: High Roller","677: Hunter","678: See Ya Later","679: Goo","680: Off World","681: Leaded Glass","682: Oceanic","683: Llama Cannon","684: Cracked Opal","685: Jungle Slipstream","686: Phantom","687: Tacticat","688: Exposure","689: Ziggy"
			};

			static const char* weapons[] = {
				"none",
				"1 - Desert Eagle",
				"2 - Dual Berettas",
				"3 - Five-Seven",
				"4 - Glock-18",
				"",
				"",
				"7 - AK-47",
				"8 - AUG",
				"9 - AWP",
				"10 - Famas",
				"11 - G3SG1",
				"",
				"13 - Galil AR",
				"14 - M249",
				"",
				"16 - M4A4",
				"17 - MAC-10",
				"",
				"19 - P90",
				"",
				"",
				"",
				"",
				"24 - UMP-45",
				"25 - XM1014",
				"26 - PP-Bizon",
				"27 - MAG-7",
				"28 - Negev",
				"29 - Sawed-Off",
				"30 - Tec-9",
				" ",
				"32 - P2000",
				"33 - MP7",
				"34 - MP9",
				"35 - Nova",
				"36 - P250",
				"38 - SCAR-20",
				"39 - SG553",
				"40 - SSG 08",
			};
			static int itemDefinitionIndex = 0;

			ImGui::Text("Skin Changer");


			ImGui::Text("Current Weapon: %s", pWeaponData[iWeaponID]);

			ImGui::PushItemWidth(362.f);

			static int iOldWeaponID = -1;

			ImGui::InputInt("Weapon##WeaponSelect", &iWeaponID);
			ImGui::Combo("Weapons", &itemDefinitionIndex, weapons, IM_ARRAYSIZE(weapons));

			iWeaponSelectIndex = pWeaponItemIndexData[iWeaponID];

			if (iOldWeaponID != iWeaponID)
				iWeaponSelectSkinIndex = GetWeaponSkinIndexFromPaintKit(g_SkinChangerCfg[iWeaponSelectIndex].nFallbackPaintKit);

			iOldWeaponID = iWeaponID;

			string WeaponSkin = pWeaponData[iWeaponID];
			WeaponSkin += " Skin";

			ImGui::ComboBoxArray(WeaponSkin.c_str(), &iWeaponSelectSkinIndex, WeaponSkins[iWeaponID].SkinNames);

			ImGui::Combo("Weapon Quality", &g_SkinChangerCfg[pWeaponItemIndexData[iWeaponID]].iEntityQuality, quality_items, IM_ARRAYSIZE(quality_items));
			ImGui::SliderFloat("Weapon Wear", &g_SkinChangerCfg[pWeaponItemIndexData[iWeaponID]].flFallbackWear, 0.001f, 1.f);
			ImGui::InputInt("Weapon Seed", &g_SkinChangerCfg[pWeaponItemIndexData[iWeaponID]].nFallbackSeed, 1, 9999999999999999999, ImGuiInputTextFlags_CharsDecimal);
			ImGui::InputInt("Weapon StatTrak", &g_SkinChangerCfg[pWeaponItemIndexData[iWeaponID]].nFallbackStatTrak, 1, 100, ImGuiInputTextFlags_CharsDecimal);
			ImGui::Combo("Weapon Skin", &g_SkinChangerCfg[pWeaponItemIndexData[iWeaponID]].nFallbackPaintKit, skins_items, IM_ARRAYSIZE(skins_items));
			ImGui::InputInt("Weapon Skin ID", &g_SkinChangerCfg[pWeaponItemIndexData[iWeaponID]].nFallbackPaintKit, 1, 689, ImGuiInputTextFlags_CharsDecimal);
			ImGui::Checkbox("Skin Filter List", &Settings::Misc::skinids);


			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Combo("Gloves", &Settings::Skin::gloves_skin, gloves_listbox_items,
				IM_ARRAYSIZE(gloves_listbox_items));


			ImGui::PopItemWidth();

			if (ImGui::Button("Apply##Skin"))
			{
				ForceFullUpdate();
			}
			ImGui::End();
		}

		ImGui::GetStyle().WindowMinSize = OldMinSize;
	}

	if (Settings::Misc::misc_KnifeChanger && bIsGuiVisible)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);

		ImGui::SetNextWindowSize(ImVec2(500.f, 380.f));

		if (ImGui::Begin("Knife Changer", &Settings::Misc::misc_KnifeChanger,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{

			const char* knife_models_items[] =
			{
				"Default","Bayonet","Flip","Gut","Karambit" ,"M9 Bayonet",
				"Huntsman","Falchion","Bowie","Butterfly","Shadow Daggers"
			};

			const char* quality_items[] =
			{
				"Normal","Genuine","Vintage","Unusual","Community","Developer",
				"Self-Made","Customized","Strange","Completed","Tournament"
			};

			const char* skins_items[] =
			{
				"", "",  "2: Groundwater","3: Candy Apple", "",
				"5: Forest Ddpat","6: Arctic Camo",  "",
				"8: Desert Storm","9: Bengal Tiger","10: Copperhead",
				"11: Skulls","12: Crimson Web","13: Blue Streak","14: Red Laminate"
				,"15: Gunsmoke","16: Jungle Tiger","17: Urban Ddpat",  "",
				"", "20: Virus","21: Granite Marbleized","22: Contrast Spray",
				"",  "", "25: Forest Leaves","26: Lichen Dashed",
				"27: Bone Mask","28: Anodized Navy",  "",  "30: Snake Camo",
				"",  "32: Silver","33: Hot Rod","34: Metallic Ddpat",  "",
				"36: Ossified","37: Blaze","38: Fade","39: Bulldozer","40: Night",
				"41: Copper","42: Blue Steel","43: Stained","44: Case Hardened",
				"",  "46: Contractor","47: Colony","48: Dragon Tattoo",
				"",   "",  "51: Lightning Strike",  "",
				"",   "",   "",   "",   "",
				"",    "59: Slaughter","60: Dark Water"
				,"61: Hypnotic","62: Bloomstick",  "",   ""
				,   "",   "",  "67: Cold Blooded",  "",   ""
				,  "70: Carbon Fiber","71: Scorpion","72: Safari Mesh","73: Wings","74: Polar Camo"
				,"75: Blizzard Marbleized","76: Winter Forest","77: Boreal Forest","78: Forest Night"
				,  "",   "",   "",   "",  "83: Orange Ddpat","84: Pink Ddpat"
				,  "",   "",   "",   "",  "",  "90: Mudder",  ""
				,  "92: Cyanospatter","93: Caramel",  "", "95: Grassland","96: Blue Spruce",  ""
				,  "98: Ultraviolet","99: Sand Dune","100: Storm","101: Tornado","102: Whiteout",  ""
				,  "104: Grassland Leaves",  "",   "", "107: Polar Mesh",  "",   ""
				, "110: Condemned","111: Glacier Mesh",  "",   "",   "",   "",  "116: Sand Mesh",  "",   "", "119: Sage Spray",  "",   "", "122: Jungle Spray",  "",  "124: Sand Spray",  ""
				,  "",  "",  "",  "",  "",  "",  "",  "",  "", "135: Urban Perforated"
				,"136: Waves Perforated",  "",   "",   "",  "", "141: Orange Peel",  "",  "143: Urban Masked", "", "", "", "147: Jungle Dashed"
				,"148: Sand Dashed","149: Urban Dashed", "", "151: Jungle", "", "153: Demolition","154: Afterimage","155: Bullet Rain","156: Death by Kitty","157: Palm","158: Walnut","159: Brass", "", "", "162: Splash", "","164: Modern Hunter","165: Splash Jam","166: Blaze Orange","167: Radiation Hazard","168: Nuclear Threat","169: Fallout Warning","170: Predator","171: Irradiated Alert","172: Black Laminate", "","174: Boom","175: Scorched","176: Faded Zebra","177: Memento","178: Doomkitty","179: Nuclear Threat","180: Fire Serpent","181: Corticera","182: Emerald Dragon","183: Overgrowth","184: Corticera","185: Golden Koi","186: Wave Spray","187: Zirka","188: Graven","189: Bright Water","190: Black Limba","191: Tempest","192: Shattered","193: Bone Pile","194: Spitfire","195: Demeter","196: Emerald","197: Anodized Navy","198: Hazard","199: Dry Season","200: Mayan Dreams","201: Palm","202: Jungle Ddpat","203: Rust Coat","204: Mosaico","205: Jungle","206: Tornado","207: Facets","208: Sand Dune","209: Groundwater","210: Anodized Gunmetal","211: Ocean Foam","212: Graphite","213: Ocean Foam","214: Graphite","215: X-Ray","216: Blue Titanium","217: Blood Tiger","218: Hexane","219: Hive","220: Hemoglobin","221: Serum","222: Blood in the Water","223: Nightshade","224: Water Sigil","225: Ghost Camo","226: Blue Laminate","227: Electric Hive","228: Blind Spot","229: Azure Zebra","230: Steel Disruption","231: Cobalt Disruption","232: Crimson Web","233: Tropical Storm","234: Ash Wood","235: Varicamo","236: Night Ops","237: Urban Rubble","238: Varicamo Blue", "", "240: Calicamo","241: Hunting Blind","242: Army Mesh","243: Gator Mesh","244: Teardown","245: Army Recon","246: Amber Fade","247: Damascus Steel","248: Red Quartz","249: Cobalt Quartz","250: Full Stop","251: Pit Viper","252: Silver Quartz","253: Acid Fade","254: Nitro","255: Asiimov","256: The Kraken","257: Guardian","258: Mehndi","259: Redline","260: Pulse","261: Marina","262: Rose Iron","263: Rising Skull","264: Sandstorm","265: Kami","266: Magma","267: Cobalt Halftone","268: Tread Plate","269: The Fuschia Is Now","270: Victoria","271: Undertow","272: Titanium Bit","273: Heirloom","274: Copper Galaxy","275: Red Fragcam","276: Panther","277: Stainless","278: Blue Fissure","279: Asiimov","280: Chameleon","281: Corporal","282: Redline","283: Trigon","284: Heat","285: Terrain","286: Antique","287: Pulse","288: Sergeant","289: Sandstorm","290: Guardian","291: Heaven Guard", "", "293: Death Rattle","294: Green Apple","295: Franklin","296: Meteorite","297: Tuxedo","298: Army Sheen","299: Caged Steel","300: Emerald Pinstripe","301: Atomic Alloy","302: Vulcan","303: Isaac","304: Slashed","305: Torque","306: Antique","307: Retribution","308: Kami","309: Howl","310: Curse","311: Desert Warfare","312: Cyrex","313: Orion","314: Heaven Guard","315: Poison Dart","316: Jaguar","317: Bratatat","318: Road Rash","319: Detour","320: Red Python","321: Master Piece","322: Nitro","323: Rust Coat", "", "325: Chalice","326: Knight","327: Chainmail","328: Hand Cannon","329: Dark Age","330: Briar", "", "332: Royal Blue","333: Indigo","334: Twist","335: Module","336: Desert-Strike","337: Tatter","338: Pulse","339: Caiman","340: Jet Set","341: First Class","342: Leather","343: Commuter","344: Dragon Lore","345: First Class","346: Coach Class","347: Pilot","348: Red Leather","349: Osiris","350: Tigris","351: Conspiracy","352: Fowl Play","353: Water Elemental","354: Urban Hazard","355: Desert-Strike","356: Koi","357: Ivory","358: Supernova","359: Asiimov","360: Cyrex","361: Abyss","362: Labyrinth","363: Traveler","364: Business Class","365: Olive Plaid","366: Green Plaid","367: Reactor","368: Setting Sun","369: Nuclear Waste","370: Bone Machine","371: Styx","372: Nuclear Garden","373: Contamination","374: Toxic","375: Radiation Hazard","376: Chemical Green","377: Hot Shot","378: Fallout Warning","379: Cerberus","380: Wasteland Rebel","381: Grinder","382: Murky","383: Basilisk","384: Griffin","385: Firestarter","386: Dart","387: Urban Hazard","388: Cartel","389: Fire Elemental","390: Highwayman","391: Cardiac","392: Delusion","393: Tranquility","394: Cartel","395: Man-O'-War","396: Urban Shock","397: Naga","398: Chatterbox","399: Catacombs","400: 龍王 (Dragon King)","401: System Lock","402: Malachite","403: Deadly Poison","404: Muertos","405: Serenity","406: Grotto","407: Quicksilver","", "409: Tiger Tooth","410: Damascus Steel","411: Damascus Steel", "", "413: Marble Fade","414: Rust Coat","415: Doppler","416: Doppler","417: Doppler","418: Doppler","419: Doppler","420: Doppler","421: Doppler","422: Elite Build","423: Armor Core","424: Worm God","425: Bronze Deco","426: Valence","427: Monkey Business","428: Eco","429: Djinn","430: Hyper Beast","431: Heat","432: Man-O'-War","433: Neon Rider","434: Origami","435: Pole Position","436: Grand Prix","437: Twilight Galaxy","438: Chronos","439: Hades","440: Icarus Fell","441: Minotaur's Labyrinth","442: Asterion","443: Pathfinder","444: Daedalus","445: Hot Rod","446: Medusa","447: Duelist","448: Pandora's Box","449: Poseidon","450: Moon in Libra","451: Sun in Leo","452: Shipping Forecast","453: Emerald","454: Para Green","455: Akihabara Accept","456: Hydroponic","457: Bamboo Print","458: Bamboo Shadow","459: Bamboo Forest","460: Aqua Terrace", "", "462: Counter Terrace","463: Terrace","464: Neon Kimono","465: Orange Kimono","466: Crimson Kimono","467: Mint Kimono","468: Midnight Storm","469: Sunset Storm 壱","470: Sunset Storm 弐","471: Daybreak","472: Impact Drill","473: Seabird","474: Aquamarine Revenge","475: Hyper Beast","476: Yellow Jacket","477: Neural Net","478: Rocket Pop","479: Bunsen Burner","480: Evil Daimyo","481: Nemesis","482: Ruby Poison Dart","483: Loudmouth","484: Ranger","485: Handgun","486: Elite Build","487: Cyrex","488: Riot","489: Torque","490: Frontside Misty","491: Dualing Dragons","492: Survivor Z","493: Flux","494: Stone Cold","495: Wraiths","496: Nebula Crusader","497: Golden Coil","498: Rangeen","499: Cobalt Core","500: Special Delivery","501: Wingshot","502: Green Marine","503: Big Iron","504: Kill Confirmed","505: Scumbria","506: Point Disarray","507: Ricochet","508: Fuel Rod","509: Corinthian","510: Retrobution","511: The Executioner","512: Royal Paladin", "", "514: Power Loader","515: Imperial","516: Shapewood","517: Yorick","518: Outbreak","519: Tiger Moth","520: Avalanche","521: Teclu Burner","522: Fade","523: Amber Fade","524: Fuel Injector","525: Elite Build","526: Photic Zone","527: Kumicho Dragon","528: Cartel","529: Valence","530: Triumvirate", "", "532: Royal Legion","533: The Battlestar","534: Lapis Gator","535: Praetorian","536: Impire","537: Hyper Beast","538: Necropos","539: Jambiya","540: Lead Conduit","541: Fleet Flock","542: Judgement of Anubis","543: Red Astor","544: Ventilators","545: Orange Crash","546: Firefight","547: Spectre","548: Chantico's Fire","549: Bioleak","550: Oceanic","551: Asiimov","552: Fubar","553: Atlas","554: Ghost Crusader","555: Re-Entry","556: Primal Saber","557: Black Tie","558: Lore","559: Lore","560: Lore","561: Lore","562: Lore","563: Black Laminate","564: Black Laminate","565: Black Laminate","566: Black Laminate","567: Black Laminate","568: Gamma Doppler","569: Gamma Doppler","570: Gamma Doppler","571: Gamma Doppler","572: Gamma Doppler","573: Autotronic","574: Autotronic","575: Autotronic","576: Autotronic","577: Autotronic","578: Bright Water","579: Bright Water","580: Freehand","581: Freehand","582: Freehand","583: Aristocrat","584: Phobos","585: Violent Daimyo","586: Wasteland Rebel","587: Mecha Industries","588: Desolate Space","589: Carnivore","590: Exo","591: Imperial Dragon","592: Iron Clad","593: Chopper","594: Harvester","595: Reboot","596: Limelight","597: Bloodsport","598: Aerial","599: Ice Cap","600: Neon Revolution","601: Syd Mead","602: Imprint","603: Directive","604: Roll Cage","605: Scumbria","606: Ventilator","607: Weasel","608: Petroglyph","609: Airlock","610: Dazzle","611: Grim","612: Powercore","613: Triarch","614: Fuel Injector","615: Briefing","616: Slipstream","617: Doppler","618: Doppler","619: Doppler","620: Ultraviolet","621: Ultraviolet","622: Polymer","623: Ironwork","624: Dragonfire","625: Royal Consorts","626: Mecha Industries","627: Cirrus","628: Stinger","629: Black Sand","630: Sand Scale","631: Flashback","632: Buzz Kill","633: Sonar","634: Gila","635: Turf","636: Shallow Grave","637: Cyrex","638: Wasteland Princess","639: Bloodsport","640: Fever Dream","641: Jungle Slipstream","642: Blueprint","643: Xiangliu","644: Decimator","645: Oxide Blaze","646: Capillary","647: Crimson Tsunami","648: Emerald Poison Dart","649: Akoben","650: Ripple","651: Last Dive","652: Scaffold","653: Neo-Noir","654: Seasons","655: Zander","656: Orbit mk01","657: Blueprint","658: Cobra Strike","659: Macabre","660: Hyper Beast","661: Sugar Rush","662: Oni Taiji","663: Briefing","664: Hellfire","665: Aloha","666: Hard Water","667: Woodsman","668: Red Rock","669: Death Grip","670: Death's Head","671: Cut out","672: Metal Flowers","673: Morris","674: Triqua","675: The Empress","676: High Roller","677: Hunter","678: See Ya Later","679: Goo","680: Off World","681: Leaded Glass","682: Oceanic","683: Llama Cannon","684: Cracked Opal","685: Jungle Slipstream","686: Phantom","687: Tacticat","688: Exposure","689: Ziggy"
			};

			ImGui::Text("Knife Changer");

			ImGui::Combo("Knife CT Model", &Settings::Skin::knf_ct_model, knife_models_items, IM_ARRAYSIZE(knife_models_items));
			ImGui::Combo("Knife T Model", &Settings::Skin::knf_tt_model, knife_models_items, IM_ARRAYSIZE(knife_models_items));
			ImGui::Combo("Knife CT Skin Combo", &Settings::Skin::knf_ct_skin, skins_items, IM_ARRAYSIZE(skins_items));
			ImGui::InputInt("Knife CT Skin Input", &Settings::Skin::knf_ct_skin, 1, 689, ImGuiInputTextFlags_CharsDecimal);
			ImGui::Combo("Knife T Skin Combo", &Settings::Skin::knf_tt_skin, skins_items, IM_ARRAYSIZE(skins_items));
			ImGui::InputInt("Knife T Skin Input", &Settings::Skin::knf_tt_skin, 1, 689, ImGuiInputTextFlags_CharsDecimal);
			ImGui::Checkbox("Skin Filter List", &Settings::Misc::skinids);

			static int iSelectKnifeCTSkinIndex = -1;
			static int iSelectKnifeTTSkinIndex = -1;

			int iKnifeCTModelIndex = Settings::Skin::knf_ct_model;
			int iKnifeTTModelIndex = Settings::Skin::knf_tt_model;

			static int iOldKnifeCTModelIndex = -1;
			static int iOldKnifeTTModelIndex = -1;

			if (iOldKnifeCTModelIndex != iKnifeCTModelIndex && Settings::Skin::knf_ct_model)
				iSelectKnifeCTSkinIndex = GetKnifeSkinIndexFromPaintKit(Settings::Skin::knf_ct_skin, false);

			if (iOldKnifeTTModelIndex != iKnifeTTModelIndex && Settings::Skin::knf_tt_model)
				iSelectKnifeTTSkinIndex = GetKnifeSkinIndexFromPaintKit(Settings::Skin::knf_ct_skin, true);

			iOldKnifeCTModelIndex = iKnifeCTModelIndex;
			iOldKnifeTTModelIndex = iKnifeTTModelIndex;

			string KnifeCTModel = knife_models_items[Settings::Skin::knf_ct_model];
			string KnifeTTModel = knife_models_items[Settings::Skin::knf_tt_model];

			KnifeCTModel += " Skin##KCT";
			KnifeTTModel += " Skin##KTT";

			ImGui::SliderFloat("Knife CT Wear", &g_SkinChangerCfg[WEAPON_KNIFE].flFallbackWear, 0.001f, 1.f);
			ImGui::Combo("Knife CT Quality", &g_SkinChangerCfg[WEAPON_KNIFE].iEntityQuality, quality_items, IM_ARRAYSIZE(quality_items));
			ImGui::InputInt("Knife CT Seed", &g_SkinChangerCfg[WEAPON_KNIFE].nFallbackSeed, 1, 9999999999999999999, ImGuiInputTextFlags_CharsDecimal);
			ImGui::ComboBoxArray(KnifeCTModel.c_str(), &iSelectKnifeCTSkinIndex, KnifeSkins[iKnifeCTModelIndex].SkinNames);


			ImGui::SliderFloat("Knife T Wear", &g_SkinChangerCfg[WEAPON_KNIFE_T].flFallbackWear, 0.001f, 1.f);
			ImGui::Combo("Knife T Quality", &g_SkinChangerCfg[WEAPON_KNIFE_T].iEntityQuality, quality_items, IM_ARRAYSIZE(quality_items));
			ImGui::InputInt("Knife T Seed", &g_SkinChangerCfg[WEAPON_KNIFE_T].nFallbackSeed, 1, 9999999999999999999, ImGuiInputTextFlags_CharsDecimal);
			ImGui::ComboBoxArray(KnifeTTModel.c_str(), &iSelectKnifeTTSkinIndex, KnifeSkins[iKnifeTTModelIndex].SkinNames);

			if (ImGui::Button("Apply##Skin"))
			{
				ForceFullUpdate();
			}
			ImGui::End();
		}

		ImGui::GetStyle().WindowMinSize = OldMinSize;
	}

	/*
	if (Settings::Misc::misc_StickerChanger && bIsGuiVisible)
	{
	ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

	ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);

	ImGui::SetNextWindowSize(ImVec2(500.f, 295.f));

	Settings::Sticker::stickerSafeWeapon = SafeWeaponID();
	Stick[Settings::Sticker::stickerSafeWeapon].StickersEnabled = true;

	if (ImGui::Begin("Sticker Changer", &Settings::Misc::misc_StickerChanger,
	ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
	static int iSlot = 0;
	const char* slots[] ={ "Back", "Back-Mid", "Front-Mid", "Front" };

	ImGui::Combo("Slot", &iSlot, slots, IM_ARRAYSIZE(slots));
	ImGui::SliderFloat("Wear ", &Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear, 0.f, 1.f, "%.3f");
	ImGui::SliderFloat("Size", &Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale, 0.f, 1.f, "%.3f");
	ImGui::SliderInt("Rotate", &Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation, 0, 360);
	ImGui::InputInt("ID", &Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID);

	if (ImGui::Button("Accept Stickers##Skin"))
	{
	switch (Settings::Sticker::stickerSafeWeapon)
	{
	case 1:
	Settings::Sticker::sticker_deserteagle_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_deserteagle_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_deserteagle_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_deserteagle_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_deserteagle_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 2:
	Settings::Sticker::sticker_elites_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_elites_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_elites_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_elites_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_elites_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 3:
	Settings::Sticker::sticker_fiveseven_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_fiveseven_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_fiveseven_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_fiveseven_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_fiveseven_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 4:
	Settings::Sticker::sticker_glock_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_glock_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_glock_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_glock_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_glock_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 7:
	Settings::Sticker::sticker_ak_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_ak_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_ak_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_ak_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_ak_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 8:
	Settings::Sticker::sticker_aug_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_aug_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_aug_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_aug_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_aug_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 9:
	Settings::Sticker::sticker_awp_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_awp_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_awp_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_awp_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_awp_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 10:
	Settings::Sticker::sticker_famas_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_famas_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_famas_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_famas_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_famas_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 11:
	Settings::Sticker::sticker_g35g1_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_g35g1_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_g35g1_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_g35g1_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_g35g1_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 13:
	Settings::Sticker::sticker_galil_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_galil_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_galil_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_galil_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_galil_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 14:
	Settings::Sticker::sticker_m249_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_m249_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_m249_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_m249_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_m249_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 16:
	Settings::Sticker::sticker_m4a4_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_m4a4_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_m4a4_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_m4a4_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_m4a4_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 17:
	Settings::Sticker::sticker_mac10_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_mac10_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_mac10_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_mac10_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_mac10_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 19:
	Settings::Sticker::sticker_p90_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_p90_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_p90_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_p90_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_p90_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 24:
	Settings::Sticker::sticker_ump_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_ump_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_ump_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_ump_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_ump_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 25:
	Settings::Sticker::sticker_xm_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_xm_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_xm_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_xm_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_xm_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 26:
	Settings::Sticker::sticker_bizon_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_bizon_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_bizon_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_bizon_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_bizon_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 27:
	Settings::Sticker::sticker_mag7_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_mag7_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_mag7_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_mag7_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_mag7_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 28:
	Settings::Sticker::sticker_negev_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_negev_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_negev_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_negev_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_negev_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 29:
	Settings::Sticker::sticker_sawedoff_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_sawedoff_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_sawedoff_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_sawedoff_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_sawedoff_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 30:
	Settings::Sticker::sticker_tec_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_tec_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_tec_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_tec_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_tec_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 32:
	Settings::Sticker::sticker_p2000_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_p2000_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_p2000_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_p2000_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_p2000_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 33:
	Settings::Sticker::sticker_mp7_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_mp7_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_mp7_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_mp7_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_mp7_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 34:
	Settings::Sticker::sticker_mp9_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_mp9_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_mp9_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_mp9_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_mp9_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 35:
	Settings::Sticker::sticker_nova_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_nova_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_nova_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_nova_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_nova_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 36:
	Settings::Sticker::sticker_p250_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_p250_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_p250_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_p250_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_p250_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 38:
	Settings::Sticker::sticker_scar_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_scar_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_scar_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_scar_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_scar_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 39:
	Settings::Sticker::sticker_sg_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_sg_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_sg_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_sg_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_sg_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	case 40:
	Settings::Sticker::sticker_ssg_iSlot[iSlot] = iSlot;
	Settings::Sticker::sticker_ssg_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
	Settings::Sticker::sticker_ssg_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
	Settings::Sticker::sticker_ssg_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
	Settings::Sticker::sticker_ssg_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
	break;
	}

	ForceFullUpdate();
	}

	static int stickers = 0;

	ImGui::End();
	}

	ImGui::GetStyle().WindowMinSize = OldMinSize;

	}
	*/

	if (Settings::Misc::misc_StickerChanger && bIsGuiVisible)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);

		ImGui::SetNextWindowSize(ImVec2(500.f, 295.f));

		if (ImGui::Begin("Sticker Changer", &Settings::Misc::misc_StickerChanger,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			int SafeWeapon = SafeWeaponID();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Checkbox("Stickers", &Stick[SafeWeapon].StickersEnabled);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("You must be in game to enable this.");
			ImGui::Separator();
			if (SafeWeapon != 0)
			{
				static int iSlot = 0;
				const char* slots[] =
				{
					"Slot 1", "Slot 2", "Slot 3", "Slot 4"
				};
				ImGui::Combo("Slot", &iSlot, slots, IM_ARRAYSIZE(slots));
				ImGui::SliderFloat("Wear ", &Stick[SafeWeapon].Stickers[iSlot].flWear, 0.f, 1.f, "%.3f");
				ImGui::SliderFloat("Size", &Stick[SafeWeapon].Stickers[iSlot].flScale, 0.f, 1.f, "%.3f");
				ImGui::SliderInt("Rotate", &Stick[SafeWeapon].Stickers[iSlot].iRotation, 0, 360);
				ImGui::InputInt("ID Input", &Stick[SafeWeapon].Stickers[iSlot].iID);
				ImGui::Checkbox("Sticker Filter List", &Settings::Misc::stickerids);

				if (ImGui::Button("Accept Stickers##Skin"))
				{
					ForceFullUpdate();
				}

				static int stickers = 0;
			}
			ImGui::End();
		}

		ImGui::GetStyle().WindowMinSize = OldMinSize;

	}


	if (Settings::Misc::window_inventorychanger && bIsGuiVisible)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);

		ImGui::SetNextWindowSize(ImVec2(500.f, 355.f));

		if (ImGui::Begin("Inventory Changer", &Settings::Misc::window_inventorychanger,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			ImGui::Columns(2, nullptr, false);
			ImGui::Checkbox("Enable Inventory Changer", &Settings::InventoryChanger::enabled);
			static int itemDefinitionIndex = 0;
			static int paintKit = 0;
			static int rarity = 0;
			static int seed = 0;
			static float wear = 0.f;

			const char* skins_items[] =
			{
				"", "",  "2: Groundwater","3: Candy Apple", "",
				"5: Forest Ddpat","6: Arctic Camo",  "",
				"8: Desert Storm","9: Bengal Tiger","10: Copperhead",
				"11: Skulls","12: Crimson Web","13: Blue Streak","14: Red Laminate"
				,"15: Gunsmoke","16: Jungle Tiger","17: Urban Ddpat",  "",
				"", "20: Virus","21: Granite Marbleized","22: Contrast Spray",
				"",  "", "25: Forest Leaves","26: Lichen Dashed",
				"27: Bone Mask","28: Anodized Navy",  "",  "30: Snake Camo",
				"",  "32: Silver","33: Hot Rod","34: Metallic Ddpat",  "",
				"36: Ossified","37: Blaze","38: Fade","39: Bulldozer","40: Night",
				"41: Copper","42: Blue Steel","43: Stained","44: Case Hardened",
				"",  "46: Contractor","47: Colony","48: Dragon Tattoo",
				"",   "",  "51: Lightning Strike",  "",
				"",   "",   "",   "",   "",
				"",    "59: Slaughter","60: Dark Water"
				,"61: Hypnotic","62: Bloomstick",  "",   ""
				,   "",   "",  "67: Cold Blooded",  "",   ""
				,  "70: Carbon Fiber","71: Scorpion","72: Safari Mesh","73: Wings","74: Polar Camo"
				,"75: Blizzard Marbleized","76: Winter Forest","77: Boreal Forest","78: Forest Night"
				,  "",   "",   "",   "",  "83: Orange Ddpat","84: Pink Ddpat"
				,  "",   "",   "",   "",  "",  "90: Mudder",  ""
				,  "92: Cyanospatter","93: Caramel",  "", "95: Grassland","96: Blue Spruce",  ""
				,  "98: Ultraviolet","99: Sand Dune","100: Storm","101: Tornado","102: Whiteout",  ""
				,  "104: Grassland Leaves",  "",   "", "107: Polar Mesh",  "",   ""
				, "110: Condemned","111: Glacier Mesh",  "",   "",   "",   "",  "116: Sand Mesh",  "",   "", "119: Sage Spray",  "",   "", "122: Jungle Spray",  "",  "124: Sand Spray",  ""
				,  "",  "",  "",  "",  "",  "",  "",  "",  "", "135: Urban Perforated"
				,"136: Waves Perforated",  "",   "",   "",  "", "141: Orange Peel",  "",  "143: Urban Masked", "", "", "", "147: Jungle Dashed"
				,"148: Sand Dashed","149: Urban Dashed", "", "151: Jungle", "", "153: Demolition","154: Afterimage","155: Bullet Rain","156: Death by Kitty","157: Palm","158: Walnut","159: Brass", "", "", "162: Splash", "","164: Modern Hunter","165: Splash Jam","166: Blaze Orange","167: Radiation Hazard","168: Nuclear Threat","169: Fallout Warning","170: Predator","171: Irradiated Alert","172: Black Laminate", "","174: Boom","175: Scorched","176: Faded Zebra","177: Memento","178: Doomkitty","179: Nuclear Threat","180: Fire Serpent","181: Corticera","182: Emerald Dragon","183: Overgrowth","184: Corticera","185: Golden Koi","186: Wave Spray","187: Zirka","188: Graven","189: Bright Water","190: Black Limba","191: Tempest","192: Shattered","193: Bone Pile","194: Spitfire","195: Demeter","196: Emerald","197: Anodized Navy","198: Hazard","199: Dry Season","200: Mayan Dreams","201: Palm","202: Jungle Ddpat","203: Rust Coat","204: Mosaico","205: Jungle","206: Tornado","207: Facets","208: Sand Dune","209: Groundwater","210: Anodized Gunmetal","211: Ocean Foam","212: Graphite","213: Ocean Foam","214: Graphite","215: X-Ray","216: Blue Titanium","217: Blood Tiger","218: Hexane","219: Hive","220: Hemoglobin","221: Serum","222: Blood in the Water","223: Nightshade","224: Water Sigil","225: Ghost Camo","226: Blue Laminate","227: Electric Hive","228: Blind Spot","229: Azure Zebra","230: Steel Disruption","231: Cobalt Disruption","232: Crimson Web","233: Tropical Storm","234: Ash Wood","235: Varicamo","236: Night Ops","237: Urban Rubble","238: Varicamo Blue", "", "240: Calicamo","241: Hunting Blind","242: Army Mesh","243: Gator Mesh","244: Teardown","245: Army Recon","246: Amber Fade","247: Damascus Steel","248: Red Quartz","249: Cobalt Quartz","250: Full Stop","251: Pit Viper","252: Silver Quartz","253: Acid Fade","254: Nitro","255: Asiimov","256: The Kraken","257: Guardian","258: Mehndi","259: Redline","260: Pulse","261: Marina","262: Rose Iron","263: Rising Skull","264: Sandstorm","265: Kami","266: Magma","267: Cobalt Halftone","268: Tread Plate","269: The Fuschia Is Now","270: Victoria","271: Undertow","272: Titanium Bit","273: Heirloom","274: Copper Galaxy","275: Red Fragcam","276: Panther","277: Stainless","278: Blue Fissure","279: Asiimov","280: Chameleon","281: Corporal","282: Redline","283: Trigon","284: Heat","285: Terrain","286: Antique","287: Pulse","288: Sergeant","289: Sandstorm","290: Guardian","291: Heaven Guard", "", "293: Death Rattle","294: Green Apple","295: Franklin","296: Meteorite","297: Tuxedo","298: Army Sheen","299: Caged Steel","300: Emerald Pinstripe","301: Atomic Alloy","302: Vulcan","303: Isaac","304: Slashed","305: Torque","306: Antique","307: Retribution","308: Kami","309: Howl","310: Curse","311: Desert Warfare","312: Cyrex","313: Orion","314: Heaven Guard","315: Poison Dart","316: Jaguar","317: Bratatat","318: Road Rash","319: Detour","320: Red Python","321: Master Piece","322: Nitro","323: Rust Coat", "", "325: Chalice","326: Knight","327: Chainmail","328: Hand Cannon","329: Dark Age","330: Briar", "", "332: Royal Blue","333: Indigo","334: Twist","335: Module","336: Desert-Strike","337: Tatter","338: Pulse","339: Caiman","340: Jet Set","341: First Class","342: Leather","343: Commuter","344: Dragon Lore","345: First Class","346: Coach Class","347: Pilot","348: Red Leather","349: Osiris","350: Tigris","351: Conspiracy","352: Fowl Play","353: Water Elemental","354: Urban Hazard","355: Desert-Strike","356: Koi","357: Ivory","358: Supernova","359: Asiimov","360: Cyrex","361: Abyss","362: Labyrinth","363: Traveler","364: Business Class","365: Olive Plaid","366: Green Plaid","367: Reactor","368: Setting Sun","369: Nuclear Waste","370: Bone Machine","371: Styx","372: Nuclear Garden","373: Contamination","374: Toxic","375: Radiation Hazard","376: Chemical Green","377: Hot Shot","378: Fallout Warning","379: Cerberus","380: Wasteland Rebel","381: Grinder","382: Murky","383: Basilisk","384: Griffin","385: Firestarter","386: Dart","387: Urban Hazard","388: Cartel","389: Fire Elemental","390: Highwayman","391: Cardiac","392: Delusion","393: Tranquility","394: Cartel","395: Man-O'-War","396: Urban Shock","397: Naga","398: Chatterbox","399: Catacombs","400: 龍王 (Dragon King)","401: System Lock","402: Malachite","403: Deadly Poison","404: Muertos","405: Serenity","406: Grotto","407: Quicksilver","", "409: Tiger Tooth","410: Damascus Steel","411: Damascus Steel", "", "413: Marble Fade","414: Rust Coat","415: Doppler","416: Doppler","417: Doppler","418: Doppler","419: Doppler","420: Doppler","421: Doppler","422: Elite Build","423: Armor Core","424: Worm God","425: Bronze Deco","426: Valence","427: Monkey Business","428: Eco","429: Djinn","430: Hyper Beast","431: Heat","432: Man-O'-War","433: Neon Rider","434: Origami","435: Pole Position","436: Grand Prix","437: Twilight Galaxy","438: Chronos","439: Hades","440: Icarus Fell","441: Minotaur's Labyrinth","442: Asterion","443: Pathfinder","444: Daedalus","445: Hot Rod","446: Medusa","447: Duelist","448: Pandora's Box","449: Poseidon","450: Moon in Libra","451: Sun in Leo","452: Shipping Forecast","453: Emerald","454: Para Green","455: Akihabara Accept","456: Hydroponic","457: Bamboo Print","458: Bamboo Shadow","459: Bamboo Forest","460: Aqua Terrace", "", "462: Counter Terrace","463: Terrace","464: Neon Kimono","465: Orange Kimono","466: Crimson Kimono","467: Mint Kimono","468: Midnight Storm","469: Sunset Storm 壱","470: Sunset Storm 弐","471: Daybreak","472: Impact Drill","473: Seabird","474: Aquamarine Revenge","475: Hyper Beast","476: Yellow Jacket","477: Neural Net","478: Rocket Pop","479: Bunsen Burner","480: Evil Daimyo","481: Nemesis","482: Ruby Poison Dart","483: Loudmouth","484: Ranger","485: Handgun","486: Elite Build","487: Cyrex","488: Riot","489: Torque","490: Frontside Misty","491: Dualing Dragons","492: Survivor Z","493: Flux","494: Stone Cold","495: Wraiths","496: Nebula Crusader","497: Golden Coil","498: Rangeen","499: Cobalt Core","500: Special Delivery","501: Wingshot","502: Green Marine","503: Big Iron","504: Kill Confirmed","505: Scumbria","506: Point Disarray","507: Ricochet","508: Fuel Rod","509: Corinthian","510: Retrobution","511: The Executioner","512: Royal Paladin", "", "514: Power Loader","515: Imperial","516: Shapewood","517: Yorick","518: Outbreak","519: Tiger Moth","520: Avalanche","521: Teclu Burner","522: Fade","523: Amber Fade","524: Fuel Injector","525: Elite Build","526: Photic Zone","527: Kumicho Dragon","528: Cartel","529: Valence","530: Triumvirate", "", "532: Royal Legion","533: The Battlestar","534: Lapis Gator","535: Praetorian","536: Impire","537: Hyper Beast","538: Necropos","539: Jambiya","540: Lead Conduit","541: Fleet Flock","542: Judgement of Anubis","543: Red Astor","544: Ventilators","545: Orange Crash","546: Firefight","547: Spectre","548: Chantico's Fire","549: Bioleak","550: Oceanic","551: Asiimov","552: Fubar","553: Atlas","554: Ghost Crusader","555: Re-Entry","556: Primal Saber","557: Black Tie","558: Lore","559: Lore","560: Lore","561: Lore","562: Lore","563: Black Laminate","564: Black Laminate","565: Black Laminate","566: Black Laminate","567: Black Laminate","568: Gamma Doppler","569: Gamma Doppler","570: Gamma Doppler","571: Gamma Doppler","572: Gamma Doppler","573: Autotronic","574: Autotronic","575: Autotronic","576: Autotronic","577: Autotronic","578: Bright Water","579: Bright Water","580: Freehand","581: Freehand","582: Freehand","583: Aristocrat","584: Phobos","585: Violent Daimyo","586: Wasteland Rebel","587: Mecha Industries","588: Desolate Space","589: Carnivore","590: Exo","591: Imperial Dragon","592: Iron Clad","593: Chopper","594: Harvester","595: Reboot","596: Limelight","597: Bloodsport","598: Aerial","599: Ice Cap","600: Neon Revolution","601: Syd Mead","602: Imprint","603: Directive","604: Roll Cage","605: Scumbria","606: Ventilator","607: Weasel","608: Petroglyph","609: Airlock","610: Dazzle","611: Grim","612: Powercore","613: Triarch","614: Fuel Injector","615: Briefing","616: Slipstream","617: Doppler","618: Doppler","619: Doppler","620: Ultraviolet","621: Ultraviolet","622: Polymer","623: Ironwork","624: Dragonfire","625: Royal Consorts","626: Mecha Industries","627: Cirrus","628: Stinger","629: Black Sand","630: Sand Scale","631: Flashback","632: Buzz Kill","633: Sonar","634: Gila","635: Turf","636: Shallow Grave","637: Cyrex","638: Wasteland Princess","639: Bloodsport","640: Fever Dream","641: Jungle Slipstream","642: Blueprint","643: Xiangliu","644: Decimator","645: Oxide Blaze","646: Capillary","647: Crimson Tsunami","648: Emerald Poison Dart","649: Akoben","650: Ripple","651: Last Dive","652: Scaffold","653: Neo-Noir","654: Seasons","655: Zander","656: Orbit mk01","657: Blueprint","658: Cobra Strike","659: Macabre","660: Hyper Beast","661: Sugar Rush","662: Oni Taiji","663: Briefing","664: Hellfire","665: Aloha","666: Hard Water","667: Woodsman","668: Red Rock","669: Death Grip","670: Death's Head","671: Cut out","672: Metal Flowers","673: Morris","674: Triqua","675: The Empress","676: High Roller","677: Hunter","678: See Ya Later","679: Goo","680: Off World","681: Leaded Glass","682: Oceanic","683: Llama Cannon","684: Cracked Opal","685: Jungle Slipstream","686: Phantom","687: Tacticat","688: Exposure","689: Ziggy"
			};

			const char* quality_items[] =
			{
				"None",
				"1 - Consumer Grade (White)",
				"2 - Industrial Grade (Light Blue)",
				"3 - Mil-Spec (Darker Blue)",
				"4 - Restricted (Purple)",
				"5 - Classified (Pinkish Purple)",
				"6 - Covert (Red)",
				"7 - Exceedingly Rare (Gold)",
			};
			static const char* weapons[] = {
				"none",
				"1 - Desert Eagle",
				"2 - Dual Berettas",
				"3 - Five-Seven",
				"4 - Glock-18",
				"",
				"",
				"7 - AK-47",
				"8 - AUG",
				"9 - AWP",
				"10 - Famas",
				"11 - G3SG1",
				"",
				"13 - Galil AR",
				"14 - M249",
				"",
				"16 - M4A4",
				"17 - MAC-10",
				"",
				"19 - P90",
				"",
				"",
				"",
				"",
				"24 - UMP-45",
				"25 - XM1014",
				"26 - PP-Bizon",
				"27 - MAG-7",
				"28 - Negev",
				"29 - Sawed-Off",
				"30 - Tec-9",
				" ",
				"32 - P2000",
				"33 - MP7",
				"34 - MP9",
				"35 - Nova",
				"36 - P250",
				"38 - SCAR-20",
				"39 - SG553",
				"40 - SSG 08",
			};
			ImGui::Combo("Weapons", &itemDefinitionIndex, weapons, IM_ARRAYSIZE(weapons));
			ImGui::InputInt("Weapon ID", &itemDefinitionIndex);
			ImGui::Checkbox("Weapon Filter List", &Settings::Misc::weaponids);
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Combo("Skins", &paintKit, skins_items, IM_ARRAYSIZE(skins_items));
			ImGui::InputInt("Skin ID", &paintKit);
			ImGui::Checkbox("Skin Filter List", &Settings::Misc::skinids);
			ImGui::Spacing();
			ImGui::Spacing();
			static const char* rare[] =
			{
				"None",
				"1 - Consumer Grade (White)",
				"2 - Industrial Grade (Light Blue)",
				"3 - Mil-Spec (Darker Blue)",
				"4 - Restricted (Purple)",
				"5 - Classified (Pinkish Purple)",
				"6 - Covert (Red)",
				"7 - Exceedingly Rare (Gold)",
			};
			ImGui::Combo("Rarity", &rarity, rare, IM_ARRAYSIZE(rare));
			ImGui::InputInt("Seed", &seed);
			ImGui::SliderFloat("Wear", &wear, FLT_MIN, 1.f, "%.10f", 5);
			ImGui::Spacing();
			ImGui::Spacing();
			if (ImGui::Button("Add")) {
				Settings::InventoryChanger::weapons.insert(Settings::InventoryChanger::weapons.end(), { itemDefinitionIndex , paintKit , rarity , seed, wear });
			} ImGui::SameLine();
			if (ImGui::Button("Apply##Skin")) {
				SendClientHello();
				SendMMHello();
			}
			ImGui::NextColumn();

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
			ImGui::ListBoxHeader("Skins");
			int weapon_index = 0;
			for (auto weapon : Settings::InventoryChanger::weapons) {
				if (ImGui::Selectable(std::string(std::to_string(weapon.itemDefinitionIndex) + " " + std::to_string(weapon.paintKit)).c_str())) {
					Settings::InventoryChanger::weapons.erase(Settings::InventoryChanger::weapons.begin() + weapon_index);
				}
				weapon_index++;
			}
			ImGui::ListBoxFooter();
			ImGui::PopStyleColor();
			ImGui::Columns(1, nullptr, false);

			ImGui::GetStyle().WindowMinSize = OldMinSize;
		}
		ImGui::End();
	}

	if (Settings::Misc::window_medalchanger && bIsGuiVisible)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);

		ImGui::SetNextWindowSize(ImVec2(350.f, 350.f));

		if (ImGui::Begin("Medal Changer", &Settings::Misc::window_medalchanger,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			// ImGui::Columns(2, nullptr, false);
			ImGui::Checkbox("Enable Medal Changer", &Settings::MedalChanger::enabled);
			static int medal_id = 0;
			ImGui::InputInt("Medal ID", &medal_id);
			ImGui::Checkbox("Medal Filter List", &Settings::Misc::medalids);
			if (ImGui::Button("Add") && medal_id != 0) {
				Settings::MedalChanger::medals.insert(Settings::MedalChanger::medals.end(), medal_id);
				medal_id = 0;
			}
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
			ImGui::ListBoxHeader("Medals List");
			for (int m = 0; m < Settings::MedalChanger::medals.size(); m++) {
				if (ImGui::Selectable(std::to_string(Settings::MedalChanger::medals[m]).c_str())) {
					if (Settings::MedalChanger::equipped_medal == Settings::MedalChanger::medals[m]) {
						Settings::MedalChanger::equipped_medal = 0;
						Settings::MedalChanger::equipped_medal_override = false;
					}
					Settings::MedalChanger::medals.erase(Settings::MedalChanger::medals.begin() + m);
				}
			}
			ImGui::ListBoxFooter();
			ImGui::PopStyleColor();

			if (ImGui::Button("Apply##Medals")) {
				SendClientHello();
				SendMMHello();
			}

			ImGui::GetStyle().WindowMinSize = OldMinSize;
		}
		ImGui::End();
	}

	if (Settings::Misc::window_profilechanger && bIsGuiVisible)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);

		ImGui::SetNextWindowSize(ImVec2(500.f, 280.f));

		if (ImGui::Begin("Profile Changer", &Settings::Misc::window_profilechanger,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			ImGui::Checkbox("Enable Profile Changer", &Settings::ProfileChanger::enabled);
			
			static const char* bans[] = {
				"None",
				"You Were Kicked from the Last Match (Competitive Cooldown)",
				"You Killed Too Many Teammates (Competitive Cooldown)",
				"You Killed a Teammate at Round Start (Competitive Cooldown)",
				"You Failed to Reconnect to the Last Match (Competitive Cooldown)",
				"You Abandoned the Last Match (Competitive Cooldown)",
				"You Did Too Much Damage to Your Teammates (Competitive Cooldown)",
				"You Did Too Much Damage to Your Teammates at Round Start (Competitive Cooldown)",
				"This Account Is Permanently Untrusted (Global Cooldown)",
				"You Were Kicked from Too Many Recent Matches (Competitive Cooldown)",
				"Convicted by Overwatch - Majorly Disruptive (Global Cooldown)",
				"Convicted by Overwatch - Minorly Disruptive (Global Cooldown)",
				"Resolving Matchmaking State for Your Account (Temporary Cooldown)",
				"Resolving Matchmaking State After the Last Match (Temporary Cooldown)",
				"This Account Is Permanently Untrusted (Global Cooldown)",
				"(Global Cooldown)",
				"You Failed to Connect by Match Start. (Competitive Cooldown)",
				"You Have Kicked Too Many Teammates in Recent Matches (Competitive Cooldown)",
				"Congratulations on Your Recent Competitive Wins! Before You Play Competitive Matches Further Please Wait for Matchmaking Servers to Calibrate Your Skill Group Placement Based on Your Lastest Performance. (Temporary Cooldown)",
				"A Server Using Your Game Server Login Token Has Been Banned. Your Account Is Now Permanently Banned from Operating Game Servers, and You Have a Cooldown from Connecting to Game Servers. (Global Cooldown)"
			};
			
			static const char* ranks[] = {
				"Off",
				"Silver 1",
				"Silver 2",
				"Silver 3",
				"Silver 4",
				"Silver Elite",
				"Silver Elite Master",
				"Gold Nova 1",
				"Gold Nova 2",
				"Gold Nova 3",
				"Gold Nova Master",
				"Master Guardian 1",
				"Master Guardian 2",
				"Master Guardian Elite",
				"Distinguished Master Guardian",
				"Legendary Eagle",
				"Legendary Eagle Master",
				"Supreme Master First Class",
				"The Global Elite"
			};
			ImGui::Combo("Rank", &Settings::ProfileChanger::rank_id, ranks, IM_ARRAYSIZE(ranks));
			ImGui::SliderInt("Level", &Settings::ProfileChanger::level, 0, 40);
			ImGui::SliderInt("XP", &Settings::ProfileChanger::xp, 0, 5000);
			ImGui::InputInt("Wins", &Settings::ProfileChanger::wins);
			ImGui::InputInt("Friendly", &Settings::ProfileChanger::cmd_friendly);
			ImGui::InputInt("Teaching", &Settings::ProfileChanger::cmd_teaching);
			ImGui::InputInt("Leader", &Settings::ProfileChanger::cmd_leader);
			
			ImGui::Combo("Ban", &Settings::ProfileChanger::ban, bans, IM_ARRAYSIZE(bans));
			ImGui::InputInt("Ban Time", &Settings::ProfileChanger::time);

			if (ImGui::Button("Apply##Profile"))
			{
				SendMMHello();
			}
			ImGui::Columns(1, nullptr, false);
			ImGui::GetStyle().WindowMinSize = OldMinSize;
		}
		ImGui::End();
	}

	if (Settings::Misc::skinids && bIsGuiVisible)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);

		ImGui::SetNextWindowSize(ImVec2(350.f, 250.f));

		if (ImGui::Begin("Skin IDs", &Settings::Misc::skinids,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{

			static ImGuiTextFilter filter;
			filter.Draw();
			const char* skinids[] = {
				"2: Groundwater",
				"3: Candy Apple",
				"5: Forest DDPAT",
				"6: Arctic Camo",
				"8: Desert Storm",
				"9: Bengal Tiger",
				"10: Copperhead",
				"11: Skulls",
				"12: Crimson Web",
				"13: Blue Streak",
				"14: Red Laminate",
				"15: Gunsmoke",
				"16: Jungle Tiger",
				"17: Urban DDPAT",
				"20: Virus",
				"21: Granite Marbleized",
				"22: Contrast Spray",
				"25: Forest Leaves",
				"26: Lichen Dashed",
				"27: Bone Mask",
				"28: Anodized Navy",
				"30: Snake Camo",
				"32: Silver",
				"33: Hot Rod",
				"34: Metallic DPPAT",
				"36: Ossified",
				"37: Blaze",
				"38: Fade",
				"39: Bulldozer",
				"40: Night",
				"41: Copper",
				"42: Blue Steel",
				"43: Stained",
				"44: Case Hardened",
				"46: Contractor",
				"47: Colony",
				"48: Dragon Tattoo",
				"51: Lightning Strike",
				"59: Slaughter",
				"60: Dark Water",
				"61: Hypnotic",
				"62: Bloomstick",
				"67: Cold Blooded",
				"70: Carbon Fiber",
				"71: Scorpion",
				"72: Safari Mesh",
				"73: Wings",
				"74: Polar Camo",
				"75: Blizzard Marbleized",
				"76: Winter Forest",
				"77: Boreal Forest",
				"78: Forest Night",
				"83: Orange DDPAT",
				"84: Pink DDPAT",
				"90: Mudder",
				"92: Cyanospatter",
				"93: Caramel",
				"95: Grassland",
				"96: Blue Spruce",
				"98: Ultraviolet",
				"99: Sand Dune",
				"100: Storm",
				"101: Tornado",
				"102: Whiteout",
				"104: Grassland Leaves",
				"107: Polar Mesh",
				"110: Condemned",
				"111: Glacier Mesh",
				"116: Sand Mesh",
				"119: Sage Spray",
				"122: Jungle Spray",
				"124: Sand Spray",
				"135: Urban Perforated",
				"136: Waves Perforated",
				"141: Orange Peel",
				"143: Urban Masked",
				"147: Jungle Dashed",
				"148: Sand Dashed",
				"149: Urban Dashed",
				"151: Jungle",
				"153: Demolition",
				"154: Afterimage",
				"155: Bullet Rain",
				"156: Death by Kitty",
				"157: Palm",
				"158: Walnut",
				"159: Brass",
				"162: Splash",
				"164: Modern Hunter",
				"165: Splash Jam",
				"166: Blaze Orange",
				"167: Radiation Hazard",
				"168: Nuclear Threat",
				"169: Fallout Warning",
				"170: Predator",
				"171: Irradiated Alert",
				"172: Black Laminate",
				"174: Boom",
				"175: Scorched",
				"176: Faded Zebra",
				"177: Memento",
				"178: Doomkitty",
				"179: Nuclear Threat",
				"180: Fire Serpent",
				"181: Corticera",
				"182: Emerald Dragon",
				"183: Overgrowth",
				"184: Corticera",
				"185: Golden Koi",
				"186: Wave Spray",
				"187: Zirka",
				"188: Graven",
				"189: Bright Water",
				"190: Black Limba",
				"191: Tempest",
				"192: Shattered",
				"193: Bone Pile",
				"194: Spitfire",
				"195: Demeter",
				"196: Emerald",
				"197: Anodized Navy",
				"198: Hazard",
				"199: Dry Season",
				"200: Mayan Dreams",
				"201: Palm",
				"202: Jungle DDPAT",
				"203: Rust Coat",
				"204: Mosaico",
				"205: Jungle",
				"206: Tornado",
				"207: Facets",
				"208: Sand Dune",
				"209: Groundwater",
				"210: Anodized Gunmetal",
				"211: Ocean Foam",
				"212: Graphite",
				"213: Ocean Foam",
				"214: Graphite",
				"215: X-Ray",
				"216: Blue Titanium",
				"217: Blood Tiger",
				"218: Hexane",
				"219: Hive",
				"220: Hemoglobin",
				"221: Serum",
				"222: Blood in the Water",
				"223: Nightshade",
				"224: Water Sigil",
				"225: Ghost Camo",
				"226: Blue Laminate",
				"227: Electric Hive",
				"228: Blind Spot",
				"229: Azure Zebra",
				"230: Steel Disruption",
				"231: Cobalt Disruption",
				"232: Crimson Web",
				"233: Tropical Storm",
				"234: Ash Wood",
				"235: Varicamo",
				"236: Night Ops",
				"237: Urban Rubble",
				"238: Varicamo Blue",
				"240: Calicamo",
				"241: Hunting Blind",
				"242: Army Mesh",
				"243: Gator Mesh",
				"244: Teardown",
				"245: Army Recon",
				"246: Amber Fade",
				"247: Damascus Steel",
				"248: Red Quartz",
				"249: Cobalt Quartz",
				"250: Full Stop",
				"251: Pit Viper",
				"252: Silver Quartz",
				"253: Acid Fade",
				"254: Nitro",
				"255: Asiimov",
				"256: The Kraken",
				"257: Guardian",
				"258: Mehndi",
				"259: Redline",
				"260: Pulse",
				"261: Marina",
				"262: Rose Iron",
				"263: Rising Skull",
				"264: Sandstorm",
				"265: Kami",
				"266: Magma",
				"267: Cobalt Halftone",
				"268: Tread Plate",
				"269: The Fuschia Is Now",
				"270: Victoria",
				"271: Undertow",
				"272: Titanium Bit",
				"273: Heirloom",
				"274: Copper Galaxy",
				"275: Red Fragcam",
				"276: Panther",
				"277: Stainless",
				"278: Blue Fissure",
				"279: Asiimov",
				"280: Chameleon",
				"281: Corporal",
				"282: Redline",
				"283: Trigon",
				"284: Heat",
				"285: Terrain",
				"286: Antique",
				"287: Pulse",
				"288: Sergeant",
				"289: Sandstorm",
				"290: Guardian",
				"291: Heaven Guard",
				"293: Death Rattle",
				"294: Green Apple",
				"295: Franklin",
				"296: Meteorite",
				"297: Tuxedo",
				"298: Army Sheen",
				"299: Caged Steel",
				"300: Emerald Pinstripe",
				"301: Atomic Alloy",
				"302: Vulcan",
				"303: Isaac",
				"304: Slashed",
				"305: Torque",
				"306: Antique",
				"307: Retribution",
				"308: Kami",
				"309: Howl",
				"310: Curse",
				"311: Desert Warfare",
				"312: Cyrex",
				"313: Orion",
				"314: Heaven Guard",
				"315: Poison Dart",
				"316: Jaguar",
				"317: Bratatat",
				"318: Road Rash",
				"319: Detour",
				"320: Red Python",
				"321: Master Piece",
				"322: Nitro",
				"323: Rust Coat",
				"325: Chalice",
				"326: Knight",
				"327: Chainmail",
				"328: Hand Cannon",
				"329: Dark Age",
				"330: Briar",
				"332: Royal Blue",
				"333: Indigo",
				"334: Twist",
				"335: Module",
				"336: Desert-Strike",
				"337: Tatter",
				"338: Pulse",
				"339: Caiman",
				"340: Jet Set",
				"341: First Class",
				"342: Leather",
				"343: Commuter",
				"344: Dragon Lore",
				"345: First Class",
				"346: Coach Class",
				"347: Pilot",
				"348: Red Leather",
				"349: Osiris",
				"350: Tigris",
				"351: Conspiracy",
				"352: Fowl Play",
				"353: Water Elemental",
				"354: Urban Hazard",
				"355: Desert-Strike",
				"356: Koi",
				"357: Ivory",
				"358: Supernova",
				"359: Asiimov",
				"360: Cyrex",
				"361: Abyss",
				"362: Labyrinth",
				"363: Traveler",
				"364: Business Class",
				"365: Olive Plaid",
				"366: Green Plaid",
				"367: Reactor",
				"368: Setting Sun",
				"369: Nuclear Waste",
				"370: Bone Machine",
				"371: Styx",
				"372: Nuclear Garden",
				"373: Contamination",
				"374: Toxic",
				"375: Radiation Hazard",
				"376: Chemical Green",
				"377: Hot Shot",
				"378: Fallout Warning",
				"379: Cerberus",
				"380: Wasteland Rebel",
				"381: Grinder",
				"382: Murky",
				"383: Basilisk",
				"384: Griffin",
				"385: Firestarter",
				"386: Dart",
				"387: Urban Hazard",
				"388: Cartel",
				"389: Fire Elemental",
				"390: Highwayman",
				"391: Cardiac",
				"392: Delusion",
				"393: Tranquility",
				"394: Cartel",
				"395: Man-O'-War",
				"396: Urban Shock",
				"397: Naga",
				"398: Chatterbox",
				"399: Catacombs",
				"400: 龍王 (Dragon King)",
				"401: System Lock",
				"402: Malachite",
				"403: Deadly Poison",
				"404: Muertos",
				"405: Serenity",
				"406: Grotto",
				"407: Quicksilver",
				"409: Tiger Tooth",
				"410: Damascus Steel",
				"411: Damascus Steel",
				"413: Marble Fade",
				"414: Rust Coat",
				"415: Doppler",
				"416: Doppler",
				"417: Doppler",
				"418: Doppler",
				"419: Doppler",
				"420: Doppler",
				"421: Doppler",
				"422: Elite Build",
				"423: Armor Core",
				"424: Worm God",
				"425: Bronze Deco",
				"426: Valence",
				"427: Monkey Business",
				"428: Eco",
				"429: Djinn",
				"430: Hyper Beast",
				"431: Heat",
				"432: Man-O'-War",
				"433: Neon Rider",
				"434: Origami",
				"435: Pole Position",
				"436: Grand Prix",
				"437: Twilight Galaxy",
				"438: Chronos",
				"439: Hades",
				"440: Icarus Fell",
				"441: Minotaur's Labyrinth",
				"442: Asterion",
				"443: Pathfinder",
				"444: Daedalus",
				"445: Hot Rod",
				"446: Medusa",
				"447: Duelist",
				"448: Pandora's Box",
				"449: Poseidon",
				"450: Moon in Libra",
				"451: Sun in Leo",
				"452: Shipping Forecast",
				"453: Emerald",
				"454: Para Green",
				"455: Akihabara Accept",
				"456: Hydroponic",
				"457: Bamboo Print",
				"458: Bamboo Shadow",
				"459: Bamboo Forest",
				"460: Aqua Terrace",
				"462: Counter Terrace",
				"463: Terrace",
				"464: Neon Kimono",
				"465: Orange Kimono",
				"466: Crimson Kimono",
				"467: Mint Kimono",
				"468: Midnight Storm",
				"469: Sunset Storm 壱",
				"470: Sunset Storm 弐",
				"471: Daybreak",
				"472: Impact Drill",
				"473: Seabird",
				"474: Aquamarine Revenge",
				"475: Hyper Beast",
				"476: Yellow Jacket",
				"477: Neural Net",
				"478: Rocket Pop",
				"479: Bunsen Burner",
				"480: Evil Daimyo",
				"481: Nemesis",
				"482: Ruby Poison Dart",
				"483: Loudmouth",
				"484: Ranger",
				"485: Handgun",
				"486: Elite Build",
				"487: Cyrex",
				"488: Riot",
				"489: Torque",
				"490: Frontside Misty",
				"491: Dualing Dragons",
				"492: Survivor Z",
				"493: Flux",
				"494: Stone Cold",
				"495: Wraiths",
				"496: Nebula Crusader",
				"497: Golden Coil",
				"498: Rangeen",
				"499: Cobalt Core",
				"500: Special Delivery",
				"501: Wingshot",
				"502: Green Marine",
				"503: Big Iron",
				"504: Kill Confirmed",
				"505: Scumbria",
				"506: Point Disarray",
				"507: Ricochet",
				"508: Fuel Rod",
				"509: Corinthian",
				"510: Retrobution",
				"511: The Executioner",
				"512: Royal Paladin",
				"514: Power Loader",
				"515: Imperial",
				"516: Shapewood",
				"517: Yorick",
				"518: Outbreak",
				"519: Tiger Moth",
				"520: Avalanche",
				"521: Teclu Burner",
				"522: Fade",
				"523: Amber Fade",
				"524: Fuel Injector",
				"525: Elite Build",
				"526: Photic Zone",
				"527: Kumicho Dragon",
				"528: Cartel",
				"529: Valence",
				"530: Triumvirate",
				"532: Royal Legion",
				"533: The Battlestar",
				"534: Lapis Gator",
				"535: Praetorian",
				"536: Impire",
				"537: Hyper Beast",
				"538: Necropos",
				"539: Jambiya",
				"540: Lead Conduit",
				"541: Fleet Flock",
				"542: Judgement of Anubis",
				"543: Red Astor",
				"544: Ventilators",
				"545: Orange Crash",
				"546: Firefight",
				"547: Spectre",
				"548: Chantico's Fire",
				"549: Bioleak",
				"550: Oceanic",
				"551: Asiimov",
				"552: Fubar",
				"553: Atlas",
				"554: Ghost Crusader",
				"555: Re-Entry",
				"556: Primal Saber",
				"557: Black Tie",
				"558: Lore",
				"559: Lore",
				"560: Lore",
				"561: Lore",
				"562: Lore",
				"563: Black Laminate",
				"564: Black Laminate",
				"565: Black Laminate",
				"566: Black Laminate",
				"567: Black Laminate",
				"568: Gamma Doppler Emerald Marble",
				"569: Gamma Doppler Phase 1",
				"570: Gamma Doppler Phase 2",
				"571: Gamma Doppler Phase 3",
				"572: Gamma Doppler Phase 4",
				"573: Autotronic",
				"574: Autotronic",
				"575: Autotronic",
				"576: Autotronic",
				"577: Autotronic",
				"578: Bright Water",
				"579: Bright Water",
				"580: Freehand",
				"581: Freehand",
				"582: Freehand",
				"583: Aristocrat",
				"584: Phobos",
				"585: Violent Daimyo",
				"586: Wasteland Rebel",
				"587: Mecha Industries",
				"588: Desolate Space",
				"589: Carnivore",
				"590: Exo",
				"591: Imperial Dragon",
				"592: Iron Clad",
				"593: Chopper",
				"594: Harvester",
				"595: Reboot",
				"596: Limelight",
				"597: Bloodsport",
				"598: Aerial",
				"599: Ice Cap",
				"600: Neon Revolution",
				"601: Syd Mead",
				"602: Imprint",
				"603: Directive",
				"604: Roll Cage",
				"605: Scumbria",
				"606: Ventilator",
				"607: Weasel",
				"608: Petroglyph",
				"609: Airlock",
				"610: Dazzle",
				"611: Grim",
				"612: Powercore",
				"613: Triarch",
				"614: Fuel Injector",
				"615: Briefing",
				"616: Slipstream",
				"617: Doppler",
				"618: Doppler",
				"619: Doppler",
				"620: Ultraviolet",
				"621: Ultraviolet",
				"622: Polymer",
				"623: Ironwork",
				"624: Dragonfire",
				"625: Royal Consorts",
				"626: Mecha Industries",
				"627: Cirrus",
				"628: Stinger",
				"629: Black Sand",
				"630: Sand Scale",
				"631: Flashback",
				"632: Buzz Kill",
				"633: Sonar",
				"634: Gila",
				"635: Turf",
				"636: Shallow Grave",
				"637: Cyrex",
				"638: Wasteland Princess",
				"639: Bloodsport",
				"640: Fever Dream",
				"641: Jungle Slipstream",
				"642: Blueprint",
				"643: Xiangliu",
				"644: Decimator",
				"645: Oxide Blaze",
				"646: Capillary",
				"647: Crimson Tsunami",
				"648: Emerald Poison Dart",
				"649: Akoben",
				"650: Ripple",
				"651: Last Dive",
				"652: Scaffold",
				"653: Neo-Noir",
				"654: Seasons",
				"655: Zander",
				"656: Orbit MK01",
				"657: Blueprint",
				"658: Cobra Strike",
				"659: Macabre",
				"660: Hyper Beast",
				"661: Sugar Rush",
				"662: Oni Taiji",
				"663: Briefing",
				"664: Hellfire",
				"665: Aloha",
				"666: Hard Water",
				"667: Woodsman",
				"668: Red Rock",
				"669: Death Grip",
				"670: Death's Head",
				"671: Cut out",
				"672: Metal Flowers",
				"673: Morris",
				"674: Triqua",
				"675: The Empress",
				"676: High Roller",
				"677: Hunter",
				"678: See Ya Later",
				"679: Goo",
				"680: Off World",
				"681: Leaded Glass",
				"682: Oceanic",
				"683: Llama Cannon",
				"684: Cracked Opal",
				"685: Jungle Slipstream",
				"686: Phantom",
				"687: Tacticat",
				"688: Exposure",
				"689: Ziggy",
				"690: Stymphalian",
				"691: Mortis",
				"692: Night Riot",
				"693: Flame Test",
				"694: Moonrise",
				"695: Neo-Noir",
				"696: Bloodsport",
				"697: Black Sand",
				"698: Lionfish",
				"699: Wild Six",
				"700: Urban Hazard",
				"701: Grip",
				"702: Aloha",
				"703: Swag-7",
				"704: Arctic Wolf",
				"705: Cortex",
				"706: Oxide Blaze"
			};
			for (int i = 0; i < IM_ARRAYSIZE(skinids); i++)
				if (filter.PassFilter(skinids[i]))
					ImGui::Text("%s", skinids[i]);
			ImGui::End();
		}

		ImGui::GetStyle().WindowMinSize = OldMinSize;
	}

	if (Settings::Misc::medalids && bIsGuiVisible)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);

		ImGui::SetNextWindowSize(ImVec2(350.f, 250.f));

		if (ImGui::Begin("Medal IDs", &Settings::Misc::medalids,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			static ImGuiTextFilter filter;
			filter.Draw();
			const char* medalids[] = {
				"874 = 5 Year Veteran Coin",
				"875 = Champion at Dreamhack 2013",
				"876 = Finalist at Dreamhack 2013",
				"877 = Semifinalist at Dreamhack 2013",
				"878 = Quarterfinalist at Dreamhack 2013",
				"879 = Champion at Ems One Katowice 2014",
				"880 = Finalist at Ems One Katowice 2014",
				"881 = Semifinalist at Ems One Katowice 2014",
				"882 = Quarterfinalist at Ems One Katowice 2014",
				"883 = Champion at Esl One Cologne 2014",
				"884 = Finalist at Esl One Cologne 2014",
				"885 = Semifinalist at Esl One Cologne 2014",
				"886 = Quarterfinalist at Esl One Cologne 2014",
				"887 = Bronze Cologne 2014 Pick'em Troph",
				"888 = Silver Cologne 2014 Pick'em Trophy",
				"889 = Gold Cologne 2014 Pick'em Trophy",
				"890 = Champion at Dreamhack Winter 2014",
				"891 = Finalist at Dreamhack Winter 2014",
				"892 = Semifinalist at Dreamhack Winter 2014",
				"893 = Quarterfinalist at Dreamhack Winter 2014",
				"894 = Bronze Dreamhack 2014 Pick'em Trophy",
				"895 = Silver Dreamhack 2014 Pick'em Trophy",
				"896 = Gold Dreamhack 2014 Pick'em Trophy",
				"897 = Champion at Esl One Katowice 2015",
				"898 = Finalist at Esl One Katowice 2015",
				"899 = Semifinalist at Esl One Katowice 2015",
				"900 = Quarterfinalist at Esl One Katowice 2015",
				"901 = Bronze Katowice 2015 Pick'em Trophy",
				"902 = Silver Katowice 2015 Pick'em Trophy",
				"903 = Gold Katowice 2015 Pick'em Trophy",
				"904 = Champion at Esl One Cologne 2015",
				"905 = Finalist at Esl One Cologne 2015",
				"906 = Semifinalist at Esl One Cologne 2015",
				"907 = Quarterfinalist at Esl One Cologne 2015",
				"908 = Bronze Cologne 2015 Pick'em Trophy",
				"909 = Silver Cologne 2015 Pick'em Trophy",
				"910 = Gold Cologne 2015 Pick'em Trophy",
				"911 = Bronze Cluj-Napoca 2015 Pick'em Trophy",
				"912 = Silver Cluj-Napoca 2015 Pick'em Trophy",
				"913 = Gold Cluj-Napoca 2015 Pick'em Trophy",
				"914 = Bronze Cluj-Napoca 2015 Fantasy Trophy",
				"915 = Silver Cluj-Napoca 2015 Fantasy Trophy",
				"916 = Gold Cluj-Napoca 2015 Fantasy Trophy",
				"917 = Champion at Dreamhack Cluj-Napoca 2015",
				"918 = Finalist at Dreamhack Cluj-Napoca 2015",
				"919 = Semifinalist at Dreamhack Cluj-Napoca 2015",
				"920 = Quarterfinalist at Dreamhack Cluj-Napoca 2015",
				"921 = Bronze Columbus 2016 Pick'em Trophy",
				"922 = Silver Columbus 2016 Pick'em Trophy",
				"923 = Gold Columbus 2016 Pick'em Trophy",
				"924 = Bronze Columbus 2016 Fantasy Trophy",
				"925 = Silver Columbus 2016 Fantasy Troph",
				"926 = Gold Columbus 2016 Fantasy Trophy",
				"927 = Champion at Mlg Columbus 2016",
				"928 = Finalist at Mlg Columbus 2016",
				"929 = Semifinalist at Mlg Columbus 2016",
				"930 = Quarterfinalist at Mlg Columbus 2016",
				"931 = Champion at Esl One Cologne 2016",
				"932 = Finalist at Esl One Cologne 2016",
				"933 = Semifinalist at Esl One Cologne 2016",
				"934 = Quarterfinalist at Esl One Cologne 2016",
				"935 = Bronze Cologne 2016 Pick'em Trophy",
				"936 = Silver Cologne 2016 Pick'em Trophy",
				"937 = Gold Cologne 2016 Pick'em Trophy",
				"938 = Bronze Cologne 2016 Fantasy Trophy",
				"939 = Silver Cologne 2016 Fantasy Trophy",
				"940 = Gold Cologne 2016 Fantasy Trophy",
				"941 = Champion at Eleague Atlanta 2017",
				"942 = Finalist at Eleague Atlanta 2017",
				"943 = Semifinalist at Eleague Atlanta 2017",
				"944 = Quarterfinalist at Eleague Atlanta 2017",
				"945 = Bronze Atlanta 2017 Pick'em Trophy",
				"946 = Silver Atlanta 2017 Pick'em Trophy",
				"947 = Gold Atlanta 2017 Pick'em Trophy",
				"948 = Champion at Pgl Krakow 2017",
				"949 = Finalist at Pgl Krakow 2017",
				"950 = Semifinalist at Pgl Krakow 2017",
				"951 = Quarterfinalist at Pgl Krakow 2017 ",
				"952 = Bronze Krakow 2017 Pick'em Trophy",
				"953 = Silver Krakow 2017 Pick'em Trophy",
				"954 = Gold Krakow 2017 Pick'em Trophy",
				"955 = Champion at Eleague Boston 2018",
				"956 = Finalist at Eleague Boston 2018",
				"957 = Semifinalist at Eleague Boston 2018",
				"958 = Quarterfinalist at Eleague Boston 2018",
				"959 = Bronze Boston 2018 Pick'em Trophy",
				"960 = Silver Boston 2018 Pick'em Trophy",
				"961 = Gold Boston 2018 Pick'em Trophy",
				"1331 = 2015 Service Medal",
				"1332 = 2015 Service Medal",
				"1339 = 2016 Service Medal",
				"1340 = 2016 Service Medal",
				"1341 = 2016 Service Medal",
				"1342 = 2016 Service Medal",
				"1343 = 2016 Service Medal",
				"1344 = 2016 Service Medal",
				"1357 = 2017 Service Medal",
				"1358 = 2017 Service Medal",
				"1359 = 2017 Service Medal",
				"1360 = 2017 Service Medal",
				"1361 = 2017 Service Medal",
				"1362 = 2017 Service Medal",
				"1363 = 2017 Service Medal",
				"1367 = 2018 Service Medal",
				"1368 = 2018 Service Medal",
				"1369 = 2018 Service Medal",
				"1370 = 2018 Service Medal",
				"1371 = 2018 Service Medal",
				"1372 = 2018 Service Medal"
			};
			for (int i = 0; i < IM_ARRAYSIZE(medalids); i++)
				if (filter.PassFilter(medalids[i]))
					ImGui::Text("%s", medalids[i]);
			ImGui::End();
		}

		ImGui::GetStyle().WindowMinSize = OldMinSize;
	}

	if (Settings::Misc::weaponids && bIsGuiVisible)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);

		ImGui::SetNextWindowSize(ImVec2(350.f, 250.f));

		if (ImGui::Begin("Weapon IDs", &Settings::Misc::weaponids,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			static ImGuiTextFilter filter;
			filter.Draw();
			const char* weaponids[] = {
				"1 = Desert Eagle",
				"2 = Dual Berettas",
				"3 = Five-Seven",
				"4 = Glock-18",
				"7 = AK-47",
				"8 = AUG",
				"9 = AWP",
				"10 = Famas",
				"11 = G3SG1",
				"13 = Galil AR",
				"14 = M249",
				"16 = M4A4",
				"17 = MAC-10",
				"19 = P90",
				"24 = UMP-45",
				"25 = XM1014",
				"26 = PP-Bizon",
				"27 = MAG-7",
				"28 = Negev",
				"29 = Sawed-Fff",
				"30 = Tec-9",
				"32 = P2000",
				"33 = MP7",
				"34 = MP9",
				"35 = Nova",
				"36 = P250",
				"38 = SCAR-20",
				"39 = SG 553",
				"40 = SSG 08",
				"60 = M4A1-S",
				"61 = USP-S",
				"63 = CZ75-Auto",
				"64 = R8 Revolver",
				"500 = Bayonet",
				"505 = Flip Knife",
				"506 = Gut Knife",
				"507 = Karambit",
				"508 = M9 Bayonet",
				"509 = Huntsman Knife",
				"512 = Falchion Knife",
				"514 = Bowie Knife",
				"515 = Butterfly Knife",
				"516 = Shadow Daggers"
			};
			for (int i = 0; i < IM_ARRAYSIZE(weaponids); i++)
				if (filter.PassFilter(weaponids[i]))
					ImGui::Text("%s", weaponids[i]);

			ImGui::End();
		}

		ImGui::GetStyle().WindowMinSize = OldMinSize;
	}

	if (Settings::Misc::stickerids && bIsGuiVisible)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);

		ImGui::SetNextWindowSize(ImVec2(350.f, 250.f));

		if (ImGui::Begin("Sticker IDs", &Settings::Misc::stickerids,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			static ImGuiTextFilter filter;
			filter.Draw();
			const char* Stickers[] =
			{
				"1 - Shooter",
				"2 - Shooter (Foil)",
				"3 - Shooter Close",
				"4 - Shooter Close (Foil)",
				"5 - Blue Snowflake",
				"6 - Blue Snowflake (Foil)",
				"7 - Polar Bears",
				"8 - Polar Bears (Foil)",
				"9 - Mountain",
				"10 - Mountain (Foil)",
				"11 - Frosty The Hitman",
				"12 - Frosty The Hitman (Foil)",
				"13 - Lucky 13",
				"14 - Aces High",
				"15 - Aces High (Holo)",
				"16 - I Conquered",
				"17 - Seek & Destroy",
				"18 - Black Dog",
				"19 - Fearsome",
				"20 - Fearsome (Holo)",
				"21 - Cerberus",
				"22 - Easy Peasy",
				"23 - Luck Skill",
				"24 - Vigilance",
				"25 - Vigilance (Holo)",
				"26 - Lucky 13 (Foil)",
				"27 - Luck Skill (Foil)",
				"31 - Bish (Holo)",
				"32 - Bash (Holo)",
				"33 - Bosh (Holo)",
				"34 - Banana",
				"35 - Bomb Code",
				"36 - Chicken Lover",
				"37 - Crown (Foil)",
				"38 - Good Game",
				"39 - Good Luck",
				"40 - Have Fun",
				"41 - Let's Roll-oll",
				"42 - Let's Roll-oll (Holo)",
				"43 - Metal",
				"44 - Nice Shot",
				"46 - Stupid Banana (Foil)",
				"47 - Welcome To The Clutch",
				"48 - 3dmax | Katowice 2014",
				"49 - 3dmax (Holo) | Katowice 2014",
				"50 - Complexity Gaming | Katowice 2014",
				"51 - Complexity Gaming (Holo) | Katowice 2014",
				"52 - Team Dignitas | Katowice 2014",
				"53 - Team Dignitas (Holo) | Katowice 2014",
				"55 - Fnatic | Katowice 2014",
				"56 - Fnatic (Holo) | Katowice 2014",
				"57 - Hellraisers | Katowice 2014",
				"58 - Hellraisers (Holo) | Katowice 2014",
				"59 - Ibuypower | Katowice 2014",
				"60 - Ibuypower (Holo) | Katowice 2014",
				"61 - Team Ldlc.com | Katowice 2014",
				"62 - Team Ldlc.com (Holo) | Katowice 2014",
				"63 - Lgb Esports | Katowice 2014",
				"64 - Lgb Esports (Holo) | Katowice 2014",
				"65 - Mousesports | Katowice 2014",
				"66 - Mousesports (Holo) | Katowice 2014",
				"67 - Clan-mystik | Katowice 2014",
				"68 - Clan-mystik (Holo) | Katowice 2014",
				"69 - Natus Vincere | Katowice 2014",
				"70 - Natus Vincere (Holo) | Katowice 2014",
				"71 - Ninjas In Pyjamas | Katowice 2014",
				"72 - Ninjas In Pyjamas (Holo) | Katowice 2014",
				"73 - Reason Gaming | Katowice 2014",
				"73 - Reason Gaming | Katowice 2014",
				"74 - Reason Gaming (Holo) | Katowice 2014",
				"75 - Titan | Katowice 2014",
				"76 - Titan (Holo) | Katowice 2014",
				"77 - Virtus.pro | Katowice 2014",
				"78 - Virtus.pro (Holo) | Katowice 2014",
				"79 - Vox Eminor | Katowice 2014",
				"80 - Vox Eminor (Holo) | Katowice 2014",
				"81 - Esl Wolf (Foil) | Katowice 2014",
				"82 - Esl Skull (Foil) | Katowice 2014",
				"83 - 3dmax (Foil) | Katowice 2014",
				"84 - Complexity Gaming (Foil) | Katowice 2014",
				"85 - Team Dignitas (Foil) | Katowice 2014",
				"86 - Fnatic (Foil) | Katowice 2014",
				"87 - Hellraisers (Foil) | Katowice 2014",
				"88 - Ibuypower (Foil) | Katowice 2014",
				"89 - Team Ldlc.com (Foil) | Katowice 2014",
				"90 - Lgb Esports (Foil) | Katowice 2014",
				"91 - Mousesports (Foil) | Katowice 2014",
				"92 - Clan-mystik (Foil) | Katowice 2014",
				"93 - Natus Vincere (Foil) | Katowice 2014",
				"94 - Ninjas In Pyjamas (Foil) | Katowice 2014",
				"95 - Reason Gaming (Foil) | Katowice 2014",
				"96 - Titan (Foil) | Katowice 2014",
				"100 - Gold Esl Skull (Foil) | Katowice 2014",
				"101 - Backstab",
				"102 - King On The Field",
				"103 - Howling Dawn",
				"104 - Bomb Doge",
				"105 - Burn Them All",
				"106 - Harp Of War (Holo)",
				"107 - Flammable (Foil)",
				"108 - Headhunter (Foil)",
				"109 - Llama Cannon",
				"110 - New Sheriff (Foil)",
				"111 - My Other Awp",
				"112 - Shave Master",
				"113 - Rising Skull",
				"114 - Sneaky Beaky Like",
				"115 - Swag (Foil)",
				"116 - Teamwork (Holo)",
				"117 - To B Or Not To B",
				"118 - Winged Defuser",
				"119 - Pocket Bbq",
				"120 - Death Comes",
				"121 - Rekt (Holo)",
				"122 - Cloud9 | Cologne 2014",
				"123 - Cloud9 (Holo) | Cologne 2014",
				"124 - Fnatic | Cologne 2014",
				"125 - Fnatic (Holo) | Cologne 2014",
				"126 - Hellraisers | Cologne 2014",
				"127 - Hellraisers (Holo) | Cologne 2014",
				"128 - Ninjas In Pyjamas | Cologne 2014",
				"129 - Ninjas In Pyjamas (Holo) | Cologne 2014",
				"130 - Team Dignitas | Cologne 2014",
				"131 - Team Dignitas (Holo) | Cologne 2014",
				"132 - Team Ldlc.com | Cologne 2014",
				"133 - Team Ldlc.com (Holo) | Cologne 2014",
				"134 - Virtus.pro | Cologne 2014",
				"135 - Virtus.pro (Holo) | Cologne 2014",
				"136 - Copenhagen Wolves | Cologne 2014",
				"137 - Copenhagen Wolves (Holo) | Cologne 2014",
				"138 - Dat Team | Cologne 2014",
				"139 - Dat Team (Holo) | Cologne 2014",
				"140 - Epsilon Esports | Cologne 2014",
				"141 - Epsilon Esports (Holo) | Cologne 2014",
				"142 - Ibuypower | Cologne 2014",
				"143 - Ibuypower (Holo) | Cologne 2014",
				"144 - London Conspiracy | Cologne 2014",
				"145 - London Conspiracy (Holo) | Cologne 2014",
				"146 - Natus Vincere | Cologne 2014",
				"147 - Natus Vincere (Holo) | Cologne 2014",
				"148 - Titan | Cologne 2014",
				"149 - Titan (Holo) | Cologne 2014",
				"150 - Vox Eminor | Cologne 2014",
				"151 - Vox Eminor (Holo) | Cologne 2014",
				"152 - Mts Gamegod Wolf | Cologne 2014",
				"153 - Mts Gamegod Wolf (Holo) | Cologne 2014",
				"154 - Esl One Cologne 2014 (Blue)",
				"155 - Esl One Cologne 2014 (Red)",
				"156 - Cloud9 (Foil) | Cologne 2014",
				"157 - Fnatic (Foil) | Cologne 2014",
				"158 - Hellraisers (Foil) | Cologne 2014",
				"159 - Ninjas In Pyjamas (Foil) | Cologne 2014",
				"160 - Team Dignitas (Foil) | Cologne 2014",
				"161 - Team Ldlc.com (Foil) | Cologne 2014",
				"162 - Virtus.pro (Foil) | Cologne 2014",
				"163 - Copenhagen Wolves (Foil) | Cologne 2014",
				"164 - Dat Team (Foil) | Cologne 2014",
				"165 - Epsilon Esports (Foil) | Cologne 2014",
				"166 - Ibuypower (Foil) | Cologne 2014",
				"167 - London Conspiracy (Foil) | Cologne 2014",
				"168 - Natus Vincere (Foil) | Cologne 2014",
				"169 - Titan (Foil) | Cologne 2014",
				"170 - Vox Eminor (Foil) | Cologne 2014",
				"171 - Mts Gamegod Wolf (Foil) | Cologne 2014",
				"172 - Esl One Cologne 2014 (Gold)",
				"173 - Bossy Burger",
				"174 - Cat Call",
				"175 - Chicken Strike",
				"176 - Ct In Banana",
				"177 - Don't Worry, I'm Pro",
				"178 - Fight Like A Girl",
				"179 - Flashbang",
				"180 - Kawaii Killer Ct",
				"181 - Nelu The Bear",
				"182 - One Shot One Kill",
				"183 - Shooting Star Return",
				"184 - T On Cat",
				"185 - War Penguin",
				"186 - Windy Walking Club",
				"187 - Blitzkrieg",
				"188 - Pigeon Master",
				"189 - Terrorized",
				"190 - Till Death Do Us Part",
				"191 - Stay Frosty",
				"192 - Doomed",
				"193 - Queen Of Pain",
				"194 - Trick Or Threat",
				"195 - Trick Or Treat",
				"196 - Witch",
				"197 - Zombie Lover",
				"198 - Fnatic | Dreamhack 2014",
				"199 - Fnatic (Holo) | Dreamhack 2014",
				"200 - Fnatic (Foil) | Dreamhack 2014",
				"201 - Cloud9 | Dreamhack 2014",
				"202 - Cloud9 (Holo) | Dreamhack 2014",
				"203 - Cloud9 (Foil) | Dreamhack 2014",
				"207 - Virtus.pro | Dreamhack 2014",
				"208 - Virtus.pro (Holo) | Dreamhack 2014",
				"209 - Virtus.pro (Foil) | Dreamhack 2014",
				"210 - Ninjas In Pyjamas | Dreamhack 2014",
				"211 - Ninjas In Pyjamas (Holo) | Dreamhack 2014",
				"212 - Ninjas In Pyjamas (Foil) | Dreamhack 2014",
				"213 - Natus Vincere | Dreamhack 2014",
				"214 - Natus Vincere (Holo) | Dreamhack 2014",
				"215 - Natus Vincere (Foil) | Dreamhack 2014",
				"219 - Team Dignitas | Dreamhack 2014",
				"220 - Team Dignitas (Holo) | Dreamhack 2014",
				"221 - Team Dignitas (Foil) | Dreamhack 2014",
				"222 - Bravado Gaming | Dreamhack 2014",
				"223 - Esc Gaming | Dreamhack 2014",
				"224 - Hellraisers | Dreamhack 2014",
				"225 - Ibuypower | Dreamhack 2014",
				"226 - Penta Sports | Dreamhack 2014",
				"227 - Planetkey Dynamics | Dreamhack 2014",
				"228 - Team Ldlc.com | Dreamhack 2014",
				"229 - Myxmg | Dreamhack 2014",
				"230 - Dreamhack Winter 2014",
				"231 - Dreamhack Winter 2014 (Foil)",
				"232 - 3dmax | Dreamhack 2014",
				"233 - Copenhagen Wolves | Dreamhack 2014",
				"234 - Dat Team | Dreamhack 2014",
				"235 - London Conspiracy | Dreamhack 2014",
				"236 - Mousesports | Dreamhack 2014",
				"237 - 3dmax (Gold) | Dreamhack 2014",
				"238 - Bravado Gaming (Gold) | Dreamhack 2014",
				"239 - Cloud9 (Gold) | Dreamhack 2014",
				"240 - Copenhagen Wolves (Gold) | Dreamhack 2014",
				"241 - Dat Team (Gold) | Dreamhack 2014",
				"242 - Team Dignitas (Gold) | Dreamhack 2014",
				"243 - Esc Gaming (Gold) | Dreamhack 2014",
				"244 - Fnatic (Gold) | Dreamhack 2014",
				"245 - Hellraisers (Gold) | Dreamhack 2014",
				"246 - Ibuypower 9gold) | Dreamhack 2014",
				"247 - London Conspiracy (Gold) | Dreamhack 2014",
				"248 - Mousesports (Gold) | Dreamhack 2014",
				"249 - Myxmg (Gold) | Dreamhack 2014",
				"250 - Natus Vincere (Gold) | Dreamhack 2014",
				"251 - Ninjas In Pyjamas (Gold) | Dreamhack 2014",
				"252 - Penta Sports (Gold) | Dreamhack 2014",
				"253 - Planetkey Dynamics (Gold) | Dreamhack 2014",
				"254 - Team Ldlc.com (Gold) | Dreamhack 2014",
				"255 - Virtus.pro (Gold) | Dreamhack 2014",
				"256 - Flipsid3 Tactics | Dreamhack 2014",
				"257 - Flipsid3 Tactics (Gold) | Dreamhack 2014",
				"258 - Blood Boiler",
				"259 - Dinked",
				"260 - Drug War Veteran",
				"261 - Ho Ho Ho",
				"262 - Massive Pear",
				"263 - My Little Friend",
				"264 - Pandamonium",
				"265 - Piece Of Cake",
				"266 - Sas Chicken",
				"267 - Thug Life",
				"268 - T-rekt",
				"269 - Warowl",
				"270 - Work For Ammo",
				"271 - Phoenix (Foil)",
				"272 - Bomb Squad (Foil)",
				"273 - Flickshot",
				"274 - Headshot Guarantee",
				"275 - Eco Rush",
				"276 - Just Trolling",
				"278 - Firestarter (Holo)",
				"279 - Lucky Cat (Foil)",
				"280 - Robo",
				"281 - Witchcraft",
				"282 - Wanna Fight",
				"283 - Hostage Rescue",
				"284 - Hamster Hawk",
				"285 - Headless Chicken",
				"286 - 3dmax | Katowice 2015",
				"287 - 3dmax (Holo) | Katowice 2015",
				"288 - 3dmax (Foil) | Katowice 2015",
				"289 - 3dmax (Gold) | Katowice 2015",
				"290 - Cloud9 G2a | Katowice 2015",
				"291 - Cloud9 G2a (Holo) | Katowice 2015",
				"292 - Cloud9 G2a (Foil) | Katowice 2015",
				"293 - Cloud9 G2a (Gold) | Katowice 2015",
				"294 - Counter Logic Gaming | Katowice 2015",
				"295 - Counter Logic Gaming (Holo) | Katowice 2015",
				"296 - Counter Logic Gaming (Foil) | Katowice 2015",
				"297 - Counter Logic Gaming (Gold) | Katowice 2015",
				"300 - Esl One (Foil) | Katowice 2015",
				"301 - Esl One (Gold) | Katowice 2015",
				"302 - Flipsid3 Tactics | Katowice 2015",
				"303 - Flipsid3 Tactics (Holo) | Katowice 2015",
				"304 - Flipsid3 Tactics (Foil) | Katowice 2015",
				"305 - Flipsid3 Tactics (Gold) | Katowice 2015",
				"306 - Fnatic | Katowice 2015",
				"307 - Fnatic (Holo) | Katowice 2015",
				"308 - Fnatic (Foil) | Katowice 2015",
				"309 - Fnatic (Gold) | Katowice 2015",
				"310 - Hellraisers | Katowice 2015",
				"311 - Hellraisers (Holo) | Katowice 2015",
				"312 - Hellraisers (Foil) | Katowice 2015",
				"313 - Hellraisers (Gold) | Katowice 2015",
				"314 - Keyd Stars | Katowice 2015",
				"315 - Keyd Stars (Holo) | Katowice 2015",
				"316 - Keyd Stars (Foil) | Katowice 2015",
				"317 - Keyd Stars (Gold) | Katowice 2015",
				"318 - Lgb Esports | Katowice 2015",
				"319 - Lgb Esports (Holo) | Katowice 2015",
				"320 - Lgb Esports (Foil) | Katowice 2015",
				"321 - Lgb Esports (Gold) | Katowice 2015",
				"322 - Natus Vincere | Katowice 2015",
				"323 - Natus Vincere (Holo) | Katowice 2015",
				"324 - Natus Vincere (Foil) | Katowice 2015",
				"325 - Natus Vincere (Gold) | Katowice 2015",
				"326 - Ninjas In Pyjamas  | Katowice 2015",
				"327 - Ninjas In Pyjamas (Holo) | Katowice 2015",
				"328 - Ninjas In Pyjamas (Foil) | Katowice 2015",
				"329 - Ninjas In Pyjamas (Gold) | Katowice 2015",
				"330 - Penta Sports  | Katowice 2015",
				"331 - Penta Sports (Holo) | Katowice 2015",
				"332 - Penta Sports (Foil) | Katowice 2015",
				"333 - Penta Sports (Gold) | Katowice 2015",
				"334 - Team Envyus | Katowice 2015",
				"335 - Team Envyus (Holo) | Katowice 2015",
				"336 - Team Envyus (Foil) | Katowice 2015",
				"337 - Team Envyus (Gold) | Katowice 2015",
				"338 - Tsm Kinguin | Katowice 2015",
				"339 - Tsm Kinguin (Holo) | Katowice 2015",
				"340 - Tsm Kinguin (Foil) | Katowice 2015",
				"341 - Tsm Kinguin (Gold) | Katowice 2015",
				"342 - Titan | Katowice 2015",
				"343 - Titan (Holo) | Katowice 2015",
				"344 - Titan (Foil) | Katowice 2015",
				"345 - Titan (Gold) | Katowice 2015",
				"346 - Virtus.pro | Katowice 2015",
				"347 - Virtus.pro (Holo) | Katowice 2015",
				"348 - Virtus.pro (Foil) | Katowice 2015",
				"349 - Virtus.pro (Gold) | Katowice 2015",
				"350 - Vox Eminor  | Katowice 2015",
				"351 - Vox Eminor (Holo) | Katowice 2015",
				"352 - Vox Eminor (Foil) | Katowice 2015",
				"353 - Vox Eminor (Gold) | Katowice 2015",
				"354 - Esl One | Katowice 2015",
				"355 - Chabo",
				"356 - Bombsquad",
				"357 - Bombsquad (Foil)",
				"358 - The Guru",
				"359 - Silent Ninja",
				"360 - Silent Ninja (Foil)",
				"361 - The Samurai",
				"362 - Skull Lil Boney",
				"363 - Skulltorgeist",
				"364 - Skull Troop",
				"365 - Salute!",
				"366 - The Spartan",
				"367 - Unicorn",
				"368 - Unicorn (Holo)",
				"369 - The Zombie",
				"370 - Awp Country",
				"371 - Chi Bomb",
				"372 - Doru The Fox",
				"373 - Knife Club",
				"374 - Cs On The Mind",
				"375 - Ninja Defuse",
				"376 - Pros Don't Fake",
				"377 - Kawaii Killer Terrorist",
				"378 - Baaa-ckstabber!",
				"379 - Delicious Tears",
				"380 - Pronax | Cologne 2015",
				"381 - Pronax (Foil) | Cologne 2015",
				"382 - Pronax (Gold) | Cologne 2015",
				"383 - Flusha | Cologne 2015",
				"384 - Flusha (Foil) | Cologne 2015",
				"385 - Flusha (Gold) | Cologne 2015",
				"386 - Jw | Cologne 2015",
				"387 - Jw (Foil) | Cologne 2015",
				"388 - Jw (Gold) | Cologne 2015",
				"389 - Krimz | Cologne 2015",
				"390 - Krimz (Foil) | Cologne 2015",
				"391 - Krimz (Gold) | Cologne 2015",
				"392 - Olofmeister | Cologne 2015",
				"393 - Olofmeister (Foil) | Cologne 2015",
				"394 - Olofmeister (Gold) | Cologne 2015",
				"395 - Fallen | Cologne 2015",
				"396 - Fallen (Foil) | Cologne 2015",
				"397 - Fallen (Gold) | Cologne 2015",
				"398 - Steel | Cologne 2015",
				"399 - Steel (Foil) | Cologne 2015",
				"400 - Steel (Gold) | Cologne 2015",
				"401 - Fer | Cologne 2015",
				"402 - Fer (Foil) | Cologne 2015",
				"403 - Fer (Gold) | Cologne 2015",
				"404 - Boltz | Cologne 2015",
				"405 - Boltz (Foil) | Cologne 2015",
				"406 - Boltz (Gold) | Cologne 2015",
				"407 - Coldzera | Cologne 2015",
				"408 - Coldzera (Foil) | Cologne 2015",
				"409 - Coldzera (Gold) | Cologne 2015",
				"410 - Guardian | Cologne 2015",
				"411 - Guardian (Foil) | Cologne 2015",
				"412 - Guardian (Gold) | Cologne 2015",
				"413 - Zeus | Cologne 2015",
				"414 - Zeus (Foil) | Cologne 2015",
				"415 - Zeus (Gold) | Cologne 2015",
				"416 - Seized | Cologne 2015",
				"417 - Seized (Foil) | Cologne 2015",
				"418 - Seized (Gold) | Cologne 2015",
				"419 - Edward | Cologne 2015",
				"420 - Edward (Foil) | Cologne 2015",
				"421 - Edward (Gold) | Cologne 2015",
				"422 - Flamie | Cologne 2015",
				"423 - Flamie (Foil) | Cologne 2015",
				"424 - Flamie (Gold) | Cologne 2015",
				"425 - Xizt | Cologne 2015",
				"426 - Xizt (Foil) | Cologne 2015",
				"427 - Xizt (Gold) | Cologne 2015",
				"428 - F0rest | Cologne 2015",
				"429 - F0rest (Foil) | Cologne 2015",
				"430 - F0rest (Gold) | Cologne 2015",
				"431 - Get_right | Cologne 2015",
				"432 - Get_right (Foil) | Cologne 2015",
				"433 - Get_right (Gold) | Cologne 2015",
				"434 - Friberg | Cologne 2015",
				"435 - Friberg (Foil) | Cologne 2015",
				"436 - Friberg (Gold) | Cologne 2015",
				"437 - Allu | Cologne 2015",
				"438 - Allu (Foil) | Cologne 2015",
				"439 - Allu (Gold) | Cologne 2015",
				"440 - Kennys | Cologne 2015",
				"441 - Kennys (Foil) | Cologne 2015",
				"442 - Kennys (Gold) | Cologne 2015",
				"443 - Kioshima | Cologne 2015",
				"444 - Kioshima (Foil) | Cologne 2015",
				"445 - Kioshima (Gold) | Cologne 2015",
				"446 - Happy | Cologne 2015",
				"447 - Happy (Foil) | Cologne 2015",
				"448 - Happy (Gold) | Cologne 2015",
				"449 - Apex | Cologne 2015",
				"450 - Apex (Foil) | Cologne 2015",
				"451 - Apex (Gold) | Cologne 2015",
				"452 - Nbk- | Cologne 2015",
				"453 - Nbk- (Foil) | Cologne 2015",
				"454 - Nbk- (Gold) | Cologne 2015",
				"455 - Karrigan | Cologne 2015",
				"456 - Karrigan (Foil) | Cologne 2015",
				"457 - Karrigan (Gold) | Cologne 2015",
				"458 - Device | Cologne 2015",
				"459 - Device (Foil) | Cologne 2015",
				"460 - Device (Gold) | Cologne 2015",
				"461 - Dupreeh | Cologne 2015",
				"462 - Dupreeh (Foil) | Cologne 2015",
				"463 - Dupreeh (Gold) | Cologne 2015",
				"464 - Xyp9x | Cologne 2015",
				"465 - Xyp9x (Foil) | Cologne 2015",
				"466 - Xyp9x (Gold) | Cologne 2015",
				"467 - Cajunb | Cologne 2015",
				"468 - Cajunb (Foil) | Cologne 2015",
				"469 - Cajunb (Gold) | Cologne 2015",
				"470 - Neo | Cologne 2015",
				"471 - Neo (Foil) | Cologne 2015",
				"472 - Neo (Gold) | Cologne 2015",
				"473 - Pashabiceps | Cologne 2015",
				"474 - Pashabiceps (Foil) | Cologne 2015",
				"475 - Pashabiceps (Gold) | Cologne 2015",
				"476 - Taz | Cologne 2015",
				"477 - Taz (Foil) | Cologne 2015",
				"478 - Taz (Gold) | Cologne 2015",
				"479 - Snax | Cologne 2015",
				"480 - Snax (Foil) | Cologne 2015",
				"481 - Snax (Gold) | Cologne 2015",
				"482 - Byali | Cologne 2015",
				"483 - Byali (Foil) | Cologne 2015",
				"484 - Byali (Gold) | Cologne 2015",
				"485 - Chrisj | Cologne 2015",
				"486 - Chrisj (Foil) | Cologne 2015",
				"487 - Chrisj (Gold) | Cologne 2015",
				"488 - Gob B | Cologne 2015",
				"489 - Gob B (Foil) | Cologne 2015",
				"490 - Gob B (Gold) | Cologne 2015",
				"491 - Denis | Cologne 2015",
				"492 - Denis (Foil) | Cologne 2015",
				"493 - Denis (Gold) | Cologne 2015",
				"494 - Nex | Cologne 2015",
				"495 - Nex (Foil) | Cologne 2015",
				"496 - Nex (Gold) | Cologne 2015",
				"497 - Spiidi | Cologne 2015",
				"498 - Spiidi (Foil) | Cologne 2015",
				"499 - Spiidi (Gold) | Cologne 2015",
				"500 - Azr | Cologne 2015",
				"501 - Azr (Foil) | Cologne 2015",
				"502 - Azr (Gold) | Cologne 2015",
				"503 - Havoc | Cologne 2015",
				"504 - Havoc (Foil) | Cologne 2015",
				"505 - Havoc (Gold) | Cologne 2015",
				"506 - Jks | Cologne 2015",
				"507 - Jks (Foil) | Cologne 2015",
				"508 - Jks (Gold) | Cologne 2015",
				"509 - Spunj | Cologne 2015",
				"510 - Spunj (Foil) | Cologne 2015",
				"511 - Spunj (Gold) | Cologne 2015",
				"512 - Yam | Cologne 2015",
				"513 - Yam (Foil) | Cologne 2015",
				"514 - Yam (Gold) | Cologne 2015",
				"515 - Ustilo | Cologne 2015",
				"516 - Ustilo (Foil) | Cologne 2015",
				"517 - Ustilo (Gold) | Cologne 2015",
				"518 - Rickeh | Cologne 2015",
				"519 - Rickeh (Foil) | Cologne 2015",
				"520 - Rickeh (Gold) | Cologne 2015",
				"521 - Emagine | Cologne 2015",
				"522 - Emagine (Foil) | Cologne 2015",
				"523 - Emagine (Gold) | Cologne 2015",
				"524 - Snyper | Cologne 2015",
				"525 - Snyper (Foil) | Cologne 2015",
				"526 - Snyper (Gold) | Cologne 2015",
				"527 - James | Cologne 2015",
				"528 - James (Foil) | Cologne 2015",
				"529 - James (Gold) | Cologne 2015",
				"530 - Markeloff | Cologne 2015",
				"531 - Markeloff (Foil) | Cologne 2015",
				"532 - Markeloff (Gold) | Cologne 2015",
				"533 - B1ad3 | Cologne 2015",
				"534 - B1ad3 (Foil) | Cologne 2015",
				"535 - B1ad3 (Gold) | Cologne 2015",
				"536 - Bondik | Cologne 2015",
				"537 - Bondik (Foil) | Cologne 2015",
				"538 - Bondik (Gold) | Cologne 2015",
				"539 - Worldedit | Cologne 2015",
				"540 - Worldedit (Foil) | Cologne 2015",
				"541 - Worldedit (Gold) | Cologne 2015",
				"542 - Davcost | Cologne 2015",
				"543 - Davcost (Foil) | Cologne 2015",
				"544 - Davcost (Gold) | Cologne 2015",
				"545 - Dennis | Cologne 2015",
				"546 - Dennis (Foil) | Cologne 2015",
				"547 - Dennis (Gold) | Cologne 2015",
				"548 - Scream | Cologne 2015",
				"549 - Scream (Foil) | Cologne 2015",
				"550 - Scream (Gold) | Cologne 2015",
				"551 - Rain | Cologne 2015",
				"552 - Rain (Foil) | Cologne 2015",
				"553 - Rain (Gold) | Cologne 2015",
				"554 - Maikelele | Cologne 2015",
				"555 - Maikelele (Foil) | Cologne 2015",
				"556 - Maikelele (Gold) | Cologne 2015",
				"557 - Fox | Cologne 2015",
				"558 - Fox (Foil) | Cologne 2015",
				"559 - Fox (Gold) | Cologne 2015",
				"560 - Rallen | Cologne 2015",
				"561 - Rallen (Foil) | Cologne 2015",
				"562 - Rallen (Gold) | Cologne 2015",
				"563 - Hyper | Cologne 2015",
				"564 - Hyper (Foil) | Cologne 2015",
				"565 - Hyper (Gold) | Cologne 2015",
				"566 - Peet | Cologne 2015",
				"567 - Peet (Foil) | Cologne 2015",
				"568 - Peet (Gold) | Cologne 2015",
				"569 - Furlan | Cologne 2015",
				"570 - Furlan (Foil) | Cologne 2015",
				"571 - Furlan (Gold) | Cologne 2015",
				"572 - Gruby | Cologne 2015",
				"573 - Gruby (Foil) | Cologne 2015",
				"574 - Gruby (Gold) | Cologne 2015",
				"575 - Maniac | Cologne 2015",
				"576 - Maniac (Foil) | Cologne 2015",
				"577 - Maniac (Gold) | Cologne 2015",
				"578 - Ex6tenz | Cologne 2015",
				"579 - Ex6tenz (Foil) | Cologne 2015",
				"580 - Ex6tenz (Gold) | Cologne 2015",
				"581 - Shox | Cologne 2015",
				"582 - Shox (Foil) | Cologne 2015",
				"583 - Shox (Gold) | Cologne 2015",
				"584 - Smithzz | Cologne 2015",
				"585 - Smithzz (Foil) | Cologne 2015",
				"586 - Smithzz (Gold) | Cologne 2015",
				"587 - Rpk | Cologne 2015",
				"588 - Rpk (Foil) | Cologne 2015",
				"589 - Rpk (Gold) | Cologne 2015",
				"590 - Hazed | Cologne 2015",
				"591 - Hazed (Foil) | Cologne 2015",
				"592 - Hazed (Gold) | Cologne 2015",
				"593 - Fns | Cologne 2015",
				"594 - Fns (Foil) | Cologne 2015",
				"595 - Fns (Gold) | Cologne 2015",
				"596 - Jdm64 | Cologne 2015",
				"597 - Jdm64 (Foil) | Cologne 2015",
				"598 - Jdm64 (Gold) | Cologne 2015",
				"599 - Reltuc | Cologne 2015",
				"600 - Reltuc (Foil) | Cologne 2015",
				"601 - Reltuc (Gold) | Cologne 2015",
				"602 - Tarik | Cologne 2015",
				"603 - Tarik (Foil) | Cologne 2015",
				"604 - Tarik (Gold) | Cologne 2015",
				"605 - N0thing | Cologne 2015",
				"606 - N0thing (Foil) | Cologne 2015",
				"607 - N0thing (Gold) | Cologne 2015",
				"608 - Seang@res | Cologne 2015",
				"609 - Seang@res (Foil) | Cologne 2015",
				"610 - Seang@res (Gold) | Cologne 2015",
				"611 - Shroud | Cologne 2015",
				"612 - Shroud (Foil) | Cologne 2015",
				"613 - Shroud (Gold) | Cologne 2015",
				"614 - Freakazoid | Cologne 2015",
				"615 - Freakazoid (Foil) | Cologne 2015",
				"616 - Freakazoid (Gold) | Cologne 2015",
				"617 - Skadoodle | Cologne 2015",
				"618 - Skadoodle (Foil) | Cologne 2015",
				"619 - Skadoodle (Gold) | Cologne 2015",
				"620 - Fnatic | Cologne 2015",
				"621 - Fnatic (Foil) | Cologne 2015",
				"622 - Fnatic (Gold) | Cologne 2015",
				"623 - Virtus.pro | Cologne 2015",
				"624 - Virtus.pro (Foil) | Cologne 2015",
				"625 - Virtus.pro (Gold) | Cologne 2015",
				"626 - Mousesports | Cologne 2015",
				"627 - Mousesports (Foil) | Cologne 2015",
				"628 - Mousesports (Gold) | Cologne 2015",
				"629 - Natus Vincere | Cologne 2015",
				"630 - Natus Vincere (Foil) | Cologne 2015",
				"631 - Natus Vincere (Gold) | Cologne 2015",
				"632 - Renegades | Cologne 2015",
				"633 - Renegades (Foil) | Cologne 2015",
				"634 - Renegades (Gold) | Cologne 2015",
				"635 - Team Kinguin | Cologne 2015",
				"636 - Team Kinguin (Foil) | Cologne 2015",
				"637 - Team Kinguin (Gold) | Cologne 2015",
				"638 - Team Ebettle | Cologne 2015",
				"639 - Team Ebettle (Foil) | Cologne 2015",
				"640 - Team Ebettle (Gold) | Cologne 2015",
				"641 - Cloud9 G2a | Cologne 2015",
				"642 - Cloud9 G2a (Foil) | Cologne 2015",
				"643 - Cloud9 G2a (Gold) | Cologne 2015",
				"644 - Ninjas In Pyjamas | Cologne 2015",
				"645 - Ninjas In Pyjamas (Foil) | Cologne 2015",
				"646 - Ninjas In Pyjamas (Gold) | Cologne 2015",
				"647 - Team Envyus | Cologne 2015",
				"648 - Team Envyus (Foil) | Cologne 2015",
				"649 - Team Envyus (Gold) | Cologne 2015",
				"650 - Luminosity Gaming | Cologne 2015",
				"651 - Luminosity Gaming (Foil) | Cologne 2015",
				"652 - Luminosity Gaming (Gold) | Cologne 2015",
				"653 - Team Solomid | Cologne 2015",
				"654 - Team Solomid (Foil) | Cologne 2015",
				"655 - Team Solomid (Gold) | Cologne 2015",
				"656 - Team Immunity | Cologne 2015",
				"657 - Team Immunity (Foil) | Cologne 2015",
				"658 - Team Immunity (Gold) | Cologne 2015",
				"659 - Flipsid3 Tactics | Cologne 2015",
				"660 - Flipsid3 Tactics (Foil) | Cologne 2015",
				"661 - Flipsid3 Tactics (Gold) | Cologne 2015",
				"662 - Titan | Cologne 2015",
				"663 - Titan (Foil) | Cologne 2015",
				"664 - Titan (Gold) | Cologne 2015",
				"665 - Counter Logic Gaming | Cologne 2015",
				"666 - Counter Logic Gaming (Foil) | Cologne 2015",
				"667 - Counter Logic Gaming (Gold) | Cologne 2015",
				"668 - Esl | Cologne 2015",
				"669 - Esl (Foil) | Cologne 2015",
				"670 - Esl (Gold) | Cologne 2015",
				"671 - Reltuc | Cluj-napoca 2015",
				"672 - Reltuc (Foil) | Cluj-napoca 2015",
				"673 - Reltuc (Gold) | Cluj-napoca 2015",
				"674 - Fns | Cluj-napoca 2015",
				"675 - Fns (Foil) | Cluj-napoca 2015",
				"676 - Fns (Gold) | Cluj-napoca 2015",
				"677 - Hazed | Cluj-napoca 2015",
				"678 - Hazed (Foil) | Cluj-napoca 2015",
				"679 - Hazed (Gold) | Cluj-napoca 2015",
				"680 - Jdm64 | Cluj-napoca 2015",
				"681 - Jdm64 (Foil) | Cluj-napoca 2015",
				"682 - Jdm64 (Gold) | Cluj-napoca 2015",
				"683 - Tarik | Cluj-napoca 2015",
				"684 - Tarik (Foil) | Cluj-napoca 2015",
				"685 - Tarik (Gold) | Cluj-napoca 2015",
				"686 - Freakazoid | Cluj-napoca 2015",
				"687 - Freakazoid (Foil) | Cluj-napoca 2015",
				"688 - Freakazoid (Gold) | Cluj-napoca 2015",
				"689 - Seang@res | Cluj-napoca 2015",
				"690 - Seang@res (Foil) | Cluj-napoca 2015",
				"691 - Seang@res (Gold) | Cluj-napoca 2015",
				"692 - Shroud | Cluj-napoca 2015",
				"693 - Shroud (Foil) | Cluj-napoca 2015",
				"694 - Shroud (Gold) | Cluj-napoca 2015",
				"695 - Skadoodle | Cluj-napoca 2015",
				"696 - Skadoodle (Foil) | Cluj-napoca 2015",
				"697 - Skadoodle (Gold) | Cluj-napoca 2015",
				"698 - N0thing | Cluj-napoca 2015",
				"699 - N0thing (Foil) | Cluj-napoca 2015",
				"700 - N0thing (Gold) | Cluj-napoca 2015",
				"701 - Apex | Cluj-napoca 2015",
				"702 - Apex (Foil) | Cluj-napoca 2015",
				"703 - Apex (Gold) | Cluj-napoca 2015",
				"704 - Happy | Cluj-napoca 2015",
				"705 - Happy (Foil) | Cluj-napoca 2015",
				"706 - Happy (Gold) | Cluj-napoca 2015",
				"707 - Kioshima | Cluj-napoca 2015",
				"708 - Kioshima (Foil) | Cluj-napoca 2015",
				"709 - Kioshima (Gold) | Cluj-napoca 2015",
				"710 - Kennys | Cluj-napoca 2015",
				"711 - Kennys (Foil) | Cluj-napoca 2015",
				"712 - Kennys (Gold) | Cluj-napoca 2015",
				"713 - Nbk- | Cluj-napoca 2015",
				"714 - Nbk- (Foil) | Cluj-napoca 2015",
				"715 - Nbk- (Gold) | Cluj-napoca 2015",
				"716 - B1ad3 | Cluj-napoca 2015",
				"717 - B1ad3 (Foil) | Cluj-napoca 2015",
				"718 - B1ad3 (Gold) | Cluj-napoca 2015",
				"719 - Bondik | Cluj-napoca 2015",
				"720 - Bondik (Foil) | Cluj-napoca 2015",
				"721 - Bondik (Gold) | Cluj-napoca 2015",
				"722 - Davcost | Cluj-napoca 2015",
				"723 - Davcost (Foil) | Cluj-napoca 2015",
				"724 - Davcost (Gold) | Cluj-napoca 2015",
				"725 - Markeloff | Cluj-napoca 2015",
				"726 - Markeloff (Foil) | Cluj-napoca 2015",
				"727 - Markeloff (Gold) | Cluj-napoca 2015",
				"728 - Worldedit | Cluj-napoca 2015",
				"729 - Worldedit (Foil) | Cluj-napoca 2015",
				"730 - Worldedit (Gold) | Cluj-napoca 2015",
				"731 - Flusha | Cluj-napoca 2015",
				"732 - Flusha (Foil) | Cluj-napoca 2015",
				"733 - Flusha (Gold) | Cluj-napoca 2015",
				"734 - Jw | Cluj-napoca 2015",
				"735 - Jw (Foil) | Cluj-napoca 2015",
				"736 - Jw (Gold) | Cluj-napoca 2015",
				"737 - Krimz | Cluj-napoca 2015",
				"738 - Krimz (Foil) | Cluj-napoca 2015",
				"739 - Krimz (Gold) | Cluj-napoca 2015",
				"740 - Olofmeister | Cluj-napoca 2015",
				"741 - Olofmeister (Foil) | Cluj-napoca 2015",
				"742 - Olofmeister (Gold) | Cluj-napoca 2015",
				"743 - Pronax | Cluj-napoca 2015",
				"744 - Pronax (Foil) | Cluj-napoca 2015",
				"745 - Pronax (Gold) | Cluj-napoca 2015",
				"746 - Dennis | Cluj-napoca 2015",
				"747 - Dennis (Foil) | Cluj-napoca 2015",
				"748 - Dennis (Gold) | Cluj-napoca 2015",
				"749 - Fox | Cluj-napoca 2015",
				"750 - Fox (Foil) | Cluj-napoca 2015",
				"751 - Fox (Gold) | Cluj-napoca 2015",
				"752 - Maikelele | Cluj-napoca 2015",
				"753 - Maikelele (Foil) | Cluj-napoca 2015",
				"754 - Maikelele (Gold) | Cluj-napoca 2015",
				"755 - Rain | Cluj-napoca 2015",
				"756 - Rain (Foil) | Cluj-napoca 2015",
				"757 - Rain (Gold) | Cluj-napoca 2015",
				"758 - Jkaem | Cluj-napoca 2015",
				"759 - Jkaem (Foil) | Cluj-napoca 2015",
				"760 - Jkaem (Gold) | Cluj-napoca 2015",
				"761 - Boltz | Cluj-napoca 2015",
				"762 - Boltz (Foil) | Cluj-napoca 2015",
				"763 - Boltz (Gold) | Cluj-napoca 2015",
				"764 - Coldzera | Cluj-napoca 2015",
				"765 - Coldzera (Foil) | Cluj-napoca 2015",
				"766 - Coldzera (Gold) | Cluj-napoca 2015",
				"767 - Fallen | Cluj-napoca 2015",
				"768 - Fallen (Foil) | Cluj-napoca 2015",
				"769 - Fallen (Gold) | Cluj-napoca 2015",
				"770 - Fer | Cluj-napoca 2015",
				"771 - Fer (Foil) | Cluj-napoca 2015",
				"772 - Fer (Gold) | Cluj-napoca 2015",
				"773 - Steel | Cluj-napoca 2015",
				"774 - Steel (Foil) | Cluj-napoca 2015",
				"775 - Steel (Gold) | Cluj-napoca 2015",
				"776 - Chrisj | Cluj-napoca 2015",
				"777 - Chrisj (Foil) | Cluj-napoca 2015",
				"778 - Chrisj (Gold) | Cluj-napoca 2015",
				"779 - Denis | Cluj-napoca 2015",
				"780 - Denis (Foil) | Cluj-napoca 2015",
				"781 - Denis (Gold) | Cluj-napoca 2015",
				"782 - Gob B | Cluj-napoca 2015",
				"783 - Gob B (Foil) | Cluj-napoca 2015",
				"784 - Gob B (Gold) | Cluj-napoca 2015",
				"785 - Nex | Cluj-napoca 2015",
				"786 - Nex (Foil) | Cluj-napoca 2015",
				"787 - Nex (Gold) | Cluj-napoca 2015",
				"788 - Niko | Cluj-napoca 2015",
				"789 - Niko (Foil) | Cluj-napoca 2015",
				"790 - Niko (Gold) | Cluj-napoca 2015",
				"791 - Edward | Cluj-napoca 2015",
				"792 - Edward (Foil) | Cluj-napoca 2015",
				"793 - Edward (Gold) | Cluj-napoca 2015",
				"794 - Flamie | Cluj-napoca 2015",
				"795 - Flamie (Foil) | Cluj-napoca 2015",
				"796 - Flamie (Gold) | Cluj-napoca 2015",
				"797 - Guardian | Cluj-napoca 2015",
				"798 - Guardian (Foil) | Cluj-napoca 2015",
				"799 - Guardian (Gold) | Cluj-napoca 2015",
				"800 - Seized | Cluj-napoca 2015",
				"801 - Seized (Foil) | Cluj-napoca 2015",
				"802 - Seized (Gold) | Cluj-napoca 2015",
				"803 - Zeus | Cluj-napoca 2015",
				"804 - Zeus (Foil) | Cluj-napoca 2015",
				"805 - Zeus (Gold) | Cluj-napoca 2015",
				"806 - Allu | Cluj-napoca 2015",
				"807 - Allu (Foil) | Cluj-napoca 2015",
				"808 - Allu (Gold) | Cluj-napoca 2015",
				"809 - F0rest | Cluj-napoca 2015",
				"810 - F0rest (Foil) | Cluj-napoca 2015",
				"811 - F0rest (Gold) | Cluj-napoca 2015",
				"812 - Friberg | Cluj-napoca 2015",
				"813 - Friberg (Foil) | Cluj-napoca 2015",
				"814 - Friberg (Gold) | Cluj-napoca 2015",
				"815 - Get_right | Cluj-napoca 2015",
				"816 - Get_right (Foil) | Cluj-napoca 2015",
				"817 - Get_right (Gold) | Cluj-napoca 2015",
				"818 - Xizt | Cluj-napoca 2015",
				"819 - Xizt (Foil) | Cluj-napoca 2015",
				"820 - Xizt (Gold) | Cluj-napoca 2015",
				"821 - Aizy | Cluj-napoca 2015",
				"822 - Aizy (Foil) | Cluj-napoca 2015",
				"823 - Aizy (Gold) | Cluj-napoca 2015",
				"824 - Kjaerbye | Cluj-napoca 2015",
				"825 - Kjaerbye (Foil) | Cluj-napoca 2015",
				"826 - Kjaerbye (Gold) | Cluj-napoca 2015",
				"827 - Msl | Cluj-napoca 2015",
				"828 - Msl (Foil) | Cluj-napoca 2015",
				"829 - Msl (Gold) | Cluj-napoca 2015",
				"830 - Pimp | Cluj-napoca 2015",
				"831 - Pimp (Foil) | Cluj-napoca 2015",
				"832 - Pimp (Gold) | Cluj-napoca 2015",
				"833 - Tenzki | Cluj-napoca 2015",
				"834 - Tenzki (Foil) | Cluj-napoca 2015",
				"835 - Tenzki (Gold) | Cluj-napoca 2015",
				"836 - Adren | Cluj-napoca 2015",
				"837 - Adren (Foil) | Cluj-napoca 2015",
				"838 - Adren (Gold) | Cluj-napoca 2015",
				"839 - Elige | Cluj-napoca 2015",
				"840 - Elige (Foil) | Cluj-napoca 2015",
				"841 - Elige (Gold) | Cluj-napoca 2015",
				"842 - Fugly | Cluj-napoca 2015",
				"843 - Fugly (Foil) | Cluj-napoca 2015",
				"844 - Fugly (Gold) | Cluj-napoca 2015",
				"845 - Hiko | Cluj-napoca 2015",
				"846 - Hiko (Foil) | Cluj-napoca 2015",
				"847 - Hiko (Gold) | Cluj-napoca 2015",
				"848 - Nitr0 | Cluj-napoca 2015",
				"849 - Nitr0 (Foil) | Cluj-napoca 2015",
				"850 - Nitr0 (Gold) | Cluj-napoca 2015",
				"851 - Ex6tenz | Cluj-napoca 2015",
				"852 - Ex6tenz (Foil) | Cluj-napoca 2015",
				"853 - Ex6tenz (Gold) | Cluj-napoca 2015",
				"854 - Rpk | Cluj-napoca 2015",
				"855 - Rpk (Foil) | Cluj-napoca 2015",
				"856 - Rpk (Gold) | Cluj-napoca 2015",
				"857 - Scream | Cluj-napoca 2015",
				"858 - Scream (Foil) | Cluj-napoca 2015",
				"859 - Scream (Gold) | Cluj-napoca 2015",
				"860 - Shox | Cluj-napoca 2015",
				"861 - Shox (Foil) | Cluj-napoca 2015",
				"862 - Shox (Gold) | Cluj-napoca 2015",
				"863 - Smithzz | Cluj-napoca 2015",
				"864 - Smithzz (Foil) | Cluj-napoca 2015",
				"865 - Smithzz (Gold) | Cluj-napoca 2015",
				"866 - Cajunb | Cluj-napoca 2015",
				"867 - Cajunb (Foil) | Cluj-napoca 2015",
				"868 - Cajunb (Gold) | Cluj-napoca 2015",
				"869 - Device | Cluj-napoca 2015",
				"870 - Device (Foil) | Cluj-napoca 2015",
				"871 - Device (Gold) | Cluj-napoca 2015",
				"872 - Dupreeh | Cluj-napoca 2015",
				"873 - Dupreeh (Foil) | Cluj-napoca 2015",
				"874 - Dupreeh (Gold) | Cluj-napoca 2015",
				"875 - Karrigan | Cluj-napoca 2015",
				"876 - Karrigan (Foil) | Cluj-napoca 2015",
				"877 - Karrigan (Gold) | Cluj-napoca 2015",
				"878 - Xyp9x | Cluj-napoca 2015",
				"879 - Xyp9x (Foil) | Cluj-napoca 2015",
				"880 - Xyp9x (Gold) | Cluj-napoca 2015",
				"881 - Furlan | Cluj-napoca 2015",
				"882 - Furlan (Foil) | Cluj-napoca 2015",
				"883 - Furlan (Gold) | Cluj-napoca 2015",
				"884 - Gruby | Cluj-napoca 2015",
				"885 - Gruby (Foil) | Cluj-napoca 2015",
				"886 - Gruby (Gold) | Cluj-napoca 2015",
				"887 - Hyper | Cluj-napoca 2015",
				"888 - Hyper (Foil) | Cluj-napoca 2015",
				"889 - Hyper (Gold) | Cluj-napoca 2015",
				"890 - Peet | Cluj-napoca 2015",
				"891 - Peet (Foil) | Cluj-napoca 2015",
				"892 - Peet (Gold) | Cluj-napoca 2015",
				"893 - Rallen | Cluj-napoca 2015",
				"894 - Rallen (Foil) | Cluj-napoca 2015",
				"895 - Rallen (Gold) | Cluj-napoca 2015",
				"896 - Byali | Cluj-napoca 2015",
				"897 - Byali (Foil) | Cluj-napoca 2015",
				"898 - Byali (Gold) | Cluj-napoca 2015",
				"899 - Neo | Cluj-napoca 2015",
				"900 - Neo (Foil) | Cluj-napoca 2015",
				"901 - Neo (Gold) | Cluj-napoca 2015",
				"902 - Pashabiceps | Cluj-napoca 2015",
				"903 - Pashabiceps (Foil) | Cluj-napoca 2015",
				"904 - Pashabiceps (Gold) | Cluj-napoca 2015",
				"905 - Snax | Cluj-napoca 2015",
				"906 - Snax (Foil) | Cluj-napoca 2015",
				"907 - Snax (Gold) | Cluj-napoca 2015",
				"908 - Taz | Cluj-napoca 2015",
				"909 - Taz (Foil) | Cluj-napoca 2015",
				"910 - Taz (Gold) | Cluj-napoca 2015",
				"911 - Ninjas In Pyjamas | Cluj-napoca 2015",
				"912 - Ninjas In Pyjamas (Foil) | Cluj-napoca 2015",
				"913 - Ninjas In Pyjamas (Gold) | Cluj-napoca 2015",
				"914 - Team Dignitas | Cluj-napoca 2015",
				"915 - Team Dignitas (Foil) | Cluj-napoca 2015",
				"916 - Team Dignitas (Gold) | Cluj-napoca 2015",
				"917 - Counter Logic Gaming | Cluj-napoca 2015",
				"918 - Counter Logic Gaming (Foil) | Cluj-napoca 2015",
				"919 - Counter Logic Gaming (Gold) | Cluj-napoca 2015",
				"920 - Vexed Gaming | Cluj-napoca 2015",
				"921 - Vexed Gaming (Foil) | Cluj-napoca 2015",
				"922 - Vexed Gaming (Gold) | Cluj-napoca 2015",
				"923 - Flipsid3 Tactics | Cluj-napoca 2015",
				"924 - Flipsid3 Tactics (Foil) | Cluj-napoca 2015",
				"925 - Flipsid3 Tactics (Gold) | Cluj-napoca 2015",
				"926 - Team Liquid | Cluj-napoca 2015",
				"927 - Team Liquid (Foil) | Cluj-napoca 2015",
				"928 - Team Liquid (Gold) | Cluj-napoca 2015",
				"929 - Mousesports | Cluj-napoca 2015",
				"930 - Mousesports (Foil) | Cluj-napoca 2015",
				"931 - Mousesports (Gold) | Cluj-napoca 2015",
				"932 - Natus Vincere | Cluj-napoca 2015",
				"933 - Natus Vincere (Foil) | Cluj-napoca 2015",
				"934 - Natus Vincere (Gold) | Cluj-napoca 2015",
				"935 - Virtus.pro | Cluj-napoca 2015",
				"936 - Virtus.pro (Foil) | Cluj-napoca 2015",
				"937 - Virtus.pro (Gold) | Cluj-napoca 2015",
				"938 - Cloud9 | Cluj-napoca 2015",
				"939 - Cloud9 (Foil) | Cluj-napoca 2015",
				"940 - Cloud9 (Gold) | Cluj-napoca 2015",
				"941 - G2 Esports | Cluj-napoca 2015",
				"942 - G2 Esports (Foil) | Cluj-napoca 2015",
				"943 - G2 Esports (Gold) | Cluj-napoca 2015",
				"944 - Titan | Cluj-napoca 2015",
				"945 - Titan (Foil) | Cluj-napoca 2015",
				"946 - Titan (Gold) | Cluj-napoca 2015",
				"947 - Team Solomid | Cluj-napoca 2015",
				"948 - Team Solomid (Foil) | Cluj-napoca 2015",
				"949 - Team Solomid (Gold) | Cluj-napoca 2015",
				"950 - Team Envyus | Cluj-napoca 2015",
				"951 - Team Envyus (Foil) | Cluj-napoca 2015",
				"952 - Team Envyus (Gold) | Cluj-napoca 2015",
				"953 - Fnatic | Cluj-napoca 2015",
				"954 - Fnatic (Foil) | Cluj-napoca 2015",
				"955 - Fnatic (Gold) | Cluj-napoca 2015",
				"956 - Luminosity Gaming | Cluj-napoca 2015",
				"957 - Luminosity Gaming (Foil) | Cluj-napoca 2015",
				"958 - Luminosity Gaming (Gold) | Cluj-napoca 2015",
				"959 - Dreamhack | Cluj-napoca 2015",
				"960 - Dreamhack (Foil) | Cluj-napoca 2015",
				"961 - Dreamhack (Gold) | Cluj-napoca 2015",
				"962 - Ivette",
				"963 - Kimberly",
				"964 - Martha",
				"965 - Merietta",
				"966 - Sherry",
				"967 - Tamara",
				"968 - Ivette (Holo)",
				"969 - Kimberly (Holo)",
				"970 - Martha (Holo)",
				"971 - Merietta (Holo)",
				"972 - Sherry (Holo)",
				"973 - Tamara (Holo)",
				"974 - Boom",
				"975 - Boom (Holo)",
				"976 - Boom (Foil)",
				"977 - Countdown",
				"978 - Countdown (Holo)",
				"979 - Countdown (Foil)",
				"980 - Don't Worry",
				"981 - Don't Worry (Holo)",
				"982 - Don't Worry (Foil)",
				"983 - Hard Cluck Life",
				"984 - Hard Cluck Life (Holo)",
				"985 - Hard Cluck Life (Foil)",
				"986 - Move It",
				"987 - Move It (Holo)",
				"988 - Move It (Foil)",
				"989 - The Awper",
				"990 - The Baiter",
				"991 - The Bomber",
				"992 - The Bot",
				"993 - The Fragger",
				"994 - The Leader",
				"995 - The Lurker",
				"996 - The 'nader",
				"997 - The Ninja",
				"998 - Support",
				"999 - The Awper (Foil)"
				"1000 - The Bomber (Foil)",
				"1001 - The Fragger (Foil)",
				"1002 - The Leader (Foil)",
				"1003 - The Nader (Foil)",
				"1004 - Ninja (Foil)",
				"1005 - The Pro (Foil)",
				"1006 - Support (Foil)",
				"1007 - Ninjas In Pyjamas | Mlg Columbus 2016",
				"1008 - Ninjas In Pyjamas (Holo) | Mlg Columbus 2016",
				"1009 - Ninjas In Pyjamas (Foil) | Mlg Columbus 2016",
				"1010 - Ninjas In Pyjamas (Gold) | Mlg Columbus 2016",
				"1011 - Splyce | Mlg Columbus 2016",
				"1012 - Splyce (Holo) | Mlg Columbus 2016",
				"1013 - Splyce (Foil) | Mlg Columbus 2016",
				"1014 - Splyce (Gold) | Mlg Columbus 2016",
				"1015 - Counter Logic Gaming | Mlg Columbus 2016",
				"1016 - Counter Logic Gaming (Holo) | Mlg Columbus 2016",
				"1017 - Counter Logic Gaming (Foil) | Mlg Columbus 2016",
				"1018 - Counter Logic Gaming (Gold) | Mlg Columbus 2016",
				"1019 - Gambit Gaming | Mlg Columbus 2016",
				"1020 - Gambit Gaming (Holo) | Mlg Columbus 2016",
				"1021 - Gambit Gaming (Foil) | Mlg Columbus 2016",
				"1022 - Gambit Gaming (Gold) | Mlg Columbus 2016",
				"1023 - Flipsid3 Tactics | Mlg Columbus 2016",
				"1024 - Flipsid3 Tactics (Holo) | Mlg Columbus 2016",
				"1025 - Flipsid3 Tactics (Foil) | Mlg Columbus 2016",
				"1026 - Flipsid3 Tactics (Gold) | Mlg Columbus 2016",
				"1027 - Team Liquid | Mlg Columbus 2016",
				"1028 - Team Liquid (Holo) | Mlg Columbus 2016",
				"1029 - Team Liquid (Foil) | Mlg Columbus 2016",
				"1030 - Team Liquid (Gold) | Mlg Columbus 2016",
				"1031 - Mousesports | Mlg Columbus 2016",
				"1032 - Mousesports (Holo) | Mlg Columbus 2016",
				"1033 - Mousesports (Foil) | Mlg Columbus 2016",
				"1034 - Mousesports (Gold) | Mlg Columbus 2016",
				"1035 - Natus Vincere | Mlg Columbus 2016",
				"1036 - Natus Vincere (Holo) | Mlg Columbus 2016",
				"1037 - Natus Vincere (Foil) | Mlg Columbus 2016",
				"1038 - Natus Vincere (Gold) | Mlg Columbus 2016",
				"1039 - Virtus.pro | Mlg Columbus 2016",
				"1040 - Virtus.pro (Holo) | Mlg Columbus 2016",
				"1041 - Virtus.pro (Foil) | Mlg Columbus 2016",
				"1042 - Virtus.pro (Gold) | Mlg Columbus 2016",
				"1043 - Cloud9 | Mlg Columbus 2016",
				"1044 - Cloud9 (Holo) | Mlg Columbus 2016",
				"1045 - Cloud9 (Foil) | Mlg Columbus 2016",
				"1046 - Cloud9 (Gold) | Mlg Columbus 2016",
				"1047 - G2 Esports | Mlg Columbus 2016",
				"1048 - G2 Esports (Holo) | Mlg Columbus 2016",
				"1049 - G2 Esports (Foil) | Mlg Columbus 2016",
				"1050 - G2 Esports (Gold) | Mlg Columbus 2016",
				"1051 - Faze Clan | Mlg Columbus 2016",
				"1052 - Faze Clan (Holo) | Mlg Columbus 2016",
				"1053 - Faze Clan (Foil) | Mlg Columbus 2016",
				"1054 - Faze Clan (Gold) | Mlg Columbus 2016",
				"1055 - Astralis | Mlg Columbus 2016",
				"1056 - Astralis (Holo) | Mlg Columbus 2016",
				"1057 - Astralis (Foil) | Mlg Columbus 2016",
				"1058 - Astralis (Gold) | Mlg Columbus 2016",
				"1059 - Team Envyus | Mlg Columbus 2016",
				"1060 - Team Envyus (Holo) | Mlg Columbus 2016",
				"1061 - Team Envyus (Foil) | Mlg Columbus 2016",
				"1062 - Team Envyus (Gold) | Mlg Columbus 2016",
				"1063 - Fnatic | Mlg Columbus 2016",
				"1064 - Fnatic (Holo) | Mlg Columbus 2016",
				"1065 - Fnatic (Foil) | Mlg Columbus 2016",
				"1066 - Fnatic (Gold) | Mlg Columbus 2016",
				"1067 - Luminosity Gaming | Mlg Columbus 2016",
				"1068 - Luminosity Gaming (Holo) | Mlg Columbus 2016",
				"1069 - Luminosity Gaming (Foil) | Mlg Columbus 2016",
				"1070 - Luminosity Gaming (Gold) | Mlg Columbus 2016",
				"1071 - Mlg | Mlg Columbus 2016",
				"1072 - Mlg (Holo) | Mlg Columbus 2016",
				"1073 - Mlg (Foil) | Mlg Columbus 2016",
				"1074 - Mlg (Gold) | Mlg Columbus 2016",
				"1075 - Reltuc | Mlg Columbus 2016",
				"1076 - Reltuc (Foil) | Mlg Columbus 2016",
				"1077 - Reltuc (Gold) | Mlg Columbus 2016",
				"1078 - Fugly | Mlg Columbus 2016",
				"1079 - Fugly (Foil) | Mlg Columbus 2016",
				"1080 - Fugly (Gold) | Mlg Columbus 2016",
				"1081 - Hazed | Mlg Columbus 2016",
				"1082 - Hazed (Foil) | Mlg Columbus 2016",
				"1083 - Hazed (Gold) | Mlg Columbus 2016",
				"1084 - Jdm64 | Mlg Columbus 2016",
				"1085 - Jdm64 (Foil) | Mlg Columbus 2016",
				"1086 - Jdm64 (Gold) | Mlg Columbus 2016",
				"1087 - Tarik | Mlg Columbus 2016",
				"1088 - Tarik (Foil) | Mlg Columbus 2016",
				"1089 - Tarik (Gold) | Mlg Columbus 2016",
				"1090 - Freakazoid | Mlg Columbus 2016",
				"1091 - Freakazoid (Foil) | Mlg Columbus 2016",
				"1092 - Freakazoid (Gold) | Mlg Columbus 2016",
				"1093 - Stewie2k | Mlg Columbus 2016",
				"1094 - Stewie2k (Foil) | Mlg Columbus 2016",
				"1095 - Stewie2k (Gold) | Mlg Columbus 2016",
				"1096 - Shroud | Mlg Columbus 2016",
				"1097 - Shroud (Foil) | Mlg Columbus 2016",
				"1098 - Shroud (Gold) | Mlg Columbus 2016",
				"1099 - Skadoodle | Mlg Columbus 2016",
				"1100 - Skadoodle (Foil) | Mlg Columbus 2016",
				"1101 - Skadoodle (Gold) | Mlg Columbus 2016",
				"1102 - N0thing | Mlg Columbus 2016",
				"1103 - N0thing (Foil) | Mlg Columbus 2016",
				"1104 - N0thing (Gold) | Mlg Columbus 2016",
				"1105 - Apex | Mlg Columbus 2016",
				"1106 - Apex (Foil) | Mlg Columbus 2016",
				"1107 - Apex (Gold) | Mlg Columbus 2016",
				"1108 - Happy | Mlg Columbus 2016",
				"1109 - Happy (Foil) | Mlg Columbus 2016",
				"1110 - Happy (Gold) | Mlg Columbus 2016",
				"1111 - Devil | Mlg Columbus 2016",
				"1112 - Devil (Foil) | Mlg Columbus 2016",
				"1113 - Devil (Gold) | Mlg Columbus 2016",
				"1114 - Kennys | Mlg Columbus 2016",
				"1115 - Kennys (Foil) | Mlg Columbus 2016",
				"1116 - Kennys (Gold) | Mlg Columbus 2016",
				"1117 - Nbk- | Mlg Columbus 2016",
				"1118 - Nbk- (Foil) | Mlg Columbus 2016",
				"1119 - Nbk- (Gold) | Mlg Columbus 2016",
				"1120 - B1ad3 | Mlg Columbus 2016",
				"1121 - B1ad3 (Foil) | Mlg Columbus 2016",
				"1122 - B1ad3 (Gold) | Mlg Columbus 2016",
				"1123 - Bondik | Mlg Columbus 2016",
				"1124 - Bondik (Foil) | Mlg Columbus 2016",
				"1125 - Bondik (Gold) | Mlg Columbus 2016",
				"1126 - Shara | Mlg Columbus 2016",
				"1127 - Shara (Foil) | Mlg Columbus 2016",
				"1128 - Shara (Gold) | Mlg Columbus 2016",
				"1129 - Markeloff | Mlg Columbus 2016",
				"1130 - Markeloff (Foil) | Mlg Columbus 2016",
				"1131 - Markeloff (Gold) | Mlg Columbus 2016",
				"1132 - Worldedit | Mlg Columbus 2016",
				"1133 - Worldedit (Foil) | Mlg Columbus 2016",
				"1134 - Worldedit (Gold) | Mlg Columbus 2016",
				"1135 - Flusha | Mlg Columbus 2016",
				"1136 - Flusha (Foil) | Mlg Columbus 2016",
				"1137 - Flusha (Gold) | Mlg Columbus 2016",
				"1138 - Jw | Mlg Columbus 2016",
				"1139 - Jw (Foil) | Mlg Columbus 2016",
				"1140 - Jw (Gold) | Mlg Columbus 2016",
				"1141 - Krimz | Mlg Columbus 2016",
				"1142 - Krimz (Foil) | Mlg Columbus 2016",
				"1143 - Krimz (Gold) | Mlg Columbus 2016",
				"1144 - Olofmeister | Mlg Columbus 2016",
				"1145 - Olofmeister (Foil) | Mlg Columbus 2016",
				"1146 - Olofmeister (Gold) | Mlg Columbus 2016",
				"1147 - Dennis | Mlg Columbus 2016",
				"1148 - Dennis (Foil) | Mlg Columbus 2016",
				"1149 - Dennis (Gold) | Mlg Columbus 2016",
				"1150 - Aizy | Mlg Columbus 2016",
				"1151 - Aizy (Foil) | Mlg Columbus 2016",
				"1152 - Aizy (Gold) | Mlg Columbus 2016",
				"1153 - Fox | Mlg Columbus 2016",
				"1154 - Fox (Foil) | Mlg Columbus 2016",
				"1155 - Fox (Gold) | Mlg Columbus 2016",
				"1156 - Maikelele | Mlg Columbus 2016",
				"1157 - Maikelele (Foil) | Mlg Columbus 2016",
				"1158 - Maikelele (Gold) | Mlg Columbus 2016",
				"1159 - Rain | Mlg Columbus 2016",
				"1160 - Rain (Foil) | Mlg Columbus 2016",
				"1161 - Rain (Gold) | Mlg Columbus 2016",
				"1162 - Jkaem | Mlg Columbus 2016",
				"1163 - Jkaem (Foil) | Mlg Columbus 2016",
				"1164 - Jkaem (Gold) | Mlg Columbus 2016",
				"1165 - Fnx | Mlg Columbus 2016",
				"1166 - Fnx (Foil) | Mlg Columbus 2016",
				"1167 - Fnx (Gold) | Mlg Columbus 2016",
				"1168 - Coldzera | Mlg Columbus 2016",
				"1169 - Coldzera (Foil) | Mlg Columbus 2016",
				"1170 - Coldzera (Gold) | Mlg Columbus 2016",
				"1171 - Fallen | Mlg Columbus 2016",
				"1172 - Fallen (Foil) | Mlg Columbus 2016",
				"1173 - Fallen (Gold) | Mlg Columbus 2016",
				"1174 - Fer | Mlg Columbus 2016",
				"1175 - Fer (Foil) | Mlg Columbus 2016",
				"1176 - Fer (Gold) | Mlg Columbus 2016",
				"1177 - Taco | Mlg Columbus 2016",
				"1178 - Taco (Foil) | Mlg Columbus 2016",
				"1179 - Taco (Gold) | Mlg Columbus 2016",
				"1180 - Chrisj | Mlg Columbus 2016",
				"1181 - Chrisj (Foil) | Mlg Columbus 2016",
				"1182 - Chrisj (Gold) | Mlg Columbus 2016",
				"1183 - Denis | Mlg Columbus 2016",
				"1184 - Denis (Foil) | Mlg Columbus 2016",
				"1185 - Denis (Gold) | Mlg Columbus 2016",
				"1186 - Spiidi | Mlg Columbus 2016",
				"1187 - Spiidi (Foil) | Mlg Columbus 2016",
				"1188 - Spiidi (Gold) | Mlg Columbus 2016",
				"1189 - Nex | Mlg Columbus 2016",
				"1190 - Nex (Foil) | Mlg Columbus 2016",
				"1191 - Nex (Gold) | Mlg Columbus 2016",
				"1192 - Niko | Mlg Columbus 2016",
				"1193 - Niko (Foil) | Mlg Columbus 2016",
				"1194 - Niko (Gold) | Mlg Columbus 2016",
				"1195 - Edward | Mlg Columbus 2016",
				"1196 - Edward (Foil) | Mlg Columbus 2016",
				"1197 - Edward (Gold) | Mlg Columbus 2016",
				"1198 - Flamie | Mlg Columbus 2016",
				"1199 - Flamie (Foil) | Mlg Columbus 2016",
				"1200 - Flamie (Gold) | Mlg Columbus 2016",
				"1201 - Guardian | Mlg Columbus 2016",
				"1202 - Guardian (Foil) | Mlg Columbus 2016",
				"1203 - Guardian (Gold) | Mlg Columbus 2016",
				"1204 - Seized | Mlg Columbus 2016",
				"1205 - Seized (Foil) | Mlg Columbus 2016",
				"1206 - Seized (Gold) | Mlg Columbus 2016",
				"1207 - Zeus | Mlg Columbus 2016",
				"1208 - Zeus (Foil) | Mlg Columbus 2016",
				"1209 - Zeus (Gold) | Mlg Columbus 2016",
				"1210 - Pyth | Mlg Columbus 2016",
				"1211 - Pyth (Foil) | Mlg Columbus 2016",
				"1212 - Pyth (Gold) | Mlg Columbus 2016",
				"1213 - F0rest | Mlg Columbus 2016",
				"1214 - F0rest (Foil) | Mlg Columbus 2016",
				"1215 - F0rest (Gold) | Mlg Columbus 2016",
				"1216 - Friberg | Mlg Columbus 2016",
				"1217 - Friberg (Foil) | Mlg Columbus 2016",
				"1218 - Friberg (Gold) | Mlg Columbus 2016",
				"1219 - Get_right | Mlg Columbus 2016",
				"1220 - Get_right (Foil) | Mlg Columbus 2016",
				"1221 - Get_right (Gold) | Mlg Columbus 2016",
				"1222 - Xizt | Mlg Columbus 2016",
				"1223 - Xizt (Foil) | Mlg Columbus 2016",
				"1224 - Xizt (Gold) | Mlg Columbus 2016",
				"1225 - Jasonr | Mlg Columbus 2016",
				"1226 - Jasonr (Foil) | Mlg Columbus 2016",
				"1227 - Jasonr (Gold) | Mlg Columbus 2016",
				"1228 - Arya | Mlg Columbus 2016",
				"1229 - Arya (Foil) | Mlg Columbus 2016",
				"1230 - Arya (Gold) | Mlg Columbus 2016",
				"1231 - Professor_chaos | Mlg Columbus 2016",
				"1232 - Professor_chaos (Foil) | Mlg Columbus 2016",
				"1233 - Professor_chaos (Gold) | Mlg Columbus 2016",
				"1234 - Davey | Mlg Columbus 2016",
				"1235 - Davey (Foil) | Mlg Columbus 2016",
				"1236 - Davey (Gold) | Mlg Columbus 2016",
				"1237 - Abe | Mlg Columbus 2016",
				"1238 - Abe (Foil) | Mlg Columbus 2016",
				"1239 - Abe (Gold) | Mlg Columbus 2016",
				"1240 - Adren | Mlg Columbus 2016",
				"1241 - Adren (Foil) | Mlg Columbus 2016",
				"1242 - Adren (Gold) | Mlg Columbus 2016",
				"1243 - Elige | Mlg Columbus 2016",
				"1244 - Elige (Foil) | Mlg Columbus 2016",
				"1245 - Elige (Gold) | Mlg Columbus 2016",
				"1246 - S1mple | Mlg Columbus 2016",
				"1247 - S1mple (Foil) | Mlg Columbus 2016",
				"1248 - S1mple (Gold) | Mlg Columbus 2016",
				"1249 - Hiko | Mlg Columbus 2016",
				"1250 - Hiko (Foil) | Mlg Columbus 2016",
				"1251 - Hiko (Gold) | Mlg Columbus 2016",
				"1252 - Nitr0 | Mlg Columbus 2016",
				"1253 - Nitr0 (Foil) | Mlg Columbus 2016",
				"1254 - Nitr0 (Gold) | Mlg Columbus 2016",
				"1255 - Ex6tenz | Mlg Columbus 2016",
				"1256 - Ex6tenz (Foil) | Mlg Columbus 2016",
				"1257 - Ex6tenz (Gold) | Mlg Columbus 2016",
				"1258 - Rpk | Mlg Columbus 2016",
				"1259 - Rpk (Foil) | Mlg Columbus 2016",
				"1260 - Rpk (Gold) | Mlg Columbus 2016",
				"1261 - Scream | Mlg Columbus 2016",
				"1262 - Scream (Foil) | Mlg Columbus 2016",
				"1263 - Scream (Gold) | Mlg Columbus 2016",
				"1264 - Shox | Mlg Columbus 2016",
				"1265 - Shox (Foil) | Mlg Columbus 2016",
				"1266 - Shox (Gold) | Mlg Columbus 2016",
				"1267 - Smithzz | Mlg Columbus 2016",
				"1268 - Smithzz (Foil) | Mlg Columbus 2016",
				"1269 - Smithzz (Gold) | Mlg Columbus 2016",
				"1270 - Cajunb | Mlg Columbus 2016",
				"1271 - Cajunb (Foil) | Mlg Columbus 2016",
				"1272 - Cajunb (Gold) | Mlg Columbus 2016",
				"1273 - Device | Mlg Columbus 2016",
				"1274 - Device (Foil) | Mlg Columbus 2016",
				"1275 - Device (Gold) | Mlg Columbus 2016",
				"1276 - Dupreeh | Mlg Columbus 2016",
				"1277 - Dupreeh (Foil) | Mlg Columbus 2016",
				"1278 - Dupreeh (Gold) | Mlg Columbus 2016",
				"1279 - Karrigan | Mlg Columbus 2016",
				"1280 - Karrigan (Foil) | Mlg Columbus 2016",
				"1281 - Karrigan (Gold) | Mlg Columbus 2016",
				"1282 - Xyp9x | Mlg Columbus 2016",
				"1283 - Xyp9x (Foil) | Mlg Columbus 2016",
				"1284 - Xyp9x (Gold) | Mlg Columbus 2016",
				"1285 - Waylander | Mlg Columbus 2016",
				"1286 - Waylander (Foil) | Mlg Columbus 2016",
				"1287 - Waylander (Gold) | Mlg Columbus 2016",
				"1288 - Dosia | Mlg Columbus 2016",
				"1289 - Dosia (Foil) | Mlg Columbus 2016",
				"1290 - Dosia (Gold) | Mlg Columbus 2016",
				"1291 - Hooch | Mlg Columbus 2016",
				"1292 - Hooch (Foil) | Mlg Columbus 2016",
				"1293 - Hooch (Gold) | Mlg Columbus 2016",
				"1294 - Mou | Mlg Columbus 2016",
				"1295 - Mou (Foil) | Mlg Columbus 2016",
				"1296 - Mou (Gold) | Mlg Columbus 2016",
				"1297 - Adren  | Mlg Columbus 2016",
				"1298 - Adren (Foil)  | Mlg Columbus 2016",
				"1299 - Adren (Gold)  | Mlg Columbus 2016",
				"1300 - Byali | Mlg Columbus 2016",
				"1301 - Byali (Foil) | Mlg Columbus 2016",
				"1302 - Byali (Gold) | Mlg Columbus 2016",
				"1303 - Neo | Mlg Columbus 2016",
				"1304 - Neo (Foil) | Mlg Columbus 2016",
				"1305 - Neo (Gold) | Mlg Columbus 2016",
				"1306 - Pashabiceps | Mlg Columbus 2016",
				"1307 - Pashabiceps (Foil) | Mlg Columbus 2016",
				"1308 - Pashabiceps (Gold) | Mlg Columbus 2016",
				"1309 - Snax | Mlg Columbus 2016",
				"1310 - Snax (Foil) | Mlg Columbus 2016",
				"1311 - Snax (Gold) | Mlg Columbus 2016",
				"1312 - Taz | Mlg Columbus 2016",
				"1313 - Taz (Foil) | Mlg Columbus 2016",
				"1314 - Taz (Gold) | Mlg Columbus 2016",
				"1315 - All-stars Orange (Holo)",
				"1316 - All-stars Blue (Holo)",
				"1317 - Ninjas In Pyjamas | Cologne 2016",
				"1318 - Ninjas In Pyjamas (Holo) | Cologne 2016",
				"1319 - Ninjas In Pyjamas (Foil) | Cologne 2016",
				"1320 - Ninjas In Pyjamas (Gold) | Cologne 2016",
				"1321 - Optic Gaming | Cologne 2016",
				"1322 - Optic Gaming (Holo) | Cologne 2016",
				"1323 - Optic Gaming (Foil) | Cologne 2016",
				"1324 - Optic Gaming (Gold) | Cologne 2016",
				"1325 - Counter Logic Gaming | Cologne 2016",
				"1326 - Counter Logic Gaming (Holo) | Cologne 2016",
				"1327 - Counter Logic Gaming (Foil) | Cologne 2016",
				"1328 - Counter Logic Gaming (Gold) | Cologne 2016",
				"1329 - Gambit Gaming | Cologne 2016",
				"1330 - Gambit Gaming (Holo) | Cologne 2016",
				"1331 - Gambit Gaming (Foil) | Cologne 2016",
				"1332 - Gambit Gaming (Gold) | Cologne 2016",
				"1333 - Flipsid3 Tactics | Cologne 2016",
				"1334 - Flipsid3 Tactics (Holo) | Cologne 2016",
				"1335 - Flipsid3 Tactics (Foil) | Cologne 2016",
				"1336 - Flipsid3 Tactics (Gold) | Cologne 2016",
				"1337 - Team Liquid | Cologne 2016",
				"1338 - Team Liquid (Holo) | Cologne 2016",
				"1339 - Team Liquid (Foil) | Cologne 2016",
				"1340 - Team Liquid (Gold) | Cologne 2016",
				"1341 - Mousesports | Cologne 2016",
				"1342 - Mousesports (Holo) | Cologne 2016",
				"1343 - Mousesports (Foil) | Cologne 2016",
				"1344 - Mousesports (Gold) | Cologne 2016",
				"1345 - Natus Vincere | Cologne 2016",
				"1346 - Natus Vincere (Holo) | Cologne 2016",
				"1347 - Natus Vincere (Foil) | Cologne 2016",
				"1348 - Natus Vincere (Gold) | Cologne 2016",
				"1349 - Virtus.pro | Cologne 2016",
				"1350 - Virtus.pro (Holo) | Cologne 2016",
				"1351 - Virtus.pro (Foil) | Cologne 2016",
				"1352 - Virtus.pro (Gold) | Cologne 2016",
				"1353 - Sk Gaming | Cologne 2016",
				"1354 - Sk Gaming (Holo) | Cologne 2016",
				"1355 - Sk Gaming (Foil) | Cologne 2016",
				"1356 - Sk Gaming (Gold) | Cologne 2016",
				"1357 - G2 Esports | Cologne 2016",
				"1358 - G2 Esports (Holo) | Cologne 2016",
				"1359 - G2 Esports (Foil) | Cologne 2016",
				"1360 - G2 Esports (Gold) | Cologne 2016",
				"1361 - Faze Clan | Cologne 2016",
				"1362 - Faze Clan (Holo) | Cologne 2016",
				"1363 - Faze Clan (Foil) | Cologne 2016",
				"1364 - Faze Clan (Gold) | Cologne 2016",
				"1365 - Astralis | Cologne 2016",
				"1366 - Astralis (Holo) | Cologne 2016",
				"1367 - Astralis (Foil) | Cologne 2016",
				"1368 - Astralis (Gold) | Cologne 2016",
				"1369 - Team Envyus | Cologne 2016",
				"1370 - Team Envyus (Holo) | Cologne 2016",
				"1371 - Team Envyus (Foil) | Cologne 2016",
				"1372 - Team Envyus (Gold) | Cologne 2016",
				"1373 - Fnatic | Cologne 2016",
				"1374 - Fnatic (Holo) | Cologne 2016",
				"1375 - Fnatic (Foil) | Cologne 2016",
				"1376 - Fnatic (Gold) | Cologne 2016",
				"1377 - Team Dignitas | Cologne 2016",
				"1378 - Team Dignitas (Holo) | Cologne 2016",
				"1379 - Team Dignitas (Foil) | Cologne 2016",
				"1380 - Team Dignitas (Gold) | Cologne 2016",
				"1381 - Esl | Cologne 2016",
				"1382 - Esl (Holo) | Cologne 2016",
				"1383 - Esl (Foil) | Cologne 2016",
				"1384 - Esl (Gold) | Cologne 2016",
				"1385 - Reltuc | Cologne 2016",
				"1386 - Reltuc (Foil) | Cologne 2016",
				"1387 - Reltuc (Gold) | Cologne 2016",
				"1388 - Koosta | Cologne 2016",
				"1389 - Koosta (Foil) | Cologne 2016",
				"1390 - Koosta (Gold) | Cologne 2016",
				"1391 - Hazed | Cologne 2016",
				"1392 - Hazed (Foil) | Cologne 2016",
				"1393 - Hazed (Gold) | Cologne 2016",
				"1394 - Pita | Cologne 2016",
				"1395 - Pita (Foil) | Cologne 2016",
				"1396 - Pita (Gold) | Cologne 2016",
				"1397 - Tarik | Cologne 2016",
				"1398 - Tarik (Foil) | Cologne 2016",
				"1399 - Tarik (Gold) | Cologne 2016",
				"1400 - Daps | Cologne 2016",
				"1401 - Daps (Foil) | Cologne 2016",
				"1402 - Daps (Gold) | Cologne 2016",
				"1403 - Mixwell | Cologne 2016",
				"1404 - Mixwell (Foil) | Cologne 2016",
				"1405 - Mixwell (Gold) | Cologne 2016",
				"1406 - Naf | Cologne 2016",
				"1407 - Naf (Foil) | Cologne 2016",
				"1408 - Naf (Gold) | Cologne 2016",
				"1409 - Rush | Cologne 2016",
				"1410 - Rush (Foil) | Cologne 2016",
				"1411 - Rush (Gold) | Cologne 2016",
				"1412 - Stanislaw | Cologne 2016",
				"1413 - Stanislaw (Foil) | Cologne 2016",
				"1414 - Stanislaw (Gold) | Cologne 2016",
				"1415 - Apex | Cologne 2016",
				"1416 - Apex (Foil) | Cologne 2016",
				"1417 - Apex (Gold) | Cologne 2016",
				"1418 - Happy | Cologne 2016",
				"1419 - Happy (Foil) | Cologne 2016",
				"1420 - Happy (Gold) | Cologne 2016",
				"1421 - Devil | Cologne 2016",
				"1422 - Devil (Foil) | Cologne 2016",
				"1423 - Devil (Gold) | Cologne 2016",
				"1424 - Kennys | Cologne 2016",
				"1425 - Kennys (Foil) | Cologne 2016",
				"1426 - Kennys (Gold) | Cologne 2016",
				"1427 - Nbk- | Cologne 2016",
				"1428 - Nbk- (Foil) | Cologne 2016",
				"1429 - Nbk- (Gold) | Cologne 2016",
				"1430 - B1ad3 | Cologne 2016",
				"1431 - B1ad3 (Foil) | Cologne 2016",
				"1432 - B1ad3 (Gold) | Cologne 2016",
				"1433 - Waylander | Cologne 2016",
				"1434 - Waylander (Foil) | Cologne 2016",
				"1435 - Waylander (Gold) | Cologne 2016",
				"1436 - Shara | Cologne 2016",
				"1437 - Shara (Foil) | Cologne 2016",
				"1438 - Shara (Gold) | Cologne 2016",
				"1439 - Markeloff | Cologne 2016",
				"1440 - Markeloff (Foil) | Cologne 2016",
				"1441 - Markeloff (Gold) | Cologne 2016",
				"1442 - Worldedit | Cologne 2016",
				"1443 - Worldedit (Foil) | Cologne 2016",
				"1444 - Worldedit (Gold) | Cologne 2016",
				"1445 - Flusha | Cologne 2016",
				"1446 - Flusha (Foil) | Cologne 2016",
				"1447 - Flusha (Gold) | Cologne 2016",
				"1448 - Jw | Cologne 2016",
				"1449 - Jw (Foil) | Cologne 2016",
				"1450 - Jw (Gold) | Cologne 2016",
				"1451 - Krimz | Cologne 2016",
				"1452 - Krimz (Foil) | Cologne 2016",
				"1453 - Krimz (Gold) | Cologne 2016",
				"1454 - Olofmeister | Cologne 2016",
				"1455 - Olofmeister (Foil) | Cologne 2016",
				"1456 - Olofmeister (Gold) | Cologne 2016",
				"1457 - Dennis | Cologne 2016",
				"1458 - Dennis (Foil) | Cologne 2016",
				"1459 - Dennis (Gold) | Cologne 2016",
				"1460 - Aizy | Cologne 2016",
				"1461 - Aizy (Foil) | Cologne 2016",
				"1462 - Aizy (Gold) | Cologne 2016",
				"1463 - Fox | Cologne 2016",
				"1464 - Fox (Foil) | Cologne 2016",
				"1465 - Fox (Gold) | Cologne 2016",
				"1466 - Kioshima | Cologne 2016",
				"1467 - Kioshima (Foil) | Cologne 2016",
				"1468 - Kioshima (Gold) | Cologne 2016",
				"1469 - Rain | Cologne 2016",
				"1470 - Rain (Foil) | Cologne 2016",
				"1471 - Rain (Gold) | Cologne 2016",
				"1472 - Jkaem | Cologne 2016",
				"1473 - Jkaem (Foil) | Cologne 2016",
				"1474 - Jkaem (Gold) | Cologne 2016",
				"1475 - Coldzera | Cologne 2016",
				"1476 - Coldzera (Foil) | Cologne 2016",
				"1477 - Coldzera (Gold) | Cologne 2016",
				"1478 - Fallen | Cologne 2016",
				"1479 - Fallen (Foil) | Cologne 2016",
				"1480 - Fallen (Gold) | Cologne 2016",
				"1481 - Fer | Cologne 2016",
				"1482 - Fer (Foil) | Cologne 2016",
				"1483 - Fer (Gold) | Cologne 2016",
				"1484 - Fnx | Cologne 2016",
				"1485 - Fnx (Foil) | Cologne 2016",
				"1486 - Fnx (Gold) | Cologne 2016",
				"1487 - Taco | Cologne 2016",
				"1488 - Taco (Foil) | Cologne 2016",
				"1489 - Taco (Gold) | Cologne 2016",
				"1490 - Chrisj | Cologne 2016",
				"1491 - Chrisj (Foil) | Cologne 2016",
				"1492 - Chrisj (Gold) | Cologne 2016",
				"1493 - Denis | Cologne 2016",
				"1494 - Denis (Foil) | Cologne 2016",
				"1495 - Denis (Gold) | Cologne 2016",
				"1496 - Spiidi | Cologne 2016",
				"1497 - Spiidi (Foil) | Cologne 2016",
				"1498 - Spiidi (Gold) | Cologne 2016",
				"1499 - Nex | Cologne 2016",
				"1500 - Nex (Foil) | Cologne 2016",
				"1501 - Nex (Gold) | Cologne 2016",
				"1502 - Niko | Cologne 2016",
				"1503 - Niko (Foil) | Cologne 2016",
				"1504 - Niko (Gold) | Cologne 2016",
				"1505 - Edward | Cologne 2016",
				"1506 - Edward (Foil) | Cologne 2016",
				"1507 - Edward (Gold) | Cologne 2016",
				"1508 - Flamie | Cologne 2016",
				"1509 - Flamie (Foil) | Cologne 2016",
				"1510 - Flamie (Gold) | Cologne 2016",
				"1511 - Guardian | Cologne 2016",
				"1512 - Guardian (Foil) | Cologne 2016",
				"1513 - Guardian (Gold) | Cologne 2016",
				"1514 - Seized | Cologne 2016",
				"1515 - Seized (Foil) | Cologne 2016",
				"1516 - Seized (Gold) | Cologne 2016",
				"1517 - Zeus | Cologne 2016",
				"1518 - Zeus (Foil) | Cologne 2016",
				"1519 - Zeus (Gold) | Cologne 2016",
				"1520 - Pyth | Cologne 2016",
				"1521 - Pyth (Foil) | Cologne 2016",
				"1522 - Pyth (Gold) | Cologne 2016",
				"1523 - F0rest | Cologne 2016",
				"1524 - F0rest (Foil) | Cologne 2016",
				"1525 - F0rest (Gold) | Cologne 2016",
				"1526 - Friberg | Cologne 2016",
				"1527 - Friberg (Foil) | Cologne 2016",
				"1528 - Friberg (Gold) | Cologne 2016",
				"1529 - Get_right | Cologne 2016",
				"1530 - Get_right (Foil) | Cologne 2016",
				"1531 - Get_right (Gold) | Cologne 2016",
				"1532 - Xizt | Cologne 2016",
				"1533 - Xizt (Foil) | Cologne 2016",
				"1534 - Xizt (Gold) | Cologne 2016",
				"1535 - Cajunb | Cologne 2016",
				"1536 - Cajunb (Foil) | Cologne 2016",
				"1537 - Cajunb (Gold) | Cologne 2016",
				"1538 - Msl | Cologne 2016",
				"1539 - Msl (Foil) | Cologne 2016",
				"1540 - Msl (Gold) | Cologne 2016",
				"1541 - Tenzki | Cologne 2016",
				"1542 - Tenzki (Foil) | Cologne 2016",
				"1543 - Tenzki (Gold) | Cologne 2016",
				"1544 - Rubino | Cologne 2016",
				"1545 - Rubino (Foil) | Cologne 2016",
				"1546 - Rubino (Gold) | Cologne 2016",
				"1547 - K0nfig | Cologne 2016",
				"1548 - K0nfig (Foil) | Cologne 2016",
				"1549 - K0nfig (Gold) | Cologne 2016",
				"1550 - Jdm64 | Cologne 2016",
				"1551 - Jdm64 (Foil) | Cologne 2016",
				"1552 - Jdm64 (Gold) | Cologne 2016",
				"1553 - Elige | Cologne 2016",
				"1554 - Elige (Foil) | Cologne 2016",
				"1555 - Elige (Gold) | Cologne 2016",
				"1556 - S1mple | Cologne 2016",
				"1557 - S1mple (Foil) | Cologne 2016",
				"1558 - S1mple (Gold) | Cologne 2016",
				"1559 - Hiko | Cologne 2016",
				"1560 - Hiko (Foil) | Cologne 2016",
				"1561 - Hiko (Gold) | Cologne 2016",
				"1562 - Nitr0 | Cologne 2016",
				"1563 - Nitr0 (Foil) | Cologne 2016",
				"1564 - Nitr0 (Gold) | Cologne 2016",
				"1565 - Bodyy | Cologne 2016",
				"1566 - Bodyy (Foil) | Cologne 2016",
				"1567 - Bodyy (Gold) | Cologne 2016",
				"1568 - Rpk | Cologne 2016",
				"1569 - Rpk (Foil) | Cologne 2016",
				"1570 - Rpk (Gold) | Cologne 2016",
				"1571 - Scream | Cologne 2016",
				"1572 - Scream (Foil) | Cologne 2016",
				"1573 - Scream (Gold) | Cologne 2016",
				"1574 - Shox | Cologne 2016",
				"1575 - Shox (Foil) | Cologne 2016",
				"1576 - Shox (Gold) | Cologne 2016",
				"1577 - Smithzz | Cologne 2016",
				"1578 - Smithzz (Foil) | Cologne 2016",
				"1579 - Smithzz (Gold) | Cologne 2016",
				"1580 - Gla1ve | Cologne 2016",
				"1581 - Gla1ve (Foil) | Cologne 2016",
				"1582 - Gla1ve (Gold) | Cologne 2016",
				"1583 - Device | Cologne 2016",
				"1584 - Device (Foil) | Cologne 2016",
				"1585 - Device (Gold) | Cologne 2016",
				"1586 - Dupreeh | Cologne 2016",
				"1587 - Dupreeh (Foil) | Cologne 2016",
				"1588 - Dupreeh (Gold) | Cologne 2016",
				"1589 - Karrigan | Cologne 2016",
				"1590 - Karrigan (Foil) | Cologne 2016",
				"1591 - Karrigan (Gold) | Cologne 2016",
				"1592 - Xyp9x | Cologne 2016",
				"1593 - Xyp9x (Foil) | Cologne 2016",
				"1594 - Xyp9x (Gold) | Cologne 2016",
				"1595 - Spaze | Cologne 2016",
				"1596 - Spaze (Foil) | Cologne 2016",
				"1597 - Spaze (Gold) | Cologne 2016",
				"1598 - Dosia | Cologne 2016",
				"1599 - Dosia (Foil) | Cologne 2016",
				"1600 - Dosia (Gold) | Cologne 2016",
				"1601 - Hooch | Cologne 2016",
				"1602 - Hooch (Foil) | Cologne 2016",
				"1603 - Hooch (Gold) | Cologne 2016",
				"1604 - Mou | Cologne 2016",
				"1605 - Mou (Foil) | Cologne 2016",
				"1606 - Mou (Gold) | Cologne 2016",
				"1607 - Adren | Cologne 2016",
				"1608 - Adren (Foil) | Cologne 2016",
				"1609 - Adren (Gold) | Cologne 2016",
				"1610 - Byali | Cologne 2016",
				"1611 - Byali (Foil) | Cologne 2016",
				"1612 - Byali (Gold) | Cologne 2016",
				"1613 - Neo | Cologne 2016",
				"1614 - Neo (Foil) | Cologne 2016",
				"1615 - Neo (Gold) | Cologne 2016",
				"1616 - Pashabiceps | Cologne 2016",
				"1617 - Pashabiceps (Foil) | Cologne 2016",
				"1618 - Pashabiceps (Gold) | Cologne 2016",
				"1619 - Snax | Cologne 2016",
				"1620 - Snax (Foil) | Cologne 2016",
				"1621 - Snax (Gold) | Cologne 2016",
				"1622 - Taz | Cologne 2016",
				"1623 - Taz (Foil) | Cologne 2016",
				"1624 - Taz (Gold) | Cologne 2016",
				"1625 - Boris",
				"1626 - Max",
				"1627 - Stan",
				"1628 - Jack",
				"1629 - Perry",
				"1630 - Viggo",
				"1631 - Joan",
				"1632 - Boris (Holo)",
				"1633 - Max (Holo)",
				"1634 - Stan (Holo)",
				"1635 - Jack (Holo)",
				"1636 - Perry (Holo)",
				"1637 - Viggo (Holo)",
				"1638 - Joan (Holo)",
				"1639 - Basilisk",
				"1640 - Dragon",
				"1641 - Hippocamp",
				"1642 - Manticore",
				"1643 - Pegasus",
				"1644 - Phoenix Reborn",
				"1645 - Sphinx",
				"1646 - Hippocamp (Holo)",
				"1647 - Manticore (Holo)",
				"1648 - Pegasus (Holo)",
				"1649 - Sphinx (Holo)",
				"1650 - Basilisk (Foil)",
				"1651 - Dragon (Foil)",
				"1652 - Phoenix Reborn (Foil)",
				"1653 - Blood Boiler",
				"1654 - Chabo",
				"1655 - Drug War Veteran",
				"1656 - Flickshot",
				"1657 - Hamster Hawk",
				"1658 - Ivette",
				"1659 - Kawaii Killer Ct",
				"1660 - Kawaii Killer Terrorist",
				"1661 - Martha",
				"1662 - Old School",
				"1663 - Pocket Bbq",
				"1664 - Rekt",
				"1665 - Shave Master",
				"1666 - Shooting Star Return",
				"1667 - Rising Skull",
				"1668 - Tamara",
				"1669 - Unicorn",
				"1670 - Winged Defuser",
				"1671 - Ace",
				"1672 - Banana",
				"1673 - Cerberus",
				"1674 - Clutch King",
				"1675 - Crown",
				"1676 - Guardian",
				"1677 - Ez",
				"1678 - Fire Serpent",
				"1679 - Howling Dawn",
				"1680 - Kisses",
				"1681 - Easy Peasy",
				"1682 - Nice Shot",
				"1683 - Phoenix",
				"1684 - Real Mvp",
				"1685 - R.i.p.i.p.",
				"1686 - Skull N' Crosshairs",
				"1687 - Welcome To The Clutch",
				"1688 - Wings",
				"1697 - X-axes",
				"1698 - Death Sentence",
				"1699 - Chess King",
				"1700 - King Me",
				"1701 - Keep The Change",
				"1702 - Double",
				"1703 - Eco",
				"1704 - Tilt",
				"1705 - Speechless",
				"1706 - Qq",
				"1707 - Mr. Teeth",
				"1708 - Ninja",
				"1709 - Worry",
				"1710 - Rage Mode",
				"1711 - Eye Spy",
				"1712 - Ggez",
				"1713 - Ggwp",
				"1714 - Glhf",
				"1715 - Quickdraw",
				"1716 - Backstab",
				"1717 - Loser",
				"1718 - Sheriff",
				"1719 - Tombstone",
				"1720 - Heart",
				"1721 - 8-ball",
				"1722 - Lambda",
				"1723 - Still Happy",
				"1724 - Jump Shot",
				"1725 - Karambit",
				"1726 - X-knives",
				"1727 - Toasted",
				"1728 - Bling",
				"1729 - Noscope",
				"1730 - Piggles",
				"1731 - Popdog",
				"1732 - Cocky",
				"1733 - Nacl",
				"1734 - Sorry",
				"1735 - Eat It",
				"1736 - Take Flight",
				"1737 - Gtg",
				"1738 - Astralis | Atlanta 2017",
				"1739 - Astralis (Holo) | Atlanta 2017",
				"1740 - Astralis (Foil) | Atlanta 2017",
				"1741 - Astralis (Gold) | Atlanta 2017",
				"1742 - Team Envyus | Atlanta 2017",
				"1743 - Team Envyus (Holo) | Atlanta 2017",
				"1744 - Team Envyus (Foil) | Atlanta 2017",
				"1745 - Team Envyus (Gold) | Atlanta 2017",
				"1746 - Faze Clan | Atlanta 2017",
				"1747 - Faze Clan (Holo) | Atlanta 2017",
				"1748 - Faze Clan (Foil) | Atlanta 2017",
				"1749 - Faze Clan (Gold) | Atlanta 2017",
				"1750 - Flipsid3 Tactics | Atlanta 2017",
				"1751 - Flipsid3 Tactics (Holo) | Atlanta 2017",
				"1752 - Flipsid3 Tactics (Foil) | Atlanta 2017",
				"1753 - Flipsid3 Tactics (Gold) | Atlanta 2017",
				"1754 - Fnatic | Atlanta 2017",
				"1755 - Fnatic (Holo) | Atlanta 2017",
				"1756 - Fnatic (Foil) | Atlanta 2017",
				"1757 - Fnatic (Gold) | Atlanta 2017",
				"1758 - G2 Esports | Atlanta 2017",
				"1759 - G2 Esports (Holo) | Atlanta 2017",
				"1760 - G2 Esports (Foil) | Atlanta 2017",
				"1761 - G2 Esports (Gold) | Atlanta 2017",
				"1762 - Gambit Gaming | Atlanta 2017",
				"1763 - Gambit Gaming (Holo) | Atlanta 2017",
				"1764 - Gambit Gaming (Foil) | Atlanta 2017",
				"1765 - Gambit Gaming (Gold) | Atlanta 2017",
				"1766 - Godsent | Atlanta 2017",
				"1767 - Godsent (Holo) | Atlanta 2017",
				"1768 - Godsent (Foil) | Atlanta 2017",
				"1769 - Godsent (Gold) | Atlanta 2017",
				"1770 - Hellraisers | Atlanta 2017",
				"1771 - Hellraisers (Holo) | Atlanta 2017",
				"1772 - Hellraisers (Foil) | Atlanta 2017",
				"1773 - Hellraisers (Gold) | Atlanta 2017",
				"1774 - Mousesports | Atlanta 2017",
				"1775 - Mousesports (Holo) | Atlanta 2017",
				"1776 - Mousesports (Foil) | Atlanta 2017",
				"1777 - Mousesports (Gold) | Atlanta 2017",
				"1778 - Natus Vincere | Atlanta 2017",
				"1779 - Natus Vincere (Holo) | Atlanta 2017",
				"1780 - Natus Vincere (Foil) | Atlanta 2017",
				"1781 - Natus Vincere (Gold) | Atlanta 2017",
				"1782 - North | Atlanta 2017",
				"1783 - North (Holo) | Atlanta 2017",
				"1784 - North (Foil) | Atlanta 2017",
				"1785 - North (Gold) | Atlanta 2017",
				"1786 - Optic Gaming | Atlanta 2017",
				"1787 - Optic Gaming (Holo) | Atlanta 2017",
				"1788 - Optic Gaming (Foil) | Atlanta 2017",
				"1789 - Optic Gaming (Gold) | Atlanta 2017",
				"1790 - Sk Gaming | Atlanta 2017",
				"1791 - Sk Gaming (Holo) | Atlanta 2017",
				"1792 - Sk Gaming (Foil) | Atlanta 2017",
				"1793 - Sk Gaming (Gold) | Atlanta 2017",
				"1794 - Team Liquid | Atlanta 2017",
				"1795 - Team Liquid (Holo) | Atlanta 2017",
				"1796 - Team Liquid (Foil) | Atlanta 2017",
				"1797 - Team Liquid (Gold) | Atlanta 2017",
				"1798 - Virtus.pro | Atlanta 2017",
				"1799 - Virtus.pro (Holo) | Atlanta 2017",
				"1800 - Virtus.pro (Foil) | Atlanta 2017",
				"1801 - Virtus.pro (Gold) | Atlanta 2017",
				"1802 - Eleague | Atlanta 2017",
				"1803 - Eleague (Holo) | Atlanta 2017",
				"1804 - Eleague (Foil) | Atlanta 2017",
				"1805 - Eleague (Gold) | Atlanta 2017",
				"1806 - Astralis | Atlanta 2017",
				"1807 - Team Envyus | Atlanta 2017",
				"1808 - Faze Clan | Atlanta 2017",
				"1809 - Flipsid3 Tactics | Atlanta 2017",
				"1810 - Fnatic | Atlanta 2017",
				"1811 - G2 Esports | Atlanta 2017",
				"1812 - Gambit Gaming | Atlanta 2017",
				"1813 - Godsent | Atlanta 2017",
				"1814 - Hellraisers | Atlanta 2017",
				"1815 - Mousesports | Atlanta 2017",
				"1816 - Natus Vincere | Atlanta 2017",
				"1817 - North | Atlanta 2017",
				"1818 - Optic Gaming | Atlanta 2017",
				"1819 - Sk Gaming | Atlanta 2017",
				"1820 - Team Liquid | Atlanta 2017",
				"1821 - Virtus.pro | Atlanta 2017",
				"1822 - Eleague | Atlanta 2017",
				"1823 - Styko | Atlanta 2017",
				"1824 - Styko (Foil) | Atlanta 2017",
				"1825 - Styko (Gold) | Atlanta 2017",
				"1826 - Zero | Atlanta 2017",
				"1827 - Zero (Foil) | Atlanta 2017",
				"1828 - Zero (Gold) | Atlanta 2017",
				"1829 - Deadfox | Atlanta 2017",
				"1830 - Deadfox (Foil) | Atlanta 2017",
				"1831 - Deadfox (Gold) | Atlanta 2017",
				"1832 - Bondik | Atlanta 2017",
				"1833 - Bondik (Foil) | Atlanta 2017",
				"1834 - Bondik (Gold) | Atlanta 2017",
				"1835 - Ange1 | Atlanta 2017",
				"1836 - Ange1 (Foil) | Atlanta 2017",
				"1837 - Ange1 (Gold) | Atlanta 2017",
				"1838 - Tarik | Atlanta 2017",
				"1839 - Tarik (Foil) | Atlanta 2017",
				"1840 - Tarik (Gold) | Atlanta 2017",
				"1841 - Mixwell | Atlanta 2017",
				"1842 - Mixwell (Foil) | Atlanta 2017",
				"1843 - Mixwell (Gold) | Atlanta 2017",
				"1844 - Naf | Atlanta 2017",
				"1845 - Naf (Foil) | Atlanta 2017",
				"1846 - Naf (Gold) | Atlanta 2017",
				"1847 - Rush | Atlanta 2017",
				"1848 - Rush (Foil) | Atlanta 2017",
				"1849 - Rush (Gold) | Atlanta 2017",
				"1850 - Stanislaw | Atlanta 2017",
				"1851 - Stanislaw (Foil) | Atlanta 2017",
				"1852 - Stanislaw (Gold) | Atlanta 2017",
				"1853 - Apex | Atlanta 2017",
				"1854 - Apex (Foil) | Atlanta 2017",
				"1855 - Apex (Gold) | Atlanta 2017",
				"1856 - Happy | Atlanta 2017",
				"1857 - Happy (Foil) | Atlanta 2017",
				"1858 - Happy (Gold) | Atlanta 2017",
				"1859 - Sixer | Atlanta 2017",
				"1860 - Sixer (Foil) | Atlanta 2017",
				"1861 - Sixer (Gold) | Atlanta 2017",
				"1862 - Kennys | Atlanta 2017",
				"1863 - Kennys (Foil) | Atlanta 2017",
				"1864 - Kennys (Gold) | Atlanta 2017",
				"1865 - Nbk- | Atlanta 2017",
				"1866 - Nbk- (Foil) | Atlanta 2017",
				"1867 - Nbk- (Gold) | Atlanta 2017",
				"1868 - B1ad3 | Atlanta 2017",
				"1869 - B1ad3 (Foil) | Atlanta 2017",
				"1870 - B1ad3 (Gold) | Atlanta 2017",
				"1871 - Waylander | Atlanta 2017",
				"1872 - Waylander (Foil) | Atlanta 2017",
				"1873 - Waylander (Gold) | Atlanta 2017",
				"1874 - Electronic | Atlanta 2017",
				"1875 - Electronic (Foil) | Atlanta 2017",
				"1876 - Electronic (Gold) | Atlanta 2017",
				"1877 - Markeloff | Atlanta 2017",
				"1878 - Markeloff (Foil) | Atlanta 2017",
				"1879 - Markeloff (Gold) | Atlanta 2017",
				"1880 - Worldedit | Atlanta 2017",
				"1881 - Worldedit (Foil) | Atlanta 2017",
				"1882 - Worldedit (Gold) | Atlanta 2017",
				"1883 - Twist | Atlanta 2017",
				"1884 - Twist (Foil) | Atlanta 2017",
				"1885 - Twist (Gold) | Atlanta 2017",
				"1886 - Disco Doplan | Atlanta 2017",
				"1887 - Disco Doplan (Foil) | Atlanta 2017",
				"1888 - Disco Doplan (Gold) | Atlanta 2017",
				"1889 - Krimz | Atlanta 2017",
				"1890 - Krimz (Foil) | Atlanta 2017",
				"1891 - Krimz (Gold) | Atlanta 2017",
				"1892 - Olofmeister | Atlanta 2017",
				"1893 - Olofmeister (Foil) | Atlanta 2017",
				"1894 - Olofmeister (Gold) | Atlanta 2017",
				"1895 - Dennis | Atlanta 2017",
				"1896 - Dennis (Foil) | Atlanta 2017",
				"1897 - Dennis (Gold) | Atlanta 2017",
				"1898 - Aizy | Atlanta 2017",
				"1899 - Aizy (Foil) | Atlanta 2017",
				"1900 - Aizy (Gold) | Atlanta 2017",
				"1901 - Allu | Atlanta 2017",
				"1902 - Allu (Foil) | Atlanta 2017",
				"1903 - Allu (Gold) | Atlanta 2017",
				"1904 - Kioshima | Atlanta 2017",
				"1905 - Kioshima (Foil) | Atlanta 2017",
				"1906 - Kioshima (Gold) | Atlanta 2017",
				"1907 - Rain | Atlanta 2017",
				"1908 - Rain (Foil) | Atlanta 2017",
				"1909 - Rain (Gold) | Atlanta 2017",
				"1910 - Karrigan | Atlanta 2017",
				"1911 - Karrigan (Foil) | Atlanta 2017",
				"1912 - Karrigan (Gold) | Atlanta 2017",
				"1913 - Coldzera | Atlanta 2017",
				"1914 - Coldzera (Foil) | Atlanta 2017",
				"1915 - Coldzera (Gold) | Atlanta 2017",
				"1916 - Fallen | Atlanta 2017",
				"1917 - Fallen (Foil) | Atlanta 2017",
				"1918 - Fallen (Gold) | Atlanta 2017",
				"1919 - Fer | Atlanta 2017",
				"1920 - Fer (Foil) | Atlanta 2017",
				"1921 - Fer (Gold) | Atlanta 2017",
				"1922 - Fox | Atlanta 2017",
				"1923 - Fox (Foil) | Atlanta 2017",
				"1924 - Fox (Gold) | Atlanta 2017",
				"1925 - Taco | Atlanta 2017",
				"1926 - Taco (Foil) | Atlanta 2017",
				"1927 - Taco (Gold) | Atlanta 2017",
				"1928 - Chrisj | Atlanta 2017",
				"1929 - Chrisj (Foil) | Atlanta 2017",
				"1930 - Chrisj (Gold) | Atlanta 2017",
				"1931 - Denis | Atlanta 2017",
				"1932 - Denis (Foil) | Atlanta 2017",
				"1933 - Denis (Gold) | Atlanta 2017",
				"1934 - Spiidi | Atlanta 2017",
				"1935 - Spiidi (Foil) | Atlanta 2017",
				"1936 - Spiidi (Gold) | Atlanta 2017",
				"1937 - Lowel | Atlanta 2017",
				"1938 - Lowel (Foil) | Atlanta 2017",
				"1939 - Lowel (Gold) | Atlanta 2017",
				"1940 - Niko | Atlanta 2017",
				"1941 - Niko (Foil) | Atlanta 2017",
				"1942 - Niko (Gold) | Atlanta 2017",
				"1943 - Edward | Atlanta 2017",
				"1944 - Edward (Foil) | Atlanta 2017",
				"1945 - Edward (Gold) | Atlanta 2017",
				"1946 - Flamie | Atlanta 2017",
				"1947 - Flamie (Foil) | Atlanta 2017",
				"1948 - Flamie (Gold) | Atlanta 2017",
				"1949 - Guardian | Atlanta 2017",
				"1950 - Guardian (Foil) | Atlanta 2017",
				"1951 - Guardian (Gold) | Atlanta 2017",
				"1952 - Seized | Atlanta 2017",
				"1953 - Seized (Foil) | Atlanta 2017",
				"1954 - Seized (Gold) | Atlanta 2017",
				"1955 - S1mple | Atlanta 2017",
				"1956 - S1mple (Foil) | Atlanta 2017",
				"1957 - S1mple (Gold) | Atlanta 2017",
				"1958 - Znajder | Atlanta 2017",
				"1959 - Znajder (Foil) | Atlanta 2017",
				"1960 - Znajder (Gold) | Atlanta 2017",
				"1961 - Lekr0 | Atlanta 2017",
				"1962 - Lekr0 (Foil) | Atlanta 2017",
				"1963 - Lekr0 (Gold) | Atlanta 2017",
				"1964 - Pronax | Atlanta 2017",
				"1965 - Pronax (Foil) | Atlanta 2017",
				"1966 - Pronax (Gold) | Atlanta 2017",
				"1967 - Jw | Atlanta 2017",
				"1968 - Jw (Foil) | Atlanta 2017",
				"1969 - Jw (Gold) | Atlanta 2017",
				"1970 - Flusha | Atlanta 2017",
				"1971 - Flusha (Foil) | Atlanta 2017",
				"1972 - Flusha (Gold) | Atlanta 2017",
				"1973 - Cajunb | Atlanta 2017",
				"1974 - Cajunb (Foil) | Atlanta 2017",
				"1975 - Cajunb (Gold) | Atlanta 2017",
				"1976 - Msl | Atlanta 2017",
				"1977 - Msl (Foil) | Atlanta 2017",
				"1978 - Msl (Gold) | Atlanta 2017",
				"1979 - Magisk | Atlanta 2017",
				"1980 - Magisk (Foil) | Atlanta 2017",
				"1981 - Magisk (Gold) | Atlanta 2017",
				"1982 - Rubino | Atlanta 2017",
				"1983 - Rubino (Foil) | Atlanta 2017",
				"1984 - Rubino (Gold) | Atlanta 2017",
				"1985 - K0nfig | Atlanta 2017",
				"1986 - K0nfig (Foil) | Atlanta 2017",
				"1987 - K0nfig (Gold) | Atlanta 2017",
				"1988 - Jdm64 | Atlanta 2017",
				"1989 - Jdm64 (Foil) | Atlanta 2017",
				"1990 - Jdm64 (Gold) | Atlanta 2017",
				"1991 - Elige | Atlanta 2017",
				"1992 - Elige (Foil) | Atlanta 2017",
				"1993 - Elige (Gold) | Atlanta 2017",
				"1994 - Pimp | Atlanta 2017",
				"1995 - Pimp (Foil) | Atlanta 2017",
				"1996 - Pimp (Gold) | Atlanta 2017",
				"1997 - Hiko | Atlanta 2017",
				"1998 - Hiko (Foil) | Atlanta 2017",
				"1999 - Hiko (Gold) | Atlanta 2017",
				"2000 - Nitr0 | Atlanta 2017",
				"2001 - Nitr0 (Foil) | Atlanta 2017",
				"2002 - Nitr0 (Gold) | Atlanta 2017",
				"2003 - Bodyy | Atlanta 2017",
				"2004 - Bodyy (Foil) | Atlanta 2017",
				"2005 - Bodyy (Gold) | Atlanta 2017",
				"2006 - Rpk | Atlanta 2017",
				"2007 - Rpk (Foil) | Atlanta 2017",
				"2008 - Rpk (Gold) | Atlanta 2017",
				"2009 - Scream | Atlanta 2017",
				"2010 - Scream (Foil) | Atlanta 2017",
				"2011 - Scream (Gold) | Atlanta 2017",
				"2012 - Shox | Atlanta 2017",
				"2013 - Shox (Foil) | Atlanta 2017",
				"2014 - Shox (Gold) | Atlanta 2017",
				"2015 - Smithzz | Atlanta 2017",
				"2016 - Smithzz (Foil) | Atlanta 2017",
				"2017 - Smithzz (Gold) | Atlanta 2017",
				"2018 - Gla1ve | Atlanta 2017",
				"2019 - Gla1ve (Foil) | Atlanta 2017",
				"2020 - Gla1ve (Gold) | Atlanta 2017",
				"2021 - Device | Atlanta 2017",
				"2022 - Device (Foil) | Atlanta 2017",
				"2023 - Device (Gold) | Atlanta 2017",
				"2024 - Dupreeh | Atlanta 2017",
				"2025 - Dupreeh (Foil) | Atlanta 2017",
				"2026 - Dupreeh (Gold) | Atlanta 2017",
				"2027 - Kjaerbye | Atlanta 2017",
				"2028 - Kjaerbye (Foil) | Atlanta 2017",
				"2029 - Kjaerbye (Gold) | Atlanta 2017",
				"2030 - Xyp9x | Atlanta 2017",
				"2031 - Xyp9x (Foil) | Atlanta 2017",
				"2032 - Xyp9x (Gold) | Atlanta 2017",
				"2033 - Zeus | Atlanta 2017",
				"2034 - Zeus (Foil) | Atlanta 2017",
				"2035 - Zeus (Gold) | Atlanta 2017",
				"2036 - Dosia | Atlanta 2017",
				"2037 - Dosia (Foil) | Atlanta 2017",
				"2038 - Dosia (Gold) | Atlanta 2017",
				"2039 - Hobbit | Atlanta 2017",
				"2040 - Hobbit (Foil) | Atlanta 2017",
				"2041 - Hobbit (Gold) | Atlanta 2017",
				"2042 - Mou | Atlanta 2017",
				"2043 - Mou (Foil) | Atlanta 2017",
				"2044 - Mou (Gold) | Atlanta 2017",
				"2045 - Adren | Atlanta 2017",
				"2046 - Adren (Foil) | Atlanta 2017",
				"2047 - Adren (Gold) | Atlanta 2017",
				"2048 - Byali | Atlanta 2017",
				"2049 - Byali (Foil) | Atlanta 2017",
				"2050 - Byali (Gold) | Atlanta 2017",
				"2051 - Neo | Atlanta 2017",
				"2052 - Neo (Foil) | Atlanta 2017",
				"2053 - Neo (Gold) | Atlanta 2017",
				"2054 - Pashabiceps | Atlanta 2017",
				"2055 - Pashabiceps (Foil) | Atlanta 2017",
				"2056 - Pashabiceps (Gold) | Atlanta 2017",
				"2057 - Snax | Atlanta 2017",
				"2058 - Snax (Foil) | Atlanta 2017",
				"2059 - Snax (Gold) | Atlanta 2017",
				"2060 - Taz | Atlanta 2017",
				"2061 - Taz (Foil) | Atlanta 2017",
				"2062 - Taz (Gold) | Atlanta 2017",
				"2063 - Astralis | Krakow 2017",
				"2064 - Astralis (Holo) | Krakow 2017",
				"2065 - Astralis (Foil) | Krakow 2017",
				"2066 - Astralis (Gold) | Krakow 2017",
				"2067 - Virtus.pro | Krakow 2017",
				"2068 - Virtus.pro (Holo) | Krakow 2017",
				"2069 - Virtus.pro (Foil) | Krakow 2017",
				"2070 - Virtus.pro (Gold) | Krakow 2017",
				"2071 - Fnatic | Krakow 2017",
				"2072 - Fnatic (Holo) | Krakow 2017",
				"2073 - Fnatic (Foil) | Krakow 2017",
				"2074 - Fnatic (Gold) | Krakow 2017",
				"2075 - Sk Gaming | Krakow 2017",
				"2076 - Sk Gaming (Holo) | Krakow 2017",
				"2077 - Sk Gaming (Foil) | Krakow 2017",
				"2078 - Sk Gaming (Gold) | Krakow 2017",
				"2079 - Natus Vincere | Krakow 2017",
				"2080 - Natus Vincere (Holo) | Krakow 2017",
				"2081 - Natus Vincere (Foil) | Krakow 2017",
				"2082 - Natus Vincere (Gold) | Krakow 2017",
				"2083 - Gambit | Krakow 2017",
				"2084 - Gambit (Holo) | Krakow 2017",
				"2085 - Gambit (Foil) | Krakow 2017",
				"2086 - Gambit (Gold) | Krakow 2017",
				"2087 - North | Krakow 2017",
				"2088 - North (Holo) | Krakow 2017",
				"2089 - North (Foil) | Krakow 2017",
				"2090 - North (Gold) | Krakow 2017",
				"2091 - Faze Clan | Krakow 2017",
				"2092 - Faze Clan (Holo) | Krakow 2017",
				"2093 - Faze Clan (Foil) | Krakow 2017",
				"2094 - Faze Clan (Gold) | Krakow 2017",
				"2095 - Mousesports | Krakow 2017",
				"2096 - Mousesports (Holo) | Krakow 2017",
				"2097 - Mousesports (Foil) | Krakow 2017",
				"2098 - Mousesports (Gold) | Krakow 2017",
				"2099 - G2 Esports | Krakow 2017",
				"2100 - G2 Esports (Holo) | Krakow 2017",
				"2101 - G2 Esports (Foil) | Krakow 2017",
				"2102 - G2 Esports (Gold) | Krakow 2017",
				"2103 - Big | Krakow 2017",
				"2104 - Big (Holo) | Krakow 2017",
				"2105 - Big (Foil) | Krakow 2017",
				"2106 - Big (Gold) | Krakow 2017",
				"2107 - Cloud9 | Krakow 2017",
				"2108 - Cloud9 (Holo) | Krakow 2017",
				"2109 - Cloud9 (Foil) | Krakow 2017",
				"2110 - Cloud9 (Gold) | Krakow 2017",
				"2111 - Penta Sports | Krakow 2017",
				"2112 - Penta Sports (Holo) | Krakow 2017",
				"2113 - Penta Sports (Foil) | Krakow 2017",
				"2114 - Penta Sports (Gold) | Krakow 2017",
				"2115 - Flipsid3 Tactics | Krakow 2017",
				"2116 - Flipsid3 Tactics (Holo) | Krakow 2017",
				"2117 - Flipsid3 Tactics (Foil) | Krakow 2017",
				"2118 - Flipsid3 Tactics (Gold) | Krakow 2017",
				"2119 - Immortals | Krakow 2017",
				"2120 - Immortals (Holo) | Krakow 2017",
				"2121 - Immortals (Foil) | Krakow 2017",
				"2122 - Immortals (Gold) | Krakow 2017",
				"2123 - Vega Squadron | Krakow 2017",
				"2124 - Vega Squadron (Holo) | Krakow 2017",
				"2125 - Vega Squadron (Foil) | Krakow 2017",
				"2126 - Vega Squadron (Gold) | Krakow 2017",
				"2127 - Pgl | Krakow 2017",
				"2128 - Pgl (Holo) | Krakow 2017",
				"2129 - Pgl (Foil) | Krakow 2017",
				"2130 - Pgl (Gold) | Krakow 2017",
				"2131 - Astralis | Krakow 2017",
				"2132 - Virtus.pro | Krakow 2017",
				"2133 - Fnatic | Krakow 2017",
				"2134 - Sk Gaming | Krakow 2017",
				"2135 - Natus Vincere | Krakow 2017",
				"2136 - Gambit | Krakow 2017",
				"2137 - North | Krakow 2017",
				"2138 - Faze Clan | Krakow 2017",
				"2139 - Mousesports | Krakow 2017",
				"2140 - G2 Esports | Krakow 2017",
				"2141 - Big | Krakow 2017",
				"2142 - Cloud9 | Krakow 2017",
				"2143 - Penta Sports | Krakow 2017",
				"2144 - Flipsid3 Tactics | Krakow 2017",
				"2145 - Immortals | Krakow 2017",
				"2146 - Vega Squadron | Krakow 2017",
				"2147 - Pgl | Krakow 2017",
				"2148 - Device | Krakow 2017",
				"2149 - Device (Foil) | Krakow 2017",
				"2150 - Device (Gold) | Krakow 2017",
				"2151 - Dupreeh | Krakow 2017",
				"2152 - Dupreeh (Foil) | Krakow 2017",
				"2153 - Dupreeh (Gold) | Krakow 2017",
				"2154 - Gla1ve | Krakow 2017",
				"2155 - Gla1ve (Foil) | Krakow 2017",
				"2156 - Gla1ve (Gold) | Krakow 2017",
				"2157 - Kjaerbye | Krakow 2017",
				"2158 - Kjaerbye (Foil) | Krakow 2017",
				"2159 - Kjaerbye (Gold) | Krakow 2017",
				"2160 - Xyp9x | Krakow 2017",
				"2161 - Xyp9x (Foil) | Krakow 2017",
				"2162 - Xyp9x (Gold) | Krakow 2017",
				"2163 - Byali | Krakow 2017",
				"2164 - Byali (Foil) | Krakow 2017",
				"2165 - Byali (Gold) | Krakow 2017",
				"2166 - Neo | Krakow 2017",
				"2167 - Neo (Foil) | Krakow 2017",
				"2168 - Neo (Gold) | Krakow 2017",
				"2169 - Pashabiceps | Krakow 2017",
				"2170 - Pashabiceps (Foil) | Krakow 2017",
				"2171 - Pashabiceps (Gold) | Krakow 2017",
				"2172 - Snax | Krakow 2017",
				"2173 - Snax (Foil) | Krakow 2017",
				"2174 - Snax (Gold) | Krakow 2017",
				"2175 - Taz | Krakow 2017",
				"2176 - Taz (Foil) | Krakow 2017",
				"2177 - Taz (Gold) | Krakow 2017",
				"2178 - Dennis | Krakow 2017",
				"2179 - Dennis (Foil) | Krakow 2017",
				"2180 - Dennis (Gold) | Krakow 2017",
				"2181 - Flusha | Krakow 2017",
				"2182 - Flusha (Foil) | Krakow 2017",
				"2183 - Flusha (Gold) | Krakow 2017",
				"2184 - Jw | Krakow 2017",
				"2185 - Jw (Foil) | Krakow 2017",
				"2186 - Jw (Gold) | Krakow 2017",
				"2187 - Krimz | Krakow 2017",
				"2188 - Krimz (Foil) | Krakow 2017",
				"2189 - Krimz (Gold) | Krakow 2017",
				"2190 - Olofmeister | Krakow 2017",
				"2191 - Olofmeister (Foil) | Krakow 2017",
				"2192 - Olofmeister (Gold) | Krakow 2017",
				"2193 - Coldzera | Krakow 2017",
				"2194 - Coldzera (Foil) | Krakow 2017",
				"2195 - Coldzera (Gold) | Krakow 2017",
				"2196 - Fallen | Krakow 2017",
				"2197 - Fallen (Foil) | Krakow 2017",
				"2198 - Fallen (Gold) | Krakow 2017",
				"2199 - Felps | Krakow 2017",
				"2200 - Felps (Foil) | Krakow 2017",
				"2201 - Felps (Gold) | Krakow 2017",
				"2202 - Fer | Krakow 2017",
				"2203 - Fer (Foil) | Krakow 2017",
				"2204 - Fer (Gold) | Krakow 2017",
				"2205 - Taco | Krakow 2017",
				"2206 - Taco (Foil) | Krakow 2017",
				"2207 - Taco (Gold) | Krakow 2017",
				"2208 - Edward | Krakow 2017",
				"2209 - Edward (Foil) | Krakow 2017",
				"2210 - Edward (Gold) | Krakow 2017",
				"2211 - Flamie | Krakow 2017",
				"2212 - Flamie (Foil) | Krakow 2017",
				"2213 - Flamie (Gold) | Krakow 2017",
				"2214 - Guardian | Krakow 2017",
				"2215 - Guardian (Foil) | Krakow 2017",
				"2216 - Guardian (Gold) | Krakow 2017",
				"2217 - S1mple | Krakow 2017",
				"2218 - S1mple (Foil) | Krakow 2017",
				"2219 - S1mple (Gold) | Krakow 2017",
				"2220 - Seized | Krakow 2017",
				"2221 - Seized (Foil) | Krakow 2017",
				"2222 - Seized (Gold) | Krakow 2017",
				"2223 - Adren | Krakow 2017",
				"2224 - Adren (Foil) | Krakow 2017",
				"2225 - Adren (Gold) | Krakow 2017",
				"2226 - Dosia | Krakow 2017",
				"2227 - Dosia (Foil) | Krakow 2017",
				"2228 - Dosia (Gold) | Krakow 2017",
				"2229 - Hobbit | Krakow 2017",
				"2230 - Hobbit (Foil) | Krakow 2017",
				"2231 - Hobbit (Gold) | Krakow 2017",
				"2232 - Mou | Krakow 2017",
				"2233 - Mou (Foil) | Krakow 2017",
				"2234 - Mou (Gold) | Krakow 2017",
				"2235 - Zeus | Krakow 2017",
				"2236 - Zeus (Foil) | Krakow 2017",
				"2237 - Zeus (Gold) | Krakow 2017",
				"2238 - Aizy | Krakow 2017",
				"2239 - Aizy (Foil) | Krakow 2017",
				"2240 - Aizy (Gold) | Krakow 2017",
				"2241 - Cajunb | Krakow 2017",
				"2242 - Cajunb (Foil) | Krakow 2017",
				"2243 - Cajunb (Gold) | Krakow 2017",
				"2244 - K0nfig | Krakow 2017",
				"2245 - K0nfig (Foil) | Krakow 2017",
				"2246 - K0nfig (Gold) | Krakow 2017",
				"2247 - Magisk | Krakow 2017",
				"2248 - Magisk (Foil) | Krakow 2017",
				"2249 - Magisk (Gold) | Krakow 2017",
				"2250 - Msl | Krakow 2017",
				"2251 - Msl (Foil) | Krakow 2017",
				"2252 - Msl (Gold) | Krakow 2017",
				"2253 - Allu | Krakow 2017",
				"2254 - Allu (Foil) | Krakow 2017",
				"2255 - Allu (Gold) | Krakow 2017",
				"2256 - Karrigan | Krakow 2017",
				"2257 - Karrigan (Foil) | Krakow 2017",
				"2258 - Karrigan (Gold) | Krakow 2017",
				"2259 - Kioshima | Krakow 2017",
				"2260 - Kioshima (Foil) | Krakow 2017",
				"2261 - Kioshima (Gold) | Krakow 2017",
				"2262 - Niko | Krakow 2017",
				"2263 - Niko (Foil) | Krakow 2017",
				"2264 - Niko (Gold) | Krakow 2017",
				"2265 - Rain | Krakow 2017",
				"2266 - Rain (Foil) | Krakow 2017",
				"2267 - Rain (Gold) | Krakow 2017",
				"2268 - Chrisj | Krakow 2017",
				"2269 - Chrisj (Foil) | Krakow 2017",
				"2270 - Chrisj (Gold) | Krakow 2017",
				"2271 - Denis | Krakow 2017",
				"2272 - Denis (Foil) | Krakow 2017",
				"2273 - Denis (Gold) | Krakow 2017",
				"2274 - Lowel | Krakow 2017",
				"2275 - Lowel (Foil) | Krakow 2017",
				"2276 - Lowel (Gold) | Krakow 2017",
				"2277 - Oskar | Krakow 2017",
				"2278 - Oskar (Foil) | Krakow 2017",
				"2279 - Oskar (Gold) | Krakow 2017",
				"2280 - Ropz | Krakow 2017",
				"2281 - Ropz (Foil) | Krakow 2017",
				"2282 - Ropz (Gold) | Krakow 2017",
				"2283 - Apex | Krakow 2017",
				"2284 - Apex (Foil) | Krakow 2017",
				"2285 - Apex (Gold) | Krakow 2017",
				"2286 - Bodyy | Krakow 2017",
				"2287 - Bodyy (Foil) | Krakow 2017",
				"2288 - Bodyy (Gold) | Krakow 2017",
				"2289 - Kennys | Krakow 2017",
				"2290 - Kennys (Foil) | Krakow 2017",
				"2291 - Kennys (Gold) | Krakow 2017",
				"2292 - Nbk- | Krakow 2017",
				"2293 - Nbk- (Foil) | Krakow 2017",
				"2294 - Nbk- (Gold) | Krakow 2017",
				"2295 - Shox | Krakow 2017",
				"2296 - Shox (Foil) | Krakow 2017",
				"2297 - Shox (Gold) | Krakow 2017",
				"2298 - Gob B | Krakow 2017",
				"2299 - Gob B (Foil) | Krakow 2017",
				"2300 - Gob B (Gold) | Krakow 2017",
				"2301 - Keev | Krakow 2017",
				"2302 - Keev (Foil) | Krakow 2017",
				"2303 - Keev (Gold) | Krakow 2017",
				"2304 - Legija | Krakow 2017",
				"2305 - Legija (Foil) | Krakow 2017",
				"2306 - Legija (Gold) | Krakow 2017",
				"2307 - Nex | Krakow 2017",
				"2308 - Nex (Foil) | Krakow 2017",
				"2309 - Nex (Gold) | Krakow 2017",
				"2310 - Tabsen | Krakow 2017",
				"2311 - Tabsen (Foil) | Krakow 2017",
				"2312 - Tabsen (Gold) | Krakow 2017",
				"2313 - Autimatic | Krakow 2017",
				"2314 - Autimatic (Foil) | Krakow 2017",
				"2315 - Autimatic (Gold) | Krakow 2017",
				"2316 - N0thing | Krakow 2017",
				"2317 - N0thing (Foil) | Krakow 2017",
				"2318 - N0thing (Gold) | Krakow 2017",
				"2319 - Shroud | Krakow 2017",
				"2320 - Shroud (Foil) | Krakow 2017",
				"2321 - Shroud (Gold) | Krakow 2017",
				"2322 - Skadoodle | Krakow 2017",
				"2323 - Skadoodle (Foil) | Krakow 2017",
				"2324 - Skadoodle (Gold) | Krakow 2017",
				"2325 - Stewie2k | Krakow 2017",
				"2326 - Stewie2k (Foil) | Krakow 2017",
				"2327 - Stewie2k (Gold) | Krakow 2017",
				"2328 - Hs | Krakow 2017",
				"2329 - Hs (Foil) | Krakow 2017",
				"2330 - Hs (Gold) | Krakow 2017",
				"2331 - Innocent | Krakow 2017",
				"2332 - Innocent (Foil) | Krakow 2017",
				"2333 - Innocent (Gold) | Krakow 2017",
				"2334 - Krystal | Krakow 2017",
				"2335 - Krystal (Foil) | Krakow 2017",
				"2336 - Krystal (Gold) | Krakow 2017",
				"2337 - Sunny | Krakow 2017",
				"2338 - Sunny (Foil) | Krakow 2017",
				"2339 - Sunny (Gold) | Krakow 2017",
				"2340 - Zehn | Krakow 2017",
				"2341 - Zehn (Foil) | Krakow 2017",
				"2342 - Zehn (Gold) | Krakow 2017",
				"2343 - B1ad3 | Krakow 2017",
				"2344 - B1ad3 (Foil) | Krakow 2017",
				"2345 - B1ad3 (Gold) | Krakow 2017",
				"2346 - Electronic | Krakow 2017",
				"2347 - Electronic (Foil) | Krakow 2017",
				"2348 - Electronic (Gold) | Krakow 2017",
				"2349 - Markeloff | Krakow 2017",
				"2350 - Markeloff (Foil) | Krakow 2017",
				"2351 - Markeloff (Gold) | Krakow 2017",
				"2352 - Waylander | Krakow 2017",
				"2353 - Waylander (Foil) | Krakow 2017",
				"2354 - Waylander (Gold) | Krakow 2017",
				"2355 - Worldedit | Krakow 2017",
				"2356 - Worldedit (Foil) | Krakow 2017",
				"2357 - Worldedit (Gold) | Krakow 2017",
				"2358 - Boltz | Krakow 2017",
				"2359 - Boltz (Foil) | Krakow 2017",
				"2360 - Boltz (Gold) | Krakow 2017",
				"2361 - Hen1 | Krakow 2017",
				"2362 - Hen1 (Foil) | Krakow 2017",
				"2363 - Hen1 (Gold) | Krakow 2017",
				"2364 - Kngv- | Krakow 2017",
				"2365 - Kngv- (Foil) | Krakow 2017",
				"2366 - Kngv- (Gold) | Krakow 2017",
				"2367 - Lucas1 | Krakow 2017",
				"2368 - Lucas1 (Foil) | Krakow 2017",
				"2369 - Lucas1 (Gold) | Krakow 2017",
				"2370 - Steel | Krakow 2017",
				"2371 - Steel (Foil) | Krakow 2017",
				"2372 - Steel (Gold) | Krakow 2017",
				"2373 - Chopper | Krakow 2017",
				"2374 - Chopper (Foil) | Krakow 2017",
				"2375 - Chopper (Gold) | Krakow 2017",
				"2376 - Hutji | Krakow 2017",
				"2377 - Hutji (Foil) | Krakow 2017",
				"2378 - Hutji (Gold) | Krakow 2017",
				"2379 - Jr | Krakow 2017",
				"2380 - Jr (Foil) | Krakow 2017",
				"2381 - Jr (Gold) | Krakow 2017",
				"2382 - Keshandr | Krakow 2017",
				"2383 - Keshandr (Foil) | Krakow 2017",
				"2384 - Keshandr (Gold) | Krakow 2017",
				"2385 - Mir | Krakow 2017",
				"2386 - Mir (Foil) | Krakow 2017",
				"2387 - Mir (Gold) | Krakow 2017",
				"2388 - Water Gun",
				"2389 - Cheongsam",
				"2390 - Cheongsam (Holo)",
				"2391 - Fancy Koi",
				"2392 - Fancy Koi (Foil)",
				"2393 - Guardian Dragon",
				"2394 - Guardian Dragon (Foil)",
				"2395 - Hotpot",
				"2396 - Noodles",
				"2397 - Rice Bomb",
				"2398 - Terror Rice",
				"2399 - Mahjong Fa",
				"2400 - Mahjong Rooster",
				"2401 - Mahjong Zhong",
				"2402 - Toy Tiger",
				"2403 - God Of Fortune",
				"2404 - Huaji",
				"2405 - Nezha",
				"2406 - Rage",
				"2407 - Longevity",
				"2408 - Longevity (Foil)",
				"2409 - Non-veg",
				"2410 - Pixiu",
				"2411 - Pixiu (Foil)",
				"2412 - Twin Koi",
				"2413 - Twin Koi (Holo)",
				"2414 - Shaolin",
				"2415 - Green Swallow",
				"2416 - Blue Swallow",
				"2417 - Zombie Hop",
				"2418 - Water Gun",
				"2419 - Cheongsam",
				"2420 - Guardian Dragon",
				"2421 - Rage",
				"2422 - God Of Fortune",
				"2423 - Hotpot",
				"2424 - Fancy Koi",
				"2425 - Longevity",
				"2426 - Nezha",
				"2427 - Noodles",
				"2428 - Non-veg",
				"2429 - Pixiu",
				"2430 - Twin Koi",
				"2431 - Rice Bomb",
				"2432 - Terror Rice",
				"2433 - Shaolin",
				"2434 - Toy Tiger",
				"2435 - Zombie Hop",
				"2436 - Gambit Esports | Boston 2018",
				"2437 - Gambit Esports (Holo) | Boston 2018",
				"2438 - Gambit Esports (Foil) | Boston 2018",
				"2439 - Gambit Esports (Gold) | Boston 2018",
				"2440 - 100 Thieves | Boston 2018",
				"2441 - 100 Thieves (Holo) | Boston 2018",
				"2442 - 100 Thieves (Foil) | Boston 2018",
				"2443 - 100 Thieves (Gold) | Boston 2018",
				"2444 - Astralis | Boston 2018",
				"2445 - Astralis (Holo) | Boston 2018",
				"2446 - Astralis (Foil) | Boston 2018",
				"2447 - Astralis (Gold) | Boston 2018",
				"2448 - Virtus.pro | Boston 2018",
				"2449 - Virtus.pro (Holo) | Boston 2018",
				"2450 - Virtus.pro (Foil) | Boston 2018",
				"2451 - Virtus.pro (Gold) | Boston 2018",
				"2452 - Fnatic | Boston 2018",
				"2453 - Fnatic (Holo) | Boston 2018",
				"2454 - Fnatic (Foil) | Boston 2018",
				"2455 - Fnatic (Gold) | Boston 2018",
				"2456 - Sk Gaming | Boston 2018",
				"2457 - Sk Gaming (Holo) | Boston 2018",
				"2458 - Sk Gaming (Foil) | Boston 2018",
				"2459 - Sk Gaming (Gold) | Boston 2018",
				"2460 - Big | Boston 2018",
				"2461 - Big (Holo) | Boston 2018",
				"2462 - Big (Foil) | Boston 2018",
				"2463 - Big (Gold) | Boston 2018",
				"2464 - North | Boston 2018",
				"2465 - North (Holo) | Boston 2018",
				"2466 - North (Foil) | Boston 2018",
				"2467 - North (Gold) | Boston 2018",
				"2468 - G2 Esports | Boston 2018",
				"2469 - G2 Esports (Holo) | Boston 2018",
				"2470 - G2 Esports (Foil) | Boston 2018",
				"2471 - G2 Esports (Gold) | Boston 2018",
				"2472 - Cloud9 | Boston 2018",
				"2473 - Cloud9 (Holo) | Boston 2018",
				"2474 - Cloud9 (Foil) | Boston 2018",
				"2475 - Cloud9 (Gold) | Boston 2018",
				"2476 - Flipsid3 Tactics | Boston 2018",
				"2477 - Flipsid3 Tactics (Holo) | Boston 2018",
				"2478 - Flipsid3 Tactics (Foil) | Boston 2018",
				"2479 - Flipsid3 Tactics (Gold) | Boston 2018",
				"2480 - Natus Vincere | Boston 2018",
				"2481 - Natus Vincere (Holo) | Boston 2018",
				"2482 - Natus Vincere (Foil) | Boston 2018",
				"2483 - Natus Vincere (Gold) | Boston 2018",
				"2484 - Mousesports | Boston 2018",
				"2485 - Mousesports (Holo) | Boston 2018",
				"2486 - Mousesports (Foil) | Boston 2018",
				"2487 - Mousesports (Gold) | Boston 2018",
				"2488 - Sprout Esports | Boston 2018",
				"2489 - Sprout Esports (Holo) | Boston 2018",
				"2490 - Sprout Esports (Foil) | Boston 2018",
				"2491 - Sprout Esports (Gold) | Boston 2018",
				"2492 - Faze Clan | Boston 2018",
				"2493 - Faze Clan (Holo) | Boston 2018",
				"2494 - Faze Clan (Foil) | Boston 2018",
				"2495 - Faze Clan (Gold) | Boston 2018",
				"2496 - Vega Squadron | Boston 2018",
				"2497 - Vega Squadron (Holo) | Boston 2018",
				"2498 - Vega Squadron (Foil) | Boston 2018",
				"2499 - Vega Squadron (Gold) | Boston 2018",
				"2500 - Space Soldiers | Boston 2018",
				"2501 - Space Soldiers (Holo) | Boston 2018",
				"2502 - Space Soldiers (Foil) | Boston 2018",
				"2503 - Space Soldiers (Gold) | Boston 2018",
				"2504 - Team Liquid | Boston 2018",
				"2505 - Team Liquid (Holo) | Boston 2018",
				"2506 - Team Liquid (Foil) | Boston 2018",
				"2507 - Team Liquid (Gold) | Boston 2018",
				"2508 - Avangar | Boston 2018",
				"2509 - Avangar (Holo) | Boston 2018",
				"2510 - Avangar (Foil) | Boston 2018",
				"2511 - Avangar (Gold) | Boston 2018",
				"2512 - Renegades | Boston 2018",
				"2513 - Renegades (Holo) | Boston 2018",
				"2514 - Renegades (Foil) | Boston 2018",
				"2515 - Renegades (Gold) | Boston 2018",
				"2516 - Team Envyus | Boston 2018",
				"2517 - Team Envyus (Holo) | Boston 2018",
				"2518 - Team Envyus (Foil) | Boston 2018",
				"2519 - Team Envyus (Gold) | Boston 2018",
				"2520 - Misfits Gaming | Boston 2018",
				"2521 - Misfits Gaming (Holo) | Boston 2018",
				"2522 - Misfits Gaming (Foil) | Boston 2018",
				"2523 - Misfits Gaming (Gold) | Boston 2018",
				"2524 - Quantum Bellator Fire | Boston 2018",
				"2525 - Quantum Bellator Fire (Holo) | Boston 2018",
				"2526 - Quantum Bellator Fire (Foil) | Boston 2018",
				"2527 - Quantum Bellator Fire (Gold) | Boston 2018",
				"2528 - Tyloo | Boston 2018",
				"2529 - Tyloo (Holo) | Boston 2018",
				"2530 - Tyloo (Foil) | Boston 2018",
				"2531 - Tyloo (Gold) | Boston 2018",
				"2532 - Eleague | Boston 2018",
				"2533 - Eleague (Holo) | Boston 2018",
				"2534 - Eleague (Foil) | Boston 2018",
				"2535 - Eleague (Gold) | Boston 2018",
				"2536 - Gambit Esports | Boston 2018",
				"2537 - 100 Thieves | Boston 2018",
				"2538 - Astralis | Boston 2018",
				"2539 - Virtus.pro | Boston 2018",
				"2540 - Fnatic | Boston 2018",
				"2541 - Sk Gaming | Boston 2018",
				"2542 - Big | Boston 2018",
				"2543 - North | Boston 2018",
				"2544 - G2 Esports | Boston 2018",
				"2545 - Cloud9 | Boston 2018",
				"2546 - Flipsid3 Tactics | Boston 2018",
				"2547 - Natus Vincere | Boston 2018",
				"2548 - Mousesports | Boston 2018",
				"2549 - Sprout Esports | Boston 2018",
				"2550 - Faze Clan | Boston 2018",
				"2551 - Vega Squadron | Boston 2018",
				"2552 - Space Soldiers | Boston 2018",
				"2553 - Team Liquid | Boston 2018",
				"2554 - Avangar | Boston 2018",
				"2555 - Renegades | Boston 2018",
				"2556 - Team Envyus | Boston 2018",
				"2557 - Misfits Gaming | Boston 2018",
				"2558 - Quantum Bellator Fire | Boston 2018",
				"2559 - Tyloo | Boston 2018",
				"2560 - Eleague | Boston 2018",
				"2561 - Adren | Boston 2018",
				"2562 - Adren (Foil) | Boston 2018",
				"2563 - Adren (Gold) | Boston 2018",
				"2564 - Dosia | Boston 2018",
				"2565 - Dosia (Foil) | Boston 2018",
				"2566 - Dosia (Gold) | Boston 2018",
				"2567 - Fitch | Boston 2018",
				"2568 - Fitch (Foil) | Boston 2018",
				"2569 - Fitch (Gold) | Boston 2018",
				"2570 - Hobbit | Boston 2018",
				"2571 - Hobbit (Foil) | Boston 2018",
				"2572 - Hobbit (Gold) | Boston 2018",
				"2573 - Mou | Boston 2018",
				"2574 - Mou (Foil) | Boston 2018",
				"2575 - Mou (Gold) | Boston 2018",
				"2576 - Bit | Boston 2018",
				"2577 - Bit (Foil) | Boston 2018",
				"2578 - Bit (Gold) | Boston 2018",
				"2579 - Fnx | Boston 2018",
				"2580 - Fnx (Foil) | Boston 2018",
				"2581 - Fnx (Gold) | Boston 2018",
				"2582 - Hen1 | Boston 2018",
				"2583 - Hen1 (Foil) | Boston 2018",
				"2584 - Hen1 (Gold) | Boston 2018",
				"2585 - Kngv- | Boston 2018",
				"2586 - Kngv- (Foil) | Boston 2018",
				"2587 - Kngv- (Gold) | Boston 2018",
				"2588 - Lucas1 | Boston 2018",
				"2589 - Lucas1 (Foil) | Boston 2018",
				"2590 - Lucas1 (Gold) | Boston 2018",
				"2591 - Device | Boston 2018",
				"2592 - Device (Foil) | Boston 2018",
				"2593 - Device (Gold) | Boston 2018",
				"2594 - Dupreeh | Boston 2018",
				"2595 - Dupreeh (Foil) | Boston 2018",
				"2596 - Dupreeh (Gold) | Boston 2018",
				"2597 - Gla1ve | Boston 2018",
				"2598 - Gla1ve (Foil) | Boston 2018",
				"2599 - Gla1ve (Gold) | Boston 2018",
				"2600 - Kjaerbye | Boston 2018",
				"2601 - Kjaerbye (Foil) | Boston 2018",
				"2602 - Kjaerbye (Gold) | Boston 2018",
				"2603 - Xyp9x | Boston 2018",
				"2604 - Xyp9x (Foil) | Boston 2018",
				"2605 - Xyp9x (Gold) | Boston 2018",
				"2606 - Byali | Boston 2018",
				"2607 - Byali (Foil) | Boston 2018",
				"2608 - Byali (Gold) | Boston 2018",
				"2609 - Neo | Boston 2018",
				"2610 - Neo (Foil) | Boston 2018",
				"2611 - Neo (Gold) | Boston 2018",
				"2612 - Pashabiceps | Boston 2018",
				"2613 - Pashabiceps (Foil) | Boston 2018",
				"2614 - Pashabiceps (Gold) | Boston 2018",
				"2615 - Snax | Boston 2018",
				"2616 - Snax (Foil) | Boston 2018",
				"2617 - Snax (Gold) | Boston 2018",
				"2618 - Taz | Boston 2018",
				"2619 - Taz (Foil) | Boston 2018",
				"2620 - Taz (Gold) | Boston 2018",
				"2621 - Flusha | Boston 2018",
				"2622 - Flusha (Foil) | Boston 2018",
				"2623 - Flusha (Gold) | Boston 2018",
				"2624 - Golden | Boston 2018",
				"2625 - Golden (Foil) | Boston 2018",
				"2626 - Golden (Gold) | Boston 2018",
				"2627 - Jw | Boston 2018",
				"2628 - Jw (Foil) | Boston 2018",
				"2629 - Jw (Gold) | Boston 2018",
				"2630 - Krimz | Boston 2018",
				"2631 - Krimz (Foil) | Boston 2018",
				"2632 - Krimz (Gold) | Boston 2018",
				"2633 - Lekr0 | Boston 2018",
				"2634 - Lekr0 (Foil) | Boston 2018",
				"2635 - Lekr0 (Gold) | Boston 2018",
				"2636 - Coldzera | Boston 2018",
				"2637 - Coldzera (Foil) | Boston 2018",
				"2638 - Coldzera (Gold) | Boston 2018",
				"2639 - Fallen | Boston 2018",
				"2640 - Fallen (Foil) | Boston 2018",
				"2641 - Fallen (Gold) | Boston 2018",
				"2642 - Felps | Boston 2018",
				"2643 - Felps (Foil) | Boston 2018",
				"2644 - Felps (Gold) | Boston 2018",
				"2645 - Fer | Boston 2018",
				"2646 - Fer (Foil) | Boston 2018",
				"2647 - Fer (Gold) | Boston 2018",
				"2648 - Taco | Boston 2018",
				"2649 - Taco (Foil) | Boston 2018",
				"2650 - Taco (Gold) | Boston 2018",
				"2651 - Gob B | Boston 2018",
				"2652 - Gob B (Foil) | Boston 2018",
				"2653 - Gob B (Gold) | Boston 2018",
				"2654 - Keev | Boston 2018",
				"2655 - Keev (Foil) | Boston 2018",
				"2656 - Keev (Gold) | Boston 2018",
				"2657 - Legija | Boston 2018",
				"2658 - Legija (Foil) | Boston 2018",
				"2659 - Legija (Gold) | Boston 2018",
				"2660 - Nex | Boston 2018",
				"2661 - Nex (Foil) | Boston 2018",
				"2662 - Nex (Gold) | Boston 2018",
				"2663 - Tabsen | Boston 2018",
				"2664 - Tabsen (Foil) | Boston 2018",
				"2665 - Tabsen (Gold) | Boston 2018",
				"2666 - Aizy | Boston 2018",
				"2667 - Aizy (Foil) | Boston 2018",
				"2668 - Aizy (Gold) | Boston 2018",
				"2669 - Cajunb | Boston 2018",
				"2670 - Cajunb (Foil) | Boston 2018",
				"2671 - Cajunb (Gold) | Boston 2018",
				"2672 - K0nfig | Boston 2018",
				"2673 - K0nfig (Foil) | Boston 2018",
				"2674 - K0nfig (Gold) | Boston 2018",
				"2675 - Msl | Boston 2018",
				"2676 - Msl (Foil) | Boston 2018",
				"2677 - Msl (Gold) | Boston 2018",
				"2678 - V4lde | Boston 2018",
				"2679 - V4lde (Foil) | Boston 2018",
				"2680 - V4lde (Gold) | Boston 2018",
				"2681 - Apex | Boston 2018",
				"2682 - Apex (Foil) | Boston 2018",
				"2683 - Apex (Gold) | Boston 2018",
				"2684 - Bodyy | Boston 2018",
				"2685 - Bodyy (Foil) | Boston 2018",
				"2686 - Bodyy (Gold) | Boston 2018",
				"2687 - Kennys | Boston 2018",
				"2688 - Kennys (Foil) | Boston 2018",
				"2689 - Kennys (Gold) | Boston 2018",
				"2690 - Nbk- | Boston 2018",
				"2691 - Nbk- (Foil) | Boston 2018",
				"2692 - Nbk- (Gold) | Boston 2018",
				"2693 - Shox | Boston 2018",
				"2694 - Shox (Foil) | Boston 2018",
				"2695 - Shox (Gold) | Boston 2018",
				"2696 - Autimatic | Boston 2018",
				"2697 - Autimatic (Foil) | Boston 2018",
				"2698 - Autimatic (Gold) | Boston 2018",
				"2699 - Rush | Boston 2018",
				"2700 - Rush (Foil) | Boston 2018",
				"2701 - Rush (Gold) | Boston 2018",
				"2702 - Skadoodle | Boston 2018",
				"2703 - Skadoodle (Foil) | Boston 2018",
				"2704 - Skadoodle (Gold) | Boston 2018",
				"2705 - Stewie2k | Boston 2018",
				"2706 - Stewie2k (Foil) | Boston 2018",
				"2707 - Stewie2k (Gold) | Boston 2018",
				"2708 - Tarik | Boston 2018",
				"2709 - Tarik (Foil) | Boston 2018",
				"2710 - Tarik (Gold) | Boston 2018",
				"2711 - B1ad3 | Boston 2018",
				"2712 - B1ad3 (Foil) | Boston 2018",
				"2713 - B1ad3 (Gold) | Boston 2018",
				"2714 - Markeloff | Boston 2018",
				"2715 - Markeloff (Foil) | Boston 2018",
				"2716 - Markeloff (Gold) | Boston 2018",
				"2717 - Seized | Boston 2018",
				"2718 - Seized (Foil) | Boston 2018",
				"2719 - Seized (Gold) | Boston 2018",
				"2720 - Waylander | Boston 2018",
				"2721 - Waylander (Foil) | Boston 2018",
				"2722 - Waylander (Gold) | Boston 2018",
				"2723 - Worldedit | Boston 2018",
				"2724 - Worldedit (Foil) | Boston 2018",
				"2725 - Worldedit (Gold) | Boston 2018",
				"2726 - Edward | Boston 2018",
				"2727 - Edward (Foil) | Boston 2018",
				"2728 - Edward (Gold) | Boston 2018",
				"2729 - Electronic | Boston 2018",
				"2730 - Electronic (Foil) | Boston 2018",
				"2731 - Electronic (Gold) | Boston 2018",
				"2732 - Flamie | Boston 2018",
				"2733 - Flamie (Foil) | Boston 2018",
				"2734 - Flamie (Gold) | Boston 2018",
				"2735 - S1mple | Boston 2018",
				"2736 - S1mple (Foil) | Boston 2018",
				"2737 - S1mple (Gold) | Boston 2018",
				"2738 - Zeus | Boston 2018",
				"2739 - Zeus (Foil) | Boston 2018",
				"2740 - Zeus (Gold) | Boston 2018",
				"2741 - Chrisj | Boston 2018",
				"2742 - Chrisj (Foil) | Boston 2018",
				"2743 - Chrisj (Gold) | Boston 2018",
				"2744 - Oskar | Boston 2018",
				"2745 - Oskar (Foil) | Boston 2018",
				"2746 - Oskar (Gold) | Boston 2018",
				"2747 - Ropz | Boston 2018",
				"2748 - Ropz (Foil) | Boston 2018",
				"2749 - Ropz (Gold) | Boston 2018",
				"2750 - Styko | Boston 2018",
				"2751 - Styko (Foil) | Boston 2018",
				"2752 - Styko (Gold) | Boston 2018",
				"2753 - Sunny | Boston 2018",
				"2754 - Sunny (Foil) | Boston 2018",
				"2755 - Sunny (Gold) | Boston 2018",
				"2756 - Denis | Boston 2018",
				"2757 - Denis (Foil) | Boston 2018",
				"2758 - Denis (Gold) | Boston 2018",
				"2759 - Innocent | Boston 2018",
				"2760 - Innocent (Foil) | Boston 2018",
				"2761 - Innocent (Gold) | Boston 2018",
				"2762 - Krystal | Boston 2018",
				"2763 - Krystal (Foil) | Boston 2018",
				"2764 - Krystal (Gold) | Boston 2018",
				"2765 - Spiidi | Boston 2018",
				"2766 - Spiidi (Foil) | Boston 2018",
				"2767 - Spiidi (Gold) | Boston 2018",
				"2768 - Zehn | Boston 2018",
				"2769 - Zehn (Foil) | Boston 2018",
				"2770 - Zehn (Gold) | Boston 2018",
				"2771 - Guardian | Boston 2018",
				"2772 - Guardian (Foil) | Boston 2018",
				"2773 - Guardian (Gold) | Boston 2018",
				"2774 - Karrigan | Boston 2018",
				"2775 - Karrigan (Foil) | Boston 2018",
				"2776 - Karrigan (Gold) | Boston 2018",
				"2777 - Niko | Boston 2018",
				"2778 - Niko (Foil) | Boston 2018",
				"2779 - Niko (Gold) | Boston 2018",
				"2780 - Olofmeister | Boston 2018",
				"2781 - Olofmeister (Foil) | Boston 2018",
				"2782 - Olofmeister (Gold) | Boston 2018",
				"2783 - Rain | Boston 2018",
				"2784 - Rain (Foil) | Boston 2018",
				"2785 - Rain (Gold) | Boston 2018",
				"2786 - Chopper | Boston 2018",
				"2787 - Chopper (Foil) | Boston 2018",
				"2788 - Chopper (Gold) | Boston 2018",
				"2789 - Hutji | Boston 2018",
				"2790 - Hutji (Foil) | Boston 2018",
				"2791 - Hutji (Gold) | Boston 2018",
				"2792 - Jr | Boston 2018",
				"2793 - Jr (Foil) | Boston 2018",
				"2794 - Jr (Gold) | Boston 2018",
				"2795 - Keshandr | Boston 2018",
				"2796 - Keshandr (Foil) | Boston 2018",
				"2797 - Keshandr (Gold) | Boston 2018",
				"2798 - Mir | Boston 2018",
				"2799 - Mir (Foil) | Boston 2018",
				"2800 - Mir (Gold) | Boston 2018",
				"2801 - Calyx | Boston 2018",
				"2802 - Calyx (Foil) | Boston 2018",
				"2803 - Calyx (Gold) | Boston 2018",
				"2804 - Maj3r | Boston 2018",
				"2805 - Maj3r (Foil) | Boston 2018",
				"2806 - Maj3r (Gold) | Boston 2018",
				"2807 - Ngin | Boston 2018",
				"2808 - Ngin (Foil) | Boston 2018",
				"2809 - Ngin (Gold) | Boston 2018",
				"2810 - Paz | Boston 2018",
				"2811 - Paz (Foil) | Boston 2018",
				"2812 - Paz (Gold) | Boston 2018",
				"2813 - Xantares | Boston 2018",
				"2814 - Xantares (Foil) | Boston 2018",
				"2815 - Xantares (Gold) | Boston 2018",
				"2816 - Elige | Boston 2018",
				"2817 - Elige (Foil) | Boston 2018",
				"2818 - Elige (Gold) | Boston 2018",
				"2819 - Jdm64 | Boston 2018",
				"2820 - Jdm64 (Foil) | Boston 2018",
				"2821 - Jdm64 (Gold) | Boston 2018",
				"2822 - Nitr0 | Boston 2018",
				"2823 - Nitr0 (Foil) | Boston 2018",
				"2824 - Nitr0 (Gold) | Boston 2018",
				"2825 - Stanislaw | Boston 2018",
				"2826 - Stanislaw (Foil) | Boston 2018",
				"2827 - Stanislaw (Gold) | Boston 2018",
				"2828 - Twistzz | Boston 2018",
				"2829 - Twistzz (Foil) | Boston 2018",
				"2830 - Twistzz (Gold) | Boston 2018",
				"2831 - Buster | Boston 2018",
				"2832 - Buster (Foil) | Boston 2018",
				"2833 - Buster (Gold) | Boston 2018",
				"2834 - Dimasick | Boston 2018",
				"2835 - Dimasick (Foil) | Boston 2018",
				"2836 - Dimasick (Gold) | Boston 2018",
				"2837 - Jame | Boston 2018",
				"2838 - Jame (Foil) | Boston 2018",
				"2839 - Jame (Gold) | Boston 2018",
				"2840 - Krizzen | Boston 2018",
				"2841 - Krizzen (Foil) | Boston 2018",
				"2842 - Krizzen (Gold) | Boston 2018",
				"2843 - Qikert | Boston 2018",
				"2844 - Qikert (Foil) | Boston 2018",
				"2845 - Qikert (Gold) | Boston 2018",
				"2846 - Azr | Boston 2018",
				"2847 - Azr (Foil) | Boston 2018",
				"2848 - Azr (Gold) | Boston 2018",
				"2849 - Jks | Boston 2018",
				"2850 - Jks (Foil) | Boston 2018",
				"2851 - Jks (Gold) | Boston 2018",
				"2852 - Naf | Boston 2018",
				"2853 - Naf (Foil) | Boston 2018",
				"2854 - Naf (Gold) | Boston 2018",
				"2855 - Nifty | Boston 2018",
				"2856 - Nifty (Foil) | Boston 2018",
				"2857 - Nifty (Gold) | Boston 2018",
				"2858 - Ustilo | Boston 2018",
				"2859 - Ustilo (Foil) | Boston 2018",
				"2860 - Ustilo (Gold) | Boston 2018",
				"2861 - Happy | Boston 2018",
				"2862 - Happy (Foil) | Boston 2018",
				"2863 - Happy (Gold) | Boston 2018",
				"2864 - Rpk | Boston 2018",
				"2865 - Rpk (Foil) | Boston 2018",
				"2866 - Rpk (Gold) | Boston 2018",
				"2867 - Scream | Boston 2018",
				"2868 - Scream (Foil) | Boston 2018",
				"2869 - Scream (Gold) | Boston 2018",
				"2870 - Sixer | Boston 2018",
				"2871 - Sixer (Foil) | Boston 2018",
				"2872 - Sixer (Gold) | Boston 2018",
				"2873 - Xms | Boston 2018",
				"2874 - Xms (Foil) | Boston 2018",
				"2875 - Xms (Gold) | Boston 2018",
				"2876 - Amanek | Boston 2018",
				"2877 - Amanek (Foil) | Boston 2018",
				"2878 - Amanek (Gold) | Boston 2018",
				"2879 - Devoduvek | Boston 2018",
				"2880 - Devoduvek (Foil) | Boston 2018",
				"2881 - Devoduvek (Gold) | Boston 2018",
				"2882 - Seang@res | Boston 2018",
				"2883 - Seang@res (Foil) | Boston 2018",
				"2884 - Seang@res (Gold) | Boston 2018",
				"2885 - Shahzam | Boston 2018",
				"2886 - Shahzam (Foil) | Boston 2018",
				"2887 - Shahzam (Gold) | Boston 2018",
				"2888 - Sick | Boston 2018",
				"2889 - Sick (Foil) | Boston 2018",
				"2890 - Sick (Gold) | Boston 2018",
				"2891 - Balblna | Boston 2018",
				"2892 - Balblna (Foil) | Boston 2018",
				"2893 - Balblna (Gold) | Boston 2018",
				"2894 - Boombl4 | Boston 2018",
				"2895 - Boombl4 (Foil) | Boston 2018",
				"2896 - Boombl4 (Gold) | Boston 2018",
				"2897 - Jmqa | Boston 2018",
				"2898 - Jmqa (Foil) | Boston 2018",
				"2899 - Jmqa (Gold) | Boston 2018",
				"2900 - Kvik | Boston 2018",
				"2901 - Kvik (Foil) | Boston 2018",
				"2902 - Kvik (Gold) | Boston 2018",
				"2903 - Waterfallz | Boston 2018",
				"2904 - Waterfallz (Foil) | Boston 2018",
				"2905 - Waterfallz (Gold) | Boston 2018",
				"2906 - Bntet | Boston 2018",
				"2907 - Bntet (Foil) | Boston 2018",
				"2908 - Bntet (Gold) | Boston 2018",
				"2909 - Bondik | Boston 2018",
				"2910 - Bondik (Foil) | Boston 2018",
				"2911 - Bondik (Gold) | Boston 2018",
				"2912 - Captainmo | Boston 2018",
				"2913 - Captainmo (Foil) | Boston 2018",
				"2914 - Captainmo (Gold) | Boston 2018",
				"2915 - Dd | Boston 2018",
				"2916 - Dd (Foil) | Boston 2018",
				"2917 - Dd (Gold) | Boston 2018",
				"2918 - Somebody | Boston 2018",
				"2919 - Somebody (Foil) | Boston 2018",
				"2920 - Somebody (Gold) | Boston 2018",
				"2935 - Flash Gaming | Boston 2018",
				"2936 - Flash Gaming (Holo) | Boston 2018",
				"2937 - Flash Gaming (Foil) | Boston 2018",
				"2938 - Flash Gaming (Gold) | Boston 2018",
				"2939 - Flash Gaming | Boston 2018",
				"2940 - Attacker | Boston 2018",
				"2941 - Attacker (Foil) | Boston 2018",
				"2942 - Attacker (Gold) | Boston 2018",
				"2943 - Karsa | Boston 2018",
				"2944 - Karsa (Foil) | Boston 2018",
				"2945 - Karsa (Gold) | Boston 2018",
				"2946 - Kaze | Boston 2018",
				"2947 - Kaze (Foil) | Boston 2018",
				"2948 - Kaze (Gold) | Boston 2018",
				"2949 - Loveyy | Boston 2018",
				"2950 - Loveyy (Foil) | Boston 2018",
				"2951 - Loveyy (Gold) | Boston 2018",
				"2952 - Summer | Boston 2018",
				"2953 - Summer (Foil) | Boston 2018",
				"2954 - Summer (Gold) | Boston 2018",
			};
			for (int i = 0; i < IM_ARRAYSIZE(Stickers); i++)
				if (filter.PassFilter(Stickers[i]))
					ImGui::Text("%s", Stickers[i]);

			ImGui::End();
		}

		ImGui::GetStyle().WindowMinSize = OldMinSize;
	}


	if (bIsGuiVisible)
	{
		int pX, pY;
		Interfaces::InputSystem()->GetCursorPosition(&pX, &pY);

		ImGui::GetIO().MousePos.x = (float)pX;
		ImGui::GetIO().MousePos.y = (float)pY;

		Client::OnRenderGUI();
	}

	g_pGui->GUI_End_Render();
}



void CGui::MenuColor()
{
	ImGuiStyle& style = ImGui::GetStyle();

	if (Settings::Misc::misc_RainbowMenu)
	{
		static float rainbow;
		rainbow += Settings::Misc::misc_RainbowSpeed;
		if (rainbow > 1.f) rainbow = 0.f;

		ImVec4 mainColorActive = Color2::ToImColor(Color2::FromHSB(rainbow, 1.f, 0.7f));

		style.Colors[ImGuiCol_Text] = ImVec4(1.f, 1.f, 1.f, .78f);
		style.Colors[ImGuiCol_TextDisabled] = mainColorActive;;
		style.Colors[ImGuiCol_WindowBg] = mainColorActive;
		style.Colors[ImGuiCol_ChildWindowBg] = mainColorActive;
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.13f, 0.14f, 0.17f, 0.78f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = mainColorActive;
		style.Colors[ImGuiCol_ScrollbarGrabActive] = mainColorActive;
		style.Colors[ImGuiCol_ComboBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = mainColorActive;
		style.Colors[ImGuiCol_SliderGrab] = mainColorActive;
		style.Colors[ImGuiCol_SliderGrabActive] = mainColorActive;
		style.Colors[ImGuiCol_Button] = ImVec4(0.13f, 0.14f, 0.17f, 0.85f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.13f, 0.14f, 0.17f, 0.86f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_Header] = mainColorActive;
		style.Colors[ImGuiCol_HeaderHovered] = mainColorActive;
		style.Colors[ImGuiCol_HeaderActive] = mainColorActive;
		style.Colors[ImGuiCol_Column] = ImVec4(0.15f, 0.00f, 0.00f, 0.35f);
		style.Colors[ImGuiCol_ColumnHovered] = mainColorActive;
		style.Colors[ImGuiCol_ColumnActive] = mainColorActive;
		style.Colors[ImGuiCol_ResizeGrip] = mainColorActive;
		style.Colors[ImGuiCol_ResizeGripHovered] = mainColorActive;
		style.Colors[ImGuiCol_ResizeGripActive] = mainColorActive;
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.00f, 0.00f, 0.00f, 0.0f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.0f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.0f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
		style.Colors[ImGuiCol_PlotLinesHovered] = mainColorActive;
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = mainColorActive;
		style.Colors[ImGuiCol_TextSelectedBg] = mainColorActive;
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.22f, 0.27f, 0.73f);
	}
	else
	{

		float r = Settings::Misc::misc_MenuColor[0];
		float g = Settings::Misc::misc_MenuColor[1];
		float b = Settings::Misc::misc_MenuColor[2];

		float tR = Settings::Misc::misc_TextColor[0];
		float tG = Settings::Misc::misc_TextColor[1];
		float tB = Settings::Misc::misc_TextColor[2];

		style.Colors[ImGuiCol_Text] = ImVec4(tR, tG, tB, .78f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(r, g, b, 0.78f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(r, g, b, 1.f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(r, g, b, 0.75f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.13f, 0.14f, 0.17f, 0.78f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(r, g, b, 0.78f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(r, g, b, 1.00f);
		style.Colors[ImGuiCol_ComboBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(r, g, b, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(r, g, b, 0.37f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(r, g, b, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.13f, 0.14f, 0.17f, 0.85f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.13f, 0.14f, 0.17f, 0.86f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(r, g, b, 0.76f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(r, g, b, 0.86f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(r, g, b, 1.00f);
		style.Colors[ImGuiCol_Column] = ImVec4(0.15f, 0.00f, 0.00f, 0.35f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(r, g, b, 0.59f);
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(r, g, b, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(r, g, b, 0.63f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(r, g, b, 0.78f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(r, g, b, 1.00f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.00f, 0.00f, 0.00f, 0.0f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.0f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.0f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(r, g, b, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(r, g, b, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(r, g, b, 0.43f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.22f, 0.27f, 0.73f);
	}
}

void CGui::purple()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(0.87f, 0.85f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.87f, 0.85f, 0.92f, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.12f, 0.16f, 0.71f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.27f, 0.20f, 0.39f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.87f, 0.85f, 0.92f, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.27f, 0.20f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.34f, 0.19f, 0.63f, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.46f, 0.27f, 0.80f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.34f, 0.19f, 0.63f, 0.45f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.34f, 0.19f, 0.63f, 0.35f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.34f, 0.19f, 0.63f, 0.78f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.27f, 0.20f, 0.39f, 0.57f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.27f, 0.20f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.19f, 0.63f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.34f, 0.19f, 0.63f, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.27f, 0.20f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.34f, 0.19f, 0.63f, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.19f, 0.63f, 0.24f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.34f, 0.19f, 0.63f, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.34f, 0.19f, 0.63f, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.34f, 0.19f, 0.63f, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.34f, 0.19f, 0.63f, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.87f, 0.85f, 0.92f, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.87f, 0.85f, 0.92f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.87f, 0.85f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.34f, 0.19f, 0.63f, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.34f, 0.19f, 0.63f, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.87f, 0.85f, 0.92f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.87f, 0.85f, 0.92f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.87f, 0.85f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.87f, 0.85f, 0.92f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.87f, 0.85f, 0.92f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.34f, 0.19f, 0.63f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void CGui::DefaultSheme1()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void CGui::RedSheme()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.58f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.92f, 0.18f, 0.29f, 0.37f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.92f, 0.18f, 0.29f, 0.75f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.15f, 0.00f, 0.00f, 0.35f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.59f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.92f, 0.18f, 0.29f, 0.63f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.67f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.78f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.22f, 0.27f, 0.73f);
}

void CGui::darkblue()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

void CGui::MidNightSheme()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(0.85f, 0.89f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.85f, 0.89f, 0.92f, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.14f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.20f, 0.30f, 0.39f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.85f, 0.89f, 0.92f, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.30f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.32f, 0.60f, 0.92f, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.19f, 0.43f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.77f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.77f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.77f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.08f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 0.47f, 1.00f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.10f, 0.38f, 0.62f, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.13f, 0.44f, 0.72f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.12f, 0.45f, 0.55f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.00f, 0.00f, 0.77f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.00f, 0.00f, 0.77f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.77f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.05f, 0.27f, 0.48f, 0.59f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.05f, 0.27f, 0.48f, 0.59f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 0.24f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.85f, 0.89f, 0.92f, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.85f, 0.89f, 0.92f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.85f, 0.89f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.19f, 0.43f, 0.63f, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.19f, 0.43f, 0.63f, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.19f, 0.43f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.85f, 0.89f, 0.92f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.85f, 0.89f, 0.92f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.85f, 0.89f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.85f, 0.89f, 0.92f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.19f, 0.43f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.85f, 0.89f, 0.92f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.19f, 0.43f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.19f, 0.43f, 0.63f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void CGui::NightSheme()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(0.85f, 0.87f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.85f, 0.87f, 0.92f, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.14f, 0.19f, 0.36f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.85f, 0.87f, 0.92f, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.19f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.10f, 0.19f, 0.49f, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.10f, 0.19f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.19f, 0.49f, 0.45f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.19f, 0.49f, 0.35f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.19f, 0.49f, 0.78f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.19f, 0.36f, 0.57f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.14f, 0.19f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.10f, 0.19f, 0.49f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.10f, 0.19f, 0.49f, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.10f, 0.19f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.14f, 0.19f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.16f, 0.86f, 0.90f, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.10f, 0.19f, 0.49f, 0.24f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.07f, 0.26f, 0.53f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.19f, 0.49f, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.10f, 0.19f, 0.49f, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.10f, 0.19f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.19f, 0.49f, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.10f, 0.19f, 0.49f, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.10f, 0.19f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.85f, 0.87f, 0.92f, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.85f, 0.87f, 0.92f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.85f, 0.87f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.10f, 0.19f, 0.49f, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.10f, 0.19f, 0.49f, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.10f, 0.19f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.85f, 0.87f, 0.92f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.85f, 0.87f, 0.92f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.85f, 0.87f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.85f, 0.87f, 0.92f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.10f, 0.19f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.85f, 0.87f, 0.92f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.10f, 0.19f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.10f, 0.19f, 0.49f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void CGui::DunnoSheme()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.65f, 0.00f, 0.06f, 0.03f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.06f, 0.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.00f, 0.06f, 0.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.65f, 0.00f, 0.06f, 3.14f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.65f, 0.00f, 0.06f, 3.14f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.65f, 0.00f, 0.06f, 3.14f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 3.14f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 3.14f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.65f, 0.00f, 0.06f, 3.14f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.65f, 0.00f, 0.06f, 3.14f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.65f, 0.00f, 0.06f, 3.14f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.00f, 0.06f, 3.14f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.65f, 0.00f, 0.06f, 3.14f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.00f, 0.06f, 3.14f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.05f, 0.27f, 0.48f, 0.59f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 0.24f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.85f, 0.89f, 0.92f, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.85f, 0.89f, 0.92f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.85f, 0.89f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.19f, 0.43f, 0.63f, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.19f, 0.43f, 0.63f, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.19f, 0.43f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.85f, 0.89f, 0.92f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.85f, 0.89f, 0.92f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.85f, 0.89f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.85f, 0.89f, 0.92f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.19f, 0.43f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.85f, 0.89f, 0.92f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.19f, 0.43f, 0.63f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.19f, 0.43f, 0.63f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void CGui::BlueSheme()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f); // - 
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.13f, 0.65f, 0.92f, 0.78f); // - 
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f); // - 
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.58f); // - 
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f); // - 
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f); // - 
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // - 
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f); // - 
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.13f, 0.65f, 0.92f, 0.78f); // + 
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.13f, 0.65f, 0.92f, 1.00f); // + 
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f); // - 
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f); // - 
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.13f, 0.65f, 0.92f, 1.00f); // + 
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f); // - 
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f); // - 
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f); // - 
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.13f, 0.65f, 0.92f, 0.78f); // + 
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.13f, 0.65f, 0.92f, 1.00f); // + 
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f); // - 
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.13f, 0.65f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.13f, 0.65f, 0.92f, 0.37f); // + 
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.13f, 0.65f, 0.92f, 1.00f); // + 
	style.Colors[ImGuiCol_Button] = ImVec4(0.13f, 0.65f, 0.92f, 0.75f); // + 
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.13f, 0.65f, 0.92f, 0.86f); // + 
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.65f, 0.92f, 1.00f); // + 
	style.Colors[ImGuiCol_Header] = ImVec4(0.13f, 0.65f, 0.92f, 0.76f); // + 
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.13f, 0.65f, 0.92f, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.13f, 0.65f, 0.92f, 1.00f); // + 
	style.Colors[ImGuiCol_Column] = ImVec4(0.15f, 0.00f, 0.00f, 0.35f); // - 
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.13f, 0.65f, 0.92f, 0.59f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.13f, 0.65f, 0.92f, 1.00f); // + 
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.13f, 0.65f, 0.92f, 0.63f); // + 
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.13f, 0.65f, 0.92f, 0.78f); // + 
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.13f, 0.65f, 0.92f, 1.00f); // + 
	style.Colors[ImGuiCol_CloseButton] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f); // - 
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.67f); // - 
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.78f); // - 
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.13f, 0.65f, 0.92f, 1.00f); // + 
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.13f, 0.65f, 0.92f, 1.00f); // + 
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.13f, 0.65f, 0.92f, 0.43f); // + 
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.22f, 0.27f, 0.73f); // - 
}

void CGui::MidNight2()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void CGui::BlackSheme2()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.11f, 0.11f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.02f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.12f, 0.11f, 0.21f, 0.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.09f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.12f, 0.11f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.05f, 0.02f, 0.05f, 0.98f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.02f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.12f, 0.11f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.18f, 0.18f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.12f, 0.11f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.09f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.11f, 0.11f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.09f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.12f, 0.11f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.18f, 0.18f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.09f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.12f, 0.11f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.18f, 0.18f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.12f, 0.11f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.18f, 0.18f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.18f, 0.18f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 0.02f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
}

void CGui::green()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(0.89f, 0.92f, 0.85f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.89f, 0.92f, 0.85f, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.17f, 0.20f, 0.14f, 0.55f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.37f, 0.49f, 0.20f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.89f, 0.92f, 0.85f, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.37f, 0.49f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.53f, 0.78f, 0.17f, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.53f, 0.78f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.53f, 0.78f, 0.17f, 0.45f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.53f, 0.78f, 0.17f, 0.35f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.53f, 0.78f, 0.17f, 0.78f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.37f, 0.49f, 0.20f, 0.57f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.37f, 0.49f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.53f, 0.78f, 0.17f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.53f, 0.78f, 0.17f, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.53f, 0.78f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.37f, 0.49f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.53f, 0.78f, 0.17f, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.53f, 0.78f, 0.17f, 0.24f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.53f, 0.78f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.53f, 0.78f, 0.17f, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.53f, 0.78f, 0.17f, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.53f, 0.78f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.53f, 0.78f, 0.17f, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.53f, 0.78f, 0.17f, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.78f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.89f, 0.92f, 0.85f, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.89f, 0.92f, 0.85f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.89f, 0.92f, 0.85f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.53f, 0.78f, 0.17f, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.53f, 0.78f, 0.17f, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.53f, 0.78f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.89f, 0.92f, 0.85f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.89f, 0.92f, 0.85f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.89f, 0.92f, 0.85f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.89f, 0.92f, 0.85f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.53f, 0.78f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.89f, 0.92f, 0.85f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.53f, 0.78f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.53f, 0.78f, 0.17f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

}

void CGui::pink()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.85f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.92f, 0.92f, 0.85f, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.20f, 0.19f, 0.14f, 0.55f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.49f, 0.47f, 0.20f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.92f, 0.92f, 0.85f, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.49f, 0.47f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.78f, 0.75f, 0.17f, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.75f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.78f, 0.75f, 0.17f, 0.45f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.78f, 0.75f, 0.17f, 0.35f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.78f, 0.75f, 0.17f, 0.78f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.49f, 0.47f, 0.20f, 0.57f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.49f, 0.47f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.78f, 0.75f, 0.17f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.78f, 0.75f, 0.17f, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.78f, 0.75f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.49f, 0.47f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.75f, 0.17f, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.78f, 0.75f, 0.17f, 0.24f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.78f, 0.75f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.78f, 0.75f, 0.17f, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.78f, 0.75f, 0.17f, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.78f, 0.75f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.78f, 0.75f, 0.17f, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.78f, 0.75f, 0.17f, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.78f, 0.75f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.92f, 0.92f, 0.85f, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.92f, 0.92f, 0.85f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.92f, 0.92f, 0.85f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.78f, 0.75f, 0.17f, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.75f, 0.17f, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.75f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.92f, 0.92f, 0.85f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.92f, 0.92f, 0.85f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.92f, 0.92f, 0.85f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.92f, 0.92f, 0.85f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.78f, 0.75f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.92f, 0.92f, 0.85f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.78f, 0.75f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.78f, 0.75f, 0.17f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}
void CGui::blue()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(0.85f, 0.87f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.85f, 0.87f, 0.92f, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.15f, 0.20f, 0.55f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.20f, 0.27f, 0.49f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.85f, 0.87f, 0.92f, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.27f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.17f, 0.31f, 0.78f, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.17f, 0.31f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.17f, 0.31f, 0.78f, 0.45f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.31f, 0.78f, 0.35f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.17f, 0.31f, 0.78f, 0.78f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.27f, 0.49f, 0.57f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.27f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.17f, 0.31f, 0.78f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.17f, 0.31f, 0.78f, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.17f, 0.31f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.20f, 0.27f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.17f, 0.31f, 0.78f, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.17f, 0.31f, 0.78f, 0.24f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.17f, 0.31f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.17f, 0.31f, 0.78f, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.17f, 0.31f, 0.78f, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.17f, 0.31f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.17f, 0.31f, 0.78f, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.17f, 0.31f, 0.78f, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.17f, 0.31f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.85f, 0.87f, 0.92f, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.85f, 0.87f, 0.92f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.85f, 0.87f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.17f, 0.31f, 0.78f, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.17f, 0.31f, 0.78f, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.17f, 0.31f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.85f, 0.87f, 0.92f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.85f, 0.87f, 0.92f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.85f, 0.87f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.85f, 0.87f, 0.92f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.17f, 0.31f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.85f, 0.87f, 0.92f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.17f, 0.31f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.17f, 0.31f, 0.78f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

}

void CGui::yellow()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.51f, 0.51f, 0.51f, 0.55f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.35f, 0.33f, 0.33f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.86f, 0.86f, 0.86f, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.35f, 0.33f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.82f, 0.82f, 0.82f, 0.92f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(1.02f, 0.94f, 0.94f, 0.45f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.02f, 0.94f, 0.94f, 0.35f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.02f, 0.94f, 0.94f, 0.78f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.35f, 0.33f, 0.33f, 0.57f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.35f, 0.33f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.02f, 0.94f, 0.94f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.02f, 0.94f, 0.94f, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.02f, 0.94f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.35f, 0.33f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(1.02f, 0.94f, 0.94f, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.02f, 0.94f, 0.94f, 0.24f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.02f, 0.94f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(1.02f, 0.94f, 0.94f, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.02f, 0.94f, 0.94f, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.02f, 0.94f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(1.02f, 0.94f, 0.94f, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.02f, 0.94f, 0.94f, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.02f, 0.94f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.92f, 0.85f, 0.85f, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.92f, 0.85f, 0.85f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.92f, 0.85f, 0.85f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.02f, 0.94f, 0.94f, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.02f, 0.94f, 0.94f, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.02f, 0.94f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.92f, 0.85f, 0.85f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.92f, 0.85f, 0.85f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.92f, 0.85f, 0.85f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.02f, 0.94f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.92f, 0.85f, 0.85f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.02f, 0.94f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.02f, 0.94f, 0.94f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}


void CGui::BlackSheme()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.25f, 0.90f, 0.83f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.25f, 0.90f, 0.20f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.25f, 0.90f, 0.87f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 0.40f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_WindowBg];
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.46f, 0.65f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.25f, 0.90f, 0.83f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}



namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	IMGUI_API bool ComboBoxArray(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	IMGUI_API bool TabLabels(const char **tabLabels, int tabSize, int &tabIndex, int *tabOrder)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		const ImVec2 itemSpacing = style.ItemSpacing;
		const ImVec4 color = style.Colors[ImGuiCol_Button];
		const ImVec4 colorActive = style.Colors[ImGuiCol_ButtonActive];
		const ImVec4 colorHover = style.Colors[ImGuiCol_ButtonHovered];
		const ImVec4 colorText = style.Colors[ImGuiCol_Text];
		style.ItemSpacing.x = 2.5;
		style.ItemSpacing.y = 1;
		const ImVec4 colorSelectedTab = ImVec4(color.x, color.y, color.z, color.w*0.5f);
		const ImVec4 colorSelectedTabHovered = ImVec4(colorHover.x, colorHover.y, colorHover.z, colorHover.w*0.5f);
		const ImVec4 colorSelectedTabText = ImVec4(colorText.x*0.8f, colorText.y*0.8f, colorText.z*0.8f, colorText.w*0.8f);

		if (tabSize>0 && (tabIndex<0 || tabIndex >= tabSize))
		{
			if (!tabOrder)
			{
				tabIndex = 0;
			}
			else
			{
				tabIndex = -1;
			}
		}

		float windowWidth = 0.f, sumX = 0.f;
		windowWidth = ImGui::GetWindowWidth() - style.WindowPadding.x - (ImGui::GetScrollMaxY()>0 ? style.ScrollbarSize : 0.f);

		static int draggingTabIndex = -1; int draggingTabTargetIndex = -1;
		static ImVec2 draggingtabSize(0, 0);
		static ImVec2 draggingTabOffset(0, 0);

		const bool isMMBreleased = ImGui::IsMouseReleased(2);
		const bool isMouseDragging = ImGui::IsMouseDragging(0, 2.f);
		int justClosedTabIndex = -1, newtabIndex = tabIndex;

		bool selection_changed = false; bool noButtonDrawn = true;

		for (int j = 0, i; j < tabSize; j++)
		{
			i = tabOrder ? tabOrder[j] : j;
			if (i == -1) continue;

			if (sumX > 0.f)
			{
				sumX += style.ItemSpacing.x;
				sumX += ImGui::CalcTextSize(tabLabels[i]).x + 2.f*style.FramePadding.x;

				if (sumX>windowWidth)
				{
					sumX = 0.f;
				}
			}

			if (i != tabIndex)
			{
				// Push the style
				style.Colors[ImGuiCol_Button] = colorSelectedTab;
				style.Colors[ImGuiCol_ButtonActive] = colorSelectedTab;
				style.Colors[ImGuiCol_ButtonHovered] = colorSelectedTabHovered;
				style.Colors[ImGuiCol_Text] = colorSelectedTabText;
			}
			// Draw the button
			ImGui::PushID(i);   // otherwise two tabs with the same name would clash.
			if (ImGui::Button(tabLabels[i], ImVec2(100.f, 100.f))) { selection_changed = (tabIndex != i); newtabIndex = i; } // Tab Size
			ImGui::PopID();
			if (i != tabIndex)
			{
				// Reset the style
				style.Colors[ImGuiCol_Button] = color;
				style.Colors[ImGuiCol_ButtonActive] = colorActive;
				style.Colors[ImGuiCol_ButtonHovered] = colorHover;
				style.Colors[ImGuiCol_Text] = colorText;
			}
			noButtonDrawn = false;

			if (sumX == 0.f) sumX = style.WindowPadding.x + ImGui::GetItemRectSize().x; // First element of a line

			if (ImGui::IsItemHoveredRect())
			{
				if (tabOrder)
				{
					// tab reordering
					if (isMouseDragging)
					{
						if (draggingTabIndex == -1)
						{
							draggingTabIndex = j;
							draggingtabSize = ImGui::GetItemRectSize();
							const ImVec2& mp = ImGui::GetIO().MousePos;
							const ImVec2 draggingTabCursorPos = ImGui::GetCursorPos();
							draggingTabOffset = ImVec2(
								mp.x + draggingtabSize.x*0.5f - sumX + ImGui::GetScrollX(),
								mp.y + draggingtabSize.y*0.5f - draggingTabCursorPos.y + ImGui::GetScrollY()
							);

						}
					}
					else if (draggingTabIndex >= 0 && draggingTabIndex<tabSize && draggingTabIndex != j)
					{
						draggingTabTargetIndex = j;
					}
				}
			}
		}

		tabIndex = newtabIndex;


		// Draw tab label while mouse drags it
		if (draggingTabIndex >= 0 && draggingTabIndex<tabSize)
		{
			const ImVec2& mp = ImGui::GetIO().MousePos;
			const ImVec2 wp = ImGui::GetWindowPos();
			ImVec2 start(wp.x + mp.x - draggingTabOffset.x - draggingtabSize.x*0.5f, wp.y + mp.y - draggingTabOffset.y - draggingtabSize.y*0.5f);
			const ImVec2 end(start.x + draggingtabSize.x, start.y + draggingtabSize.y);
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			const float draggedBtnAlpha = 0.65f;
			const ImVec4& btnColor = style.Colors[ImGuiCol_Button];
			drawList->AddRectFilled(start, end, ImColor(btnColor.x, btnColor.y, btnColor.z, btnColor.w*draggedBtnAlpha), style.FrameRounding);
			start.x += style.FramePadding.x; start.y += style.FramePadding.y;
			const ImVec4& txtColor = style.Colors[ImGuiCol_Text];
			drawList->AddText(start, ImColor(txtColor.x, txtColor.y, txtColor.z, txtColor.w*draggedBtnAlpha), tabLabels[tabOrder[draggingTabIndex]]);

			ImGui::SetMouseCursor(ImGuiMouseCursor_Move);
		}

		// Drop tab label
		if (draggingTabTargetIndex != -1)
		{
			// swap draggingTabIndex and draggingTabTargetIndex in tabOrder
			const int tmp = tabOrder[draggingTabTargetIndex];
			tabOrder[draggingTabTargetIndex] = tabOrder[draggingTabIndex];
			tabOrder[draggingTabIndex] = tmp;
			//fprintf(stderr,"%d %d\n",draggingTabIndex,draggingTabTargetIndex);
			draggingTabTargetIndex = draggingTabIndex = -1;
		}

		// Reset draggingTabIndex if necessary
		if (!isMouseDragging) draggingTabIndex = -1;

		// Change selected tab when user closes the selected tab
		if (tabIndex == justClosedTabIndex && tabIndex >= 0)
		{
			tabIndex = -1;
			for (int j = 0, i; j < tabSize; j++)
			{
				i = tabOrder ? tabOrder[j] : j;
				if (i == -1)
				{
					continue;
				}
				tabIndex = i;
				break;
			}
		}

		// Restore the style
		style.Colors[ImGuiCol_Button] = color;
		style.Colors[ImGuiCol_ButtonActive] = colorActive;
		style.Colors[ImGuiCol_ButtonHovered] = colorHover;
		style.Colors[ImGuiCol_Text] = colorText;
		style.ItemSpacing = itemSpacing;

		return selection_changed;
	}
}

void CGui::TabIcons()
{
	ImGui::PushFont(Tabfont);
	ImGuiStyle& style = ImGui::GetStyle();
	style.FramePadding = ImVec2(3.5, 3.5);
	style.FrameRounding = 20.0f;
	style.ItemSpacing = ImVec2(5, 5);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 20.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 20.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

	style.Colors[ImGuiCol_Text] = ImVec4(0, 0, 0, 255);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.10f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.60f, 0.60f, 0.60f, 0.60f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.78f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 1.00f, 1.00f, 0.75f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.00f, 1.00f, 1.00f, 0.35f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.63f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
}

void CGui::TabColors()
{
	ImGui::PushFont(font);
	ImGuiStyle& style = ImGui::GetStyle();
	style.FramePadding = ImVec2(3.5, 3.5);
	style.FrameRounding = 5.0f;
	style.ItemSpacing = ImVec2(5, 5);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 5.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 5.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

	style.Colors[ImGuiCol_Text] = ImVec4(0, 0, 0, 255);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.10f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.60f, 0.60f, 0.60f, 0.60f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.78f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 1.00f, 1.00f, 0.75f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.00f, 1.00f, 1.00f, 0.35f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.63f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
}

void CGui::bluefont()
{
	ImGui::PushFont(font);
	ImGuiStyle& style = ImGui::GetStyle();
	style.FramePadding = ImVec2(3.5, 3.5);
	style.FrameRounding = 5.0f;
	style.ItemSpacing = ImVec2(5, 5);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 5.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 5.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.55f, 0.17f, 0.17f, 1.07f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.30f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.10f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(1.00f, 0.10f, 0.10f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(1.00f, 0.10f, 0.10f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.60f, 0.60f, 0.60f, 0.60f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.78f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(1.00f, 0.10f, 0.10f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.00f, 1.00f, 1.00f, 0.35f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.63f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(1.00f, 0.10f, 0.10f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.22745098039f, 0.39607843137f, 0.94117647058f, 1.00f);

}

void CGui::IconFont()
{
	ImGui::PushFont(Iconfont);
	ImGuiStyle& style = ImGui::GetStyle();
	style.FramePadding = ImVec2(3.5, 3.5);
	style.FrameRounding = 10.0f;
	style.ItemSpacing = ImVec2(5, 5);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 10.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 10.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);



	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.10f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.78f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 1.00f, 1.00f, 0.75f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.00f, 1.00f, 1.00f, 0.35f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.63f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);

}

void CGui::NameFont()
{
	ImGui::PushFont(Namefont);
	ImGuiStyle& style = ImGui::GetStyle();
	style.FramePadding = ImVec2(3.5, 3.5);
	style.FrameRounding = 10.0f;
	style.ItemSpacing = ImVec2(5, 5);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 5.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 5.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.10f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.60f, 0.60f, 0.60f, 0.60f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.78f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 1.00f, 1.00f, 0.75f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.00f, 1.00f, 1.00f, 0.35f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.63f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);

}