_XimEncodeIMATTRIBUTE(
    Xim			  im,
    XIMResourceList	  res_list,
    unsigned int	  res_num,
    XIMArg		 *arg,
    XIMArg		**arg_ret,
    char		 *buf,
    int			  size,
    int			 *ret_len,
    XPointer		  top,
    unsigned long	  mode)
{
    register XIMArg	*p;
    XIMResourceList	 res;
    int			 check;
    CARD16		*buf_s;
    int			 len;
    int			 min_len = sizeof(CARD16) /* sizeof attribute ID */
				 + sizeof(INT16); /* sizeof value length */

    *ret_len = 0;
    for (p = arg; p->name; p++) {
	if (!(res = _XimGetResourceListRec(res_list, res_num, p->name))) {
	    if (_XimSetInnerIMAttributes(im, top, p, mode))
		continue;
	    return p->name;
	}

	check = _XimCheckIMMode(res, mode);
	if (check == XIM_CHECK_INVALID)
	    continue;
	else if (check == XIM_CHECK_ERROR)
	    return p->name;

	if (!(_XimEncodeLocalIMAttr(res, top, p->value)))
	    return p->name;

	buf_s = (CARD16 *)buf;
	if (!(_XimValueToAttribute(res, (XPointer)&buf_s[2], (size - min_len),
				p->value, &len, mode, (XPointer)NULL)))
	    return p->name;

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