nfp_flower_reprs_reify(struct nfp_app *app, enum nfp_repr_type type,
		       bool exists)
{
	struct nfp_reprs *reprs;
	int i, err, count = 0;

	reprs = rcu_dereference_protected(app->reprs[type],
					  lockdep_is_held(&app->pf->lock));
	if (!reprs)
		return 0;

	for (i = 0; i < reprs->num_reprs; i++) {
		struct net_device *netdev;

		netdev = nfp_repr_get_locked(app, reprs, i);
		if (netdev) {
			struct nfp_repr *repr = netdev_priv(netdev);

			err = nfp_flower_cmsg_portreify(repr, exists);
			if (err)
				return err;
			count++;
		}
	}

	return count;
}