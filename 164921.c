AUpvlist _af_pv_pointer (void *val)
{
	AUpvlist	ret = AUpvnew(1);
	AUpvsetparam(ret, 0, 0);
	AUpvsetvaltype(ret, 0, AU_PVTYPE_PTR);
	AUpvsetval(ret, 0, &val);
	return ret;
}