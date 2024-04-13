gs_gstate_update_device(gs_gstate *pgs, gx_device *dev)
{
    gx_set_cmap_procs(pgs, dev);
    gx_unset_both_dev_colors(pgs);
}