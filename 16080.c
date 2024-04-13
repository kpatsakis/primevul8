gx_device_set_media_from_hwsize(gx_device *dev)
{
    int rot = (dev->LeadingEdge & 1);
    double x = dev->width * 72.0 / dev->HWResolution[0];
    double y = dev->height * 72.0 / dev->HWResolution[1];

    if (rot) {
        dev->MediaSize[1] = x;
        dev->MediaSize[0] = y;
    } else {
        dev->MediaSize[0] = x;
        dev->MediaSize[1] = y;
    }
}