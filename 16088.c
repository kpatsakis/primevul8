gx_device_enum_ptr(gx_device * dev)
{
    if (dev == 0 || dev->memory == 0)
        return 0;
    return dev;
}