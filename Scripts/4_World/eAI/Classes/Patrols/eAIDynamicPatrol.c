class eAIDynamicPatrol : eAIPatrol
{
	private static int NUMBER_DYNAMIC_PATROLS;

	vector m_Position;
	autoptr array<vector> m_Waypoints;
	eAIWaypointBehavior m_WaypointBehaviour;
	float m_MinimumRadius;
	float m_MaximumRadius;
	float m_DespawnRadius; // m_MaximumRadius + 10%
	int m_NumberOfAI;
	int m_RespawnTime;
	string m_Loadout;
	ref eAIFaction m_Faction;

	eAIGroup m_Group;
	float m_LastSpawn;
	bool m_CanSpawn;

	/**
	 * @brief Creates a dynamic patrol which spawns a patrol under the right conditions.
	 * 
	 * @param pos the position that the trigger distance is calculated from
	 * @param waypoints an array of points which the patrol will traverse
	 * @param behaviour how the waypoints will be traversed
	 * @param loadout the loadout each member is given @todo change to AI "type" which may have a different FSM/Goal tree
	 * @param count the number of ai to be spawned in the patrol
	 * @param faction the faction the patrol will be spawned to
	 * @param respawnTime the time between patrol spawns
	 * @param minR miminum distance between the patrol and nearest player for a patrol to not (re)spawn
	 * @param maxR maximum distance between the patrol and nearest player for a patrol to (re)spawn
	 * 
	 * @return the patrol instance
	 */
	static eAIDynamicPatrol Create(vector pos, array<vector> waypoints, eAIWaypointBehavior behaviour, string loadout = "SoldierLoadout.json", int count = 1, int respawnTime = 600, eAIFaction faction = null, bool autoStart = true, float minR = 300, float maxR = 800)
	{
		#ifdef EAI_TRACE
		auto trace = CF_Trace_0("eAIDynamicPatrol", "Create");
		#endif

		eAIDynamicPatrol patrol;
		Class.CastTo(patrol, ((typename)eAIDynamicPatrol).Spawn());
		patrol.m_Position = pos;
		patrol.m_Waypoints = waypoints;
		patrol.m_WaypointBehaviour = behaviour;
		patrol.m_NumberOfAI = count;
		patrol.m_Loadout = loadout;
		patrol.m_RespawnTime = respawnTime;
		patrol.m_MinimumRadius = minR;
		patrol.m_MaximumRadius = maxR;
		patrol.m_DespawnRadius = maxR * 1.1;
		patrol.m_Faction = faction;
		patrol.m_CanSpawn = true;
		if (patrol.m_Faction == null) patrol.m_Faction = new eAIFactionCivilian();
		if (autoStart) patrol.Start();
		return patrol;
	}

	private eAIBase SpawnAI(vector pos)
	{
		#ifdef EAI_TRACE
		auto trace = CF_Trace_0(this, "SpawnAI");
		#endif

		eAIBase ai;
		if (!Class.CastTo(ai, GetGame().CreateObject(GetRandomAI(), pos))) return null;

		ai.SetPosition(pos);

		HumanLoadout.Apply(ai, m_Loadout);
				
		return ai;
	}

	private bool IsGroupDestroyed()
	{
		#ifdef EAI_TRACE
		auto trace = CF_Trace_0(this, "IsGroupDestroyed");
		#endif

		for (int i = 0; i < m_Group.Count(); i++)
		{
			DayZPlayerImplement member = m_Group.GetMember(i);
			if (member && member.IsInherited(PlayerBase) && member.IsAlive())
			{
				return false;
			}
		}

		return true;
	}

	void Spawn()
	{
		#ifdef EAI_TRACE
		auto trace = CF_Trace_0(this, "Spawn");
		#endif

		if (m_Group) return;

		m_LastSpawn = 0;
		m_CanSpawn = false;

		eAIBase ai = SpawnAI(m_Position);
		m_Group = ai.GetGroup();
		m_Group.SetFaction(m_Faction);
		m_Group.SetWaypointBehaviour(m_WaypointBehaviour);
		foreach (vector v : m_Waypoints) m_Group.AddWaypoint(v);

		int count = m_NumberOfAI - 1;
		while (count != 0)
		{
			ai = SpawnAI(m_Position);
			ai.SetGroup(m_Group);
			count--;
		}

		NUMBER_DYNAMIC_PATROLS++;
	}

	void Despawn()
	{
		#ifdef EAI_TRACE
		auto trace = CF_Trace_0(this, "Despawn");
		#endif

		if (!m_Group) return;

		m_Group.RemoveAllMembers();
		m_Group = null;
		m_LastSpawn = 0;
		m_CanSpawn = false;

		NUMBER_DYNAMIC_PATROLS = Math.Max(NUMBER_DYNAMIC_PATROLS - 1, 0);
	}

	override void OnUpdate()
	{
		#ifdef EAI_TRACE
		auto trace = CF_Trace_0(this, "OnUpdate");
		#endif

		super.OnUpdate();

		vector patrolPos = m_Position;
		DayZPlayerImplement leader = null;
		if (m_Group && m_Group.GetLeader())
		{
			leader = m_Group.GetLeader();
			patrolPos = leader.GetPosition();
		}
		
		autoptr array<Man> players = {};
		GetGame().GetPlayers(players);
		float minimumDistance = 50000.0;
		foreach (auto player : players)
		{
			float dist = vector.Distance(patrolPos, player.GetPosition());
			if (dist < minimumDistance && leader != player) minimumDistance = dist;
		}

		if (m_Group)
		{
			if (IsGroupDestroyed() || minimumDistance > m_DespawnRadius)
			{
				Despawn();
			}
		}
		else
		{
			if (m_CanSpawn)
			{
				if (minimumDistance < m_MaximumRadius && minimumDistance > m_MinimumRadius)
				{
					Spawn();
				}
			}
			else
			{
				m_LastSpawn += eAIPatrol.UPDATE_RATE_IN_SECONDS;

				bool timeCheck = m_LastSpawn > m_RespawnTime;
				bool countCheck = eAISettings.GetMaximumDynamicPatrols() == -1 || NUMBER_DYNAMIC_PATROLS < eAISettings.GetMaximumDynamicPatrols();

				if (timeCheck && countCheck)
				{
					m_LastSpawn = 0;
					m_CanSpawn = true;
				}
			}
		}
	}
};
