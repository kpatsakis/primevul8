_XimEncodeStatusValue(
    Xic			 ic,
    XIMResourceList	 res,
    XIMArg		*p)
{
    if (res->xrm_name == XrmStringToQuark(XNStdColormap)) {
	XStandardColormap	*colormap_ret = NULL;
	int			 count;

	if (!(XGetRGBColormaps(ic->core.im->core.display,
				ic->core.focus_window, &colormap_ret,
				&count, (Atom)p->value)))
	    return False;

	XFree(colormap_ret);
    } else if (res->xrm_name == XrmStringToQuark(XNFontSet)) {
	int		  list_ret;
	XFontStruct	**struct_list;
	char		**name_list;
	char		 *tmp;
	int		  len;
	register int	  i;

	if (!p->value)
	    return False;

	Xfree(ic->private.proto.status_font);

	list_ret = XFontsOfFontSet((XFontSet)p->value,
						 &struct_list, &name_list);
	for (i = 0, len = 0; i < list_ret; i++) {
	     len += (strlen(name_list[i]) + sizeof(char));
	}
	if (!(tmp = Xmalloc(len+1))) {
	    ic->private.proto.status_font = NULL;
	    return False;
	}

	tmp[0] = '\0';
	for(i = 0; i < list_ret; i++) {
	    strcat(tmp, name_list[i]);
	    strcat(tmp, ",");
	}
	tmp[len - 1] = 0;
	ic->private.proto.status_font        = tmp;
	ic->private.proto.status_font_length = len - 1;
    }
    return True;
}