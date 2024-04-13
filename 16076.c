gs_copydevice(gx_device ** pnew_dev, const gx_device * dev, gs_memory_t * mem)
{
    return gs_copydevice2(pnew_dev, dev, false, mem);
}