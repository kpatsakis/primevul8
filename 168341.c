_XimCheckInnerICAttributes(
    Xic			 ic,
    XIMArg		*arg,
    unsigned long	 mode)
{
    XIMResourceList	 res;
    int			 check;

    if (!(res = _XimGetResourceListRec(ic->private.proto.ic_inner_resources,
			ic->private.proto.ic_num_inner_resources, arg->name)))
	return False;

    check = _XimCheckICMode(res, mode);
    if(check == XIM_CHECK_INVALID)
	return True;
    else if(check == XIM_CHECK_ERROR)
	return False;

    return True;
}