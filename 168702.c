static const char* nego_state_string(NEGO_STATE state)
{
	static const char* const NEGO_STATE_STRINGS[] = { "NEGO_STATE_INITIAL", "NEGO_STATE_EXT",
		                                              "NEGO_STATE_NLA",     "NEGO_STATE_TLS",
		                                              "NEGO_STATE_RDP",     "NEGO_STATE_FAIL",
		                                              "NEGO_STATE_FINAL",   "NEGO_STATE_INVALID" };
	if (state >= ARRAYSIZE(NEGO_STATE_STRINGS))
		return NEGO_STATE_STRINGS[ARRAYSIZE(NEGO_STATE_STRINGS) - 1];
	return NEGO_STATE_STRINGS[state];
}