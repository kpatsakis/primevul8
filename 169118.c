static int process_fonts_line(ASS_Track *track, char *str)
{
    size_t len;

    if (!strncmp(str, "fontname:", 9)) {
        char *p = str + 9;
        skip_spaces(&p);
        if (track->parser_priv->fontname) {
            decode_font(track);
        }
        track->parser_priv->fontname = strdup(p);
        if (!track->parser_priv->fontname)
            return -1;
        ass_msg(track->library, MSGL_V, "Fontname: %s",
                track->parser_priv->fontname);
        return 0;
    }

    if (!track->parser_priv->fontname) {
        ass_msg(track->library, MSGL_V, "Not understood: '%s'", str);
        return 1;
    }

    len = strlen(str);
    if (track->parser_priv->fontdata_used >=
        SIZE_MAX - FFMAX(len, 100 * 1024)) {
        goto mem_fail;
    } else if (track->parser_priv->fontdata_used + len >
               track->parser_priv->fontdata_size) {
        size_t new_size =
                track->parser_priv->fontdata_size + FFMAX(len, 100 * 1024);
        if (!ASS_REALLOC_ARRAY(track->parser_priv->fontdata, new_size))
            goto mem_fail;
        track->parser_priv->fontdata_size = new_size;
    }
    memcpy(track->parser_priv->fontdata + track->parser_priv->fontdata_used,
           str, len);
    track->parser_priv->fontdata_used += len;

    return 0;

mem_fail:
    reset_embedded_font_parsing(track->parser_priv);
    return -1;
}