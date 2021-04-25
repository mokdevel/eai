class eAICommandMove extends eAICommandBase
{
	static const int TURN_STATE_NONE = 0;
	static const int TURN_STATE_TURNING = 1;
	
	private int m_PreviousInteractionLayer;

	private bool m_UseAimPosition;
	private vector m_AimPosition;

	private float m_Turn;
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
	
	private float m_SpeedUpdateTime;
	private float m_MovementSpeed;
	private float m_TargetSpeed;
	private float m_SpeedLimit;
	
	private vector m_MovementCorrection;
			
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

	void SetAimPosition(bool use, vector position = "0 0 0")
	{
		m_UseAimPosition = use;
		m_AimPosition = position;
	}

	void SetSpeedLimit(float speedIdx)
	{
		m_SpeedLimit = speedIdx;

		if (m_SpeedLimit < 0 || m_SpeedLimit > 3) m_SpeedLimit = 3;
	}
	
	void SetTargetSpeed(float target)
	{
		if (m_TargetSpeed > target && target < 2.0 && m_SpeedUpdateTime < 0.1) return;

		m_SpeedUpdateTime = 0;
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

	override void PreAnimUpdate(float pDt)
	{
		m_SpeedUpdateTime += pDt;
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

						m_ReevaluateTurnTime = (m_TurnDifferenceStart / 90.0);

						m_Table.CallTurn(this);

						m_TurnState = TURN_STATE_TURNING;
					}
					break;
				case TURN_STATE_TURNING:
					m_TurnTime += pDt;

					if (Math.AbsFloat(m_TurnDifferenceStart - m_TurnDifference) / pDt < 0.1)
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
		}
		else
		{
			m_TurnState = TURN_STATE_NONE;
		}

		PreAnim_SetFilteredHeading(-m_TurnTarget * Math.DEG2RAD, 0.1, 30.0);
	}

	override void PrePhysUpdate(float pDt)
	{
		vector debug_points[2];
		
		vector translation;
		PrePhys_GetTranslation(translation);
		vector position = m_Unit.ModelToWorld(translation);
		
		vector transform[4];
		m_Unit.GetTransform(transform);

		float wayPointDistance = 0.0;
		int wayPointIndex;
		vector wayPoint = position;
		bool isFinal = true;

		SetTargetSpeed(0.0);
		SetTargetDirection(0.0);

		PhxInteractionLayers collisionLayerMask = PhxInteractionLayers.ROADWAY|PhxInteractionLayers.BUILDING|PhxInteractionLayers.FENCE|PhxInteractionLayers.VEHICLE;
		Object hitObject; //! always null and low priority fix at BI
		vector hitPosition;
		vector hitNormal;
		float hitFraction;
		
		if (m_Unit.PathCount() >= 2)
		{
			wayPointIndex = m_Unit.FindNext(position, wayPointDistance);
			wayPoint = m_Unit.PathGet(wayPointIndex);

			float y = GetGame().SurfaceY(wayPoint[0], wayPoint[2]);			
			if (y > wayPoint[1]) wayPoint[1] = y;
			
#ifndef SERVER
			m_Unit.AddShape(Shape.CreateSphere(0xFFFFFFFF, ShapeFlags.VISIBLE | ShapeFlags.WIREFRAME, wayPoint, 0.3));
#endif
			
			vector orig_WayPoint = wayPoint;
			if (DayZPhysics.SphereCastBullet(wayPoint, wayPoint - Vector(0.0, 10.0, 0.0), 0.3, collisionLayerMask|PhxInteractionLayers.TERRAIN, m_Unit, hitObject, hitPosition, hitNormal, hitFraction)) wayPoint = hitPosition;
			
#ifndef SERVER
			m_Unit.AddShape(Shape.CreateSphere(0xFF0000FF, ShapeFlags.VISIBLE | ShapeFlags.WIREFRAME, wayPoint, 0.05));
#endif
			
			wayPointDistance = vector.DistanceSq(wayPoint, position);
			
			vector newWayPoint = wayPoint;

			if (wayPointDistance > 0.01 && wayPointDistance < 1.0 && DayZPhysics.RayCastBullet(position, newWayPoint, collisionLayerMask, m_Unit, hitObject, hitPosition, hitNormal, hitFraction))
			{
				wayPoint = hitPosition;
			}
			else 
			{
				wayPoint = newWayPoint;
			}
			
#ifndef SERVER
			debug_points[0] = position;
			debug_points[1] = wayPoint;
			m_Unit.AddShape(Shape.CreateLines(0xFFFF00FF, ShapeFlags.NOZBUFFER, debug_points, 2));
#endif

#ifndef SERVER
			m_Unit.AddShape(Shape.CreateSphere(0xFFFFFF00, ShapeFlags.VISIBLE | ShapeFlags.WIREFRAME, wayPoint + Vector(0.0, 0.2, 0.0), 0.3));
#endif
				
#ifndef SERVER
			m_Unit.AddShape(Shape.CreateSphere(0xFFFFFF00, ShapeFlags.VISIBLE | ShapeFlags.WIREFRAME, wayPoint - Vector(0.0, 0.2, 0.0), 0.3));
#endif
				
			if (DayZPhysics.SphereCastBullet(wayPoint + Vector(0.0, 0.2, 0.0), wayPoint - Vector(0.0, 0.2, 0.0), 0.3, collisionLayerMask, m_Unit, hitObject, hitPosition, hitNormal, hitFraction)) 
			{
				wayPoint = hitPosition;
			
#ifndef SERVER
				m_Unit.AddShape(Shape.CreateSphere(0xFF00FF00, ShapeFlags.VISIBLE | ShapeFlags.WIREFRAME, wayPoint, 0.05));
#endif
			}
			else
			{
			
#ifndef SERVER
				m_Unit.AddShape(Shape.CreateSphere(0xFFFF0000, ShapeFlags.VISIBLE | ShapeFlags.WIREFRAME, wayPoint, 0.05));
#endif
			}

			isFinal = wayPointIndex == m_Unit.PathCount() - 1;
		}

		float minFinal = 0.1;

		if (!isFinal || !m_UseAimPosition || wayPointDistance > minFinal)
		{
			m_AimPosition = wayPoint;
		}
		
		vector pathDir = vector.Direction(position, m_AimPosition).Normalized();
		m_Turn = m_Unit.GetOrientation()[0];
		m_TurnTarget = pathDir.VectorToAngles()[0];
		if (m_TurnTarget > 180.0) m_TurnTarget = m_TurnTarget - 360.0;

		if (m_TurnTarget < m_Turn)
		{
			m_TurnDifference = m_Turn - m_TurnTarget;
		}
		else
		{
			m_TurnDifference = m_TurnTarget - m_Turn;
		}

		if (isFinal && wayPointDistance < minFinal)
		{
			SetTargetSpeed(0.0);
		}
		else if (isFinal && wayPointDistance < 8.0)
		{
			SetTargetSpeed(1.0);
		}
		else if (wayPointDistance < 20.0)
		{
			SetTargetSpeed(2.0);
		}
		else
		{
			SetTargetSpeed(Math.Lerp(m_TargetSpeed, 3.0, pDt * 2.0));
		}
		
		if (m_Raised) SetSpeedLimit(1.0);
		else SetSpeedLimit(-1.0);

		SetTargetDirection(0.0);
		
		// TODO: this is only temporary code and a better solution has to be found later on
		// This fix is for when the AI is meant to be moving faster but height elevation is blocking us
		// Reason why this is temporary; it effictively is telling the player to jump.
		m_MovementCorrection = vector.Zero;
					
		dBodyEnableGravity(m_Unit, true);
		if (m_MovementSpeed != 0 && translation.LengthSq() < 0.1 * pDt)
		{			
			if (wayPointDistance < 1.0)
			{
				float yDiff = wayPoint[1] - position[1];
				
				if (yDiff > 0.01 && yDiff <= 0.4)
				{
					m_MovementCorrection = Vector(0, yDiff / pDt, 0);

					translation[1] = translation[1] + (yDiff * pDt);
					
					dBodyEnableGravity(m_Unit, false);
				}
			}
		}

		PrePhys_SetTranslation(translation);
	}

	override bool PostPhysUpdate(float pDt)
	{
		return true;
	}
};
