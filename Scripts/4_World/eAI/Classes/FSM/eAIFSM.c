class eAIFSM
{
    static const int EXIT = 0;
    static const int CONTINUE = 1;

    private autoptr array<ref eAIState> m_States;
    private autoptr array<ref eAITransition> m_Transitions;

    private eAIState m_CurrentState;
    private eAIState m_ParentState;
    private bool m_Running = true;

    protected string m_Name;
    protected string m_DefaultState;
    protected eAIBase m_Unit;

    void eAIFSM(eAIBase unit, eAIState parentState)
    {
        m_Unit = unit;
        m_ParentState = parentState;

        m_States = new array<ref eAIState>();
        m_Transitions = new array<ref eAITransition>();
    }

    string GetName()
    {
        return m_Name;
    }

    eAIBase GetUnit()
    {
        return m_Unit;
    }

    eAIState GetParent()
    {
        return m_ParentState;
    }

    void AddState(eAIState state)
    {
        m_States.Insert(state);
    }

    void AddTransition(eAITransition transition)
    {
        m_Transitions.Insert(transition);
    }

    void SortTransitions()
    {
        //TODO: if the source transition is null, push to the back of the array
        //TODO: if the destination transition is null, remove from the array
    }
	
	eAIState GetState()
    {
        return m_CurrentState;
    }

    eAIState GetState(string type)
    {
        for (int i = 0; i < m_States.Count(); i++) if (m_States[i].ClassName() == type) return m_States[i];

        return null;
    }

    eAIState GetState(typename type)
    {
        for (int i = 0; i < m_States.Count(); i++) if (m_States[i].Type() == type) return m_States[i];

        return null;
    }
	
	bool StartDefault()
	{
        //auto trace = CF_Trace_0(this, "StartDefault");

        if (m_DefaultState == "") return Start();

        eAIState src = m_CurrentState;
        eAIState dst = GetState(m_DefaultState);

        if (m_Running && src)
        {
            CF_Log.Debug("Exiting state: " + src);
            src.OnExit("", true, dst);
        }
	
		m_CurrentState = dst;
		
        if (m_CurrentState)
        {
            CF_Log.Debug("Starting state: " + m_CurrentState);
            m_CurrentState.OnEntry("", src);
            return true;
        }
		
        CF_Log.Warn("No valid state found.");
		
        return false;
	}

    bool Start(string e = "")
    {
        //auto trace = CF_Trace_1(this, "Start").Add(e);

        Param2<eAIState, bool> new_state = FindSuitableTransition(m_CurrentState, "");

        eAIState src = m_CurrentState;
        eAIState dst = new_state.param1;

        if (dst == null)
        {
            CF_Log.Warn("No valid state found. Aborting.");

            return false;
        }

        if (m_Running && m_CurrentState && m_CurrentState != dst)
        {
            CF_Log.Debug("Exiting state: " + m_CurrentState);
            m_CurrentState.OnExit(e, true, dst);
        }

        m_CurrentState = dst;

        if (m_CurrentState && src != m_CurrentState)
        {
            CF_Log.Debug("Starting state: " + m_CurrentState);
            m_CurrentState.OnEntry(e, src);
            return true;
        }

        CF_Log.Warn("No valid state found.");

        return false;
    }

    bool Abort(string e = "")
    {
        //auto trace = CF_Trace_1(this, "Abort").Add(e);

        if (m_Running && m_CurrentState)
        {
            CF_Log.Debug("Exiting state: " + m_CurrentState);
            m_CurrentState.OnExit(e, true, null);
            return true;
        }

        return false;
    }

    /**
     * @return true Tell the parent FSM that the child FSM is complete
     * @return false Tell the parent FSM that the child FSM is still running
     */
    int Update(float pDt, int pSimulationPrecision)
    {
        //auto trace = CF_Trace_2(this, "Update").Add(pDt).Add(pSimulationPrecision);

        //CF_Log.Debug("m_CurrentState: %1", "" + m_CurrentState);

        #ifdef EAI_DEBUG_FSM
        if (m_CurrentState && m_CurrentState.Debug_OnUpdate(m_Debug, m_Depth + 1, pDt, pSimulationPrecision) == CONTINUE) return CONTINUE;
        #else
        if (m_CurrentState && m_CurrentState.OnUpdate(pDt, pSimulationPrecision) == CONTINUE) return CONTINUE;
        #endif

        Param2<eAIState, bool> new_state = FindSuitableTransition(m_CurrentState, "");
        if (!new_state.param2 || (new_state.param2 && m_CurrentState == new_state.param1))
        {	
            if (!m_CurrentState) return EXIT;

            return CONTINUE;
        }

        eAIState src = m_CurrentState;

        if (m_CurrentState) m_CurrentState.OnExit("", false, new_state.param1);

        m_CurrentState = new_state.param1;

        if (m_CurrentState == null)
        {
		    CF_Log.Info("State transition exit " + src.GetName());
            return EXIT;
        }
        
		CF_Log.Info("State transition " + src.GetName() + " -> " + m_CurrentState.GetName());

        m_CurrentState.OnEntry("", src);

        return CONTINUE;
    }
	
	Param2<eAIState, bool> FindSuitableTransition(eAIState s, string e = "")
	{
        //auto trace = CF_Trace_2(this, "FindSuitableTransition").Add(s).Add(e);

        // returns tuple as a valid destination can still be null

        //TODO: store a reference to the transitions inside the state for that state

		eAIState curr_state = s;

		int count = m_Transitions.Count();
		for (int i = 0; i < count; ++i)
		{
			auto t = m_Transitions.Get(i);
			if ((t.GetSource() == curr_state || t.GetSource() == null) && (e == "" || (e != "" && t.GetEvent() == e)))
			{
                #ifdef EAI_DEBUG_FSM
				int guard = t.Debug_Guard(m_Debug, m_Depth + 1, i);
                #else
				int guard = t.Guard();
                #endif
                switch (guard)
                {
                case eAITransition.SUCCESS:
				    return new Param2<eAIState, bool>(t.GetDestination(), true);
                case eAITransition.FAIL:
				    break;
                }
			}
		}

		return new Param2<eAIState, bool>(null, false);
	}

	#ifdef EAI_DEBUG_FSM
    //! Current active debug block
    CF_DebugUI_Block m_Debug;
	int m_Depth;

	private string Debug_Prefix()
	{
		int depth = m_Depth;
		string str = "";
		while (depth > 0)
		{
			str += " ";
			depth--;
		}
		return str;
	}

	void Debug_Set(string key, int text)
	{
		m_Debug.Set(Debug_Prefix() + key, text);
	}

	void Debug_Set(string key, bool text)
	{
		m_Debug.Set(Debug_Prefix() + key, text);
	}

	void Debug_Set(string key, float text)
	{
		m_Debug.Set(Debug_Prefix() + key, text);
	}

	void Debug_Set(string key, vector text)
	{
		m_Debug.Set(Debug_Prefix() + key, text);
	}

	void Debug_Set(string key, Class text)
	{
		m_Debug.Set(Debug_Prefix() + key, text);
	}

	void Debug_Set(string key, string text)
	{
		m_Debug.Set(Debug_Prefix() + key, text);
	}

    int Debug_Update(Class dbg, int depth, float pDt, int pSimulationPrecision)
    {
        Class.CastTo(m_Debug, dbg);
        m_Depth = depth;

		Debug_Set("FSM", m_Name);
        int ret = Update(pDt, pSimulationPrecision);
        switch (ret)
        {
            case EXIT:
                Debug_Set("Status", "EXIT");
                break;
            case CONTINUE:
                Debug_Set("Status", "CONTINUE");
                break;
            default:
                Debug_Set("Status", "UNKNOWN");
                break;
        }
        return ret;
    }
    #else
	void Debug_Set(string key, int text)
	{
	}

	void Debug_Set(string key, bool text)
	{
	}

	void Debug_Set(string key, float text)
	{
	}

	void Debug_Set(string key, vector text)
	{
	}

	void Debug_Set(string key, Class text)
	{
	}

	void Debug_Set(string key, string text)
	{
	}

    int Debug_Update(Class dbg, int depth, float pDt, int pSimulationPrecision)
    {
        return Update(pDt, pSimulationPrecision);
    }
    #endif
};