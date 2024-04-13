bool _af_pv_getlong (AUpvlist pvlist, int param, long *l)
{
	int	i;

	for (i=0; i<AUpvgetmaxitems(pvlist); i++)
	{
		int	p, t;

		AUpvgetparam(pvlist, i, &p);

		if (p != param)
			continue;

		AUpvgetvaltype(pvlist, i, &t);

		/* Ensure that this parameter is of type AU_PVTYPE_LONG. */
		if (t != AU_PVTYPE_LONG)
			return false;

		AUpvgetval(pvlist, i, l);
		return true;
	}

	return false;
}