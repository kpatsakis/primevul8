pullf_create(PullFilter **pf_p, const PullFilterOps *op, void *init_arg, PullFilter *src)
{
	PullFilter *pf;
	void	   *priv;
	int			res;

	if (op->init != NULL)
	{
		res = op->init(&priv, init_arg, src);
		if (res < 0)
			return res;
	}
	else
	{
		priv = init_arg;
		res = 0;
	}

	pf = px_alloc(sizeof(*pf));
	memset(pf, 0, sizeof(*pf));
	pf->buflen = res;
	pf->op = op;
	pf->priv = priv;
	pf->src = src;
	if (pf->buflen > 0)
	{
		pf->buf = px_alloc(pf->buflen);
		pf->pos = 0;
	}
	else
	{
		pf->buf = NULL;
		pf->pos = 0;
	}
	*pf_p = pf;
	return 0;
}