_XimEncodeSavedIMATTRIBUTE(
    Xim			 im,
    XIMResourceList	 res_list,
    unsigned int	 res_num,
    int			*idx,
    char		*buf,
    int			 size,
    int			*ret_len,
    XPointer		 top,
    unsigned long	 mode)
{
    register int	 i;
    int			 num = im->private.proto.num_saved_imvalues;
    XrmQuark		*quark_list = im->private.proto.saved_imvalues;
    XIMResourceList	 res;
    XPointer		 value;
    CARD16		*buf_s;
    int			 len;
    int			 min_len = sizeof(CARD16) /* sizeof attribute ID */
				 + sizeof(INT16); /* sizeof value length */

    if (!im->private.proto.saved_imvalues) {
	*idx = -1;
	*ret_len = 0;
	return True;
    }

    *ret_len = 0;
    for (i = *idx; i < num; i++) {
	if (!(res = _XimGetResourceListRecByQuark(res_list,
						res_num, quark_list[i])))
	    continue;

	if (!_XimDecodeLocalIMAttr(res, top, value))
	    return False;

	buf_s = (CARD16 *)buf;
	if (!(_XimValueToAttribute(res, (XPointer)&buf_s[2],
			(size - min_len), value, &len, mode, (XPointer)NULL)))
	    return False;

	if (len == 0) {
	    continue;
	} else if (len < 0) {
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