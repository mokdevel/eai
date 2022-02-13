class eAIAnimationST
{
	int m_CMD_Fall;
	int m_CMD_Jump;
	int m_CMD_Land;

	int m_CMD_Turn;
	int m_CMD_StopTurn;

	int m_CMD_Vehicle_GetIn;
	int m_CMD_Vehicle_SwitchSeat;
	int m_CMD_Vehicle_GetOut;
	int m_CMD_Vehicle_ClimbOut;
	int m_CMD_Vehicle_CrawlOut;
	int m_CMD_Vehicle_JumpOut;

	int m_VAR_MovementSpeed;
	int m_VAR_MovementDirection;

	int m_VAR_TurnAmount;

	int m_EVT_LandEarlyExit;

	int m_VAR_VehicleType;
	int m_VAR_VehicleSteering;
	int m_VAR_VehicleThrottle;
	int m_VAR_VehicleClutch;
	int m_VAR_VehicleBrake;
	int m_VAR_VehicleAccelerationFB;
	int m_VAR_VehicleAccelerationLR;

	int m_EVT_LeaveVehicle;
	int m_EVT_LandVehicle;

	int m_VAR_Look;
	int m_VAR_LookDirX;
	int m_VAR_LookDirY;

	int m_VAR_AimX;
	int m_VAR_AimY;

	int m_VAR_Raised;

	void eAIAnimationST( Human human )
	{
		HumanAnimInterface hai = human.GetAnimInterface();

		m_CMD_Fall = hai.BindCommand( "CMD_Fall" );
		m_CMD_Jump = hai.BindCommand( "CMD_Jump" );
		m_CMD_Land = hai.BindCommand( "CMD_Land" );

		m_CMD_Turn = hai.BindCommand( "CMD_Turn" );
		m_CMD_StopTurn = hai.BindCommand( "CMD_StopTurn" );

		m_CMD_Vehicle_GetIn = hai.BindCommand( "CMD_Vehicle_GetIn" );
		m_CMD_Vehicle_SwitchSeat = hai.BindCommand( "CMD_Vehicle_SwitchSeat" );
		m_CMD_Vehicle_GetOut = hai.BindCommand( "CMD_Vehicle_GetOut" );
		m_CMD_Vehicle_ClimbOut = hai.BindCommand( "CMD_Vehicle_ClimbOut" );
		m_CMD_Vehicle_CrawlOut = hai.BindCommand( "CMD_Vehicle_CrawlOut" );
		m_CMD_Vehicle_JumpOut = hai.BindCommand( "CMD_Vehicle_JumpOut" );

		m_VAR_MovementSpeed = hai.BindVariableFloat( "MovementSpeed" );
		m_VAR_MovementDirection = hai.BindVariableFloat( "MovementDirection" );

		m_VAR_TurnAmount = hai.BindVariableFloat( "TurnAmount" );

		m_EVT_LandEarlyExit = hai.BindEvent( "LandEarlyExit" );

		m_VAR_VehicleType = hai.BindVariableInt( "VehicleType" );
		m_VAR_VehicleSteering = hai.BindVariableFloat( "VehicleSteering" );
		m_VAR_VehicleThrottle = hai.BindVariableFloat( "VehicleThrottle" );
		m_VAR_VehicleClutch = hai.BindVariableFloat( "VehicleClutch" );
		m_VAR_VehicleBrake = hai.BindVariableBool( "VehicleBrake" );
		m_VAR_VehicleAccelerationFB = hai.BindVariableFloat( "VehicleAccelerationFB" );
		m_VAR_VehicleAccelerationLR = hai.BindVariableFloat( "VehicleAccelerationLR" );

		m_EVT_LeaveVehicle = hai.BindEvent( "LeaveVehicle" );
		m_EVT_LandVehicle = hai.BindEvent( "LandVehicle" );

		m_VAR_Look = hai.BindVariableBool( "Look" );
		m_VAR_LookDirX = hai.BindVariableFloat( "LookDirX" );
		m_VAR_LookDirY = hai.BindVariableFloat( "LookDirY" );

		m_VAR_AimX = hai.BindVariableFloat( "eAI_AimX" );
		m_VAR_AimY = hai.BindVariableFloat( "eAI_AimY" );

		m_VAR_Raised = hai.BindVariableBool( "Raised" );
	}

	void CallFall( HumanCommandScript script, int pParamInt = 0, float pParamFloat = 0 )
	{
		script.PreAnim_CallCommand( m_CMD_Fall, pParamInt, pParamFloat );
	}

	void CallJump( HumanCommandScript script, int pParamInt = 0, float pParamFloat = 0 )
	{
		script.PreAnim_CallCommand( m_CMD_Jump, pParamInt, pParamFloat );
	}

	void CallLand( HumanCommandScript script, int pParamInt = 0, float pParamFloat = 0 )
	{
		script.PreAnim_CallCommand( m_CMD_Land, pParamInt, pParamFloat );
	}

	void CallTurn( HumanCommandScript script, int pParamInt = 0, float pParamFloat = 0 )
	{
		script.PreAnim_CallCommand( m_CMD_Turn, pParamInt, pParamFloat );
	}

	void CallStopTurn( HumanCommandScript script, int pParamInt = 0, float pParamFloat = 0 )
	{
		script.PreAnim_CallCommand( m_CMD_StopTurn, pParamInt, pParamFloat );
	}

	void CallVehicleGetIn( HumanCommandScript script, int pParamInt = 0, float pParamFloat = 0 )
	{
		script.PreAnim_CallCommand( m_CMD_Vehicle_GetIn, pParamInt, pParamFloat );
	}

	void CallVehicleSwitchSeat( HumanCommandScript script, int pParamInt = 0, float pParamFloat = 0 )
	{
		script.PreAnim_CallCommand( m_CMD_Vehicle_SwitchSeat, pParamInt, pParamFloat );
	}

	void CallVehicleGetOut( HumanCommandScript script, int pParamInt = 0, float pParamFloat = 0 )
	{
		script.PreAnim_CallCommand( m_CMD_Vehicle_GetOut, pParamInt, pParamFloat );
	}

	void CallVehicleClimbOut( HumanCommandScript script, int pParamInt = 0, float pParamFloat = 0 )
	{
		script.PreAnim_CallCommand( m_CMD_Vehicle_ClimbOut, pParamInt, pParamFloat );
	}

	void CallVehicleCrawlOut( HumanCommandScript script, int pParamInt = 0, float pParamFloat = 0 )
	{
		script.PreAnim_CallCommand( m_CMD_Vehicle_CrawlOut, pParamInt, pParamFloat );
	}

	void CallVehicleJumpOut( HumanCommandScript script, int pParamInt = 0, float pParamFloat = 0 )
	{
		script.PreAnim_CallCommand( m_CMD_Vehicle_JumpOut, pParamInt, pParamFloat );
	}

	void SetMovementSpeed( HumanCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_VAR_MovementSpeed, param );
	}

	void SetMovementDirection( HumanCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_VAR_MovementDirection, param );
	}

	void SetTurnAmount( HumanCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_VAR_TurnAmount, param );
	}

	bool IsLandEarlyExit( HumanCommandScript script )
	{
		return script.PrePhys_IsEvent( m_EVT_LandEarlyExit );
	}

	void SetVehicleType( HumanCommandScript script, int param )
	{
		script.PreAnim_SetInt( m_VAR_VehicleType, param );
	}

	void SetVehicleSteering( HumanCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_VAR_VehicleSteering, param );
	}

	void SetVehicleThrottle( HumanCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_VAR_VehicleThrottle, param );
	}
	
	void SetVehicleClutch( HumanCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_VAR_VehicleClutch, param );
	}
	
	void SetVehicleBrake( HumanCommandScript script, bool param )
	{
		script.PreAnim_SetBool( m_VAR_VehicleBrake, param );
	}
	
	void SetVehicleAccelerationFB( HumanCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_VAR_VehicleAccelerationFB, param );
	}
	
	void SetVehicleAccelerationLR( HumanCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_VAR_VehicleAccelerationLR, param );
	}

	bool IsLeaveVehicle( HumanCommandScript script )
	{
		return script.PrePhys_IsEvent( m_EVT_LeaveVehicle );
	}

	bool IsLandVehicle( HumanCommandScript script )
	{
		return script.PrePhys_IsEvent( m_EVT_LandVehicle );
	}

	void SetLook( HumanCommandScript script, bool param )
	{
		script.PreAnim_SetBool( m_VAR_Look, param );
	}

	void SetLookDirX( HumanCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_VAR_LookDirX, param );
	}

	void SetLookDirY( HumanCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_VAR_LookDirY, param );
	}

	void SetAimX( HumanCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_VAR_AimX, param );
	}

	void SetAimY( HumanCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_VAR_AimY, param );
	}

	void SetRaised( HumanCommandScript script, bool param )
	{
		script.PreAnim_SetBool( m_VAR_Raised, param );
	}
};