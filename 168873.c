xps_parse_glyphs(xps_context_t *ctx,
        char *base_uri, xps_resource_t *dict, xps_item_t *root)
{
    xps_item_t *node;
    int code;

    char *fill_uri;
    char *opacity_mask_uri;

    char *bidi_level_att;
    /*char *caret_stops_att;*/
    char *fill_att;
    char *font_size_att;
    char *font_uri_att;
    char *origin_x_att;
    char *origin_y_att;
    char *is_sideways_att;
    char *indices_att;
    char *unicode_att;
    char *style_att;
    char *transform_att;
    char *clip_att;
    char *opacity_att;
    char *opacity_mask_att;

    xps_item_t *transform_tag = NULL;
    xps_item_t *clip_tag = NULL;
    xps_item_t *fill_tag = NULL;
    xps_item_t *opacity_mask_tag = NULL;

    char *fill_opacity_att = NULL;

    xps_part_t *part;
    xps_font_t *font;

    char partname[1024];
    char *subfont;

    gs_matrix matrix;
    float font_size = 10.0;
    int subfontid = 0;
    int is_sideways = 0;
    int bidi_level = 0;

    int sim_bold = 0;
    int sim_italic = 0;

    gs_matrix shear = { 1, 0, 0.36397f, 1, 0, 0 }; /* shear by 20 degrees */

    /*
     * Extract attributes and extended attributes.
     */

    bidi_level_att = xps_att(root, "BidiLevel");
    /*caret_stops_att = xps_att(root, "CaretStops");*/
    fill_att = xps_att(root, "Fill");
    font_size_att = xps_att(root, "FontRenderingEmSize");
    font_uri_att = xps_att(root, "FontUri");
    origin_x_att = xps_att(root, "OriginX");
    origin_y_att = xps_att(root, "OriginY");
    is_sideways_att = xps_att(root, "IsSideways");
    indices_att = xps_att(root, "Indices");
    unicode_att = xps_att(root, "UnicodeString");
    style_att = xps_att(root, "StyleSimulations");
    transform_att = xps_att(root, "RenderTransform");
    clip_att = xps_att(root, "Clip");
    opacity_att = xps_att(root, "Opacity");
    opacity_mask_att = xps_att(root, "OpacityMask");

    for (node = xps_down(root); node; node = xps_next(node))
    {
        if (!strcmp(xps_tag(node), "Glyphs.RenderTransform"))
            transform_tag = xps_down(node);

        if (!strcmp(xps_tag(node), "Glyphs.OpacityMask"))
            opacity_mask_tag = xps_down(node);

        if (!strcmp(xps_tag(node), "Glyphs.Clip"))
            clip_tag = xps_down(node);

        if (!strcmp(xps_tag(node), "Glyphs.Fill"))
            fill_tag = xps_down(node);
    }

    fill_uri = base_uri;
    opacity_mask_uri = base_uri;

    xps_resolve_resource_reference(ctx, dict, &transform_att, &transform_tag, NULL);
    xps_resolve_resource_reference(ctx, dict, &clip_att, &clip_tag, NULL);
    xps_resolve_resource_reference(ctx, dict, &fill_att, &fill_tag, &fill_uri);
    xps_resolve_resource_reference(ctx, dict, &opacity_mask_att, &opacity_mask_tag, &opacity_mask_uri);

    /*
     * Check that we have all the necessary information.
     */

    if (!font_size_att || !font_uri_att || !origin_x_att || !origin_y_att)
        return gs_throw(-1, "missing attributes in glyphs element");

    if (!indices_att && !unicode_att)
        return 0; /* nothing to draw */

    if (is_sideways_att)
        is_sideways = !strcmp(is_sideways_att, "true");

    if (bidi_level_att)
        bidi_level = atoi(bidi_level_att);

    /*
     * Find and load the font resource
     */

    xps_absolute_path(partname, base_uri, font_uri_att, sizeof partname);
    subfont = strrchr(partname, '#');
    if (subfont)
    {
        subfontid = atoi(subfont + 1);
        *subfont = 0;
    }

    font = xps_hash_lookup(ctx->font_table, partname);
    if (!font)
    {
        part = xps_read_part(ctx, partname);
        if (!part)
            return gs_throw1(-1, "cannot find font resource part '%s'", partname);

        /* deobfuscate if necessary */
        if (strstr(part->name, ".odttf"))
            xps_deobfuscate_font_resource(ctx, part);
        if (strstr(part->name, ".ODTTF"))
            xps_deobfuscate_font_resource(ctx, part);

        font = xps_new_font(ctx, part->data, part->size, subfontid);
        if (!font)
            return gs_rethrow1(-1, "cannot load font resource '%s'", partname);

        xps_select_best_font_encoding(font);

        xps_hash_insert(ctx, ctx->font_table, part->name, font);

        /* NOTE: we kept part->name in the hashtable and part->data in the font */
        xps_free(ctx, part);
    }

    if (style_att)
    {
        if (!strcmp(style_att, "BoldSimulation"))
            sim_bold = 1;
        else if (!strcmp(style_att, "ItalicSimulation"))
            sim_italic = 1;
        else if (!strcmp(style_att, "BoldItalicSimulation"))
            sim_bold = sim_italic = 1;
    }

    /*
     * Set up graphics state.
     */

    gs_gsave(ctx->pgs);

    if (transform_att || transform_tag)
    {
        gs_matrix transform;

        if (transform_att)
            xps_parse_render_transform(ctx, transform_att, &transform);
        if (transform_tag)
            xps_parse_matrix_transform(ctx, transform_tag, &transform);

        gs_concat(ctx->pgs, &transform);
    }

    if (clip_att || clip_tag)
    {
        if (clip_att)
            xps_parse_abbreviated_geometry(ctx, clip_att);
        if (clip_tag)
            xps_parse_path_geometry(ctx, dict, clip_tag, 0);
        xps_clip(ctx);
    }

    font_size = atof(font_size_att);

    gs_setfont(ctx->pgs, font->font);
    gs_make_scaling(font_size, -font_size, &matrix);
    if (is_sideways)
        gs_matrix_rotate(&matrix, 90.0, &matrix);

    if (sim_italic)
        gs_matrix_multiply(&shear, &matrix, &matrix);

    gs_setcharmatrix(ctx->pgs, &matrix);

    gs_matrix_multiply(&matrix, &font->font->orig_FontMatrix, &font->font->FontMatrix);

    code = xps_begin_opacity(ctx, opacity_mask_uri, dict, opacity_att, opacity_mask_tag, false, false);
    if (code)
    {
        gs_grestore(ctx->pgs);
        return gs_rethrow(code, "cannot create transparency group");
    }

    /*
     * If it's a solid color brush fill/stroke do a simple fill
     */

    if (fill_tag && !strcmp(xps_tag(fill_tag), "SolidColorBrush"))
    {
        fill_opacity_att = xps_att(fill_tag, "Opacity");
        fill_att = xps_att(fill_tag, "Color");
        fill_tag = NULL;
    }

    if (fill_att)
    {
        float samples[XPS_MAX_COLORS];
        gs_color_space *colorspace;

        xps_parse_color(ctx, base_uri, fill_att, &colorspace, samples);
        if (fill_opacity_att)
            samples[0] *= atof(fill_opacity_att);
        xps_set_color(ctx, colorspace, samples);

        if (sim_bold)
        {
            /* widening strokes by 1% of em size */
            gs_setlinewidth(ctx->pgs, font_size * 0.02);
            gs_settextrenderingmode(ctx->pgs, 2);
        }

        code = xps_parse_glyphs_imp(ctx, font, font_size,
                atof(origin_x_att), atof(origin_y_att),
                is_sideways, bidi_level,
                indices_att, unicode_att, sim_bold && !ctx->preserve_tr_mode, sim_bold);
        if (code)
        {
            xps_end_opacity(ctx, opacity_mask_uri, dict, opacity_att, opacity_mask_tag);
            gs_grestore(ctx->pgs);
            return gs_rethrow(code, "cannot parse glyphs data");
        }

        if (sim_bold && !ctx->preserve_tr_mode)
        {
            gs_gsave(ctx->pgs);
            gs_fill(ctx->pgs);
            gs_grestore(ctx->pgs);
            gs_stroke(ctx->pgs);
        }

        gs_settextrenderingmode(ctx->pgs, 0);
    }

    /*
     * If it's a visual brush or image, use the charpath as a clip mask to paint brush
     */

    if (fill_tag)
    {
        ctx->fill_rule = 1; /* always use non-zero winding rule for char paths */
        code = xps_parse_glyphs_imp(ctx, font, font_size,
                atof(origin_x_att), atof(origin_y_att),
                is_sideways, bidi_level, indices_att, unicode_att, 1, sim_bold);
        if (code)
        {
            xps_end_opacity(ctx, opacity_mask_uri, dict, opacity_att, opacity_mask_tag);
            gs_grestore(ctx->pgs);
            return gs_rethrow(code, "cannot parse glyphs data");
        }

        code = xps_parse_brush(ctx, fill_uri, dict, fill_tag);
        if (code)
        {
            xps_end_opacity(ctx, opacity_mask_uri, dict, opacity_att, opacity_mask_tag);
            gs_grestore(ctx->pgs);
            return gs_rethrow(code, "cannot parse fill brush");
        }
    }

    xps_end_opacity(ctx, opacity_mask_uri, dict, opacity_att, opacity_mask_tag);

    gs_grestore(ctx->pgs);

    return 0;
}