gs_flushpage(gs_gstate * pgs)
{
    gx_device *dev = gs_currentdevice(pgs);

    return (*dev_proc(dev, sync_output)) (dev);
}