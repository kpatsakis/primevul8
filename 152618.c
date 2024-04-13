pushf_create(PushFilter **mp_p, const PushFilterOps *op, void *init_arg, PushFilter *next)
{
	PushFilter *mp;
	void	   *priv;
	int			res;

	if (op->init != NULL)
	{
		res = op->init(next, init_arg, &priv);
		if (res < 0)
			return res;
	}
	else
	{
		priv = init_arg;
		res = 0;
	}

	mp = px_alloc(sizeof(*mp));
	memset(mp, 0, sizeof(*mp));
	mp->block_size = res;
	mp->op = op;
	mp->priv = priv;
	mp->next = next;
	if (mp->block_size > 0)
	{
		mp->buf = px_alloc(mp->block_size);
		mp->pos = 0;
	}
	else
	{
		mp->buf = NULL;
		mp->pos = 0;
	}
	*mp_p = mp;
	return 0;
}