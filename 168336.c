_XimEncodeICATTRIBUTE(
    Xic			  ic,
    XIMResourceList	  res_list,
    unsigned int	  res_num,
    XIMArg		 *arg,
    XIMArg		**arg_ret,
    char		 *buf,
    int			  size,
    int			 *ret_len,
    XPointer		  top,
    BITMASK32		 *flag,
    unsigned long	  mode)
{
    register XIMArg	*p;
    XIMResourceList	 res;
    int			 check;
    CARD16		*buf_s;
    int			 len;
    int			 min_len = sizeof(CARD16) /* sizeof attribute ID */
				 + sizeof(INT16); /* sizeof value length */
    XrmQuark		 pre_quark;
    XrmQuark		 sts_quark;
    char		*name;

    pre_quark = XrmStringToQuark(XNPreeditAttributes);
    sts_quark = XrmStringToQuark(XNStatusAttributes);

    *ret_len = 0;
    for (p = arg; p && p->name; p++) {
	buf_s = (CARD16 *)buf;
	if (!(res = _XimGetResourceListRec(res_list, res_num, p->name))) {
	    if (_XimSetInnerICAttributes(ic, top, p, mode))
		continue;
	    return p->name;
	}

	check = _XimCheckICMode(res, mode);
	if (check == XIM_CHECK_INVALID)
	    continue;
	else if (check == XIM_CHECK_ERROR)
	    return p->name;

	if (mode & XIM_PREEDIT_ATTR) {
	    if (!(_XimEncodePreeditValue(ic, res, p)))
		return p->name;
	} else if (mode & XIM_STATUS_ATTR) {
	    if (!(_XimEncodeStatusValue(ic, res, p)))
		return p->name;
	} else {
	    if (!(_XimEncodeTopValue(ic, res, p)))
		return p->name;
	}

	if (res->resource_size == XimType_NEST) {
	    XimDefICValues	*ic_attr = (XimDefICValues *)top;

	    if (res->xrm_name == pre_quark) {
		XIMArg		*arg_rt;
		if ((name = _XimEncodeICATTRIBUTE(ic, res_list, res_num,
				(XIMArg *)p->value, &arg_rt,
				(char *)&buf_s[2], (size - min_len),
				 &len, (XPointer)&ic_attr->preedit_attr, flag,
				(mode | XIM_PREEDIT_ATTR)))) {
		    return name;
		}

	    } else if (res->xrm_name == sts_quark) {
		XIMArg		*arg_rt;
		if ((name = _XimEncodeICATTRIBUTE(ic, res_list, res_num,
				(XIMArg *)p->value,  &arg_rt,
				(char *)&buf_s[2], (size - min_len),
				 &len, (XPointer)&ic_attr->status_attr, flag,
				(mode | XIM_STATUS_ATTR)))) {
		    return name;
		}
	    }
	} else {
#ifdef EXT_MOVE
	    if (flag)
		*flag |= _XimExtenArgCheck(p);
#endif
    	    if (!(_XimEncodeLocalICAttr(ic, res, top, p, mode)))
		return p->name;

	    if (!(_XimValueToAttribute(res, (XPointer)&buf_s[2],
			 	(size - min_len), p->value,
				&len, mode, (XPointer)ic)))
		return p->name;
	}

	if (len == 0) {
	    continue;
	} else if (len < 0) {
	    *arg_ret = p;
	    return (char *)NULL;
	}

	buf_s[0] = res->id;			/* attribute ID */
	buf_s[1] = len;				/* value length */
	XIM_SET_PAD(&buf_s[2], len);		/* pad */
	len += min_len;

	buf += len;
	*ret_len += len;
	size -= len;
    }
    *arg_ret = (XIMArg *)NULL;
    return (char *)NULL;
}