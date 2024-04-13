gx_device_raster_plane(const gx_device * dev, const gx_render_plane_t *render_plane)
{
    ulong bpc = (render_plane && render_plane->index >= 0 ?
        render_plane->depth : dev->color_info.depth/(dev->is_planar ? dev->color_info.num_components : 1));
    ulong bits = (ulong) dev->width * bpc;
    int l2align;

    l2align = dev->log2_align_mod;
    if (l2align < log2_align_bitmap_mod)
        l2align = log2_align_bitmap_mod;
    return (uint)(((bits + (8 << l2align) - 1) >> (l2align + 3)) << l2align);
}