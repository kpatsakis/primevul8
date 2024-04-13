NEGO_STATE nego_get_state(rdpNego* nego)
{
	if (!nego)
		return NEGO_STATE_FAIL;

	return nego->state;
}