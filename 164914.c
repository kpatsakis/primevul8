bool _af_unique_ids (int *ids, int nids, char *idname, int iderr)
{
	int i;

	for (i = 0; i < nids; i++)
	{
		int j;
		for (j = 0; j < i; j++)
			if (ids[i] == ids[j])
			{
				_af_error(iderr, "nonunique %s id %d",
					idname, ids[i]);
				return false;
			}
	}

	return true;
}