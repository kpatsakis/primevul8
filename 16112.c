gs_output_page(gs_gstate * pgs, int num_copies, int flush)
{
    gx_device *dev = gs_currentdevice(pgs);
    cmm_dev_profile_t *dev_profile;
    int code;

    /* for devices that hook 'fill_path' in order to pick up gs_gstate */
    /* values such as dev_ht (such as tiffsep1), make a dummy call here   */
    /* to make sure that it has been called at least once		  */
    code = gs_gsave(pgs);
    if (code < 0)
        return code;
    if (((code = gs_newpath(pgs)) < 0) ||
        ((code = gs_moveto(pgs, 0.0, 0.0)) < 0) ||
    ((code = gs_setgray(pgs, 0.0)) < 0) ||
    ((code = gs_fill(pgs)) < 0))
    {
        gs_grestore(pgs);
	return code;
    }
    code = gs_grestore(pgs);
    if (code < 0)
        return code;

    if (dev->IgnoreNumCopies)
        num_copies = 1;
    if ((code = (*dev_proc(dev, output_page)) (dev, num_copies, flush)) < 0)
        return code;

    code = dev_proc(dev, get_profile)(dev, &(dev_profile));
    if (code < 0)
        return code;
    if (dev_profile->graydetection && !dev_profile->pageneutralcolor) {
        dev_profile->pageneutralcolor = true;             /* start detecting again */
        code = gsicc_mcm_begin_monitor(pgs->icc_link_cache, dev);
    }
    return code;
}