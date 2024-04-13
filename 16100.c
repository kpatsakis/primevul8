gs_copydevice2(gx_device ** pnew_dev, const gx_device * dev, bool keep_open,
               gs_memory_t * mem)
{
    gx_device *new_dev;
    const gs_memory_struct_type_t *std = dev->stype;
    const gs_memory_struct_type_t *new_std;
    gs_memory_struct_type_t *a_std = 0;
    int code;

    if (dev->stype_is_dynamic) {
        /*
         * We allocated the stype for this device previously.
         * Just allocate a new stype and copy the old one into it.
         */
        a_std = (gs_memory_struct_type_t *)
            gs_alloc_bytes_immovable(mem->non_gc_memory, sizeof(*std),
                                     "gs_copydevice(stype)");
        if (!a_std)
            return_error(gs_error_VMerror);
        *a_std = *std;
        new_std = a_std;
    } else if (std != 0 && std->ssize == dev->params_size) {
        /* Use the static stype. */
        new_std = std;
    } else {
        /* We need to figure out or adjust the stype. */
        a_std = (gs_memory_struct_type_t *)
            gs_alloc_bytes_immovable(mem->non_gc_memory, sizeof(*std),
                                     "gs_copydevice(stype)");
        if (!a_std)
            return_error(gs_error_VMerror);
        gx_device_make_struct_type(a_std, dev);
        new_std = a_std;
    }
    /*
     * Because command list devices have complicated internal pointer
     * structures, we allocate all device instances as immovable.
     */
    new_dev = gs_alloc_struct_immovable(mem, gx_device, new_std,
                                        "gs_copydevice(device)");
    if (new_dev == 0) {
        gs_free_object(mem->non_gc_memory, a_std, "gs_copydevice(stype)");
        return_error(gs_error_VMerror);
    }
    gx_device_init(new_dev, dev, mem, false);
    gx_device_set_procs(new_dev);
    new_dev->stype = new_std;
    new_dev->stype_is_dynamic = new_std != std;
    /*
     * keep_open is very dangerous.  On the other hand, so is copydevice in
     * general, since it just copies the bits without any regard to pointers
     * (including self-pointers) that they may contain.  We handle this by
     * making the default finish_copydevice forbid copying of anything other
     * than the device prototype.
     */
    new_dev->is_open = dev->is_open && keep_open;
    fill_dev_proc(new_dev, finish_copydevice, gx_default_finish_copydevice);
    code = dev_proc(new_dev, finish_copydevice)(new_dev, dev);
    if (code < 0) {
        gs_free_object(mem, new_dev, "gs_copydevice(device)");
#if 0 /* gs_free_object above calls gx_device_finalize,
         which closes the device and releaszes its stype, i.e. a_std. */
        if (a_std)
            gs_free_object(dev->memory->non_gc_memory, a_std, "gs_copydevice(stype)");
#endif
        return code;
    }
    *pnew_dev = new_dev;
    return 0;
}