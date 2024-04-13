static int selinux_inet_sys_rcv_skb(int ifindex, char *addrp, u16 family,
				    u32 peer_sid,
				    struct common_audit_data *ad)
{
	int err;
	u32 if_sid;
	u32 node_sid;

	err = sel_netif_sid(ifindex, &if_sid);
	if (err)
		return err;
	err = avc_has_perm(peer_sid, if_sid,
			   SECCLASS_NETIF, NETIF__INGRESS, ad);
	if (err)
		return err;

	err = sel_netnode_sid(addrp, family, &node_sid);
	if (err)
		return err;
	return avc_has_perm(peer_sid, node_sid,
			    SECCLASS_NODE, NODE__RECVFROM, ad);
}