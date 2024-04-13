gx_set_device_only(gs_gstate * pgs, gx_device * dev)
{
    rc_assign(pgs->device, dev, "gx_set_device_only");
}