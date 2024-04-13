AUpvlist _af_pv_double (double val)
{
	AUpvlist	ret = AUpvnew(1);
	AUpvsetparam(ret, 0, 0);
	AUpvsetvaltype(ret, 0, AU_PVTYPE_DOUBLE);
	AUpvsetval(ret, 0, &val);
	return ret;
}