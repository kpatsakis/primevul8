BOOL nego_set_selected_protocol(rdpNego* nego, UINT32 SelectedProtocol)
{
	if (!nego)
		return FALSE;

	nego->SelectedProtocol = SelectedProtocol;
	return TRUE;
}