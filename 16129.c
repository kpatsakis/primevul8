gs_gstate_putdeviceparams(gs_gstate *pgs, gx_device *dev, gs_param_list *plist)
{
    int code;
    gx_device *dev2;

    if (dev)
       dev2 = dev;
    else
       dev2 = pgs->device;

    code = gs_putdeviceparams(dev2, plist);
    if (code >= 0)
        gs_gstate_update_device(pgs, dev2);
    return code;
}