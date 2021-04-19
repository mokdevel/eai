typedef Param6<int, bool, int, int, vector, vector> eAIAimingProfile_SyncParams;

class eAIAimingProfile
{
	Man m_Arbiter;

	EntityAI m_Hands;
	eAIBase m_Player;

	int m_LastUpdated;

	// model space position and direction, re-calculate worldspace on action
	vector m_Position;
	vector m_Direction;

	void eAIAimingProfile(eAIBase ai)
	{
		m_Player = ai;
	}

	void Update()
	{
		m_Hands = m_Player.GetHumanInventory().GetEntityInHands();
		m_LastUpdated = GetGame().GetTime();

		Weapon_Base weapon;
		if (Class.CastTo(weapon, m_Hands))
		{
			vector barrel_start = weapon.GetSelectionPositionLS("konec hlavne");
			vector barrel_end = weapon.GetSelectionPositionLS("usti hlavne");
			barrel_start = weapon.ModelToWorld(barrel_start);
			barrel_end = weapon.ModelToWorld(barrel_end);

			vector transform[4];
			m_Player.GetTransform(transform);

			barrel_start = barrel_start.InvMultiply4(transform);
			barrel_end = barrel_end.InvMultiply4(transform);

			m_Position = barrel_start;
			m_Direction = vector.Direction(barrel_start, barrel_end).Normalized();

			return;
		}

		//TODO: calculate raw direction and position from head bone
	}

	bool Get(out vector position, out vector direction)
	{
		bool verify = GetGame().IsServer() && GetGame().IsMultiplayer();

		if (!verify) Update();

		//if (verify && GetGame().GetTime() - m_LastUpdated > 1000.0) return false;

		if (verify && m_Player.GetHumanInventory().GetEntityInHands() != m_Hands) return false;

		vector transform[4];
		m_Player.GetTransform(transform);

		position = m_Position.Multiply4(transform);
		direction = m_Direction.Multiply3(transform);

		return true;
	}

	void UpdateArbiter(Man arbiter)
	{
		if (m_Arbiter != arbiter)
		{
			if (m_Arbiter)
			{
				SendRPC("OnEnd", null, false);
			}

			m_Arbiter = arbiter;

			if (m_Arbiter)
			{
				SendRPC("OnStart", null, false);
			}
		}
	}

	private void SendRPC(string function, Param param, bool g)
	{
		GetRPCManager().SendRPC("eAIAimingProfileManager", function, param, g, m_Arbiter.GetIdentity(), m_Player);	
	}

	void SyncToServer()
	{
		if (GetGame().IsServer()) return;

		int low, high;
		bool hasHands = m_Hands != null;
		if (hasHands)
		{
			m_Hands.GetNetworkID(low, high);
		}

		GetRPCManager().SendRPC("eAIAimingProfileManager", "OnSync", new eAIAimingProfile_SyncParams(m_LastUpdated, hasHands, low, high, m_Position, m_Direction), false, null, m_Player);
	}

	void Serialize(ParamsWriteContext ctx)
	{
		ctx.Write(m_LastUpdated);

		ctx.Write(m_Hands != null);
		if (m_Hands)
		{
			int low, high;
			m_Hands.GetNetworkID(low, high);
			ctx.Write(low);
			ctx.Write(high);
		}

		ctx.Write(m_Position);
		ctx.Write(m_Direction);
	}

	void Deserialize(ParamsReadContext ctx)
	{
		ctx.Read(m_LastUpdated);

		bool hasHands;
		ctx.Read(hasHands);
		if (hasHands)
		{
			int low, high;
			ctx.Read(low);
			ctx.Read(high);
			
			Class.CastTo(m_Hands, GetGame().GetObjectByNetworkId(low, high));
		}

		ctx.Read(m_Position);
		ctx.Read(m_Direction);
	}
};