#include "Hook.h"
#include "intrin.h"

#define HasGrenade g_pPlayers->GetLocal()->WeaponIndex == WEAPON_SMOKEGRENADE | g_pPlayers->GetLocal()->WeaponIndex == WEAPON_HEGRENADE | g_pPlayers->GetLocal()->WeaponIndex == WEAPON_INCGRENADE | g_pPlayers->GetLocal()->WeaponIndex == WEAPON_MOLOTOV | g_pPlayers->GetLocal()->WeaponIndex == WEAPON_DECOY | g_pPlayers->GetLocal()->WeaponIndex == WEAPON_FLASHBANG

using namespace Client;

//[enc_string_enable /]
//[junk_enable /]

namespace Engine
{
	namespace Hook
	{
		CSX::Hook::VTable IDirect3DDevice9Table;
		CSX::Hook::VTable SoundTable;
		CSX::Hook::VTable ClientModeTable;
		CSX::Hook::VTable GameEventTable;
		CSX::Hook::VTable ModelRenderTable;
		CSX::Hook::VTable ClientTable;
		CSX::Hook::VTable SurfaceTable;
		CSX::Hook::VTable EngineTable;
		CSX::Hook::VTable SteamGameCoordinatorTable;

		IDirect3DDevice9* g_pDevice = nullptr;

