pushf_write(PushFilter *mp, const uint8 *data, int len)
{
	int			need,
				res;

	/*
	 * no buffering
	 */
	if (mp->block_size <= 0)
		return wrap_process(mp, data, len);

	/*
	 * try to empty buffer
	 */
	need = mp->block_size - mp->pos;
	if (need > 0)
	{
		if (len < need)
		{
			memcpy(mp->buf + mp->pos, data, len);
			mp->pos += len;
			return 0;
		}
		memcpy(mp->buf + mp->pos, data, need);
		len -= need;
		data += need;
	}

	/*
	 * buffer full, process
	 */
	res = wrap_process(mp, mp->buf, mp->block_size);
	if (res < 0)
		return res;
	mp->pos = 0;

	/*
	 * now process directly from data
	 */
	while (len > 0)
	{
		if (len > mp->block_size)
		{
			res = wrap_process(mp, data, mp->block_size);
			if (res < 0)
				return res;
			data += mp->block_size;
			len -= mp->block_size;
		}
		else
		{
			memcpy(mp->buf, data, len);
			mp->pos += len;
			break;
		}
	}
	return 0;
}