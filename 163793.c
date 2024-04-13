static __net_init int ppp_init_net(struct net *net)
{
	struct ppp_net *pn;
	int err;

	pn = kzalloc(sizeof(*pn), GFP_KERNEL);
	if (!pn)
		return -ENOMEM;

	idr_init(&pn->units_idr);
	mutex_init(&pn->all_ppp_mutex);

	INIT_LIST_HEAD(&pn->all_channels);
	INIT_LIST_HEAD(&pn->new_channels);

	spin_lock_init(&pn->all_channels_lock);

	err = net_assign_generic(net, ppp_net_id, pn);
	if (err) {
		kfree(pn);
		return err;
	}

	return 0;
}