gx_device_make_struct_type(gs_memory_struct_type_t *st,
                           const gx_device *dev)
{
    if (dev->stype)
        *st = *dev->stype;
    else if (dev_proc(dev, get_xfont_procs) == gx_forward_get_xfont_procs)
        *st = st_device_forward;
    else
        *st = st_device;
    st->ssize = dev->params_size;
}