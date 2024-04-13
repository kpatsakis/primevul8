gx_device_set_width_height(gx_device * dev, int width, int height)
{
    dev->width = width;
    dev->height = height;
    gx_device_set_media_from_hwsize(dev);
}