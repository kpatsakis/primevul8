_XimValueToAttribute(
    XIMResourceList	 res,
    XPointer		 buf,
    int			 buf_size,
    XPointer		 value,
    int			*len,
    unsigned long	 mode,
    XPointer		 param)
{
    int			 ret_len;

    switch (res->resource_size) {
    case XimType_SeparatorOfNestedList:
    case XimType_NEST:
	*len = 0;
	break;

    case XimType_CARD8:
	ret_len = sizeof(CARD8);
	if (buf_size < ret_len + XIM_PAD(ret_len)) {
	    *len = -1;
	    return False;
	}

	*((CARD8 *)buf) = (CARD8)(long)value;
	*len = ret_len;
	break;

    case XimType_CARD16:
	ret_len = sizeof(CARD16);
	if (buf_size < ret_len + XIM_PAD(ret_len)) {
	    *len = -1;
	    return False;
	}

	*((CARD16 *)buf) = (CARD16)(long)value;
	*len = ret_len;
	break;

    case XimType_CARD32:
    case XimType_Window:
    case XimType_XIMHotKeyState:
	ret_len = sizeof(CARD32);
	if (buf_size < ret_len + XIM_PAD(ret_len)) {
	    *len = -1;
	    return False;
	}

	*((CARD32 *)buf) = (CARD32)(long)value;
	*len = ret_len;
	break;

    case XimType_STRING8:
	if (!value) {
	    *len = 0;
	    return False;
	}

	ret_len = strlen((char *)value);
	if (buf_size < ret_len + XIM_PAD(ret_len)) {
	    *len = -1;
	    return False;
	}

	(void)memcpy((char *)buf, (char *)value, ret_len);
	*len = ret_len;
	break;

    case XimType_XRectangle:
	{
	    XRectangle	*rect = (XRectangle *)value;
	    CARD16	*buf_s = (CARD16 *)buf;

	    if (!rect) {
		*len = 0;
		return False;
	    }

	    ret_len = sizeof(INT16)		/* sizeof X */
	    	    + sizeof(INT16)		/* sizeof Y */
	            + sizeof(CARD16)		/* sizeof width */
	            + sizeof(CARD16);		/* sizeof height */
	    if (buf_size < ret_len + XIM_PAD(ret_len)) {
		*len = -1;
		return False;
	    }

	    buf_s[0] = (CARD16)rect->x;		/* X */
	    buf_s[1] = (CARD16)rect->y;		/* Y */
	    buf_s[2] = (CARD16)rect->width;	/* width */
	    buf_s[3] = (CARD16)rect->height;	/* heght */
	    *len = ret_len;
	    break;
	}

    case XimType_XPoint:
	{
	    XPoint	*point = (XPoint *)value;
	    CARD16	*buf_s = (CARD16 *)buf;

	    if (!point) {
		*len = 0;
		return False;
	    }

	    ret_len = sizeof(INT16)		/* sizeof X */
	            + sizeof(INT16);		/* sizeof Y */
	    if (buf_size < ret_len + XIM_PAD(ret_len)) {
		*len = -1;
		return False;
	    }

	    buf_s[0] = (CARD16)point->x;		/* X */
	    buf_s[1] = (CARD16)point->y;		/* Y */
	    *len = ret_len;
	    break;
	}

    case XimType_XFontSet:
	{
	    XFontSet	 font = (XFontSet)value;
	    Xic		 ic = (Xic)param;
	    char	*base_name = NULL;
	    int		 length = 0;
	    CARD16	*buf_s = (CARD16 *)buf;

	    if (!font) {
		*len = 0;
		return False;
	    }

	    if (mode & XIM_PREEDIT_ATTR) {
		base_name = ic->private.proto.preedit_font;
		length	  = ic->private.proto.preedit_font_length;
	    } else if (mode & XIM_STATUS_ATTR) {
		base_name = ic->private.proto.status_font;
		length	  = ic->private.proto.status_font_length;
	    }

	    if (!base_name) {
		*len = 0;
		return False;
	    }

	    ret_len = sizeof(CARD16)		/* sizeof length of Base name */
		    + length;			/* sizeof Base font name list */
	    if (buf_size < ret_len + XIM_PAD(ret_len)) {
		*len = -1;
		return False;
	    }

	    buf_s[0] = (INT16)length;		/* length of Base font name */
	    (void)memcpy((char *)&buf_s[1], base_name, length);
						/* Base font name list */
	    *len = ret_len;
	    break;
	}

    case XimType_XIMHotKeyTriggers:
	{
	    XIMHotKeyTriggers	*hotkey = (XIMHotKeyTriggers *)value;
	    INT32		 num;
	    CARD32		*buf_l = (CARD32 *)buf;
	    register CARD32	*key = (CARD32 *)&buf_l[1];
	    register int	 i;

	    if (!hotkey) {
		*len = 0;
		return False;
	    }
	    num = (INT32)hotkey->num_hot_key;

	    ret_len = sizeof(INT32)		/* sizeof number of key list */
	           + (sizeof(CARD32)		/* sizeof keysyn */
	           +  sizeof(CARD32)		/* sizeof modifier */
	           +  sizeof(CARD32))		/* sizeof modifier_mask */
	           *  num;			/* number of key list */
	    if (buf_size < ret_len + XIM_PAD(ret_len)) {
		*len = -1;
		return False;
	    }

	    buf_l[0] = num;		/* number of key list */
	    for (i = 0; i < num; i++, key += 3) {
		key[0] = (CARD32)(hotkey->key[i].keysym);
						/* keysym */
		key[1] = (CARD32)(hotkey->key[i].modifier);
						/* modifier */
		key[2] = (CARD32)(hotkey->key[i].modifier_mask);
						/* modifier_mask */
	    }
	    *len = ret_len;
	    break;
	}

    case XimType_XIMStringConversion:
	{
	    *len = 0;
	    break;
	}

    default:
	return False;
    }
    return True;
}