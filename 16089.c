gs_setdevice_no_init(gs_gstate * pgs, gx_device * dev)
{
    /*
     * Just set the device, possibly changing color space but no other
     * device parameters.
     *
     * Make sure we don't close the device if dev == pgs->device
     * This could be done by allowing the rc_assign to close the
     * old 'dev' if the rc goes to 0 (via the device structure's
     * finalization procedure), but then the 'code' from the dev
     * closedevice would not be propagated up. We want to allow
     * the code to be handled, particularly for the pdfwrite
     * device.
     */
    if (pgs->device != NULL && pgs->device->rc.ref_count == 1 &&
        pgs->device != dev) {
        int code = gs_closedevice(pgs->device);

        if (code < 0)
            return code;
    }
    rc_assign(pgs->device, dev, "gs_setdevice_no_init");
    gs_gstate_update_device(pgs, dev);
    return pgs->overprint ? gs_do_set_overprint(pgs) : 0;
}