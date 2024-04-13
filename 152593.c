pushf_flush(PushFilter *mp)
{
	int			res;

	while (mp)
	{
		if (mp->block_size > 0)
		{
			res = wrap_process(mp, mp->buf, mp->pos);
			if (res < 0)
				return res;
		}

		if (mp->op->flush)
		{
			res = mp->op->flush(mp->next, mp->priv);
			if (res < 0)
				return res;
		}

		mp = mp->next;
	}
	return 0;
}