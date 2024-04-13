gx_device_reloc_ptr(gx_device * dev, gc_state_t * gcst)
{
    if (dev == 0 || dev->memory == 0)
        return dev;
    return RELOC_OBJ(dev);	/* gcst implicit */
}