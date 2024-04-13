pushf_free(PushFilter *mp)
{
	if (mp->op->free)
		mp->op->free(mp->priv);

	if (mp->buf)
	{
		px_memset(mp->buf, 0, mp->block_size);
		px_free(mp->buf);
	}

	px_memset(mp, 0, sizeof(*mp));
	px_free(mp);
}