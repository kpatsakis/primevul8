pullf_free(PullFilter *pf)
{
	if (pf->op->free)
		pf->op->free(pf->priv);

	if (pf->buf)
	{
		px_memset(pf->buf, 0, pf->buflen);
		px_free(pf->buf);
	}

	px_memset(pf, 0, sizeof(*pf));
	px_free(pf);
}