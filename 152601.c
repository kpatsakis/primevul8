pushf_free_all(PushFilter *mp)
{
	PushFilter *tmp;

	while (mp)
	{
		tmp = mp->next;
		pushf_free(mp);
		mp = tmp;
	}
}