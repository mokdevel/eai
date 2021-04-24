enum eAIWaypointBehavior {
	HALT,
	LOOP,
	REVERSE
}

class eAIWaypointTargetInformation extends eAITargetInformation
{
    private autoptr array<vector> waypoints = {};
	private int waypoint = -1;
	
	private eAIWaypointBehavior loopingBehavior = eAIWaypointBehavior.REVERSE;
	private bool backtrack = false;
	
	private eAIGroup m_Target; // here the target is actually the group we are in
	
	private float m_RadiusSq = 4.0;


    void eAIWaypointTargetInformation(eAIGroup target)
    {
        m_Target = target;
    }

    override EntityAI GetEntity()
    {
        return m_Target.GetLeader();
    }

    override vector GetPosition(eAIBase ai = null)
    {
        if (waypoints.Count() > 0 && waypoint > -1)
			return waypoints[waypoint];

        return GetEntity().GetPosition();
    }
	
	int AddWaypoint(vector pos) {
		waypoints.Insert(pos);
		if (waypoint < 0) waypoint++;
		return waypoint;
	}
	
	void ClearWaypoints() {
		waypoints.Clear();
		waypoint = -1;
	}
	
	int SkipWaypoint() {
		if (waypoint > -1) {
			if (backtrack) waypoint--;
			else waypoint++;
			if (waypoint == waypoints.Count()) {
				// If we have gone through all the waypoints forwards
				if (loopingBehavior == eAIWaypointBehavior.LOOP) {
					waypoint = 0;
				} else if (loopingBehavior == eAIWaypointBehavior.REVERSE) {
					waypoint -= 2;
					backtrack = true;
				} else waypoint = -1;
			} else if (waypoint == -1) {
				// If we have gone through all the waypoints backwards
				if (loopingBehavior == eAIWaypointBehavior.REVERSE) {
					waypoint = 1;
					backtrack = false;
				} else waypoint = -1;
			}
		}
		return waypoint;
	}
	
	void SetLooping(eAIWaypointBehavior loop) {
		loopingBehavior = loop;
	}

    void Update(float pDt)
    {
		if (!GetEntity()) return;
		
		vector newPos = GetEntity().GetPosition();

		//Print("Distance: " + vector.DistanceSq(newPos, GetPosition()));
		float distToWP = vector.DistanceSq(newPos, GetPosition()); // This can be zero if there is no current WP
		if (distToWP < m_RadiusSq && waypoint > -1)
			Print("eAI " + m_Target + " Going to WP " + SkipWaypoint());
		
    }
};