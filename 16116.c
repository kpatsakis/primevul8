gx_device_copy_color_procs(gx_device *dev, const gx_device *target)
{
    dev_proc_map_cmyk_color((*from_cmyk)) =
        dev_proc(dev, map_cmyk_color);
    dev_proc_map_rgb_color((*from_rgb)) =
        dev_proc(dev, map_rgb_color);
    dev_proc_map_color_rgb((*to_rgb)) =
        dev_proc(dev, map_color_rgb);

    /* The logic in this function seems a bit stale; it sets the
       old-style color procs, but not the new ones
       (get_color_mapping_procs, get_color_comp_index, encode_color,
       and decode_color). It should probably copy those as well.
    */
    if (from_cmyk == gx_forward_map_cmyk_color ||
        from_cmyk == cmyk_1bit_map_cmyk_color ||
        from_cmyk == cmyk_8bit_map_cmyk_color) {
        from_cmyk = dev_proc(target, map_cmyk_color);
        set_dev_proc(dev, map_cmyk_color,
                     (from_cmyk == cmyk_1bit_map_cmyk_color ||
                      from_cmyk == cmyk_8bit_map_cmyk_color ?
                      from_cmyk : gx_forward_map_cmyk_color));
    }
    if (from_rgb == gx_forward_map_rgb_color ||
        from_rgb == gx_default_rgb_map_rgb_color) {
        from_rgb = dev_proc(target, map_rgb_color);
        set_dev_proc(dev, map_rgb_color,
                     (from_rgb == gx_default_rgb_map_rgb_color ?
                      from_rgb : gx_forward_map_rgb_color));
    }
    if (to_rgb == gx_forward_map_color_rgb ||
        to_rgb == cmyk_1bit_map_color_rgb ||
        to_rgb == cmyk_8bit_map_color_rgb) {
        to_rgb = dev_proc(target, map_color_rgb);
        set_dev_proc(dev, map_color_rgb,
                     (to_rgb == cmyk_1bit_map_color_rgb ||
                      to_rgb == cmyk_8bit_map_color_rgb ?
                      to_rgb : gx_forward_map_color_rgb));
    }
}