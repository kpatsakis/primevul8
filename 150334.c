void __init rtnetlink_init(void)
{
	if (register_pernet_subsys(&rtnetlink_net_ops))
		panic("rtnetlink_init: cannot initialize rtnetlink\n");

	register_netdevice_notifier(&rtnetlink_dev_notifier);

	rtnl_register(PF_UNSPEC, RTM_GETLINK, rtnl_getlink,
		      rtnl_dump_ifinfo, rtnl_calcit);
	rtnl_register(PF_UNSPEC, RTM_SETLINK, rtnl_setlink, NULL, NULL);
	rtnl_register(PF_UNSPEC, RTM_NEWLINK, rtnl_newlink, NULL, NULL);
	rtnl_register(PF_UNSPEC, RTM_DELLINK, rtnl_dellink, NULL, NULL);

	rtnl_register(PF_UNSPEC, RTM_GETADDR, NULL, rtnl_dump_all, NULL);
	rtnl_register(PF_UNSPEC, RTM_GETROUTE, NULL, rtnl_dump_all, NULL);

	rtnl_register(PF_BRIDGE, RTM_NEWNEIGH, rtnl_fdb_add, NULL, NULL);
	rtnl_register(PF_BRIDGE, RTM_DELNEIGH, rtnl_fdb_del, NULL, NULL);
	rtnl_register(PF_BRIDGE, RTM_GETNEIGH, NULL, rtnl_fdb_dump, NULL);

	rtnl_register(PF_BRIDGE, RTM_GETLINK, NULL, rtnl_bridge_getlink, NULL);
	rtnl_register(PF_BRIDGE, RTM_DELLINK, rtnl_bridge_dellink, NULL, NULL);
	rtnl_register(PF_BRIDGE, RTM_SETLINK, rtnl_bridge_setlink, NULL, NULL);
}