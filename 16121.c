gs_make_null_device(gx_device_null *dev_null, gx_device *dev,
                    gs_memory_t * mem)
{
    gx_device_init((gx_device *)dev_null, (const gx_device *)&gs_null_device,
                   mem, true);
    gx_device_set_target((gx_device_forward *)dev_null, dev);
    if (dev) {
        /* The gx_device_copy_color_params() call below should
           probably copy over these new-style color mapping procs, as
           well as the old-style (map_rgb_color and friends). However,
           the change was made here instead, to minimize the potential
           impact of the patch.
        */
        gx_device *dn = (gx_device *)dev_null;
        set_dev_proc(dn, get_color_mapping_procs, gx_forward_get_color_mapping_procs);
        set_dev_proc(dn, get_color_comp_index, gx_forward_get_color_comp_index);
        set_dev_proc(dn, encode_color, gx_forward_encode_color);
        set_dev_proc(dn, decode_color, gx_forward_decode_color);
        set_dev_proc(dn, get_profile, gx_forward_get_profile);
        set_dev_proc(dn, set_graphics_type_tag, gx_forward_set_graphics_type_tag);
        set_dev_proc(dn, begin_transparency_group, gx_default_begin_transparency_group);
        set_dev_proc(dn, end_transparency_group, gx_default_end_transparency_group);
        set_dev_proc(dn, begin_transparency_mask, gx_default_begin_transparency_mask);
        set_dev_proc(dn, end_transparency_mask, gx_default_end_transparency_mask);
        set_dev_proc(dn, discard_transparency_layer, gx_default_discard_transparency_layer);
        set_dev_proc(dn, pattern_manage, gx_default_pattern_manage);
        set_dev_proc(dn, push_transparency_state, gx_default_push_transparency_state);
        set_dev_proc(dn, pop_transparency_state, gx_default_pop_transparency_state);
        set_dev_proc(dn, put_image, gx_default_put_image);
        set_dev_proc(dn, copy_planes, gx_default_copy_planes);
        set_dev_proc(dn, copy_alpha_hl_color, gx_default_no_copy_alpha_hl_color);
        dn->graphics_type_tag = dev->graphics_type_tag;	/* initialize to same as target */
        gx_device_copy_color_params(dn, dev);
    }
}