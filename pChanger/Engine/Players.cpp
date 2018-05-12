#pragma once

#include "Players.h"

namespace Engine
{
	//[junk_enable /]
	CPlayers::CPlayers()
	{
		m_pMe = new CMe();
		m_pPlayers = new CPlayer[MAX_PLAYERS_SIZE];
	}

	CPlayers::~CPlayers()
	{
		delete m_pMe;
		delete m_pPlayers;

		m_pMe = nullptr;
		m_pPlayers = nullptr;
	}

	CMe* CPlayers::GetLocal()
	{
		if (m_pMe)
			return m_pMe;

		return nullptr;
	}

	CPlayer* CPlayers::GetPlayer(BYTE Index)
	{
		if (m_pPlayers)
		{
			if (Index >= 0 && Index < MAX_PLAYERS_SIZE)
				return &m_pPlayers[Index];

			return &m_pPlayers[0];
		}

		return nullptr;
	}

	BYTE CPlayers::GetSize()
	{
		return MAX_PLAYERS_SIZE;
	}

	bool CPlayers::UpdateLocal()
	{
		int LocalIndex = Interfaces::Engine()->GetLocalPlayer();

		if (!LocalIndex || !m_pMe)
			return false;

		m_pMe->m_pEntity = (CBaseEntity*)Interfaces::EntityList()->GetClientEntity(LocalIndex);

		if (!m_pMe->m_pEntity)
			return false;

		m_pMe->bAlive = m_pMe->m_pEntity->IsValid();

		m_pMe->iIndex = LocalIndex;
		m_pMe->iFov = 90;

		m_pMe->iFlags = m_pMe->m_pEntity->GetFlags();
		m_pMe->iShotsFired = m_pMe->m_pEntity->GetShotsFired();

		m_pMe->vAimPunch = m_pMe->m_pEntity->GetAimPunchAngle();
		m_pMe->vPunch = m_pMe->m_pEntity->GetViewPunchAngle();

		m_pMe->vEyeOrigin = m_pMe->m_pEntity->GetRenderOrigin() + m_pMe->m_pEntity->GetViewOffset();

		CBaseWeapon* pWeaponEntity = m_pMe->m_pEntity->GetBaseWeapon();

		if (pWeaponEntity && pWeaponEntity->GetName())
		{
			string WeaponStr = pWeaponEntity->GetName();
			WeaponStr = WeaponStr.erase(0, 7);

			m_pMe->WeaponName = WeaponStr;
			m_pMe->WeaponType = GetWeaponType(*pWeaponEntity->GeteAttributableItem()->GetItemDefinitionIndex());
			m_pMe->WeaponIndex = *pWeaponEntity->GeteAttributableItem()->GetItemDefinitionIndex();
			m_pMe->WeaponAmmo = pWeaponEntity->GetWeaponAmmo();
			m_pMe->bInReload = pWeaponEntity->GetWeaponReload();

			if (m_pMe->WeaponAmmo < 0)
				m_pMe->WeaponAmmo = 0;

			m_pMe->m_pWeaponEntity = pWeaponEntity;

			if (m_pMe->WeaponType == WEAPON_TYPE_SNIPER)
			{
				if (pWeaponEntity->GetZoomLevel() == 1)
				{
					m_pMe->iFov = 40;
				}
				else if (pWeaponEntity->GetZoomLevel() == 2)
				{
					m_pMe->iFov = 15;
				}
			}
		}
		else
		{
			m_pMe->WeaponName = "";
			m_pMe->WeaponType = WEAPON_TYPE_UNKNOWN;
			m_pMe->WeaponIndex = 0;
			m_pMe->WeaponAmmo = 0;

			m_pMe->m_pWeaponEntity = nullptr;
		}

		m_pMe->Team = (PLAYER_TEAM)m_pMe->m_pEntity->GetTeam();

		return true;
	}

