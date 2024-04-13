gx_device_adjust_resolution(gx_device * dev,
                            int actual_width, int actual_height, int fit)
{
    double width_ratio = (double)actual_width / dev->width;
    double height_ratio = (double)actual_height / dev->height;
    double ratio =
    (fit ? min(width_ratio, height_ratio) :
     max(width_ratio, height_ratio));

    dev->HWResolution[0] *= ratio;
    dev->HWResolution[1] *= ratio;
    gx_device_set_width_height(dev, actual_width, actual_height);
    return 0;
}