xps_parse_glyphs_imp(xps_context_t *ctx, xps_font_t *font, float size,
        float originx, float originy, int is_sideways, int bidi_level,
        char *indices, char *unicode, int is_charpath, int sim_bold)
{
    xps_text_buffer_t buf;
    xps_glyph_metrics_t mtx;
    float x = originx;
    float y = originy;
    char *us = unicode;
    char *is = indices;
    int un = 0;
    int code;

    buf.count = 0;

    if (!unicode && !indices)
        return gs_throw(-1, "no text in glyphs element");

    if (us)
    {
        if (us[0] == '{' && us[1] == '}')
            us = us + 2;
        un = strlen(us);
    }

    while ((us && un > 0) || (is && *is))
    {
        int char_code = '?';
        int code_count = 1;
        int glyph_count = 1;

        if (is && *is)
        {
            is = xps_parse_cluster_mapping(is, &code_count, &glyph_count);
        }

        if (code_count < 1)
            code_count = 1;
        if (glyph_count < 1)
            glyph_count = 1;

        while (code_count--)
        {
            if (us && un > 0)
            {
                int t = xps_utf8_to_ucs(&char_code, us, un);
                if (t < 0)
                    return gs_rethrow(-1, "error decoding UTF-8 string");
                us += t; un -= t;
            }
        }

        while (glyph_count--)
        {
            int glyph_index = -1;
            float u_offset = 0.0;
            float v_offset = 0.0;
            float advance;

            if (is && *is)
                is = xps_parse_glyph_index(is, &glyph_index);

            if (glyph_index == -1)
                glyph_index = xps_encode_font_char(font, char_code);

            xps_measure_font_glyph(ctx, font, glyph_index, &mtx);
            if (is_sideways)
                advance = mtx.vadv * 100.0;
            else if (bidi_level & 1)
                advance = -mtx.hadv * 100.0;
            else
                advance = mtx.hadv * 100.0;

            if (is && *is)
            {
                is = xps_parse_glyph_metrics(is, &advance, &u_offset, &v_offset, bidi_level);
                if (*is == ';')
                    is ++;
            }

            if (bidi_level & 1)
                u_offset = -mtx.hadv * 100 - u_offset;

            u_offset = u_offset * 0.01 * size;
            v_offset = v_offset * 0.01 * size;

            /* Adjust glyph offset and advance width for emboldening */
            if (sim_bold)
            {
                advance *= 1.02f;
                u_offset += 0.01 * size;
                v_offset += 0.01 * size;
            }

            if (buf.count == XPS_TEXT_BUFFER_SIZE)
            {
                code = xps_flush_text_buffer(ctx, font, &buf, is_charpath);
                if (code)
                    return gs_rethrow(code, "cannot flush buffered text");
            }

            if (is_sideways)
            {
                buf.x[buf.count] = x + u_offset + (mtx.vorg * size);
                buf.y[buf.count] = y - v_offset + (mtx.hadv * 0.5 * size);
            }
            else
            {
                buf.x[buf.count] = x + u_offset;
                buf.y[buf.count] = y - v_offset;
            }
            buf.g[buf.count] = glyph_index;
            buf.count ++;

            x += advance * 0.01 * size;
        }
    }

    if (buf.count > 0)
    {
        code = xps_flush_text_buffer(ctx, font, &buf, is_charpath);
        if (code)
            return gs_rethrow(code, "cannot flush buffered text");
    }

    return 0;
}