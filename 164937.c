bool _af_pv_getdouble (AUpvlist pvlist, int param, double *d)
{
	int	i;

	for (i=0; i<AUpvgetmaxitems(pvlist); i++)
	{
		int	p, t;

		AUpvgetparam(pvlist, i, &p);

		if (p != param)
			continue;

		AUpvgetvaltype(pvlist, i, &t);

		/* Ensure that this parameter is of type AU_PVTYPE_DOUBLE. */
		if (t != AU_PVTYPE_DOUBLE)
			return false;

		AUpvgetval(pvlist, i, d);
		return true;
	}

	return false;
}