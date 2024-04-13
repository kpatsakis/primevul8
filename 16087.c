bool gs_is_null_device(gx_device *dev)
{
    /* Assuming null_fill_path isn't used elswhere. */
    return dev->procs.fill_path == gs_null_device.procs.fill_path;
}