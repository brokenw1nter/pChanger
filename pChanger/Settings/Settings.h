#pragma once

#include "../Main/Main.h"

#define FIX_MIN_FOV_HEAD	30
#define FIX_MIN_FOV_CHEST	50
#define FOV_BASE_DISTANCE	200

struct k_weapon_data {
	int itemDefinitionIndex;
	int paintKit;
	int rarity;
	int seed;
	float wear;
	char* name = "";
};

namespace Settings
{
	void LoadSettings( string szIniFile );
	void SaveSettings( string szIniFile );
	extern float hitmarkerAlpha;
	int TriggerCharToKey( const char* Key );

	namespace MedalChanger {
		extern bool enabled;
		extern std::vector<uint32_t> medals;
		extern bool equipped_medal_override;
		extern uint32_t equipped_medal;
	}
	namespace ProfileChanger {
		extern bool enabled;
		extern int rank_id;
		extern int wins;
		extern int cmd_friendly;
		extern int cmd_leader;
		extern int cmd_teaching;
		extern int level;
		extern int xp;
		extern int ban;
		extern int time;
	}
	namespace InventoryChanger {
		extern bool enabled;
		extern std::vector<k_weapon_data> weapons;
	}

	/*
	namespace Sticker
	{
		extern int stickerSafeWeapon;

		extern int sticker_deserteagle_iSlot[4];
		extern float sticker_deserteagle_wear[4];
		extern float sticker_deserteagle_size[4];
		extern int sticker_deserteagle_rotate[4];
		extern int sticker_deserteagle_id[4];

		extern int sticker_elites_iSlot[4];
		extern float sticker_elites_wear[4];
		extern float sticker_elites_size[4];
		extern int sticker_elites_rotate[4];
		extern int sticker_elites_id[4];

		extern int sticker_fiveseven_iSlot[4];
		extern float sticker_fiveseven_wear[4];
		extern float sticker_fiveseven_size[4];
		extern int sticker_fiveseven_rotate[4];
		extern int sticker_fiveseven_id[4];

		extern int sticker_glock_iSlot[4];
		extern float sticker_glock_wear[4];
		extern float sticker_glock_size[4];
		extern int sticker_glock_rotate[4];
		extern int sticker_glock_id[4];

		extern int sticker_ak_iSlot[4];
		extern float sticker_ak_wear[4];
		extern float sticker_ak_size[4];
		extern int sticker_ak_rotate[4];
		extern int sticker_ak_id[4];

		extern int sticker_fiveseven_iSlot[4];
		extern float sticker_fiveseven_wear[4];
		extern float sticker_fiveseven_size[4];
		extern int sticker_fiveseven_rotate[4];
		extern int sticker_fiveseven_id[4];

		extern int sticker_fiveseven_iSlot[4];
		extern float sticker_fiveseven_wear[4];
		extern float sticker_fiveseven_size[4];
		extern int sticker_fiveseven_rotate[4];
		extern int sticker_fiveseven_id[4];

		extern int sticker_aug_iSlot[4];
		extern float sticker_aug_wear[4];
		extern float sticker_aug_size[4];
		extern int sticker_aug_rotate[4];
		extern int sticker_aug_id[4];

		extern int sticker_awp_iSlot[4];
		extern float sticker_awp_wear[4];
		extern float sticker_awp_size[4];
		extern int sticker_awp_rotate[4];
		extern int sticker_awp_id[4];

		extern int sticker_famas_iSlot[4];
		extern float sticker_famas_wear[4];
		extern float sticker_famas_size[4];
		extern int sticker_famas_rotate[4];
		extern int sticker_famas_id[4];

		extern int sticker_g35g1_iSlot[4];
		extern float sticker_g35g1_wear[4];
		extern float sticker_g35g1_size[4];
		extern int sticker_g35g1_rotate[4];
		extern int sticker_g35g1_id[4];

		extern int sticker_galil_iSlot[4];
		extern float sticker_galil_wear[4];
		extern float sticker_galil_size[4];
		extern int sticker_galil_rotate[4];
		extern int sticker_galil_id[4];

		extern int sticker_m249_iSlot[4];
		extern float sticker_m249_wear[4];
		extern float sticker_m249_size[4];
		extern int sticker_m249_rotate[4];
		extern int sticker_m249_id[4];

		extern int sticker_m4a4_iSlot[4];
		extern float sticker_m4a4_wear[4];
		extern float sticker_m4a4_size[4];
		extern int sticker_m4a4_rotate[4];
		extern int sticker_m4a4_id[4];

		extern int sticker_mac10_iSlot[4];
		extern float sticker_mac10_wear[4];
		extern float sticker_mac10_size[4];
		extern int sticker_mac10_rotate[4];
		extern int sticker_mac10_id[4];

		extern int sticker_p90_iSlot[4];
		extern float sticker_p90_wear[4];
		extern float sticker_p90_size[4];
		extern int sticker_p90_rotate[4];
		extern int sticker_p90_id[4];

		extern int sticker_ump_iSlot[4];
		extern float sticker_ump_wear[4];
		extern float sticker_ump_size[4];
		extern int sticker_ump_rotate[4];
		extern int sticker_ump_id[4];

		extern int sticker_xm_iSlot[4];
		extern float sticker_xm_wear[4];
		extern float sticker_xm_size[4];
		extern int sticker_xm_rotate[4];
		extern int sticker_xm_id[4];

		extern int sticker_bizon_iSlot[4];
		extern float sticker_bizon_wear[4];
		extern float sticker_bizon_size[4];
		extern int sticker_bizon_rotate[4];
		extern int sticker_bizon_id[4];

		extern int sticker_mag7_iSlot[4];
		extern float sticker_mag7_wear[4];
		extern float sticker_mag7_size[4];
		extern int sticker_mag7_rotate[4];
		extern int sticker_mag7_id[4];

		extern int sticker_negev_iSlot[4];
		extern float sticker_negev_wear[4];
		extern float sticker_negev_size[4];
		extern int sticker_negev_rotate[4];
		extern int sticker_negev_id[4];

		extern int sticker_sawedoff_iSlot[4];
		extern float sticker_sawedoff_wear[4];
		extern float sticker_sawedoff_size[4];
		extern int sticker_sawedoff_rotate[4];
		extern int sticker_sawedoff_id[4];

		extern int sticker_tec_iSlot[4];
		extern float sticker_tec_wear[4];
		extern float sticker_tec_size[4];
		extern int sticker_tec_rotate[4];
		extern int sticker_tec_id[4];

		extern int sticker_p2000_iSlot[4];
		extern float sticker_p2000_wear[4];
		extern float sticker_p2000_size[4];
		extern int sticker_p2000_rotate[4];
		extern int sticker_p2000_id[4];

		extern int sticker_mp7_iSlot[4];
		extern float sticker_mp7_wear[4];
		extern float sticker_mp7_size[4];
		extern int sticker_mp7_rotate[4];
		extern int sticker_mp7_id[4];

		extern int sticker_mp9_iSlot[4];
		extern float sticker_mp9_wear[4];
		extern float sticker_mp9_size[4];
		extern int sticker_mp9_rotate[4];
		extern int sticker_mp9_id[4];

		extern int sticker_nova_iSlot[4];
		extern float sticker_nova_wear[4];
		extern float sticker_nova_size[4];
		extern int sticker_nova_rotate[4];
		extern int sticker_nova_id[4];

		extern int sticker_p250_iSlot[4];
		extern float sticker_p250_wear[4];
		extern float sticker_p250_size[4];
		extern int sticker_p250_rotate[4];
		extern int sticker_p250_id[4];

		extern int sticker_scar_iSlot[4];
		extern float sticker_scar_wear[4];
		extern float sticker_scar_size[4];
		extern int sticker_scar_rotate[4];
		extern int sticker_scar_id[4];

		extern int sticker_sg_iSlot[4];
		extern float sticker_sg_wear[4];
		extern float sticker_sg_size[4];
		extern int sticker_sg_rotate[4];
		extern int sticker_sg_id[4];

		extern int sticker_ssg_iSlot[4];
		extern float sticker_ssg_wear[4];
		extern float sticker_ssg_size[4];
		extern int sticker_ssg_rotate[4];
		extern int sticker_ssg_id[4];
	}
	*/

	namespace Esp
	{
		//[swap_lines]
		extern bool esp_Time;
		extern bool esp_Watermark;
		extern bool esp_Cheatbuild;
		extern bool esp_Rank;
		extern bool esp_Sound;

		extern float flTracer_Beam[3];
		extern bool esp_beams_tracer;
		extern float flTracersDuration;
		extern float flTracersWidth;

		extern bool esp_HitMarker;
		extern int esp_HitMarkerSound;
		extern float esp_HitMarkerColor[3];

		extern int esp_BombTimer;
		extern bool esp_ChromeWorld;
		extern bool esp_MinecraftMode;
		extern bool esp_LSDMode;

		extern float esp_Color_CT[3];
		extern float esp_Color_TT[3];

		extern bool esp_NightMode;
		//[/swap_lines]
	}


	namespace Skin
	{
		//[swap_lines]
		extern int knf_ct_model;
		extern int knf_ct_skin;
		extern int knf_tt_model;
		extern int knf_tt_skin;
		extern int gloves_skin;
		//[/swap_lines]
	}

