class eAITransitionType
{
    private static ref map<string, ref eAITransitionType> m_Types = new map<string, ref eAITransitionType>();

	string m_ClassName;

    void eAITransitionType()
    {
    }

    static bool Contains(string name)
    {
        return m_Types.Contains(name);
    }

    static void Add(eAITransitionType type)
    {
        m_Types.Insert(type.m_ClassName, type);
    }

    static eAITransitionType Get(string type)
    {
        return m_Types[type];
    }

    static eAITransitionType LoadXML(string fsmName, CF_XML_Tag xml_root_tag, FileHandle file)
    {
		//eAITrace trace(null, "eAITransitionType::LoadXML", fsmName);

        string from_state_name;
        auto from_state = xml_root_tag.GetTag("from_state");
        if (from_state.Count() > 0) from_state_name = from_state[0].GetAttribute("name").ValueAsString();
        string from_state_class = "eAIState";
        if (from_state_name != "") from_state_class = "eAI_" + fsmName + "_" + from_state_name + "_State";

        string to_state_name;
        auto to_state = xml_root_tag.GetTag("to_state");
        if (to_state.Count() > 0) to_state_name = to_state[0].GetAttribute("name").ValueAsString();
        string to_state_class = "eAIState";
        if (to_state_name != "") to_state_class = "eAI_" + fsmName + "_" + to_state_name + "_State";

        string event_name;
        auto evt = xml_root_tag.GetTag("event");
        if (evt.Count() > 0) event_name = evt[0].GetAttribute("name").ValueAsString();
        string event_class = "";
        if (event_name != "") event_class = "eAI_" + event_name + "_Event";

        string class_name = "eAI_" + fsmName + "_" + from_state_name + "_" + to_state_name + "_Transition";

        if (eAITransitionType.Contains(class_name)) return eAITransitionType.Get(class_name);

        eAITransitionType new_type = new eAITransitionType();
		new_type.m_ClassName = class_name;

        FPrintln(file, "class " + class_name + " extends eAITransition {");

        FPrintln(file, "private " + from_state_class + " src;");
        FPrintln(file, "private " + to_state_class + " dst;");

        FPrintln(file, "eAI_" + fsmName + "_FSM fsm;");

        FPrintln(file, "void " + class_name + "(eAIFSM _fsm, eAIBase _unit) {");
        FPrintln(file, "Class.CastTo(fsm, _fsm);");
        FPrintln(file, "m_ClassName = \"" + class_name + "\";");
        FPrintln(file, "Class.CastTo(src, _fsm.GetState(\"" + from_state_class + "\"));");
        FPrintln(file, "Class.CastTo(dst, _fsm.GetState(\"" + to_state_class + "\"));");
        FPrintln(file, "}");

        auto guard = xml_root_tag.GetTag("guard");
        if (guard.Count() > 0)
        {
            FPrintln(file, "override int Guard() {");
        	FPrintln(file, "//eAITrace trace(this, \"Guard\");");
            FPrintln(file, guard[0].GetContent().GetContent());
            FPrintln(file, "}");
        }
        
        FPrintln(file, "override eAIState GetSource() { return src; }");
        FPrintln(file, "override eAIState GetDestination() { return dst; }");
        FPrintln(file, "override string GetEvent() { return \"" + event_class + "\"; }");

        FPrintln(file, "}");
		
		eAITransitionType.Add(new_type);
		
		return new_type;
    }
};