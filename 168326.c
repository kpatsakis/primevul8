_XimEncodeTopValue(
    Xic			 ic,
    XIMResourceList	 res,
    XIMArg		*p)
{
    if (res->xrm_name == XrmStringToQuark(XNClientWindow)) {
	ic->core.client_window = (Window)p->value;
	if (ic->core.focus_window == (Window)0)
	    ic->core.focus_window = ic->core.client_window;
	_XimRegisterFilter(ic);

    } else if (res->xrm_name == XrmStringToQuark(XNFocusWindow)) {
	if (ic->core.client_window) {
	    _XimUnregisterFilter(ic);
	    ic->core.focus_window = (Window)p->value;
	    _XimRegisterFilter(ic);
	} else /* client_window not yet */
	    ic->core.focus_window = (Window)p->value;
    }
    return True;
}