	namespace Misc
	{
		//[swap_lines]
		extern QAngle qLastTickAngle;
		extern bool misc_ThirdPerson;
		extern float misc_ThirdPersonRange;
		extern const char* misc_SkyName;
		extern float misc_MenuColor[3];
		extern float misc_TextColor[3];
		extern bool misc_ArmMaterials;
		extern int misc_ArmMaterialsList;
		extern int misc_ArmMaterialsType;
		extern int misc_CurrentSky;
		extern bool misc_SkinChanger;
		extern bool misc_KnifeChanger;
		extern bool misc_StickerChanger;
		extern bool stickerids;
		extern bool skinids;
		extern bool misc_NoSky;
		extern bool misc_Snow;
		extern bool misc_EPostprocess;
		extern bool misc_Postprocess;
		extern bool misc_Bhop;
		extern int misc_Clan;
		extern bool misc_spamregular;
		extern bool misc_spamrandom;
		extern bool misc_AwpAim;
		extern bool misc_NoFlash;	
		extern bool misc_NoSmoke;
		extern bool misc_wireframesmoke;
		extern bool misc_WireHands;
		extern bool misc_NoHands;
		extern bool misc_AutoAccept;
		extern bool misc_RainbowMenu;
		extern float misc_RainbowSpeed;
		extern bool misc_FovChanger;
		extern int misc_FovView;
		extern int misc_FovModelView;
		extern float misc_AwpAimColor[3];
		extern float hitmarkerAlpha;
		extern bool skinids;
		extern bool medalids;
		extern bool weaponids;
		extern char misc_NameChange;
		extern bool window_profilechanger;
		extern bool window_medalchanger;
		extern bool window_inventorychanger;
		extern bool invalwayson;
		extern int customodelst;
		extern int customodelsct;
		extern bool custommodelson;
		extern bool misc_FullBright;
	}


}


#define CHEAT_NAME "pChanger"
#define WATER_MARK "pChanger"
#define COLORS_TEXT "Colors"
#define CONFIG_TEXT "Config"
#define VISUAL_TEXT "ESP"
#define SKIN_TEXT "Skin"
#define MISC_TEXT "Misc"
#define INVCHANGE_TEXT "InventoryChanger"
#define MEDALCHANGE_TEXT "MedalChanger"
#define PROFILECHANGE_TEXT "ProfileChanger"


#define SKIN_TEXT "Skin"
#define SKIN_TEXT_WEAR "Wear"
#define SKIN_TEXT_SEED "Seed"
#define SKIN_TEXT_STAT "StatTrak"
#define SKIN_TEXT_QUALITY "Quality"
#define MISC_TEXT "Misc"

#define CVAR_ESP_COLOR_BulletBeams "esp_bulletBeamsColor"

#define CVAR_KEY_MOUSE3 "MOUSE WHEEL"
#define CVAR_KEY_MOUSE4 "RIGHT MOUSE"
#define CVAR_KEY_MOUSE5 "LEFT ALT"

#define CVAR_ESP_RANK "esp_Rank"
#define CVAR_ESP_TIME "esp_Time"
#define CVAR_ESP_WATER "esp_Watermark"
#define CVAR_ESP_CHEATBUILD "esp_Cheatbuild"

#define CVAR_ESP_HITMARKER_ENABLED "esp_HitMarker"
#define CVAR_ESP_HITMARKER_COMBO "esp_HitMarkerSound"
#define CVAR_ESP_HITMARKER_COLOR "esp_HitMarkerColor"

#define CVAR_ESP_SOUND "esp_Sound"

#define CVAR_ESP_BEAMS "esp_BeamsEnable"

#define CVAR_ESP_BOMBTIMER "esp_BombTimer"
#define CVAR_ESP_BEAMS_WIDTH "esp_bulletBeamsWidth"
#define CVAR_ESP_BEAMS_DURATION "esp_bulletBeamsDuration"

#define C4_TIMER_STRING "bomb: %0.2f"

#define CVAR_SKIN_CT_MODEL "knf_ct_model"
#define CVAR_SKIN_CT_SKIN "knf_ct_skin"
#define CVAR_SKIN_TT_MODEL "knf_tt_model"
#define CVAR_SKIN_TT_SKIN "knf_tt_skin"
#define CVAR_SKIN_GLOVES "gloves_skin"

#define CVAR_MEDALS 'medals'
#define CVAR_EQUIPPED_MEDAL 'equipped_medals'

#define CVAR_RANK_ID "rank_id"
#define CVAR_WINS "wins"
#define CVAR_CMD_FRIENDLY "cmd_friendly"
#define CVAR_CMD_LEADER "cmd_leader"
#define CVAR_CMD_TEACHING "cmd_teaching"
#define CVAR_LEVEL "level"
#define CVAR_XP "xp"

#define CVAR_MISC_MENU_COLOR "misc_MenuColor"
#define CVAR_MISC_SKIN_CHANGER "misc_SkinChanger"
#define CVAR_MISC_KNIFE_CHANGER "misc_KnifeChanger"
#define CVAR_MISC_STICKER_CHANGER "misc_StickerChanger"
#define CVAR_MISC_EPOSTPROCESS "misc_EPostprocess"
#define CVAR_MISC_POSTPROCESS "misc_Postprocess"
#define CVAR_MISC_BHOP "misc_Bhop"
#define CVAR_MISC_AWPAIM "misc_AwpAim"
#define CVAR_MISC_NOFLASH "misc_NoFlash"
#define CVAR_MISC_NOHANDS "misc_NoHands"
#define CVAR_MISC_NOSMOKE "misc_NoSmoke"
#define CVAR_MISC_WIREFRAMESMOKE "misc_wireframesmoke"
#define CVAR_MISC_WIREHANDS "misc_WireHands"
#define CVAR_MISC_AUTOACCEPT "misc_AutoAccept"
#define CVAR_MISC_FOV_CHANGER "misc_FovChanger"
#define CVAR_MISC_FOV_VIEW "misc_FovView"
#define CVAR_MISC_FOV_MDL_VIEW "misc_FovModelView"
#define CVAR_MISC_AWPAIM_COLOR "misc_AwpAimColor"
#define CVAR_MISC_INVALWAYSON "misc_InvAlwaysOn"

