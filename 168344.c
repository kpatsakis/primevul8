_XimDecodeInnerICATTRIBUTE(
    Xic			 ic,
    XIMArg		*arg,
    unsigned long	 mode)
{
    XIMResourceList	 res;
    XimDefICValues	 ic_values;

    if (!(res = _XimGetResourceListRec(ic->private.proto.ic_inner_resources,
			ic->private.proto.ic_num_inner_resources, arg->name)))
	return False;

    _XimGetCurrentICValues(ic, &ic_values);
    if (!_XimDecodeLocalICAttr(res, (XPointer)&ic_values, arg->value, mode))
	return False;
    _XimSetCurrentICValues(ic, &ic_values);
    return True;
}