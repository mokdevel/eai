class eAICommandMove extends eAICommandBase
{
	static const int TURN_STATE_NONE = 0;
	static const int TURN_STATE_TURNING = 1;
	
	private int m_PreviousInteractionLayer;
		
	private float m_TurnTarget;
	private float m_TurnDifference;
	private float m_TurnTime;
	private float m_ReevaluateTurnTime;
	private float m_TurnDifferenceStart;
	private int m_TurnState;

	private vector m_Direction;
	private float m_MovementDirection;
	private float m_TargetMovementDirection;

	private bool m_Look;
	private float m_LookLR;
	private float m_LookUD;
	
	private bool m_Raised;
	private bool m_Fighting;
	
	private float m_MovementSpeed;
	private float m_TargetSpeed;
	private float m_SpeedLimit;
			
	void eAICommandMove(eAIBase unit, eAIAnimationST st)
	{
	}
	
	void ~eAICommandMove()
	{
	}

	override void OnActivate()
	{
		SetSpeedLimit(-1);
		
		dBodySetInteractionLayer(m_Unit, PhxInteractionLayers.CHARACTER | PhxInteractionLayers.BUILDING | PhxInteractionLayers.DOOR | PhxInteractionLayers.VEHICLE | PhxInteractionLayers.ITEM_LARGE | PhxInteractionLayers.FENCE | PhxInteractionLayers.AI);
	}

	override void OnDeactivate()
	{
	}

	override void SetLookDirection(vector direction)
	{
		vector angles = direction.VectorToAngles();
		m_LookLR = angles[0];
		m_LookUD = angles[1];
		if (m_LookLR > 180) m_LookLR = m_LookLR - 360;
		if (m_LookUD > 180) m_LookUD = m_LookUD - 360;
		m_Look = (Math.AbsFloat(m_LookLR) > 0.01) || (Math.AbsFloat(m_LookUD) > 0.01);
	}

	void SetSpeedLimit(float speedIdx)
	{
		m_SpeedLimit = speedIdx;

		if (m_SpeedLimit < 0 || m_SpeedLimit > 3) m_SpeedLimit = 3;
	}
	
	void SetTargetSpeed(float target)
	{
		m_TargetSpeed = target;
	}

	void SetTargetDirection(float target)
	{
		m_TargetMovementDirection = target;
	}
	
	void SetRaised(bool raised)
	{
		m_Raised = raised;
	}
	
	void SetFighting(bool fighting)
	{
		m_Fighting = fighting;
	}
	
	float ShortestAngle(float a, float b)
	{
		if (a - b > 0) return a - b;
		return b - a;
		
		int aa = a;
		int bb = b;
		
		int phi = Math.AbsInt(aa - bb) % 360; 
		if (phi > 180) phi = 360 - phi;
		
		if ((a - b >= 0 && a - b <= 180) || (a - b <=-180 && a- b>= -360))
		{
			return phi;
		}
		
		return -phi;
	}

	override void PreAnimUpdate(float pDt)
	{		
		m_MovementDirection += Math.Clamp((m_TargetMovementDirection - m_MovementDirection) * pDt, -180.0, 180.0);

		m_MovementSpeed = m_TargetSpeed;
		if (m_MovementSpeed > m_SpeedLimit && m_SpeedLimit != -1) m_MovementSpeed = m_SpeedLimit;	

		m_Table.SetMovementDirection(this, m_MovementDirection);
		m_Table.SetMovementSpeed(this, m_MovementSpeed);

		m_Table.SetRaised(this, m_Raised);

		m_Table.SetLook(this, m_Look);
		m_Table.SetLookDirX(this, m_LookLR);
		m_Table.SetLookDirY(this, m_LookUD);

		if (m_MovementSpeed == 0)
		{
			switch (m_TurnState)
			{
				case TURN_STATE_NONE:
					if (Math.AbsFloat(m_TurnDifference) > 5.0)
					{
						m_TurnTime = 0;
						m_TurnDifferenceStart = m_TurnDifference;

						m_ReevaluateTurnTime = 0.5 * (m_TurnDifferenceStart / 90.0);

						m_Table.CallTurn(this);

						m_TurnState = TURN_STATE_TURNING;
					}
					break;
				case TURN_STATE_TURNING:
					m_TurnTime += pDt;

					if (Math.AbsFloat(m_TurnDifferenceStart - m_TurnDifference) < 5.0)
					{
						m_Table.CallStopTurn(this);
						
						m_TurnState = TURN_STATE_NONE;
					} else if (m_TurnTime > m_ReevaluateTurnTime)
					{
						m_Table.CallStopTurn(this);

						m_TurnState = TURN_STATE_NONE;
					}

					break;
			}

			m_Table.SetTurnAmount(this, m_TurnDifference / 90.0);

		} else m_TurnState = TURN_STATE_NONE;
	}

	override void PrePhysUpdate(float pDt)
	{				
		if (m_Unit.PathCount() == 0)
		{
			SetTargetSpeed(0.0);
			SetTargetDirection(0.0);

			return;
		}

		vector translation;
		PrePhys_GetTranslation(translation);
		vector position = m_Unit.ModelToWorld(translation);
		
		float wayPointDistance;
		int wayPointIndex = m_Unit.FindNext(position, wayPointDistance);
		vector wayPoint = m_Unit.PathGet(wayPointIndex);
		wayPointDistance = vector.DistanceSq(Vector(wayPoint[0], 0, wayPoint[2]), Vector(position[0], 0, position[2]));

		bool isFinal = wayPointIndex == m_Unit.PathCount() - 1;
		
		if (!isFinal || (isFinal && wayPointDistance > 0.5))
		{			
			vector pathDir = m_Unit.WorldToModel(wayPoint).Normalized();
			m_TurnDifference = pathDir.VectorToAngles()[0];
			if (m_TurnDifference > 180.0) m_TurnDifference = m_TurnDifference - 360.0;
			
			m_TurnTarget = m_Unit.GetOrientation()[0] + m_TurnDifference;
		}
		else
		{
			m_TurnTarget = m_Unit.GetOrientation()[0];
			m_TurnDifference = 0;
		}
		
		SetTargetSpeed(3.0);

		int color = 0xFF000000;
		
		if (isFinal && wayPointDistance < 2.0)
		{
			color = 0xFFFF0000;
			SetTargetSpeed(0.0);
		}
		else if (wayPointDistance < 8.0)
		{
			color = 0xFF00FF00;
			SetTargetSpeed(1.0);
		}
		else if (wayPointDistance < 20.0)
		{
			color = 0xFF0000FF;
			SetTargetSpeed(2.0);
		}
		
#ifndef SERVER
		m_Unit.AddShape(Shape.CreateSphere(color, ShapeFlags.NOZBUFFER | ShapeFlags.WIREFRAME, wayPoint, 0.05));
#endif
		
		if (m_Fighting) SetSpeedLimit(0.0);
		else if (m_Raised) SetSpeedLimit(1.0);
		else SetSpeedLimit(-1.0);

		SetTargetDirection(0.0);

		if (m_MovementSpeed > 0.1) PrePhys_SetAngles(Vector(Math.Clamp(m_TurnDifference, -45.0, 45.0) * 10.0 * pDt, 0, 0));
	}

	override bool PostPhysUpdate(float pDt)
	{
		return true;
	}
};