		typedef HRESULT(WINAPI* Present_t)(IDirect3DDevice9* pDevice, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
		Present_t Present_o;

		typedef HRESULT(WINAPI* Reset_t)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
		Reset_t Reset_o;

		HRESULT WINAPI Hook_Present(IDirect3DDevice9* pDevice, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
		{
			Client::OnRender();

			return Present_o(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		}

		HRESULT WINAPI Hook_Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
		{
			Client::OnLostDevice();

			HRESULT hRes = Reset_o(pDevice, pPresentationParameters);

			Client::OnResetDevice();

			return hRes;
		}


		void VectorAngles(Vector forward, QAngle & ang_out)
		{
			if (forward.x == 0.0f && forward.y == 0.0f)
			{
				ang_out.x = (forward.z <= 0.0f) ? 90.0f : 270.f;
				ang_out.y = 0.0f;
			}
			else
			{
				ang_out.x = atan2(-forward.z, forward.Length2D()) * -180 / 3.14;
				ang_out.y = atan2(forward.y, forward.x) * 180 / 3.14;
			}

			ang_out.z = 0.0f;
		}

		void FixMovement(CUserCmd * pCommand, Vector ang)
		{
			float flSpeed = pCommand->Move.Length2D();

			if (flSpeed <= 0.0f)
				return;

			Vector vec_move;

			VectorAngles(pCommand->Move, vec_move);

			float flYaw = (pCommand->viewangles.y - ang.y + vec_move.y) *  0.0174532925f;

			pCommand->Move.x = cos(flYaw) * flSpeed;
			pCommand->Move.y = sin(flYaw) * flSpeed;
		}

		void NormalizeFloat(float & in)
		{
			if (in > 180.f || in < -180.f)
			{
				float ry = in / 360.f;

				if (ry < 0.f)
					ry = -ry;

				ry = round(ry);

				if (in < 0.f)
					in = (in + 360.f * ry);
				else
					in = (in - 360.f * ry);
			}
		}

		bool WINAPI Hook_CreateMove(float flInputSampleTime, CUserCmd* pCmd)
		{
			static bool isFakeAngle = false;
			static bool dir = false;
			static bool dir2 = false;

			ClientModeTable.UnHook();
			auto origAng = pCmd->viewangles;

			PDWORD pEBP;
			__asm mov pEBP, ebp;

			bool& bSendPacket = *(bool*)(*pEBP - 0x1C);

			CBaseEntity* pLocal = (CBaseEntity*)Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer());

			if (Interfaces::Engine()->IsConnected() || Interfaces::Engine()->IsInGame())
			{




				if (bSendPacket)
					Settings::Misc::qLastTickAngle = pCmd->viewangles;



				//pLocal->GetActiveWeapon();
				//pLocal->GetWeapons()
			}

			Client::OnCreateMove(pCmd);

			bool ret = Interfaces::ClientMode()->CreateMove(flInputSampleTime, pCmd);
			ClientModeTable.ReHook();
			FixMovement(pCmd, origAng);
			NormalizeFloat(pCmd->viewangles[1]);

			printf_s("%d\n", bSendPacket);
		}











		bool WINAPI Hook_IsConnected()
		{

			static void* unk = CSX::Memory::NewPatternScan(GetModuleHandleA("client.dll"), "75 04 B0 01 5F") - 2;
			if (_ReturnAddress() == unk && Settings::Misc::invalwayson)
			{
				return false;
			}
			EngineTable.UnHook();
			bool ret = Interfaces::Engine()->IsConnected();
			EngineTable.ReHook();
			return ret;
		}

		bool WINAPI Hook_OverrideView(CViewSetup* pSetup)
		{
			Client::OnOverrideView(pSetup);

			ClientModeTable.UnHook();
			bool ret = Interfaces::ClientMode()->OverrideView(pSetup);
			ClientModeTable.ReHook();
			return ret;
		}

		float WINAPI Hook_GetViewModelFOV()
		{
			ClientModeTable.UnHook();
			float fov = Interfaces::ClientMode()->GetViewModelFOV();
			ClientModeTable.ReHook();

			Client::OnGetViewModelFOV(fov);

			return fov;
		}

		EGCResults __fastcall Hook_RetrieveMessage(void* ecx, void* edx, uint32_t *punMsgType, void *pubDest, uint32_t cubDest, uint32_t *pcubMsgSize)
		{
			SteamGameCoordinatorTable.UnHook();
			EGCResults status = Interfaces::SteamGameCoordinator()->RetrieveMessage(punMsgType, pubDest, cubDest, pcubMsgSize);
			SteamGameCoordinatorTable.ReHook();

			if (status != k_EGCResultOK)
				return status;

			Client::OnRetrieveMessage(ecx, edx, punMsgType, pubDest, cubDest, pcubMsgSize);

			return status;
		}

		EGCResults __fastcall Hook_SendMessage(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData)
		{
			uint32_t messageType = unMsgType & 0x7FFFFFFF;
			void* pubDataMutable = const_cast<void*>(pubData);


			Client::OnSendMessage(ecx, edx, unMsgType, pubData, cubData);

			SteamGameCoordinatorTable.UnHook();
			EGCResults status = Interfaces::SteamGameCoordinator()->SendMessage(unMsgType, pubData, cubData);
			SteamGameCoordinatorTable.ReHook();

			return status;

		}


		bool WINAPI Hook_FireEventClientSideThink(IGameEvent* pEvent)
		{
			bool ret = false;

			if (!pEvent)
			{
				GameEventTable.UnHook();
				ret = Interfaces::GameEvent()->FireEventClientSide(pEvent);
				GameEventTable.ReHook();
				return ret;
			}

			Client::OnFireEventClientSideThink(pEvent);

			GameEventTable.UnHook();
			ret = Interfaces::GameEvent()->FireEventClientSide(pEvent);
			GameEventTable.ReHook();

			return ret;
		}

		void WINAPI Hook_FrameStageNotify(ClientFrameStage_t Stage)
		{
			Client::OnFrameStageNotify(Stage);

			ClientTable.UnHook();
			Interfaces::Client()->FrameStageNotify(Stage);
			ClientTable.ReHook();
		}

		int WINAPI Hook_EmitSound1(IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample,
			float flVolume, soundlevel_t iSoundlevel, int nSeed, int iFlags = 0, int iPitch = PITCH_NORM,
			const Vector *pOrigin = NULL, const Vector *pDirection = NULL, CUtlVector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1)
		{

			if (pSample)
				Client::OnPlaySound(pOrigin, pSample);

			SoundTable.UnHook();



			int ret = Interfaces::Sound()->EmitSound1(filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample,
				flVolume, iSoundlevel, nSeed, iFlags, iPitch,
				pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity);

			SoundTable.ReHook();

			return ret;
		}

		int WINAPI Hook_EmitSound2(IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample,
			float flVolume, float flAttenuation, int nSeed, int iFlags = 0, int iPitch = PITCH_NORM,
			const Vector *pOrigin = NULL, const Vector *pDirection = NULL, CUtlVector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1)
		{
			if (pSample)
				Client::OnPlaySound(pOrigin, pSample);

			SoundTable.UnHook();

			int ret = Interfaces::Sound()->EmitSound2(filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample,
				flVolume, flAttenuation, nSeed, iFlags, iPitch,
				pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity);

			SoundTable.ReHook();

			return ret;
		}

		void WINAPI Hook_DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t &state,
			const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld = NULL)
		{
			ModelRenderTable.UnHook();

			if (ctx && pCustomBoneToWorld)
				Client::OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);

			Interfaces::ModelRender()->DrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);

