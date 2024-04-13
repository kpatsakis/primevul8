nfp_flower_wait_repr_reify(struct nfp_app *app, atomic_t *replies, int tot_repl)
{
	struct nfp_flower_priv *priv = app->priv;

	if (!tot_repl)
		return 0;

	lockdep_assert_held(&app->pf->lock);
	if (!wait_event_timeout(priv->reify_wait_queue,
				atomic_read(replies) >= tot_repl,
				NFP_FL_REPLY_TIMEOUT)) {
		nfp_warn(app->cpp, "Not all reprs responded to reify\n");
		return -EIO;
	}

	return 0;
}