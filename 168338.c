_XimMakeIMAttrIDList(
    Xim			 im,
    XIMResourceList	 res_list,
    unsigned int	 res_num,
    XIMArg		*arg,
    CARD16		*buf,
    INT16		*len,
    unsigned long	 mode)
{
    register XIMArg	*p;
    XIMResourceList	 res;
    int			 check;

    *len = 0;
    if (!arg)
	return (char *)NULL;

    for (p = arg; p->name; p++) {
	if (!(res = _XimGetResourceListRec(res_list, res_num, p->name))) {
	    if (_XimCheckInnerIMAttributes(im, p, mode))
		continue;
	    return p->name;
	}

	check = _XimCheckIMMode(res, mode);
	if (check == XIM_CHECK_INVALID)
	    continue;
	else if (check == XIM_CHECK_ERROR)
	    return p->name;

	*buf = res->id;
	*len += sizeof(CARD16);
	 buf++;
    }
    return (char *)NULL;
}