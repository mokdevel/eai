// Copyright 2021 William Bowers
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

modded class Weapon_Base
{
	bool Hitscan(out EntityAI entity, out vector hitPosition, out int contactComponent)
	{
		eAIBase ai;
		if (!Class.CastTo(ai, GetHierarchyRootPlayer())) return false;
		
		vector position;
		vector direction;
		
		if (!ai.GetAimingProfile().Get(position, direction))
		{
			Print("Data invalid.");

			hitPosition = position;

			return false;
		}

		vector begin_point = position;
		vector end_point = position + (direction * 500.0);
				
		// Prep Raycast
		set<Object> results = new set<Object>();
		vector hitNormal;
		bool hit = DayZPhysics.RaycastRV(begin_point, end_point, hitPosition, hitNormal, contactComponent, results, null, ai, false, false, ObjIntersectIFire, 0.01);
		
		if (hit && results.Count() > 0)
		{
			if (Class.CastTo(entity, results[0])) return true;
		}

		hitPosition = begin_point;

		return false;
	}

	override void EEFired(int muzzleType, int mode, string ammoType)
	{
		super.EEFired(muzzleType, mode, ammoType);

		eAIBase ai;
		if (GetGame().IsClient() || !Class.CastTo(ai, GetHierarchyRootPlayer())) return;

		EntityAI entity;
		vector hitPosition;
		int contactComponent;
		bool hit = Hitscan(entity, hitPosition, contactComponent);

		if (hit)
		{
			string componentName = entity.GetActionComponentName(contactComponent, "fire");
			string damageZone = "";
			DamageSystem.GetDamageZoneFromComponentName(entity, componentName, damageZone);

			entity.ProcessDirectDamage(DT_FIRE_ARM, this, damageZone, ammoType, entity.WorldToModel(hitPosition), 1.0);
		}
	}
	
	/**@fn	ProcessWeaponEvent
	 * @brief	weapon's fsm handling of events
	 * @NOTE: warning: ProcessWeaponEvent can be called only within DayZPlayer::HandleWeapons (or ::CommandHandler)
	 **/
	override bool ProcessWeaponEvent(WeaponEventBase e)
	{
		eAIBase ai;
		if (Class.CastTo(ai, e.m_player))
		{
			ScriptRemoteInputUserData ctx = new ScriptRemoteInputUserData;
			ctx.Write(INPUT_UDT_WEAPON_REMOTE_EVENT);
			e.WriteToContext(ctx);
			ai.StoreInputForRemotes(ctx);

			// @NOTE: synchronous events not handled by fsm
			if (e.GetEventID() == WeaponEventID.SET_NEXT_MUZZLE_MODE)
			{
				SetNextMuzzleMode(GetCurrentMuzzle());
				return true;
			}

			if (m_fsm.ProcessEvent(e) == ProcessEventResult.FSM_OK)
				return true;
				
			return false;
		}

		return super.ProcessWeaponEvent(e);
	}

	override bool ProcessWeaponAbortEvent(WeaponEventBase e)
	{
		if (PlayerBase.Cast(e.m_player).IsAI())
		{
			if (GetGame().IsServer() && m_fsm.ProcessEvent(e) == ProcessEventResult.FSM_OK)
				return true;

			return false;
		}

		return super.ProcessWeaponAbortEvent(e);
	}
};