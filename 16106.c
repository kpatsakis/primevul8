gx_device_free_local(gx_device *dev)
{
    gx_device_finalize(dev->memory, dev);
}