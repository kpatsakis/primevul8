gx_device_set_media_size(gx_device * dev, double media_width, double media_height)
{
    dev->MediaSize[0] = media_width;
    dev->MediaSize[1] = media_height;
    gx_device_set_hwsize_from_media(dev);
}