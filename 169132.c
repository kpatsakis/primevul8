static int process_styles_line(ASS_Track *track, char *str)
{
    int ret = 0;
    if (!strncmp(str, "Format:", 7)) {
        char *p = str + 7;
        skip_spaces(&p);
        free(track->style_format);
        track->style_format = strdup(p);
        if (!track->style_format)
            return -1;
        ass_msg(track->library, MSGL_DBG2, "Style format: %s",
               track->style_format);
        if (track->track_type == TRACK_TYPE_ASS)
            custom_format_line_compatibility(track, p, ass_style_format);
        else
            custom_format_line_compatibility(track, p, ssa_style_format);
    } else if (!strncmp(str, "Style:", 6)) {
        char *p = str + 6;
        skip_spaces(&p);
        ret = process_style(track, p);
    }
    return ret;
}