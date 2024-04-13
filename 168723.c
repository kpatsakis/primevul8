UINT32 nego_get_selected_protocol(rdpNego* nego)
{
	if (!nego)
		return 0;

	return nego->SelectedProtocol;
}