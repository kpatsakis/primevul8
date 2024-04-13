gs_setdevice(gs_gstate * pgs, gx_device * dev)
{
    int code = gs_setdevice_no_erase(pgs, dev);

    if (code == 1)
        code = gs_erasepage(pgs);
    return code;
}