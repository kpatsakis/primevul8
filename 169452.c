config_interface(void)
{
	struct Interface *iface;
	for(iface=IfaceList; iface; iface=iface->next)
	{
		if (iface->AdvLinkMTU)
			set_interface_linkmtu(iface->Name, iface->AdvLinkMTU);
		if (iface->AdvCurHopLimit)
			set_interface_curhlim(iface->Name, iface->AdvCurHopLimit);
		if (iface->AdvReachableTime)
			set_interface_reachtime(iface->Name, iface->AdvReachableTime);
		if (iface->AdvRetransTimer)
			set_interface_retranstimer(iface->Name, iface->AdvRetransTimer);
	}
}