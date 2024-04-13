gx_device_copy_color_params(gx_device *dev, const gx_device *target)
{
        COPY_PARAM(color_info);
        COPY_PARAM(cached_colors);
        gx_device_copy_color_procs(dev, target);
}