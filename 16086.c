bool gx_color_info_equal(const gx_device_color_info * p1, const gx_device_color_info * p2)
{
    if (p1->anti_alias.graphics_bits != p2->anti_alias.graphics_bits)
        return false;
    if (p1->anti_alias.text_bits != p2->anti_alias.text_bits)
        return false;
    if (p1->black_component != p2->black_component)
        return false;
    if (strcmp(p1->cm_name, p2->cm_name) != 0)
        return false;
    if (p1->depth != p2->depth)
        return false;
    if (p1->dither_colors != p2->dither_colors)
        return false;
    if (p1->dither_grays != p2->dither_grays)
        return false;
    if (p1->gray_index != p2->gray_index)
        return false;
    if (p1->max_color != p2->max_color)
        return false;
    if (p1->max_components != p2->max_components)
        return false;
    if (p1->opmode != p2->opmode)
        return false;
    if (p1->polarity != p2->polarity)
        return false;
    if (p1->process_comps != p2->process_comps)
        return false;
    if (p1->separable_and_linear != p2->separable_and_linear)
        return false;
    if (p1->use_antidropout_downscaler != p2->use_antidropout_downscaler)
        return false;
    return true;
}