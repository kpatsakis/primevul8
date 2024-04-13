_XimMakeICAttrIDList(
    Xic			 ic,
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
    XrmQuark		 pre_quark;
    XrmQuark		 sts_quark;
    char		*name;
    INT16		 new_len;

    *len = 0;
    if (!arg)
	return (char *)NULL;

    pre_quark = XrmStringToQuark(XNPreeditAttributes);
    sts_quark = XrmStringToQuark(XNStatusAttributes);

    for (p = arg; p && p->name; p++) {
	if (!(res = _XimGetResourceListRec(res_list, res_num, p->name))) {
	    if (_XimCheckInnerICAttributes(ic, p, mode))
		continue;
	    *len = -1;
	    return p->name;
	}

	check = _XimCheckICMode(res, mode);
	if(check == XIM_CHECK_INVALID)
	    continue;
	else if(check == XIM_CHECK_ERROR) {
	    *len = -1;
	    return p->name;
	}

	*buf = res->id;
	*len += sizeof(CARD16);
	buf++;
	if (res->resource_size == XimType_NEST) {
	    if (res->xrm_name == pre_quark) {
		if ((name = _XimMakeICAttrIDList(ic, res_list, res_num,
				(XIMArg *)p->value, buf, &new_len,
				(mode | XIM_PREEDIT_ATTR)))) {
		    if (new_len < 0) *len = -1;
		    else *len += new_len;
		    return name;
		}
		*len += new_len;
		buf = (CARD16 *)((char *)buf + new_len);
	    } else if (res->xrm_name == sts_quark) {
		if ((name = _XimMakeICAttrIDList(ic, res_list, res_num,
				(XIMArg *)p->value, buf, &new_len,
				(mode | XIM_STATUS_ATTR)))) {
		    if (new_len < 0) *len = -1;
		    else *len += new_len;
		    return name;
		}
		*len += new_len;
		buf = (CARD16 *)((char *)buf + new_len);
	    }

	    if (!(res = _XimGetNestedListSeparator(res_list, res_num))) {
		p++;
		if (p) {
		    *len = -1;
		    return p->name;
		}
		else {
		    return (char *)NULL;
		}
	    }
	    *buf = res->id;
	    *len += sizeof(CARD16);
	    buf++;
	}
    }
    return (char *)NULL;
}