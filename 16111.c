gx_device_set_resolution(gx_device * dev, double x_dpi, double y_dpi)
{
    dev->HWResolution[0] = x_dpi;
    dev->HWResolution[1] = y_dpi;
    gx_device_set_hwsize_from_media(dev);
}