/*
#define CVAR_MISC_STICKERWEAR_DEAGLE1 "misc_Sticker_DEAGLE_wear1"
#define CVAR_MISC_STICKERSLOT_DEAGLE1 "misc_Sticker_DEAGLE_slot1"
#define CVAR_MISC_STICKERSIZE_DEAGLE1 "misc_Sticker_DEAGLE_size1"
#define CVAR_MISC_STICKERID_DEAGLE1 "misc_Sticker_DEAGLE_ID1"
#define CVAR_MISC_STICKERROTATE_DEAGLE1 "misc_Sticker_DEAGLE_rotate1"
#define CVAR_MISC_STICKERWEAR_DEAGLE2 "misc_Sticker_DEAGLE_wear2"
#define CVAR_MISC_STICKERSLOT_DEAGLE2 "misc_Sticker_DEAGLE_slot2"
#define CVAR_MISC_STICKERSIZE_DEAGLE2 "misc_Sticker_DEAGLE_size2"
#define CVAR_MISC_STICKERID_DEAGLE2 "misc_Sticker_DEAGLE_ID2"
#define CVAR_MISC_STICKERROTATE_DEAGLE2 "misc_Sticker_DEAGLE_rotate2"
#define CVAR_MISC_STICKERWEAR_DEAGLE3 "misc_Sticker_DEAGLE_wear3"
#define CVAR_MISC_STICKERSLOT_DEAGLE3 "misc_Sticker_DEAGLE_slot3"
#define CVAR_MISC_STICKERSIZE_DEAGLE3 "misc_Sticker_DEAGLE_size3"
#define CVAR_MISC_STICKERID_DEAGLE3 "misc_Sticker_DEAGLE_ID3"
#define CVAR_MISC_STICKERROTATE_DEAGLE3 "misc_Sticker_DEAGLE_rotate3"
#define CVAR_MISC_STICKERWEAR_DEAGLE4 "misc_Sticker_DEAGLE_wear4"
#define CVAR_MISC_STICKERSLOT_DEAGLE4 "misc_Sticker_DEAGLE_slot4"
#define CVAR_MISC_STICKERSIZE_DEAGLE4 "misc_Sticker_DEAGLE_size4"
#define CVAR_MISC_STICKERID_DEAGLE4 "misc_Sticker_DEAGLE_ID4"
#define CVAR_MISC_STICKERROTATE_DEAGLE4 "misc_Sticker_DEAGLE_rotate4"
#define CVAR_MISC_STICKERWEAR_ELITES1 "misc_Sticker_ELITES_wear1"
#define CVAR_MISC_STICKERSLOT_ELITES1 "misc_Sticker_ELITES_slot1"
#define CVAR_MISC_STICKERSIZE_ELITES1 "misc_Sticker_ELITES_size1"
#define CVAR_MISC_STICKERID_ELITES1 "misc_Sticker_ELITES_ID1"
#define CVAR_MISC_STICKERROTATE_ELITES1 "misc_Sticker_ELITES_rotate1"
#define CVAR_MISC_STICKERWEAR_ELITES2 "misc_Sticker_ELITES_wear2"
#define CVAR_MISC_STICKERSLOT_ELITES2 "misc_Sticker_ELITES_slot2"
#define CVAR_MISC_STICKERSIZE_ELITES2 "misc_Sticker_ELITES_size2"
#define CVAR_MISC_STICKERID_ELITES2 "misc_Sticker_ELITES_ID2"
#define CVAR_MISC_STICKERROTATE_ELITES2 "misc_Sticker_ELITES_rotate2"
#define CVAR_MISC_STICKERWEAR_ELITES3 "misc_Sticker_ELITES_wear3"
#define CVAR_MISC_STICKERSLOT_ELITES3 "misc_Sticker_ELITES_slot3"
#define CVAR_MISC_STICKERSIZE_ELITES3 "misc_Sticker_ELITES_size3"
#define CVAR_MISC_STICKERID_ELITES3 "misc_Sticker_ELITES_ID3"
#define CVAR_MISC_STICKERROTATE_ELITES3 "misc_Sticker_ELITES_rotate3"
#define CVAR_MISC_STICKERWEAR_ELITES4 "misc_Sticker_ELITES_wear4"
#define CVAR_MISC_STICKERSLOT_ELITES4 "misc_Sticker_ELITES_slot4"
#define CVAR_MISC_STICKERSIZE_ELITES4 "misc_Sticker_ELITES_size4"
#define CVAR_MISC_STICKERID_ELITES4 "misc_Sticker_ELITES_ID4"
#define CVAR_MISC_STICKERROTATE_ELITES4 "misc_Sticker_ELITES_rotate4"
#define CVAR_MISC_STICKERWEAR_FIVESEVEN1 "misc_Sticker_FIVESEVEN_wear1"
#define CVAR_MISC_STICKERSLOT_FIVESEVEN1 "misc_Sticker_FIVESEVEN_slot1"
#define CVAR_MISC_STICKERSIZE_FIVESEVEN1 "misc_Sticker_FIVESEVEN_size1"
#define CVAR_MISC_STICKERID_FIVESEVEN1 "misc_Sticker_FIVESEVEN_ID1"
#define CVAR_MISC_STICKERROTATE_FIVESEVEN1 "misc_Sticker_FIVESEVEN_rotate1"
#define CVAR_MISC_STICKERWEAR_FIVESEVEN2 "misc_Sticker_FIVESEVEN_wear2"
#define CVAR_MISC_STICKERSLOT_FIVESEVEN2 "misc_Sticker_FIVESEVEN_slot2"
#define CVAR_MISC_STICKERSIZE_FIVESEVEN2 "misc_Sticker_FIVESEVEN_size2"
#define CVAR_MISC_STICKERID_FIVESEVEN2 "misc_Sticker_FIVESEVEN_ID2"
#define CVAR_MISC_STICKERROTATE_FIVESEVEN2 "misc_Sticker_FIVESEVEN_rotate2"
#define CVAR_MISC_STICKERWEAR_FIVESEVEN3 "misc_Sticker_FIVESEVEN_wear3"
#define CVAR_MISC_STICKERSLOT_FIVESEVEN3 "misc_Sticker_FIVESEVEN_slot3"
#define CVAR_MISC_STICKERSIZE_FIVESEVEN3 "misc_Sticker_FIVESEVEN_size3"
#define CVAR_MISC_STICKERID_FIVESEVEN3 "misc_Sticker_FIVESEVEN_ID3"
#define CVAR_MISC_STICKERROTATE_FIVESEVEN3 "misc_Sticker_FIVESEVEN_rotate3"
#define CVAR_MISC_STICKERWEAR_FIVESEVEN4 "misc_Sticker_FIVESEVEN_wear4"
#define CVAR_MISC_STICKERSLOT_FIVESEVEN4 "misc_Sticker_FIVESEVEN_slot4"
#define CVAR_MISC_STICKERSIZE_FIVESEVEN4 "misc_Sticker_FIVESEVEN_size4"
#define CVAR_MISC_STICKERID_FIVESEVEN4 "misc_Sticker_FIVESEVEN_ID4"
#define CVAR_MISC_STICKERROTATE_FIVESEVEN4 "misc_Sticker_FIVESEVEN_rotate4"
#define CVAR_MISC_STICKERWEAR_GLOCK1 "misc_Sticker_GLOCK_wear1"
#define CVAR_MISC_STICKERSLOT_GLOCK1 "misc_Sticker_GLOCK_slot1"
#define CVAR_MISC_STICKERSIZE_GLOCK1 "misc_Sticker_GLOCK_size1"
#define CVAR_MISC_STICKERID_GLOCK1 "misc_Sticker_GLOCK_ID1"
#define CVAR_MISC_STICKERROTATE_GLOCK1 "misc_Sticker_GLOCK_rotate1"
#define CVAR_MISC_STICKERWEAR_GLOCK2 "misc_Sticker_GLOCK_wear2"
#define CVAR_MISC_STICKERSLOT_GLOCK2 "misc_Sticker_GLOCK_slot2"
#define CVAR_MISC_STICKERSIZE_GLOCK2 "misc_Sticker_GLOCK_size2"
#define CVAR_MISC_STICKERID_GLOCK2 "misc_Sticker_GLOCK_ID2"
#define CVAR_MISC_STICKERROTATE_GLOCK2 "misc_Sticker_GLOCK_rotate2"
#define CVAR_MISC_STICKERWEAR_GLOCK3 "misc_Sticker_GLOCK_wear3"
#define CVAR_MISC_STICKERSLOT_GLOCK3 "misc_Sticker_GLOCK_slot3"
#define CVAR_MISC_STICKERSIZE_GLOCK3 "misc_Sticker_GLOCK_size3"
#define CVAR_MISC_STICKERID_GLOCK3 "misc_Sticker_GLOCK_ID3"
#define CVAR_MISC_STICKERROTATE_GLOCK3 "misc_Sticker_GLOCK_rotate3"
#define CVAR_MISC_STICKERWEAR_GLOCK4 "misc_Sticker_GLOCK_wear4"
#define CVAR_MISC_STICKERSLOT_GLOCK4 "misc_Sticker_GLOCK_slot4"
#define CVAR_MISC_STICKERSIZE_GLOCK4 "misc_Sticker_GLOCK_size4"
#define CVAR_MISC_STICKERID_GLOCK4 "misc_Sticker_GLOCK_ID4"
#define CVAR_MISC_STICKERROTATE_GLOCK4 "misc_Sticker_GLOCK_rotate4"
#define CVAR_MISC_STICKERWEAR_AK1 "misc_Sticker_AK_wear1"
#define CVAR_MISC_STICKERSLOT_AK1 "misc_Sticker_AK_slot1"
#define CVAR_MISC_STICKERSIZE_AK1 "misc_Sticker_AK_size1"
#define CVAR_MISC_STICKERID_AK1 "misc_Sticker_AK_ID1"
#define CVAR_MISC_STICKERROTATE_AK1 "misc_Sticker_AK_rotate1"
#define CVAR_MISC_STICKERWEAR_AK2 "misc_Sticker_AK_wear2"
#define CVAR_MISC_STICKERSLOT_AK2 "misc_Sticker_AK_slot2"
#define CVAR_MISC_STICKERSIZE_AK2 "misc_Sticker_AK_size2"
#define CVAR_MISC_STICKERID_AK2 "misc_Sticker_AK_ID2"
#define CVAR_MISC_STICKERROTATE_AK2 "misc_Sticker_AK_rotate2"
#define CVAR_MISC_STICKERWEAR_AK3 "misc_Sticker_AK_wear3"
#define CVAR_MISC_STICKERSLOT_AK3 "misc_Sticker_AK_slot3"
#define CVAR_MISC_STICKERSIZE_AK3 "misc_Sticker_AK_size3"
#define CVAR_MISC_STICKERID_AK3 "misc_Sticker_AK_ID3"
#define CVAR_MISC_STICKERROTATE_AK3 "misc_Sticker_AK_rotate3"
#define CVAR_MISC_STICKERWEAR_AK4 "misc_Sticker_AK_wear4"
#define CVAR_MISC_STICKERSLOT_AK4 "misc_Sticker_AK_slot4"
#define CVAR_MISC_STICKERSIZE_AK4 "misc_Sticker_AK_size4"
#define CVAR_MISC_STICKERID_AK4 "misc_Sticker_AK_ID4"
#define CVAR_MISC_STICKERROTATE_AK4 "misc_Sticker_AK_rotate4"
#define CVAR_MISC_STICKERWEAR_AUG1 "misc_Sticker_AUG_wear1"
#define CVAR_MISC_STICKERSLOT_AUG1 "misc_Sticker_AUG_slot1"
#define CVAR_MISC_STICKERSIZE_AUG1 "misc_Sticker_AUG_size1"
#define CVAR_MISC_STICKERID_AUG1 "misc_Sticker_AUG_ID1"
#define CVAR_MISC_STICKERROTATE_AUG1 "misc_Sticker_AUG_rotate1"
#define CVAR_MISC_STICKERWEAR_AUG2 "misc_Sticker_AUG_wear2"
#define CVAR_MISC_STICKERSLOT_AUG2 "misc_Sticker_AUG_slot2"
#define CVAR_MISC_STICKERSIZE_AUG2 "misc_Sticker_AUG_size2"
#define CVAR_MISC_STICKERID_AUG2 "misc_Sticker_AUG_ID2"
#define CVAR_MISC_STICKERROTATE_AUG2 "misc_Sticker_AUG_rotate2"
#define CVAR_MISC_STICKERWEAR_AUG3 "misc_Sticker_AUG_wear3"
#define CVAR_MISC_STICKERSLOT_AUG3 "misc_Sticker_AUG_slot3"
#define CVAR_MISC_STICKERSIZE_AUG3 "misc_Sticker_AUG_size3"
#define CVAR_MISC_STICKERID_AUG3 "misc_Sticker_AUG_ID3"
#define CVAR_MISC_STICKERROTATE_AUG3 "misc_Sticker_AUG_rotate3"
#define CVAR_MISC_STICKERWEAR_AUG4 "misc_Sticker_AUG_wear4"
#define CVAR_MISC_STICKERSLOT_AUG4 "misc_Sticker_AUG_slot4"
#define CVAR_MISC_STICKERSIZE_AUG4 "misc_Sticker_AUG_size4"
#define CVAR_MISC_STICKERID_AUG4 "misc_Sticker_AUG_ID4"
#define CVAR_MISC_STICKERROTATE_AUG4 "misc_Sticker_AUG_rotate4"
#define CVAR_MISC_STICKERWEAR_AWP1 "misc_Sticker_AWP_wear1"
#define CVAR_MISC_STICKERSLOT_AWP1 "misc_Sticker_AWP_slot1"
#define CVAR_MISC_STICKERSIZE_AWP1 "misc_Sticker_AWP_size1"
#define CVAR_MISC_STICKERID_AWP1 "misc_Sticker_AWP_ID1"
#define CVAR_MISC_STICKERROTATE_AWP1 "misc_Sticker_AWP_rotate1"
#define CVAR_MISC_STICKERWEAR_AWP2 "misc_Sticker_AWP_wear2"
#define CVAR_MISC_STICKERSLOT_AWP2 "misc_Sticker_AWP_slot2"
#define CVAR_MISC_STICKERSIZE_AWP2 "misc_Sticker_AWP_size2"
#define CVAR_MISC_STICKERID_AWP2 "misc_Sticker_AWP_ID2"
#define CVAR_MISC_STICKERROTATE_AWP2 "misc_Sticker_AWP_rotate2"
#define CVAR_MISC_STICKERWEAR_AWP3 "misc_Sticker_AWP_wear3"
#define CVAR_MISC_STICKERSLOT_AWP3 "misc_Sticker_AWP_slot3"
#define CVAR_MISC_STICKERSIZE_AWP3 "misc_Sticker_AWP_size3"
#define CVAR_MISC_STICKERID_AWP3 "misc_Sticker_AWP_ID3"
#define CVAR_MISC_STICKERROTATE_AWP3 "misc_Sticker_AWP_rotate3"
#define CVAR_MISC_STICKERWEAR_AWP4 "misc_Sticker_AWP_wear4"
#define CVAR_MISC_STICKERSLOT_AWP4 "misc_Sticker_AWP_slot4"
#define CVAR_MISC_STICKERSIZE_AWP4 "misc_Sticker_AWP_size4"
#define CVAR_MISC_STICKERID_AWP4 "misc_Sticker_AWP_ID4"
#define CVAR_MISC_STICKERROTATE_AWP4 "misc_Sticker_AWP_rotate4"
#define CVAR_MISC_STICKERWEAR_FAMAS1 "misc_Sticker_FAMAS_wear1"
#define CVAR_MISC_STICKERSLOT_FAMAS1 "misc_Sticker_FAMAS_slot1"
#define CVAR_MISC_STICKERSIZE_FAMAS1 "misc_Sticker_FAMAS_size1"
#define CVAR_MISC_STICKERID_FAMAS1 "misc_Sticker_FAMAS_ID1"
#define CVAR_MISC_STICKERROTATE_FAMAS1 "misc_Sticker_FAMAS_rotate1"
#define CVAR_MISC_STICKERWEAR_FAMAS2 "misc_Sticker_FAMAS_wear2"
#define CVAR_MISC_STICKERSLOT_FAMAS2 "misc_Sticker_FAMAS_slot2"
#define CVAR_MISC_STICKERSIZE_FAMAS2 "misc_Sticker_FAMAS_size2"
#define CVAR_MISC_STICKERID_FAMAS2 "misc_Sticker_FAMAS_ID2"
#define CVAR_MISC_STICKERROTATE_FAMAS2 "misc_Sticker_FAMAS_rotate2"
#define CVAR_MISC_STICKERWEAR_FAMAS3 "misc_Sticker_FAMAS_wear3"
#define CVAR_MISC_STICKERSLOT_FAMAS3 "misc_Sticker_FAMAS_slot3"
#define CVAR_MISC_STICKERSIZE_FAMAS3 "misc_Sticker_FAMAS_size3"
#define CVAR_MISC_STICKERID_FAMAS3 "misc_Sticker_FAMAS_ID3"
#define CVAR_MISC_STICKERROTATE_FAMAS3 "misc_Sticker_FAMAS_rotate3"
#define CVAR_MISC_STICKERWEAR_FAMAS4 "misc_Sticker_FAMAS_wear4"
#define CVAR_MISC_STICKERSLOT_FAMAS4 "misc_Sticker_FAMAS_slot4"
#define CVAR_MISC_STICKERSIZE_FAMAS4 "misc_Sticker_FAMAS_size4"
#define CVAR_MISC_STICKERID_FAMAS4 "misc_Sticker_FAMAS_ID4"
#define CVAR_MISC_STICKERROTATE_FAMAS4 "misc_Sticker_FAMAS_rotate4"
#define CVAR_MISC_STICKERWEAR_G35G11 "misc_Sticker_G35G1_wear1"
#define CVAR_MISC_STICKERSLOT_G35G11 "misc_Sticker_G35G1_slot1"
#define CVAR_MISC_STICKERSIZE_G35G11 "misc_Sticker_G35G1_size1"
#define CVAR_MISC_STICKERID_G35G11 "misc_Sticker_G35G1_ID1"
#define CVAR_MISC_STICKERROTATE_G35G11 "misc_Sticker_G35G1_rotate1"
#define CVAR_MISC_STICKERWEAR_G35G12 "misc_Sticker_G35G1_wear2"
#define CVAR_MISC_STICKERSLOT_G35G12 "misc_Sticker_G35G1_slot2"
#define CVAR_MISC_STICKERSIZE_G35G12 "misc_Sticker_G35G1_size2"
#define CVAR_MISC_STICKERID_G35G12 "misc_Sticker_G35G1_ID2"
#define CVAR_MISC_STICKERROTATE_G35G12 "misc_Sticker_G35G1_rotate2"
#define CVAR_MISC_STICKERWEAR_G35G13 "misc_Sticker_G35G1_wear3"
#define CVAR_MISC_STICKERSLOT_G35G13 "misc_Sticker_G35G1_slot3"
#define CVAR_MISC_STICKERSIZE_G35G13 "misc_Sticker_G35G1_size3"
#define CVAR_MISC_STICKERID_G35G13 "misc_Sticker_G35G1_ID3"
#define CVAR_MISC_STICKERROTATE_G35G13 "misc_Sticker_G35G1_rotate3"
#define CVAR_MISC_STICKERWEAR_G35G14 "misc_Sticker_G35G1_wear4"
#define CVAR_MISC_STICKERSLOT_G35G14 "misc_Sticker_G35G1_slot4"
#define CVAR_MISC_STICKERSIZE_G35G14 "misc_Sticker_G35G1_size4"
#define CVAR_MISC_STICKERID_G35G14 "misc_Sticker_G35G1_ID4"
#define CVAR_MISC_STICKERROTATE_G35G14 "misc_Sticker_G35G1_rotate4"
#define CVAR_MISC_STICKERWEAR_GALIL1 "misc_Sticker_GALIL_wear1"
#define CVAR_MISC_STICKERSLOT_GALIL1 "misc_Sticker_GALIL_slot1"
#define CVAR_MISC_STICKERSIZE_GALIL1 "misc_Sticker_GALIL_size1"
#define CVAR_MISC_STICKERID_GALIL1 "misc_Sticker_GALIL_ID1"
#define CVAR_MISC_STICKERROTATE_GALIL1 "misc_Sticker_GALIL_rotate1"
#define CVAR_MISC_STICKERWEAR_GALIL2 "misc_Sticker_GALIL_wear2"
#define CVAR_MISC_STICKERSLOT_GALIL2 "misc_Sticker_GALIL_slot2"
#define CVAR_MISC_STICKERSIZE_GALIL2 "misc_Sticker_GALIL_size2"
#define CVAR_MISC_STICKERID_GALIL2 "misc_Sticker_GALIL_ID2"
#define CVAR_MISC_STICKERROTATE_GALIL2 "misc_Sticker_GALIL_rotate2"
#define CVAR_MISC_STICKERWEAR_GALIL3 "misc_Sticker_GALIL_wear3"
#define CVAR_MISC_STICKERSLOT_GALIL3 "misc_Sticker_GALIL_slot3"
#define CVAR_MISC_STICKERSIZE_GALIL3 "misc_Sticker_GALIL_size3"
#define CVAR_MISC_STICKERID_GALIL3 "misc_Sticker_GALIL_ID3"
#define CVAR_MISC_STICKERROTATE_GALIL3 "misc_Sticker_GALIL_rotate3"
#define CVAR_MISC_STICKERWEAR_GALIL4 "misc_Sticker_GALIL_wear4"
#define CVAR_MISC_STICKERSLOT_GALIL4 "misc_Sticker_GALIL_slot4"
#define CVAR_MISC_STICKERSIZE_GALIL4 "misc_Sticker_GALIL_size4"
#define CVAR_MISC_STICKERID_GALIL4 "misc_Sticker_GALIL_ID4"
#define CVAR_MISC_STICKERROTATE_GALIL4 "misc_Sticker_GALIL_rotate4"
#define CVAR_MISC_STICKERWEAR_M2491 "misc_Sticker_M249_wear1"
#define CVAR_MISC_STICKERSLOT_M2491 "misc_Sticker_M249_slot1"
#define CVAR_MISC_STICKERSIZE_M2491 "misc_Sticker_M249_size1"
#define CVAR_MISC_STICKERID_M2491 "misc_Sticker_M249_ID1"
#define CVAR_MISC_STICKERROTATE_M2491 "misc_Sticker_M249_rotate1"
#define CVAR_MISC_STICKERWEAR_M2492 "misc_Sticker_M249_wear2"
#define CVAR_MISC_STICKERSLOT_M2492 "misc_Sticker_M249_slot2"
#define CVAR_MISC_STICKERSIZE_M2492 "misc_Sticker_M249_size2"
#define CVAR_MISC_STICKERID_M2492 "misc_Sticker_M249_ID2"
#define CVAR_MISC_STICKERROTATE_M2492 "misc_Sticker_M249_rotate2"
#define CVAR_MISC_STICKERWEAR_M2493 "misc_Sticker_M249_wear3"
#define CVAR_MISC_STICKERSLOT_M2493 "misc_Sticker_M249_slot3"
#define CVAR_MISC_STICKERSIZE_M2493 "misc_Sticker_M249_size3"
#define CVAR_MISC_STICKERID_M2493 "misc_Sticker_M249_ID3"
#define CVAR_MISC_STICKERROTATE_M2493 "misc_Sticker_M249_rotate3"
#define CVAR_MISC_STICKERWEAR_M2494 "misc_Sticker_M249_wear4"
#define CVAR_MISC_STICKERSLOT_M2494 "misc_Sticker_M249_slot4"
#define CVAR_MISC_STICKERSIZE_M2494 "misc_Sticker_M249_size4"
#define CVAR_MISC_STICKERID_M2494 "misc_Sticker_M249_ID4"
#define CVAR_MISC_STICKERROTATE_M2494 "misc_Sticker_M249_rotate4"
#define CVAR_MISC_STICKERWEAR_M4A41 "misc_Sticker_M4A4_wear1"
#define CVAR_MISC_STICKERSLOT_M4A41 "misc_Sticker_M4A4_slot1"
#define CVAR_MISC_STICKERSIZE_M4A41 "misc_Sticker_M4A4_size1"
#define CVAR_MISC_STICKERID_M4A41 "misc_Sticker_M4A4_ID1"
#define CVAR_MISC_STICKERROTATE_M4A41 "misc_Sticker_M4A4_rotate1"
#define CVAR_MISC_STICKERWEAR_M4A42 "misc_Sticker_M4A4_wear2"
#define CVAR_MISC_STICKERSLOT_M4A42 "misc_Sticker_M4A4_slot2"
#define CVAR_MISC_STICKERSIZE_M4A42 "misc_Sticker_M4A4_size2"
#define CVAR_MISC_STICKERID_M4A42 "misc_Sticker_M4A4_ID2"
#define CVAR_MISC_STICKERROTATE_M4A42 "misc_Sticker_M4A4_rotate2"
#define CVAR_MISC_STICKERWEAR_M4A43 "misc_Sticker_M4A4_wear3"
#define CVAR_MISC_STICKERSLOT_M4A43 "misc_Sticker_M4A4_slot3"
#define CVAR_MISC_STICKERSIZE_M4A43 "misc_Sticker_M4A4_size3"
#define CVAR_MISC_STICKERID_M4A43 "misc_Sticker_M4A4_ID3"
#define CVAR_MISC_STICKERROTATE_M4A43 "misc_Sticker_M4A4_rotate3"
#define CVAR_MISC_STICKERWEAR_M4A44 "misc_Sticker_M4A4_wear4"
#define CVAR_MISC_STICKERSLOT_M4A44 "misc_Sticker_M4A4_slot4"
#define CVAR_MISC_STICKERSIZE_M4A44 "misc_Sticker_M4A4_size4"
#define CVAR_MISC_STICKERID_M4A44 "misc_Sticker_M4A4_ID4"
#define CVAR_MISC_STICKERROTATE_M4A44 "misc_Sticker_M4A4_rotate4"
#define CVAR_MISC_STICKERWEAR_MAC101 "misc_Sticker_MAC10_wear1"
#define CVAR_MISC_STICKERSLOT_MAC101 "misc_Sticker_MAC10_slot1"
#define CVAR_MISC_STICKERSIZE_MAC101 "misc_Sticker_MAC10_size1"
#define CVAR_MISC_STICKERID_MAC101 "misc_Sticker_MAC10_ID1"
#define CVAR_MISC_STICKERROTATE_MAC101 "misc_Sticker_MAC10_rotate1"
#define CVAR_MISC_STICKERWEAR_MAC102 "misc_Sticker_MAC10_wear2"
#define CVAR_MISC_STICKERSLOT_MAC102 "misc_Sticker_MAC10_slot2"
#define CVAR_MISC_STICKERSIZE_MAC102 "misc_Sticker_MAC10_size2"
#define CVAR_MISC_STICKERID_MAC102 "misc_Sticker_MAC10_ID2"
#define CVAR_MISC_STICKERROTATE_MAC102 "misc_Sticker_MAC10_rotate2"
#define CVAR_MISC_STICKERWEAR_MAC103 "misc_Sticker_MAC10_wear3"
#define CVAR_MISC_STICKERSLOT_MAC103 "misc_Sticker_MAC10_slot3"
#define CVAR_MISC_STICKERSIZE_MAC103 "misc_Sticker_MAC10_size3"
#define CVAR_MISC_STICKERID_MAC103 "misc_Sticker_MAC10_ID3"
#define CVAR_MISC_STICKERROTATE_MAC103 "misc_Sticker_MAC10_rotate3"
#define CVAR_MISC_STICKERWEAR_MAC104 "misc_Sticker_MAC10_wear4"
#define CVAR_MISC_STICKERSLOT_MAC104 "misc_Sticker_MAC10_slot4"
#define CVAR_MISC_STICKERSIZE_MAC104 "misc_Sticker_MAC10_size4"
#define CVAR_MISC_STICKERID_MAC104 "misc_Sticker_MAC10_ID4"
#define CVAR_MISC_STICKERROTATE_MAC104 "misc_Sticker_MAC10_rotate4"
#define CVAR_MISC_STICKERWEAR_P901 "misc_Sticker_P90_wear1"
#define CVAR_MISC_STICKERSLOT_P901 "misc_Sticker_P90_slot1"
#define CVAR_MISC_STICKERSIZE_P901 "misc_Sticker_P90_size1"
#define CVAR_MISC_STICKERID_P901 "misc_Sticker_P90_ID1"
#define CVAR_MISC_STICKERROTATE_P901 "misc_Sticker_P90_rotate1"
#define CVAR_MISC_STICKERWEAR_P902 "misc_Sticker_P90_wear2"
#define CVAR_MISC_STICKERSLOT_P902 "misc_Sticker_P90_slot2"
#define CVAR_MISC_STICKERSIZE_P902 "misc_Sticker_P90_size2"
#define CVAR_MISC_STICKERID_P902 "misc_Sticker_P90_ID2"
#define CVAR_MISC_STICKERROTATE_P902 "misc_Sticker_P90_rotate2"
#define CVAR_MISC_STICKERWEAR_P903 "misc_Sticker_P90_wear3"
#define CVAR_MISC_STICKERSLOT_P903 "misc_Sticker_P90_slot3"
#define CVAR_MISC_STICKERSIZE_P903 "misc_Sticker_P90_size3"
#define CVAR_MISC_STICKERID_P903 "misc_Sticker_P90_ID3"
#define CVAR_MISC_STICKERROTATE_P903 "misc_Sticker_P90_rotate3"
#define CVAR_MISC_STICKERWEAR_P904 "misc_Sticker_P90_wear4"
#define CVAR_MISC_STICKERSLOT_P904 "misc_Sticker_P90_slot4"
#define CVAR_MISC_STICKERSIZE_P904 "misc_Sticker_P90_size4"
#define CVAR_MISC_STICKERID_P904 "misc_Sticker_P90_ID4"
#define CVAR_MISC_STICKERROTATE_P904 "misc_Sticker_P90_rotate4"
#define CVAR_MISC_STICKERWEAR_UMP1 "misc_Sticker_UMP_wear1"
#define CVAR_MISC_STICKERSLOT_UMP1 "misc_Sticker_UMP_slot1"
#define CVAR_MISC_STICKERSIZE_UMP1 "misc_Sticker_UMP_size1"
#define CVAR_MISC_STICKERID_UMP1 "misc_Sticker_UMP_ID1"
#define CVAR_MISC_STICKERROTATE_UMP1 "misc_Sticker_UMP_rotate1"
#define CVAR_MISC_STICKERWEAR_UMP2 "misc_Sticker_UMP_wear2"
#define CVAR_MISC_STICKERSLOT_UMP2 "misc_Sticker_UMP_slot2"
#define CVAR_MISC_STICKERSIZE_UMP2 "misc_Sticker_UMP_size2"
#define CVAR_MISC_STICKERID_UMP2 "misc_Sticker_UMP_ID2"
#define CVAR_MISC_STICKERROTATE_UMP2 "misc_Sticker_UMP_rotate2"
#define CVAR_MISC_STICKERWEAR_UMP3 "misc_Sticker_UMP_wear3"
#define CVAR_MISC_STICKERSLOT_UMP3 "misc_Sticker_UMP_slot3"
#define CVAR_MISC_STICKERSIZE_UMP3 "misc_Sticker_UMP_size3"
#define CVAR_MISC_STICKERID_UMP3 "misc_Sticker_UMP_ID3"
#define CVAR_MISC_STICKERROTATE_UMP3 "misc_Sticker_UMP_rotate3"
#define CVAR_MISC_STICKERWEAR_UMP4 "misc_Sticker_UMP_wear4"
#define CVAR_MISC_STICKERSLOT_UMP4 "misc_Sticker_UMP_slot4"
#define CVAR_MISC_STICKERSIZE_UMP4 "misc_Sticker_UMP_size4"
#define CVAR_MISC_STICKERID_UMP4 "misc_Sticker_UMP_ID4"
#define CVAR_MISC_STICKERROTATE_UMP4 "misc_Sticker_UMP_rotate4"
#define CVAR_MISC_STICKERWEAR_XM1 "misc_Sticker_XM_wear1"
#define CVAR_MISC_STICKERSLOT_XM1 "misc_Sticker_XM_slot1"
#define CVAR_MISC_STICKERSIZE_XM1 "misc_Sticker_XM_size1"
#define CVAR_MISC_STICKERID_XM1 "misc_Sticker_XM_ID1"
#define CVAR_MISC_STICKERROTATE_XM1 "misc_Sticker_XM_rotate1"
#define CVAR_MISC_STICKERWEAR_XM2 "misc_Sticker_XM_wear2"
#define CVAR_MISC_STICKERSLOT_XM2 "misc_Sticker_XM_slot2"
#define CVAR_MISC_STICKERSIZE_XM2 "misc_Sticker_XM_size2"
#define CVAR_MISC_STICKERID_XM2 "misc_Sticker_XM_ID2"
#define CVAR_MISC_STICKERROTATE_XM2 "misc_Sticker_XM_rotate2"
#define CVAR_MISC_STICKERWEAR_XM3 "misc_Sticker_XM_wear3"
#define CVAR_MISC_STICKERSLOT_XM3 "misc_Sticker_XM_slot3"
#define CVAR_MISC_STICKERSIZE_XM3 "misc_Sticker_XM_size3"
#define CVAR_MISC_STICKERID_XM3 "misc_Sticker_XM_ID3"
#define CVAR_MISC_STICKERROTATE_XM3 "misc_Sticker_XM_rotate3"
#define CVAR_MISC_STICKERWEAR_XM4 "misc_Sticker_XM_wear4"
#define CVAR_MISC_STICKERSLOT_XM4 "misc_Sticker_XM_slot4"
#define CVAR_MISC_STICKERSIZE_XM4 "misc_Sticker_XM_size4"
#define CVAR_MISC_STICKERID_XM4 "misc_Sticker_XM_ID4"
#define CVAR_MISC_STICKERROTATE_XM4 "misc_Sticker_XM_rotate4"
#define CVAR_MISC_STICKERWEAR_BIZON1 "misc_Sticker_BIZON_wear1"
#define CVAR_MISC_STICKERSLOT_BIZON1 "misc_Sticker_BIZON_slot1"
#define CVAR_MISC_STICKERSIZE_BIZON1 "misc_Sticker_BIZON_size1"
#define CVAR_MISC_STICKERID_BIZON1 "misc_Sticker_BIZON_ID1"
#define CVAR_MISC_STICKERROTATE_BIZON1 "misc_Sticker_BIZON_rotate1"
#define CVAR_MISC_STICKERWEAR_BIZON2 "misc_Sticker_BIZON_wear2"
#define CVAR_MISC_STICKERSLOT_BIZON2 "misc_Sticker_BIZON_slot2"
#define CVAR_MISC_STICKERSIZE_BIZON2 "misc_Sticker_BIZON_size2"
#define CVAR_MISC_STICKERID_BIZON2 "misc_Sticker_BIZON_ID2"
#define CVAR_MISC_STICKERROTATE_BIZON2 "misc_Sticker_BIZON_rotate2"
#define CVAR_MISC_STICKERWEAR_BIZON3 "misc_Sticker_BIZON_wear3"
#define CVAR_MISC_STICKERSLOT_BIZON3 "misc_Sticker_BIZON_slot3"
#define CVAR_MISC_STICKERSIZE_BIZON3 "misc_Sticker_BIZON_size3"
#define CVAR_MISC_STICKERID_BIZON3 "misc_Sticker_BIZON_ID3"
#define CVAR_MISC_STICKERROTATE_BIZON3 "misc_Sticker_BIZON_rotate3"
#define CVAR_MISC_STICKERWEAR_BIZON4 "misc_Sticker_BIZON_wear4"
#define CVAR_MISC_STICKERSLOT_BIZON4 "misc_Sticker_BIZON_slot4"
#define CVAR_MISC_STICKERSIZE_BIZON4 "misc_Sticker_BIZON_size4"
#define CVAR_MISC_STICKERID_BIZON4 "misc_Sticker_BIZON_ID4"
#define CVAR_MISC_STICKERROTATE_BIZON4 "misc_Sticker_BIZON_rotate4"
#define CVAR_MISC_STICKERWEAR_MAG71 "misc_Sticker_MAG7_wear1"
#define CVAR_MISC_STICKERSLOT_MAG71 "misc_Sticker_MAG7_slot1"
#define CVAR_MISC_STICKERSIZE_MAG71 "misc_Sticker_MAG7_size1"
#define CVAR_MISC_STICKERID_MAG71 "misc_Sticker_MAG7_ID1"
#define CVAR_MISC_STICKERROTATE_MAG71 "misc_Sticker_MAG7_rotate1"
#define CVAR_MISC_STICKERWEAR_MAG72 "misc_Sticker_MAG7_wear2"
#define CVAR_MISC_STICKERSLOT_MAG72 "misc_Sticker_MAG7_slot2"
#define CVAR_MISC_STICKERSIZE_MAG72 "misc_Sticker_MAG7_size2"
#define CVAR_MISC_STICKERID_MAG72 "misc_Sticker_MAG7_ID2"
#define CVAR_MISC_STICKERROTATE_MAG72 "misc_Sticker_MAG7_rotate2"
#define CVAR_MISC_STICKERWEAR_MAG73 "misc_Sticker_MAG7_wear3"
#define CVAR_MISC_STICKERSLOT_MAG73 "misc_Sticker_MAG7_slot3"
#define CVAR_MISC_STICKERSIZE_MAG73 "misc_Sticker_MAG7_size3"
#define CVAR_MISC_STICKERID_MAG73 "misc_Sticker_MAG7_ID3"
#define CVAR_MISC_STICKERROTATE_MAG73 "misc_Sticker_MAG7_rotate3"
#define CVAR_MISC_STICKERWEAR_MAG74 "misc_Sticker_MAG7_wear4"
#define CVAR_MISC_STICKERSLOT_MAG74 "misc_Sticker_MAG7_slot4"
#define CVAR_MISC_STICKERSIZE_MAG74 "misc_Sticker_MAG7_size4"
#define CVAR_MISC_STICKERID_MAG74 "misc_Sticker_MAG7_ID4"
#define CVAR_MISC_STICKERROTATE_MAG74 "misc_Sticker_MAG7_rotate4"
#define CVAR_MISC_STICKERWEAR_NEGEV1 "misc_Sticker_NEGEV_wear1"
#define CVAR_MISC_STICKERSLOT_NEGEV1 "misc_Sticker_NEGEV_slot1"
#define CVAR_MISC_STICKERSIZE_NEGEV1 "misc_Sticker_NEGEV_size1"
#define CVAR_MISC_STICKERID_NEGEV1 "misc_Sticker_NEGEV_ID1"
#define CVAR_MISC_STICKERROTATE_NEGEV1 "misc_Sticker_NEGEV_rotate1"
#define CVAR_MISC_STICKERWEAR_NEGEV2 "misc_Sticker_NEGEV_wear2"
#define CVAR_MISC_STICKERSLOT_NEGEV2 "misc_Sticker_NEGEV_slot2"
#define CVAR_MISC_STICKERSIZE_NEGEV2 "misc_Sticker_NEGEV_size2"
#define CVAR_MISC_STICKERID_NEGEV2 "misc_Sticker_NEGEV_ID2"
#define CVAR_MISC_STICKERROTATE_NEGEV2 "misc_Sticker_NEGEV_rotate2"
#define CVAR_MISC_STICKERWEAR_NEGEV3 "misc_Sticker_NEGEV_wear3"
#define CVAR_MISC_STICKERSLOT_NEGEV3 "misc_Sticker_NEGEV_slot3"
#define CVAR_MISC_STICKERSIZE_NEGEV3 "misc_Sticker_NEGEV_size3"
#define CVAR_MISC_STICKERID_NEGEV3 "misc_Sticker_NEGEV_ID3"
#define CVAR_MISC_STICKERROTATE_NEGEV3 "misc_Sticker_NEGEV_rotate3"
#define CVAR_MISC_STICKERWEAR_NEGEV4 "misc_Sticker_NEGEV_wear4"
#define CVAR_MISC_STICKERSLOT_NEGEV4 "misc_Sticker_NEGEV_slot4"
#define CVAR_MISC_STICKERSIZE_NEGEV4 "misc_Sticker_NEGEV_size4"
#define CVAR_MISC_STICKERID_NEGEV4 "misc_Sticker_NEGEV_ID4"
#define CVAR_MISC_STICKERROTATE_NEGEV4 "misc_Sticker_NEGEV_rotate4"
#define CVAR_MISC_STICKERWEAR_SAWEDOFF1 "misc_Sticker_SAWEDOFF_wear1"
#define CVAR_MISC_STICKERSLOT_SAWEDOFF1 "misc_Sticker_SAWEDOFF_slot1"
#define CVAR_MISC_STICKERSIZE_SAWEDOFF1 "misc_Sticker_SAWEDOFF_size1"
#define CVAR_MISC_STICKERID_SAWEDOFF1 "misc_Sticker_SAWEDOFF_ID1"
#define CVAR_MISC_STICKERROTATE_SAWEDOFF1 "misc_Sticker_SAWEDOFF_rotate1"
#define CVAR_MISC_STICKERWEAR_SAWEDOFF2 "misc_Sticker_SAWEDOFF_wear2"
#define CVAR_MISC_STICKERSLOT_SAWEDOFF2 "misc_Sticker_SAWEDOFF_slot2"
#define CVAR_MISC_STICKERSIZE_SAWEDOFF2 "misc_Sticker_SAWEDOFF_size2"
#define CVAR_MISC_STICKERID_SAWEDOFF2 "misc_Sticker_SAWEDOFF_ID2"
#define CVAR_MISC_STICKERROTATE_SAWEDOFF2 "misc_Sticker_SAWEDOFF_rotate2"
#define CVAR_MISC_STICKERWEAR_SAWEDOFF3 "misc_Sticker_SAWEDOFF_wear3"
#define CVAR_MISC_STICKERSLOT_SAWEDOFF3 "misc_Sticker_SAWEDOFF_slot3"
#define CVAR_MISC_STICKERSIZE_SAWEDOFF3 "misc_Sticker_SAWEDOFF_size3"
#define CVAR_MISC_STICKERID_SAWEDOFF3 "misc_Sticker_SAWEDOFF_ID3"
#define CVAR_MISC_STICKERROTATE_SAWEDOFF3 "misc_Sticker_SAWEDOFF_rotate3"
#define CVAR_MISC_STICKERWEAR_SAWEDOFF4 "misc_Sticker_SAWEDOFF_wear4"
#define CVAR_MISC_STICKERSLOT_SAWEDOFF4 "misc_Sticker_SAWEDOFF_slot4"
#define CVAR_MISC_STICKERSIZE_SAWEDOFF4 "misc_Sticker_SAWEDOFF_size4"
#define CVAR_MISC_STICKERID_SAWEDOFF4 "misc_Sticker_SAWEDOFF_ID4"
#define CVAR_MISC_STICKERROTATE_SAWEDOFF4 "misc_Sticker_SAWEDOFF_rotate4"
#define CVAR_MISC_STICKERWEAR_TEC1 "misc_Sticker_TEC_wear1"
#define CVAR_MISC_STICKERSLOT_TEC1 "misc_Sticker_TEC_slot1"
#define CVAR_MISC_STICKERSIZE_TEC1 "misc_Sticker_TEC_size1"
#define CVAR_MISC_STICKERID_TEC1 "misc_Sticker_TEC_ID1"
#define CVAR_MISC_STICKERROTATE_TEC1 "misc_Sticker_TEC_rotate1"
#define CVAR_MISC_STICKERWEAR_TEC2 "misc_Sticker_TEC_wear2"
#define CVAR_MISC_STICKERSLOT_TEC2 "misc_Sticker_TEC_slot2"
#define CVAR_MISC_STICKERSIZE_TEC2 "misc_Sticker_TEC_size2"
#define CVAR_MISC_STICKERID_TEC2 "misc_Sticker_TEC_ID2"
#define CVAR_MISC_STICKERROTATE_TEC2 "misc_Sticker_TEC_rotate2"
#define CVAR_MISC_STICKERWEAR_TEC3 "misc_Sticker_TEC_wear3"
#define CVAR_MISC_STICKERSLOT_TEC3 "misc_Sticker_TEC_slot3"
#define CVAR_MISC_STICKERSIZE_TEC3 "misc_Sticker_TEC_size3"
#define CVAR_MISC_STICKERID_TEC3 "misc_Sticker_TEC_ID3"
#define CVAR_MISC_STICKERROTATE_TEC3 "misc_Sticker_TEC_rotate3"
#define CVAR_MISC_STICKERWEAR_TEC4 "misc_Sticker_TEC_wear4"
#define CVAR_MISC_STICKERSLOT_TEC4 "misc_Sticker_TEC_slot4"
#define CVAR_MISC_STICKERSIZE_TEC4 "misc_Sticker_TEC_size4"
#define CVAR_MISC_STICKERID_TEC4 "misc_Sticker_TEC_ID4"
#define CVAR_MISC_STICKERROTATE_TEC4 "misc_Sticker_TEC_rotate4"
#define CVAR_MISC_STICKERWEAR_P20001 "misc_Sticker_P2000_wear1"
#define CVAR_MISC_STICKERSLOT_P20001 "misc_Sticker_P2000_slot1"
#define CVAR_MISC_STICKERSIZE_P20001 "misc_Sticker_P2000_size1"
#define CVAR_MISC_STICKERID_P20001 "misc_Sticker_P2000_ID1"
#define CVAR_MISC_STICKERROTATE_P20001 "misc_Sticker_P2000_rotate1"
#define CVAR_MISC_STICKERWEAR_P20002 "misc_Sticker_P2000_wear2"
#define CVAR_MISC_STICKERSLOT_P20002 "misc_Sticker_P2000_slot2"
#define CVAR_MISC_STICKERSIZE_P20002 "misc_Sticker_P2000_size2"
#define CVAR_MISC_STICKERID_P20002 "misc_Sticker_P2000_ID2"
#define CVAR_MISC_STICKERROTATE_P20002 "misc_Sticker_P2000_rotate2"
#define CVAR_MISC_STICKERWEAR_P20003 "misc_Sticker_P2000_wear3"
#define CVAR_MISC_STICKERSLOT_P20003 "misc_Sticker_P2000_slot3"
#define CVAR_MISC_STICKERSIZE_P20003 "misc_Sticker_P2000_size3"
#define CVAR_MISC_STICKERID_P20003 "misc_Sticker_P2000_ID3"
#define CVAR_MISC_STICKERROTATE_P20003 "misc_Sticker_P2000_rotate3"
#define CVAR_MISC_STICKERWEAR_P20004 "misc_Sticker_P2000_wear4"
#define CVAR_MISC_STICKERSLOT_P20004 "misc_Sticker_P2000_slot4"
#define CVAR_MISC_STICKERSIZE_P20004 "misc_Sticker_P2000_size4"
#define CVAR_MISC_STICKERID_P20004 "misc_Sticker_P2000_ID4"
#define CVAR_MISC_STICKERROTATE_P20004 "misc_Sticker_P2000_rotate4"
#define CVAR_MISC_STICKERWEAR_MP71 "misc_Sticker_MP7_wear1"
#define CVAR_MISC_STICKERSLOT_MP71 "misc_Sticker_MP7_slot1"
#define CVAR_MISC_STICKERSIZE_MP71 "misc_Sticker_MP7_size1"
#define CVAR_MISC_STICKERID_MP71 "misc_Sticker_MP7_ID1"
#define CVAR_MISC_STICKERROTATE_MP71 "misc_Sticker_MP7_rotate1"
#define CVAR_MISC_STICKERWEAR_MP72 "misc_Sticker_MP7_wear2"
#define CVAR_MISC_STICKERSLOT_MP72 "misc_Sticker_MP7_slot2"
#define CVAR_MISC_STICKERSIZE_MP72 "misc_Sticker_MP7_size2"
#define CVAR_MISC_STICKERID_MP72 "misc_Sticker_MP7_ID2"
#define CVAR_MISC_STICKERROTATE_MP72 "misc_Sticker_MP7_rotate2"
#define CVAR_MISC_STICKERWEAR_MP73 "misc_Sticker_MP7_wear3"
#define CVAR_MISC_STICKERSLOT_MP73 "misc_Sticker_MP7_slot3"
#define CVAR_MISC_STICKERSIZE_MP73 "misc_Sticker_MP7_size3"
#define CVAR_MISC_STICKERID_MP73 "misc_Sticker_MP7_ID3"
#define CVAR_MISC_STICKERROTATE_MP73 "misc_Sticker_MP7_rotate3"
#define CVAR_MISC_STICKERWEAR_MP74 "misc_Sticker_MP7_wear4"
#define CVAR_MISC_STICKERSLOT_MP74 "misc_Sticker_MP7_slot4"
#define CVAR_MISC_STICKERSIZE_MP74 "misc_Sticker_MP7_size4"
#define CVAR_MISC_STICKERID_MP74 "misc_Sticker_MP7_ID4"
#define CVAR_MISC_STICKERROTATE_MP74 "misc_Sticker_MP7_rotate4"
#define CVAR_MISC_STICKERWEAR_MP91 "misc_Sticker_MP9_wear1"
#define CVAR_MISC_STICKERSLOT_MP91 "misc_Sticker_MP9_slot1"
#define CVAR_MISC_STICKERSIZE_MP91 "misc_Sticker_MP9_size1"
#define CVAR_MISC_STICKERID_MP91 "misc_Sticker_MP9_ID1"
#define CVAR_MISC_STICKERROTATE_MP91 "misc_Sticker_MP9_rotate1"
#define CVAR_MISC_STICKERWEAR_MP92 "misc_Sticker_MP9_wear2"
#define CVAR_MISC_STICKERSLOT_MP92 "misc_Sticker_MP9_slot2"
#define CVAR_MISC_STICKERSIZE_MP92 "misc_Sticker_MP9_size2"
#define CVAR_MISC_STICKERID_MP92 "misc_Sticker_MP9_ID2"
#define CVAR_MISC_STICKERROTATE_MP92 "misc_Sticker_MP9_rotate2"
#define CVAR_MISC_STICKERWEAR_MP93 "misc_Sticker_MP9_wear3"
#define CVAR_MISC_STICKERSLOT_MP93 "misc_Sticker_MP9_slot3"
#define CVAR_MISC_STICKERSIZE_MP93 "misc_Sticker_MP9_size3"
#define CVAR_MISC_STICKERID_MP93 "misc_Sticker_MP9_ID3"
#define CVAR_MISC_STICKERROTATE_MP93 "misc_Sticker_MP9_rotate3"
#define CVAR_MISC_STICKERWEAR_MP94 "misc_Sticker_MP9_wear4"
#define CVAR_MISC_STICKERSLOT_MP94 "misc_Sticker_MP9_slot4"
#define CVAR_MISC_STICKERSIZE_MP94 "misc_Sticker_MP9_size4"
#define CVAR_MISC_STICKERID_MP94 "misc_Sticker_MP9_ID4"
#define CVAR_MISC_STICKERROTATE_MP94 "misc_Sticker_MP9_rotate4"
#define CVAR_MISC_STICKERWEAR_NOVA1 "misc_Sticker_NOVA_wear1"
#define CVAR_MISC_STICKERSLOT_NOVA1 "misc_Sticker_NOVA_slot1"
#define CVAR_MISC_STICKERSIZE_NOVA1 "misc_Sticker_NOVA_size1"
#define CVAR_MISC_STICKERID_NOVA1 "misc_Sticker_NOVA_ID1"
#define CVAR_MISC_STICKERROTATE_NOVA1 "misc_Sticker_NOVA_rotate1"
#define CVAR_MISC_STICKERWEAR_NOVA2 "misc_Sticker_NOVA_wear2"
#define CVAR_MISC_STICKERSLOT_NOVA2 "misc_Sticker_NOVA_slot2"
#define CVAR_MISC_STICKERSIZE_NOVA2 "misc_Sticker_NOVA_size2"
#define CVAR_MISC_STICKERID_NOVA2 "misc_Sticker_NOVA_ID2"
#define CVAR_MISC_STICKERROTATE_NOVA2 "misc_Sticker_NOVA_rotate2"
#define CVAR_MISC_STICKERWEAR_NOVA3 "misc_Sticker_NOVA_wear3"
#define CVAR_MISC_STICKERSLOT_NOVA3 "misc_Sticker_NOVA_slot3"
#define CVAR_MISC_STICKERSIZE_NOVA3 "misc_Sticker_NOVA_size3"
#define CVAR_MISC_STICKERID_NOVA3 "misc_Sticker_NOVA_ID3"
#define CVAR_MISC_STICKERROTATE_NOVA3 "misc_Sticker_NOVA_rotate3"
#define CVAR_MISC_STICKERWEAR_NOVA4 "misc_Sticker_NOVA_wear4"
#define CVAR_MISC_STICKERSLOT_NOVA4 "misc_Sticker_NOVA_slot4"
#define CVAR_MISC_STICKERSIZE_NOVA4 "misc_Sticker_NOVA_size4"
#define CVAR_MISC_STICKERID_NOVA4 "misc_Sticker_NOVA_ID4"
#define CVAR_MISC_STICKERROTATE_NOVA4 "misc_Sticker_NOVA_rotate4"
#define CVAR_MISC_STICKERWEAR_P2501 "misc_Sticker_P250_wear1"
#define CVAR_MISC_STICKERSLOT_P2501 "misc_Sticker_P250_slot1"
#define CVAR_MISC_STICKERSIZE_P2501 "misc_Sticker_P250_size1"
#define CVAR_MISC_STICKERID_P2501 "misc_Sticker_P250_ID1"
#define CVAR_MISC_STICKERROTATE_P2501 "misc_Sticker_P250_rotate1"
#define CVAR_MISC_STICKERWEAR_P2502 "misc_Sticker_P250_wear2"
#define CVAR_MISC_STICKERSLOT_P2502 "misc_Sticker_P250_slot2"
#define CVAR_MISC_STICKERSIZE_P2502 "misc_Sticker_P250_size2"
#define CVAR_MISC_STICKERID_P2502 "misc_Sticker_P250_ID2"
#define CVAR_MISC_STICKERROTATE_P2502 "misc_Sticker_P250_rotate2"
#define CVAR_MISC_STICKERWEAR_P2503 "misc_Sticker_P250_wear3"
#define CVAR_MISC_STICKERSLOT_P2503 "misc_Sticker_P250_slot3"
#define CVAR_MISC_STICKERSIZE_P2503 "misc_Sticker_P250_size3"
#define CVAR_MISC_STICKERID_P2503 "misc_Sticker_P250_ID3"
#define CVAR_MISC_STICKERROTATE_P2503 "misc_Sticker_P250_rotate3"
#define CVAR_MISC_STICKERWEAR_P2504 "misc_Sticker_P250_wear4"
#define CVAR_MISC_STICKERSLOT_P2504 "misc_Sticker_P250_slot4"
#define CVAR_MISC_STICKERSIZE_P2504 "misc_Sticker_P250_size4"
#define CVAR_MISC_STICKERID_P2504 "misc_Sticker_P250_ID4"
#define CVAR_MISC_STICKERROTATE_P2504 "misc_Sticker_P250_rotate4"
#define CVAR_MISC_STICKERWEAR_SCAR1 "misc_Sticker_SCAR_wear1"
#define CVAR_MISC_STICKERSLOT_SCAR1 "misc_Sticker_SCAR_slot1"
#define CVAR_MISC_STICKERSIZE_SCAR1 "misc_Sticker_SCAR_size1"
#define CVAR_MISC_STICKERID_SCAR1 "misc_Sticker_SCAR_ID1"
#define CVAR_MISC_STICKERROTATE_SCAR1 "misc_Sticker_SCAR_rotate1"
#define CVAR_MISC_STICKERWEAR_SCAR2 "misc_Sticker_SCAR_wear2"
#define CVAR_MISC_STICKERSLOT_SCAR2 "misc_Sticker_SCAR_slot2"
#define CVAR_MISC_STICKERSIZE_SCAR2 "misc_Sticker_SCAR_size2"
#define CVAR_MISC_STICKERID_SCAR2 "misc_Sticker_SCAR_ID2"
#define CVAR_MISC_STICKERROTATE_SCAR2 "misc_Sticker_SCAR_rotate2"
#define CVAR_MISC_STICKERWEAR_SCAR3 "misc_Sticker_SCAR_wear3"
#define CVAR_MISC_STICKERSLOT_SCAR3 "misc_Sticker_SCAR_slot3"
#define CVAR_MISC_STICKERSIZE_SCAR3 "misc_Sticker_SCAR_size3"
#define CVAR_MISC_STICKERID_SCAR3 "misc_Sticker_SCAR_ID3"
#define CVAR_MISC_STICKERROTATE_SCAR3 "misc_Sticker_SCAR_rotate3"
#define CVAR_MISC_STICKERWEAR_SCAR4 "misc_Sticker_SCAR_wear4"
#define CVAR_MISC_STICKERSLOT_SCAR4 "misc_Sticker_SCAR_slot4"
#define CVAR_MISC_STICKERSIZE_SCAR4 "misc_Sticker_SCAR_size4"
#define CVAR_MISC_STICKERID_SCAR4 "misc_Sticker_SCAR_ID4"
#define CVAR_MISC_STICKERROTATE_SCAR4 "misc_Sticker_SCAR_rotate4"
#define CVAR_MISC_STICKERWEAR_SG1 "misc_Sticker_SG_wear1"
#define CVAR_MISC_STICKERSLOT_SG1 "misc_Sticker_SG_slot1"
#define CVAR_MISC_STICKERSIZE_SG1 "misc_Sticker_SG_size1"
#define CVAR_MISC_STICKERID_SG1 "misc_Sticker_SG_ID1"
#define CVAR_MISC_STICKERROTATE_SG1 "misc_Sticker_SG_rotate1"
#define CVAR_MISC_STICKERWEAR_SG2 "misc_Sticker_SG_wear2"
#define CVAR_MISC_STICKERSLOT_SG2 "misc_Sticker_SG_slot2"
#define CVAR_MISC_STICKERSIZE_SG2 "misc_Sticker_SG_size2"
#define CVAR_MISC_STICKERID_SG2 "misc_Sticker_SG_ID2"
#define CVAR_MISC_STICKERROTATE_SG2 "misc_Sticker_SG_rotate2"
#define CVAR_MISC_STICKERWEAR_SG3 "misc_Sticker_SG_wear3"
#define CVAR_MISC_STICKERSLOT_SG3 "misc_Sticker_SG_slot3"
#define CVAR_MISC_STICKERSIZE_SG3 "misc_Sticker_SG_size3"
#define CVAR_MISC_STICKERID_SG3 "misc_Sticker_SG_ID3"
#define CVAR_MISC_STICKERROTATE_SG3 "misc_Sticker_SG_rotate3"
#define CVAR_MISC_STICKERWEAR_SG4 "misc_Sticker_SG_wear4"
#define CVAR_MISC_STICKERSLOT_SG4 "misc_Sticker_SG_slot4"
#define CVAR_MISC_STICKERSIZE_SG4 "misc_Sticker_SG_size4"
#define CVAR_MISC_STICKERID_SG4 "misc_Sticker_SG_ID4"
#define CVAR_MISC_STICKERROTATE_SG4 "misc_Sticker_SG_rotate4"
#define CVAR_MISC_STICKERWEAR_SSG1 "misc_Sticker_SSG_wear1"
#define CVAR_MISC_STICKERSLOT_SSG1 "misc_Sticker_SSG_slot1"
#define CVAR_MISC_STICKERSIZE_SSG1 "misc_Sticker_SSG_size1"
#define CVAR_MISC_STICKERID_SSG1 "misc_Sticker_SSG_ID1"
#define CVAR_MISC_STICKERROTATE_SSG1 "misc_Sticker_SSG_rotate1"
#define CVAR_MISC_STICKERWEAR_SSG2 "misc_Sticker_SSG_wear2"
#define CVAR_MISC_STICKERSLOT_SSG2 "misc_Sticker_SSG_slot2"
#define CVAR_MISC_STICKERSIZE_SSG2 "misc_Sticker_SSG_size2"
#define CVAR_MISC_STICKERID_SSG2 "misc_Sticker_SSG_ID2"
#define CVAR_MISC_STICKERROTATE_SSG2 "misc_Sticker_SSG_rotate2"
#define CVAR_MISC_STICKERWEAR_SSG3 "misc_Sticker_SSG_wear3"
#define CVAR_MISC_STICKERSLOT_SSG3 "misc_Sticker_SSG_slot3"
#define CVAR_MISC_STICKERSIZE_SSG3 "misc_Sticker_SSG_size3"
#define CVAR_MISC_STICKERID_SSG3 "misc_Sticker_SSG_ID3"
#define CVAR_MISC_STICKERROTATE_SSG3 "misc_Sticker_SSG_rotate3"
#define CVAR_MISC_STICKERWEAR_SSG4 "misc_Sticker_SSG_wear4"
#define CVAR_MISC_STICKERSLOT_SSG4 "misc_Sticker_SSG_slot4"
#define CVAR_MISC_STICKERSIZE_SSG4 "misc_Sticker_SSG_size4"
#define CVAR_MISC_STICKERID_SSG4 "misc_Sticker_SSG_ID4"
#define CVAR_MISC_STICKERROTATE_SSG4 "misc_Sticker_SSG_rotate4"

#define CVAR_MISC_STICKERWEAPON_DEAGLE "misc_StickerWeaponID_DEAGLE"
#define CVAR_MISC_STICKERWEAPON_ELITES "misc_StickerWeaponID_ELITES"
#define CVAR_MISC_STICKERWEAPON_FIVESEVEN "misc_StickerWeaponID_FIVESEVEN"
#define CVAR_MISC_STICKERWEAPON_GLOCK "misc_StickerWeaponID_GLOCK"
#define CVAR_MISC_STICKERWEAPON_AK "misc_StickerWeaponID_AK"
#define CVAR_MISC_STICKERWEAPON_AUG "misc_StickerWeaponID_AUG"
#define CVAR_MISC_STICKERWEAPON_AWP "misc_StickerWeaponID_AWP"
#define CVAR_MISC_STICKERWEAPON_FAMAS "misc_StickerWeaponID_FAMAS"
#define CVAR_MISC_STICKERWEAPON_G35G1 "misc_StickerWeaponID_G35G1"
#define CVAR_MISC_STICKERWEAPON_GALIL "misc_StickerWeaponID_GALIL"
#define CVAR_MISC_STICKERWEAPON_M249 "misc_StickerWeaponID_M249"
#define CVAR_MISC_STICKERWEAPON_M4A4 "misc_StickerWeaponID_M4A4"
#define CVAR_MISC_STICKERWEAPON_MAC10 "misc_StickerWeaponID_MAC10"
#define CVAR_MISC_STICKERWEAPON_P90 "misc_StickerWeaponID_P90"
#define CVAR_MISC_STICKERWEAPON_UMP "misc_StickerWeaponID_UMP"
#define CVAR_MISC_STICKERWEAPON_XM "misc_StickerWeaponID_XM"
#define CVAR_MISC_STICKERWEAPON_BIZON "misc_StickerWeaponID_BIZON"
#define CVAR_MISC_STICKERWEAPON_MAG7 "misc_StickerWeaponID_MAG7"
#define CVAR_MISC_STICKERWEAPON_NEGEV "misc_StickerWeaponID_NEGEV"
#define CVAR_MISC_STICKERWEAPON_SAWEDOFF "misc_StickerWeaponID_SAWEDOFF"
#define CVAR_MISC_STICKERWEAPON_TEC "misc_StickerWeaponID_TEC"
#define CVAR_MISC_STICKERWEAPON_P2000 "misc_StickerWeaponID_P2000"
#define CVAR_MISC_STICKERWEAPON_MP7 "misc_StickerWeaponID_MP7"
#define CVAR_MISC_STICKERWEAPON_MP9 "misc_StickerWeaponID_MP9"
#define CVAR_MISC_STICKERWEAPON_NOVA "misc_StickerWeaponID_NOVA"
#define CVAR_MISC_STICKERWEAPON_P250 "misc_StickerWeaponID_P250"
#define CVAR_MISC_STICKERWEAPON_SCAR "misc_StickerWeaponID_SCAR"
#define CVAR_MISC_STICKERWEAPON_SG "misc_StickerWeaponID_SG"
#define CVAR_MISC_STICKERWEAPON_SSG "misc_StickerWeaponID_SSG"
#define STICKER_TEXT "Sticker"

*/