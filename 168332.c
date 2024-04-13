_XimDecodeICATTRIBUTE(
    Xic			 ic,
    XIMResourceList	 res_list,
    unsigned int	 res_num,
    CARD16		*data,
    INT16		 data_len,
    XIMArg		*arg,
    BITMASK32		 mode)
{
    register XIMArg	*p;
    XIMResourceList	 res;
    int			 check;
    INT16		 len;
    CARD16		*buf;
    INT16		 total;
    char		*name;
    INT16		 min_len = sizeof(CARD16)	/* sizeof attributeID */
			 	 + sizeof(INT16);	/* sizeof length */
    XrmQuark		 pre_quark;
    XrmQuark		 sts_quark;

    if (!arg)
	return (char *)NULL;

    pre_quark = XrmStringToQuark(XNPreeditAttributes);
    sts_quark = XrmStringToQuark(XNStatusAttributes);

    for (p = arg; p->name; p++) {
	if (!(res = _XimGetResourceListRec(res_list, res_num, p->name))) {
	    if (_XimDecodeInnerICATTRIBUTE(ic, p, mode))
		continue;
	    return p->name;
	}

	check = _XimCheckICMode(res, mode);
	if (check == XIM_CHECK_INVALID)
	    continue;
	else if (check == XIM_CHECK_ERROR)
	    return p->name;

	total = data_len;
	buf = data;
	while (total >= min_len) {
	    if (res->id == buf[0])
		break;

	    len = buf[1];
	    len += XIM_PAD(len) + min_len;
	    buf = (CARD16 *)((char *)buf + len);
	    total -= len;
	}
	if (total < min_len)
	    return p->name;

	if (res->resource_size == XimType_NEST) {
	    if (res->xrm_name == pre_quark) {
	        if ((name = _XimDecodeICATTRIBUTE(ic, res_list, res_num,
			&buf[2], buf[1], (XIMArg *)p->value,
			(mode | XIM_PREEDIT_ATTR))))
		    return name;
	    } else if (res->xrm_name == sts_quark) {
	        if ((name = _XimDecodeICATTRIBUTE(ic, res_list, res_num,
			&buf[2], buf[1], (XIMArg *)p->value,
			(mode | XIM_STATUS_ATTR))))
		    return name;
	    }
	} else {
	    if (!(_XimAttributeToValue(ic, res, &buf[2], buf[1],
							p->value, mode)))
		return p->name;
	}
    }
    return (char *)NULL;
}