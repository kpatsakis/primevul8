_XimDecodeInnerIMATTRIBUTE(
    Xim			 im,
    XIMArg		*arg)
{
    XIMResourceList	 res;
    XimDefIMValues	 im_values;

    if (!(res = _XimGetResourceListRec(im->private.proto.im_inner_resources,
			im->private.proto.im_num_inner_resources, arg->name)))
	return False;

    _XimGetCurrentIMValues(im, &im_values);
    return _XimDecodeLocalIMAttr(res, (XPointer)&im_values, arg->value);
}