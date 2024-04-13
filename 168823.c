BOOL nego_set_state(rdpNego* nego, NEGO_STATE state)
{
	if (!nego)
		return FALSE;

	nego->state = state;
	return TRUE;
}