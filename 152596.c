pullf_read(PullFilter *pf, int len, uint8 **data_p)
{
	int			res;

	if (pf->op->pull)
	{
		if (pf->buflen && len > pf->buflen)
			len = pf->buflen;
		res = pf->op->pull(pf->priv, pf->src, len, data_p,
						   pf->buf, pf->buflen);
	}
	else
		res = pullf_read(pf->src, len, data_p);
	return res;
}