	void CPlayers::Update()
	{
		if (!UpdateLocal())
		{
			m_pMe->m_pEntity = nullptr;
			m_pMe->m_pWeaponEntity = nullptr;
			return;
		}

		for (BYTE EntIndex = 0; EntIndex < GetSize(); EntIndex++)
		{
			CBaseEntity* pEntity = (CBaseEntity*)Interfaces::EntityList()->GetClientEntity(EntIndex);

			if (!pEntity || !pEntity->IsPlayer() || !pEntity->IsValid() || pEntity == m_pMe->m_pEntity
				|| EntIndex == m_pMe->iIndex || pEntity->GetClientClass()->m_ClassID != (int)CLIENT_CLASS_ID::CCSPlayer)
			{
				m_pPlayers[EntIndex].bUpdate = false;
				m_pPlayers[EntIndex].bTriggerFov = false;
				continue;
			}

			Vector vHead = pEntity->GetHitboxPosition(HITBOX_HEAD);
			Vector vOrigin = pEntity->GetRenderOrigin();

			if (vHead.IsZero() || vOrigin.IsZero())
			{
				m_pPlayers[EntIndex].bTriggerFov = false;
				continue;
			}

			Vector vTriggerHead, vTriggerHeadPos, vTriggerChest, vTriggerChestPos;

			vTriggerHead = pEntity->GetHitboxPosition(HITBOX_HEAD);
			vTriggerChest = pEntity->GetHitboxPosition(HITBOX_BODY);

			if (WorldToScreen(vHead, m_pPlayers[EntIndex].vHitboxHeadScreen) &&
				WorldToScreen(vOrigin, m_pPlayers[EntIndex].vOriginScreen))
			{
				m_pPlayers[EntIndex].vHitboxHead = vHead;
				m_pPlayers[EntIndex].vOrigin = vOrigin;
			}
			else
			{
				m_pPlayers[EntIndex].vHitboxHeadScreen = Vector(0, 0, 0);
				m_pPlayers[EntIndex].vOriginScreen = Vector(0, 0, 0);
			}


			if (WorldToScreen(vTriggerHead, vTriggerHeadPos))
			{
				m_pPlayers[EntIndex].vTriggerHeadPos = vTriggerHeadPos;
			}
			else
			{
				m_pPlayers[EntIndex].vTriggerHeadPos = Vector(0, 0, 0);
			}

			if (WorldToScreen(vTriggerChest, vTriggerChestPos))
			{
				m_pPlayers[EntIndex].vTriggerChestPos = vTriggerChestPos;
			}
			else
			{
				m_pPlayers[EntIndex].vTriggerChestPos = Vector(0, 0, 0);
			}

			if (m_pPlayers[EntIndex].vTriggerHeadPos.x > 0.f)
			{
				m_pPlayers[EntIndex].fDistanceHead = DistanceScreen(Client::g_vCenterScreen, Vector2D(m_pPlayers[EntIndex].vTriggerHeadPos.x, m_pPlayers[EntIndex].vTriggerHeadPos.y));
			}
			else
			{
				m_pPlayers[EntIndex].fDistanceHead = 0.f;
			}
			if (m_pPlayers[EntIndex].vTriggerChestPos.x > 0.f)
			{
				m_pPlayers[EntIndex].fDistanceChest = DistanceScreen(Client::g_vCenterScreen, Vector2D(m_pPlayers[EntIndex].vTriggerChestPos.x, m_pPlayers[EntIndex].vTriggerChestPos.y));
			}
			else
			{
				m_pPlayers[EntIndex].fDistanceChest = 0.f;
			}

			m_pPlayers[EntIndex].Name = pEntity->GetPlayerName();

			CBaseWeapon* pWeaponEntity = pEntity->GetBaseWeapon();

			if (pWeaponEntity && pWeaponEntity->GetName())
			{
				string WeaponStr = pWeaponEntity->GetName();
				WeaponStr = WeaponStr.erase(0, 7);

				m_pPlayers[EntIndex].WeaponName = WeaponStr;

				if (pWeaponEntity->GetWeaponAmmo() > 0)
				{
					m_pPlayers[EntIndex].iWAmmo = pWeaponEntity->GetWeaponAmmo();
				}
				else
				{
					m_pPlayers[EntIndex].iWAmmo = 0;
				}
			}
			else
			{
				m_pPlayers[EntIndex].WeaponName = "";
				m_pPlayers[EntIndex].iWAmmo = 0;
			}

			m_pPlayers[EntIndex].Team = (PLAYER_TEAM)pEntity->GetTeam();

			m_pPlayers[EntIndex].iHealth = pEntity->GetHealth();
			m_pPlayers[EntIndex].iArmor = pEntity->GetArmor();
			m_pPlayers[EntIndex].iDistance = int(m_pMe->m_pEntity->GetRenderOrigin().DistTo(pEntity->GetRenderOrigin()));

			m_pPlayers[EntIndex].bUpdate = true;
			m_pPlayers[EntIndex].bVisible = pEntity->IsVisible(m_pMe->m_pEntity);
			m_pPlayers[EntIndex].m_pEntity = pEntity;
		}
	}

	void CPlayers::Clear()
	{
		memset(m_pMe, 0, sizeof(CMe));
		memset(m_pPlayers, 0, sizeof(CPlayer) * MAX_PLAYERS_SIZE);
	}

	void CPlayers::GetHitBoxSkeleton(int nHitBoxOne, int nHitBoxTwo, CBaseEntity* pEntity, Vector* vOut)
	{
		vOut[0] = pEntity->GetHitboxPosition(nHitBoxOne);
		vOut[1] = pEntity->GetHitboxPosition(nHitBoxTwo);
	}
}




































































































































































































































































































































































































































































































































































































































