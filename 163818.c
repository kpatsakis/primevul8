static void ppp_shutdown_interface(struct ppp *ppp)
{
	struct ppp_net *pn;

	pn = ppp_pernet(ppp->ppp_net);
	mutex_lock(&pn->all_ppp_mutex);

	/* This will call dev_close() for us. */
	ppp_lock(ppp);
	if (!ppp->closing) {
		ppp->closing = 1;
		ppp_unlock(ppp);
		unregister_netdev(ppp->dev);
	} else
		ppp_unlock(ppp);

	unit_put(&pn->units_idr, ppp->file.index);
	ppp->file.dead = 1;
	ppp->owner = NULL;
	wake_up_interruptible(&ppp->file.rwait);

	mutex_unlock(&pn->all_ppp_mutex);
}