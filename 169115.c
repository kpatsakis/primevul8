static void custom_format_line_compatibility(ASS_Track *const track,
                                             const char *const fmt,
                                             const char *const std)
{
    if (!(track->parser_priv->header_flags & SINFO_SCALEDBORDER)
        && !format_line_compare(fmt, std)) {
        ass_msg(track->library, MSGL_INFO,
               "Track has custom format line(s). "
                "'ScaledBorderAndShadow' will default to 'yes'.");
        track->ScaledBorderAndShadow = 1;
    }
}