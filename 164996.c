bool _af_pv_getptr (AUpvlist pvlist, int param, void **v)
{
	int	i;

	for (i=0; i<AUpvgetmaxitems(pvlist); i++)
	{
		int	p, t;

		AUpvgetparam(pvlist, i, &p);

		if (p != param)
			continue;

		AUpvgetvaltype(pvlist, i, &t);

		/* Ensure that this parameter is of type AU_PVTYPE_PTR. */
		if (t != AU_PVTYPE_PTR)
			return false;

		AUpvgetval(pvlist, i, v);
		return true;
	}

	return false;
}