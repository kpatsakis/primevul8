_XimCheckInnerIMAttributes(
    Xim			 im,
    XIMArg		*arg,
    unsigned long	 mode)
{
    XIMResourceList	 res;
    int			 check;

    if (!(res = _XimGetResourceListRec(im->private.proto.im_inner_resources,
			im->private.proto.im_num_inner_resources, arg->name)))
	return False;

    check = _XimCheckIMMode(res, mode);
    if(check == XIM_CHECK_INVALID)
	return True;
    else if(check == XIM_CHECK_ERROR)
	return False;

    return True;
}