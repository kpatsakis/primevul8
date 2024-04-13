_XimEncodeSavedICATTRIBUTE(
    Xic			 ic,
    XIMResourceList	 res_list,
    unsigned int	 res_num,
    int			*idx,
    char		*buf,
    int			 size,
    int			*ret_len,
    XPointer		 top,
    unsigned long	 mode)
{
    int			 i;
    int			 num = ic->private.proto.num_saved_icvalues;
    XrmQuark		*quark_list = ic->private.proto.saved_icvalues;
    XIMResourceList	 res;
    XPointer		 value;
    CARD16		*buf_s;
    int			 len;
    int			 min_len = sizeof(CARD16) /* sizeof attribute ID */
				 + sizeof(INT16); /* sizeof value length */
    XrmQuark		 pre_quark;
    XrmQuark		 sts_quark;
    XrmQuark		 separator;

    if (!ic->private.proto.saved_icvalues) {
	*idx = -1;
	*ret_len = 0;
	return True;
    }

    pre_quark = XrmStringToQuark(XNPreeditAttributes);
    sts_quark = XrmStringToQuark(XNStatusAttributes);
    separator = XrmStringToQuark(XNSeparatorofNestedList);

    *ret_len = 0;
    for (i = *idx; i < num; i++) {
	if (quark_list[i] == separator) {
	    *idx = i;
	    return True;
	}

	if (!(res = _XimGetResourceListRecByQuark(res_list,
						res_num, quark_list[i])))
	    continue;

	if (!_XimDecodeLocalICAttr(res, top,(XPointer)&value, mode))
	    return False;

	if (mode & XIM_PREEDIT_ATTR) {
	    if (!(_XimEncodeSavedPreeditValue(ic, res, value))) {
		return False;
	    }
	} else if (mode & XIM_STATUS_ATTR) {
	    if (!(_XimEncodeSavedStatusValue(ic, res, value))) {
		return False;
	    }
	}

	buf_s = (CARD16 *)buf;
	if (res->resource_size == XimType_NEST) {
	    XimDefICValues	*ic_attr = (XimDefICValues *)top;

	    i++;
	    if (res->xrm_name == pre_quark) {
		if (!_XimEncodeSavedICATTRIBUTE(ic, res_list, res_num,
				 &i, (char *)&buf_s[2], (size - min_len),
				 &len, (XPointer)&ic_attr->preedit_attr,
				(mode | XIM_PREEDIT_ATTR))) {
		    return False;
		}

	    } else if (res->xrm_name == sts_quark) {
		if (!_XimEncodeSavedICATTRIBUTE(ic, res_list, res_num,
				&i, (char *)&buf_s[2], (size - min_len),
				&len, (XPointer)&ic_attr->status_attr,
				(mode | XIM_STATUS_ATTR))) {
		    return False;
		}
	    }
	} else {
	    if (!(_XimValueToAttribute(res, (XPointer)&buf_s[2],
			 	(size - min_len), value,
				&len, mode, (XPointer)ic))) {
		return False;
	    }
	}

	if (len == 0) {
	    continue;
	} else if (len < 0) {
	    if (quark_list[i] == separator)
		i++;
	    *idx = i;
	    return True;
	}

	buf_s[0] = res->id;			/* attribute ID */
	buf_s[1] = len;				/* value length */
	XIM_SET_PAD(&buf_s[2], len);		/* pad */
	len += min_len;

	buf += len;
	*ret_len += len;
	size -= len;
    }
    *idx = -1;
    return True;
}