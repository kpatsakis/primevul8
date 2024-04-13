static bool vhost_net_tx_select_zcopy(struct vhost_net *net)
{
	/* TX flush waits for outstanding DMAs to be done.
	 * Don't start new DMAs.
	 */
	return !net->tx_flush &&
		net->tx_packets / 64 >= net->tx_zcopy_err;
}