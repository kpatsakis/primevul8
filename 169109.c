static int process_line(ASS_Track *track, char *str)
{
    skip_spaces(&str);
    if (!ass_strncasecmp(str, "[Script Info]", 13)) {
        track->parser_priv->state = PST_INFO;
    } else if (!ass_strncasecmp(str, "[V4 Styles]", 11)) {
        track->parser_priv->state = PST_STYLES;
        track->track_type = TRACK_TYPE_SSA;
    } else if (!ass_strncasecmp(str, "[V4+ Styles]", 12)) {
        track->parser_priv->state = PST_STYLES;
        track->track_type = TRACK_TYPE_ASS;
    } else if (!ass_strncasecmp(str, "[Events]", 8)) {
        track->parser_priv->state = PST_EVENTS;
    } else if (!ass_strncasecmp(str, "[Fonts]", 7)) {
        track->parser_priv->state = PST_FONTS;
    } else {
        switch (track->parser_priv->state) {
        case PST_INFO:
            process_info_line(track, str);
            break;
        case PST_STYLES:
            process_styles_line(track, str);
            break;
        case PST_EVENTS:
            process_events_line(track, str);
            break;
        case PST_FONTS:
            process_fonts_line(track, str);
            break;
        default:
            break;
        }
    }
    return 0;
}