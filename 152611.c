wrap_process(PushFilter *mp, const uint8 *data, int len)
{
	int			res;

	if (mp->op->push != NULL)
		res = mp->op->push(mp->next, mp->priv, data, len);
	else
		res = pushf_write(mp->next, data, len);
	if (res > 0)
		return PXE_BUG;
	return res;
}