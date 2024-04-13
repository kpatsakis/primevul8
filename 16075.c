gx_device_set_hwsize_from_media(gx_device *dev)
{
    int rot = (dev->LeadingEdge & 1);
    double rot_media_x = rot ? dev->MediaSize[1] : dev->MediaSize[0];
    double rot_media_y = rot ? dev->MediaSize[0] : dev->MediaSize[1];

    dev->width = (int)(rot_media_x * dev->HWResolution[0] / 72.0 + 0.5);
    dev->height = (int)(rot_media_y * dev->HWResolution[1] / 72.0 + 0.5);
}