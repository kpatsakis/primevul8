gx_device_raster(const gx_device * dev, bool pad)
{
    ulong bits = (ulong) dev->width * dev->color_info.depth;
    ulong raster;
    int l2align;

    if (dev->is_planar)
        bits /= dev->color_info.num_components;

    raster = (uint)((bits + 7) >> 3);
    if (!pad)
        return raster;
    l2align = dev->log2_align_mod;
    if (l2align < log2_align_bitmap_mod)
        l2align = log2_align_bitmap_mod;
    return (uint)(((bits + (8 << l2align) - 1) >> (l2align + 3)) << l2align);
}