			ModelRenderTable.ReHook();
		}

		void WINAPI Hook_PlaySound(const char* pszSoundName)
		{
			SurfaceTable.UnHook();

			if (pszSoundName)
				Client::OnPlaySound(pszSoundName);

			Interfaces::Surface()->PlaySound(pszSoundName);

			SurfaceTable.ReHook();
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

		bool Initialize()
		{
			if (!CSX::Utils::IsModuleLoad(D3D9_DLL))
				return false;

			if (!CSX::Utils::IsModuleLoad(SHADERPIDX9_DLL))
				return false;

			if (!CSX::Utils::IsModuleLoad(GAMEOVERLAYRENDERER_DLL))
				return false;

			DWORD d3d9TablePtrPtr = CSX::Memory::FindPattern(SHADERPIDX9_DLL, D3D9_PATTERN, D3D9_MASK, 1);
			DWORD_PTR** dwPresent_o = (DWORD_PTR**)CSX::Memory::FindPattern(GAMEOVERLAYRENDERER_DLL, GMOR_PATTERN, GMOR_MASK, 1);

			if (d3d9TablePtrPtr && dwPresent_o)
			{
				g_pDevice = (IDirect3DDevice9*)(**(PDWORD*)d3d9TablePtrPtr);

				if (IDirect3DDevice9Table.InitTable(g_pDevice))
				{
					DWORD_PTR* dwAddress = *dwPresent_o;
					Present_o = (Present_t)(*dwAddress);
					*dwAddress = (DWORD_PTR)(&Hook_Present);

					Reset_o = (Reset_t)IDirect3DDevice9Table.GetHookIndex(D3D9::TABLE::Reset, Hook_Reset);

					if (!ClientModeTable.InitTable(Interfaces::ClientMode()))
						return false;

					ClientModeTable.HookIndex(TABLE::IClientMode::CreateMove, Hook_CreateMove);
					ClientModeTable.HookIndex(TABLE::IClientMode::OverrideView, Hook_OverrideView);
					ClientModeTable.HookIndex(TABLE::IClientMode::GetViewModelFOV, Hook_GetViewModelFOV);

					if (!EngineTable.InitTable(Interfaces::Engine()))
						return false;

					EngineTable.HookIndex(27, Hook_IsConnected);

					if (!SteamGameCoordinatorTable.InitTable(Interfaces::SteamGameCoordinator()))
						return false;

					SteamGameCoordinatorTable.HookIndex(0, Hook_SendMessage);
					SteamGameCoordinatorTable.HookIndex(2, Hook_RetrieveMessage);

					if (!GameEventTable.InitTable(Interfaces::GameEvent()))
						return false;

					GameEventTable.HookIndex(TABLE::IGameEventManager2::FireEventClientSide, Hook_FireEventClientSideThink);

					if (!ClientTable.InitTable(Interfaces::Client()))
						return false;

					ClientTable.HookIndex(TABLE::IBaseClientDLL::FrameStageNotify, Hook_FrameStageNotify);

					if (!SoundTable.InitTable(Interfaces::Sound()))
						return false;

					SoundTable.HookIndex(TABLE::IEngineSound::EmitSound1, Hook_EmitSound1);
					SoundTable.HookIndex(TABLE::IEngineSound::EmitSound2, Hook_EmitSound2);

					if (!ModelRenderTable.InitTable(Interfaces::ModelRender()))
						return false;

					ModelRenderTable.HookIndex(TABLE::IVModelRender::DrawModelExecute, Hook_DrawModelExecute);

					if (!SurfaceTable.InitTable(Interfaces::Surface()))
						return false;

					SurfaceTable.HookIndex(TABLE::ISurface::PlaySound, Hook_PlaySound);

					if (Client::Initialize(g_pDevice))
						SendClientHello();
					return true;
				}
			}

			return false;
		}

		void Shutdown()
		{
			IDirect3DDevice9Table.UnHook();
			SoundTable.UnHook();
			ClientModeTable.UnHook();
			GameEventTable.UnHook();
			ModelRenderTable.UnHook();
			ClientTable.UnHook();
			SteamGameCoordinatorTable.UnHook();
		}
	}
}