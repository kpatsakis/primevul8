_XimDecodeIMATTRIBUTE(
    Xim			 im,
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
    INT16		 min_len = sizeof(CARD16)	/* sizeof attributeID */
			 	 + sizeof(INT16);	/* sizeof length */

    for (p = arg; p->name; p++) {
	if (!(res = _XimGetResourceListRec(res_list, res_num, p->name))) {
	    if (_XimDecodeInnerIMATTRIBUTE(im, p))
		continue;
	    return p->name;
	}

	check = _XimCheckIMMode(res, mode);
	if(check == XIM_CHECK_INVALID)
	    continue;
	else if(check == XIM_CHECK_ERROR)
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

	if (!(_XimAttributeToValue((Xic) im->private.local.current_ic,
				   res, &buf[2], buf[1], p->value, mode)))
	    return p->name;
    }
    return (char *)NULL;
}