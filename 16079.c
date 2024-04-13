gx_device_copy_params(gx_device *dev, const gx_device *target)
{
#define COPY_ARRAY_PARAM(p) memcpy(dev->p, target->p, sizeof(dev->p))
        COPY_PARAM(width);
        COPY_PARAM(height);
        COPY_ARRAY_PARAM(MediaSize);
        COPY_ARRAY_PARAM(ImagingBBox);
        COPY_PARAM(ImagingBBox_set);
        COPY_ARRAY_PARAM(HWResolution);
        COPY_ARRAY_PARAM(Margins);
        COPY_ARRAY_PARAM(HWMargins);
        COPY_PARAM(PageCount);
        COPY_PARAM(MaxPatternBitmap);
#undef COPY_ARRAY_PARAM
        gx_device_copy_color_params(dev, target);
}